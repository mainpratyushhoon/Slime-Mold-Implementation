#include <glut.h>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>
#include <time.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;

const int WIN_W = 800;
const int WIN_H = 800;
const int NUM_AGENTS = 100000;
const int NUM_POINTS = 30;



struct Agent {
    float x, y;
    float angle;
};

struct Point {
    float x,y;
};

vector<Agent> agents;
vector<Point> points(NUM_POINTS);
vector<float> trail;

int GRID_W , GRID_H;

// Maze: 0 = free, 1 = wall
vector<int> maze;

inline int idx(int x,int y){ return y*GRID_W + x; }

void loadMap(const char* filename){
    int w,h,n;
    unsigned char* data = stbi_load(filename,&w,&h,&n,1);
    if(!data){
        cout<<"Failed to load map\n";
        return;
    }

    GRID_W = w;
    GRID_H = h;
    maze.assign(GRID_W*GRID_H,1);

    for(int y=0;y<h;y++){
        for(int x=0;x<w;x++){
            int v = data[y*w + x];
            maze[idx(x,y)] = (v > 128) ? 0 : 1; // white = road
        }
    }

    stbi_image_free(data);
}

mt19937 rng(time(0));
uniform_real_distribution<float> uni01(0.0f,1.0f);

// Parameters
float sensor_distance = 10.0f;
float sensor_angle = 0.03f;
float turn_angle = 0.3f;
float step_size = 0.1f;
float deposit_amount = 2.0f;
float evaporation = 0.05f;
float diffusion_rate = 0.1f;

// Mouse drawing
bool drawing = false;
int brush_size = 1;

int randomINT(){ return rand() % GRID_H; }

// ---------- Initialization ----------
void initAgents(){
    agents.resize(NUM_AGENTS);
    trail.assign(GRID_W*GRID_H,0.0f);

    for(auto &a : agents){
        int x,y;
        do{
            x = rand() % GRID_W;
            y = rand() % GRID_H;
        }while(maze[idx(x,y)]==1);

        a.x = x;
        a.y = y;
        a.angle = uni01(rng)*2*M_PI;
    }
}


void assignPoints(){
    for(auto &p : points){
        int x,y;
        do{
            x = rand() % GRID_W;
            y = rand() % GRID_H;
        }while(maze[idx(x,y)]==1);

        p.x = x;
        p.y = y;
        trail[idx(x,y)] = 50.0f;
    }
}


// ---------- Mouse ----------
void mouse(int button, int state, int x, int y){
    if(button == GLUT_LEFT_BUTTON){
        drawing = (state == GLUT_DOWN);
    }
}

void motion(int x,int y){
    if(!drawing) return;
    int gx = x * GRID_W / WIN_W;
    int gy = (WIN_H - y) * GRID_H / WIN_H;
    for(int dy=-brush_size; dy<=brush_size; dy++){
        for(int dx=-brush_size; dx<=brush_size; dx++){
            int nx=gx+dx, ny=gy+dy;
            if(nx>=0&&nx<GRID_W&&ny>=0&&ny<GRID_H)
                maze[idx(nx,ny)] = 1;
        }
    }
}

// ---------- Trail sampling ----------
inline float sampleTrail(float x,float y){
    int xi=(int)x, yi=(int)y;
    if(xi<0||xi>=GRID_W||yi<0||yi>=GRID_H) return 0;
    if(maze[idx(xi,yi)]==1) return 0;
    return trail[idx(xi,yi)];
}

// ---------- Deposit ----------
inline void deposit(float x,float y,float amt){
    int xi=(int)x, yi=(int)y;
    if(xi<0||xi>=GRID_W||yi<0||yi>=GRID_H) return;
    if(maze[idx(xi,yi)]==0)
        trail[idx(xi,yi)] += amt;
}

// ---------- Agent update ----------
void updateAgents(){
    for(auto &a:agents){
        float ax=a.x+cos(a.angle)*sensor_distance;
        float ay=a.y+sin(a.angle)*sensor_distance;
        float lx=a.x+cos(a.angle+sensor_angle)*sensor_distance;
        float ly=a.y+sin(a.angle+sensor_angle)*sensor_distance;
        float rx=a.x+cos(a.angle-sensor_angle)*sensor_distance;
        float ry=a.y+sin(a.angle-sensor_angle)*sensor_distance;

        float f=sampleTrail(ax,ay);
        float l=sampleTrail(lx,ly);
        float r=sampleTrail(rx,ry);

        if(l>f && l>r) a.angle+=turn_angle;
        else if(r>f && r>l) a.angle-=turn_angle;
        else a.angle+=(uni01(rng)-0.5f)*0.2f;

        a.angle+=(uni01(rng)-0.5f)*0.3f;

        float nx=a.x+cos(a.angle)*step_size;
        float ny=a.y+sin(a.angle)*step_size;

        if(nx>=0&&nx<GRID_W&&ny>=0&&ny<GRID_H&&maze[idx((int)nx,(int)ny)]==0){
            a.x=nx; a.y=ny;
        } else {
            a.angle+=M_PI*(uni01(rng)-0.5f);
        }

        deposit(a.x,a.y,deposit_amount);
    }

    // reinforce food (emergency demand)
    for(auto &p:points)
        deposit(p.x,p.y,10.0f);
}

// ---------- Diffusion ----------
void diffuse(){
    vector<float> tmp=trail;
    for(int y=1;y<GRID_H-1;y++){
        for(int x=1;x<GRID_W-1;x++){
            if(maze[idx(x,y)]) continue;
            float s=0; int c=0;
            for(int dy=-1;dy<=1;dy++)
                for(int dx=-1;dx<=1;dx++){
                    if(!maze[idx(x+dx,y+dy)]){
                        s+=trail[idx(x+dx,y+dy)];
                        c++;
                    }
                }
            tmp[idx(x,y)]=trail[idx(x,y)]*(1-diffusion_rate)+(s/c)*diffusion_rate;
        }
    }
    trail.swap(tmp);
}

void evaporate(){
    for(float &v:trail) v*=(1.0f-evaporation);
}

// ---------- Display ----------
void display(){
    glClear(GL_COLOR_BUFFER_BIT);

    updateAgents();
    diffuse();
    evaporate();

    float maxTrail=0;
    for(float v:trail) maxTrail=max(maxTrail,v);
    if(maxTrail<1e-5) maxTrail=1;

    glBegin(GL_POINTS);

    // Trail field
    for(int y=0;y<GRID_H;y++){
        for(int x=0;x<GRID_W;x++){
            float v=trail[idx(x,y)];
            if(v>0.01f){
                float c=v/maxTrail;
                glColor3f(c,c,c);
                glVertex2f((float)x/GRID_W*2-1,(float)y/GRID_H*2-1);
            }
        }
    }

    // Agents (colored by local demand)
    for(auto &a:agents){
        float v=sampleTrail(a.x,a.y);
        float c=v/maxTrail;
        glColor3f(c,0.2f,1.0f-c); // heat-style
        glVertex2f(a.x/GRID_W*2-1,a.y/GRID_H*2-1);
    }

    // Food (emergencies)
    for(auto &p:points){
        glColor3f(1,0,0);
        glVertex2f(p.x/GRID_W*2-1,p.y/GRID_H*2-1);
    }

    // Walls
    glColor3f(0,0,1);
    for(int y=0;y<GRID_H;y++)
        for(int x=0;x<GRID_W;x++)
            if(maze[idx(x,y)])
                glVertex2f(x/(float)GRID_W*2-1,y/(float)GRID_H*2-1);

    glEnd();

    glutSwapBuffers();
    glutPostRedisplay();
}

// ---------- Main ----------
int main(int argc,char**argv){
    loadMap("map.png");
    srand(time(0));
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(WIN_W,WIN_H);
    glutCreateWindow("Slime Mold Demand Field (ADRP)");

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
