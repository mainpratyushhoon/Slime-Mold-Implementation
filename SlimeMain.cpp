#include <glut.h>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>
#include <time.h>
using namespace std;

const int WIN_W = 800;
const int WIN_H = 800;
const int NUM_AGENTS = 10000;
const int NUM_POINTS = 100;

struct Agent {
    float x, y;
    float angle;
};

struct Point {
    float x,y;
};

vector<Agent> agents;
vector<Point> points(NUM_POINTS);
vector<float> trail;  // 2D trail map
int GRID_W = 200, GRID_H = 200;

// Maze: 0 = free, 1 = wall
vector<int> maze(GRID_W * GRID_H, 0);
inline int midx(int x,int y){ return y*GRID_W + x; }
inline int idx(int x,int y){ return y*GRID_W + x; }

mt19937 rng(time(0));
uniform_real_distribution<float> uni01(0.0f,1.0f);

// parameters
float sensor_distance = 10.0f;
float sensor_angle = 0.03f;
float turn_angle = 0.3f;
float step_size = 0.1f;
float deposit_amount = 2.0f;
float evaporation = 0.05f;
float diffusion_rate = 0.1f;

// mouse drawing
bool drawing = false; // left button
int brush_size = 3;   // brush radius

int randomINT(){ return rand() % GRID_H; }

// ---- Initialization ----
void initAgents(){
    agents.resize(NUM_AGENTS);
    for(int i=0;i<NUM_AGENTS;i++){
        agents[i].x = randomINT();
        agents[i].y = randomINT();
        agents[i].angle = uni01(rng)*2*M_PI;
    }
    trail.assign(GRID_W*GRID_H,0.0f);
}

void assignPoints(){
    for(int i=0;i<NUM_POINTS;i++){
        points[i].x=randomINT();
        points[i].y=randomINT();
        trail[idx((int)points[i].x,(int)points[i].y)] = 100.0f;
    }
}

// ---- Mouse callbacks ----
void mouse(int button, int state, int x, int y){
    if(button == GLUT_LEFT_BUTTON){
        drawing = (state == GLUT_DOWN);
        int gx = x * GRID_W / WIN_W;
        int gy = (WIN_H - y) * GRID_H / WIN_H;
        for(int dy=-brush_size; dy<=brush_size; dy++){
            for(int dx=-brush_size; dx<=brush_size; dx++){
                int gx2 = gx + dx;
                int gy2 = gy + dy;
                if(gx2>=0 && gx2<GRID_W && gy2>=0 && gy2<GRID_H){
                    maze[midx(gx2,gy2)] = 1;
                }
            }
        }
    }
}

void motion(int x,int y){
    if(drawing){
        int gx = x * GRID_W / WIN_W;
        int gy = (WIN_H - y) * GRID_H / WIN_H;
        for(int dy=-brush_size; dy<=brush_size; dy++){
            for(int dx=-brush_size; dx<=brush_size; dx++){
                int gx2 = gx + dx;
                int gy2 = gy + dy;
                if(gx2>=0 && gx2<GRID_W && gy2>=0 && gy2<GRID_H){
                    maze[midx(gx2,gy2)] = 1;
                }
            }
        }
    }
}

// ---- Trail sampling ----
float sampleTrailMaze(float x,float y){
    int xi=(int)x, yi=(int)y;
    if(xi<0 || xi>=GRID_W || yi<0 || yi>=GRID_H) return 0;
    if(maze[midx(xi,yi)]==1) return 0; // wall blocks sensing
    return trail[idx(xi,yi)];
}

// ---- Deposit ----
void deposit(float x,float y,float amount = deposit_amount){
    int xi=(int)x; int yi=(int)y;
    if(xi<0||xi>=GRID_W||yi<0||yi>=GRID_H) return;
    if(maze[midx(xi,yi)]==0) trail[idx(xi,yi)] += amount;
}

// ---- Agent update ----
void updateAgents(){
    for(auto &a:agents){
        float ax=a.x+cos(a.angle)*sensor_distance;
        float ay=a.y+sin(a.angle)*sensor_distance;
        float lx=a.x+cos(a.angle+sensor_angle)*sensor_distance;
        float ly=a.y+sin(a.angle+sensor_angle)*sensor_distance;
        float rx=a.x+cos(a.angle-sensor_angle)*sensor_distance;
        float ry=a.y+sin(a.angle-sensor_angle)*sensor_distance;

        float sF=sampleTrailMaze(ax,ay);
        float sL=sampleTrailMaze(lx,ly);
        float sR=sampleTrailMaze(rx,ry);

        if(sF>sL && sF>sR){
            // go straight
        } else if(sL>sR){
            a.angle+=turn_angle;
        } else if(sR>sL){
            a.angle-=turn_angle;
        } else {
            a.angle+=(uni01(rng)-0.5f)*0.2f;
        }

        a.angle += (uni01(rng)-0.5f)*0.5f; // exploration

        // proposed next position
        float nx = a.x + cosf(a.angle)*step_size;
        float ny = a.y + sinf(a.angle)*step_size;

        // move if no wall
        if(nx>=0 && nx<GRID_W && ny>=0 && ny<GRID_H && maze[midx((int)nx,(int)ny)]==0){
            a.x = nx;
            a.y = ny;
        } else {
            a.angle += (uni01(rng)-0.5f)*M_PI; // bounce
        }

        deposit(a.x,a.y);
    }

    // reinforce points
    for(auto itr: points){
        deposit(itr.x,itr.y,100.0f);
    }
}

// ---- Diffusion & Evaporation ----
void diffuse(){
    vector<float> newTrail = trail;

    for(int y=1; y<GRID_H-1; y++){
        for(int x=1; x<GRID_W-1; x++){
            if(maze[idx(x,y)]==1) continue; // skip walls
            float sum = 0.0f;
            int count = 0;
            for(int dy=-1; dy<=1; dy++){
                for(int dx=-1; dx<=1; dx++){
                    int nx = x+dx;
                    int ny = y+dy;
                    if(maze[idx(nx,ny)]==0){
                        sum += trail[idx(nx,ny)];
                        count++;
                    }
                }
            }
            if(count>0) newTrail[idx(x,y)] = trail[idx(x,y)]*(1-diffusion_rate) + (sum/count)*diffusion_rate;
        }
    }

    trail = newTrail;
}

void evaporate(){
    for(int y=0;y<GRID_H;y++){
        for(int x=0;x<GRID_W;x++){
            if(maze[idx(x,y)]==0)
                trail[idx(x,y)] *= (1.0f-evaporation);
        }
    }
}

// ---- Display ----
void display(){
    glClear(GL_COLOR_BUFFER_BIT);

    updateAgents();
    diffuse();
    evaporate();

    glBegin(GL_POINTS);

    // trails
    for(int y=0;y<GRID_H;y++){
        for(int x=0;x<GRID_W;x++){
            float v = trail[idx(x,y)];
            if(v>0.01f){
                float c = min(1.0f,v*0.05f);
                glColor3f(c,c,c);
                glVertex2f((float)x/GRID_W*2-1, (float)y/GRID_H*2-1);
            }
        }
    }

    // points
    for(auto itr:points){
        glColor3f(1,0,0);
        glVertex2f((float)itr.x/GRID_W*2 - 1, (float)itr.y/GRID_H*2 - 1);
    }

    // walls
    glColor3f(0,0,1);
    for(int y=0;y<GRID_H;y++){
        for(int x=0;x<GRID_W;x++){
            if(maze[midx(x,y)]==1){
                glVertex2f((float)x/GRID_W*2-1, (float)y/GRID_H*2-1);
            }
        }
    }

    glEnd();
    
    glutSwapBuffers();
    glutPostRedisplay();
}

// ---- Main ----
int main(int argc,char**argv){
    srand(time(0));
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(WIN_W,WIN_H);
    glutCreateWindow("Interactive Slime Mold Maze");

    glClearColor(0,0,0,1);
    glPointSize(2.0f);

    glutMouseFunc(mouse);
    glutMotionFunc(motion);

    initAgents();
    assignPoints();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}