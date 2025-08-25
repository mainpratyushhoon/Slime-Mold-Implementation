#include <glut.h>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>
#include <time.h>
using namespace std;

const int WIN_W = 800;
const int WIN_H = 800;
const int NUM_AGENTS = 5000;
const int NUM_POINTS = 5;

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
int GRID_W = 150, GRID_H = 150;

mt19937 rng(time(0));
uniform_real_distribution<float> uni01(0.0f,1.0f);

// parameters
float sensor_distance = 10.0f;
float sensor_angle = 0.3f;
float turn_angle = 0.3f;
float step_size = 0.1f;
float deposit_amount = 5.0f;
float evaporation = 0.05f;
float diffusion_rate = 0.1f;


inline int idx(int x,int y){ return y*GRID_W + x; }

int randomINT(){
    return rand()%GRID_H;
}

void initAgents(){
    agents.resize(NUM_AGENTS);
    for(int i=0;i<NUM_AGENTS;i++){
        agents[i].x = randomINT();
        agents[i].y = randomINT();
        agents[i].angle = uni01(rng)*2*M_PI;
    }
    trail.assign(GRID_W*GRID_H,0.0f);
}

float sampleTrail(float x,float y){
    int xi = (int)x; int yi = (int)y;
    if(xi<0||xi>=GRID_W||yi<0||yi>=GRID_H) return 0.0f;
    return trail[idx(xi,yi)];
}

void deposit(float x,float y,float amount = deposit_amount){
    int xi=(int)x; int yi=(int)y;
    if(xi<0||xi>=GRID_W||yi<0||yi>=GRID_H) return;
    trail[idx(xi,yi)] += amount;
}

void assignPoints(){
    for(int i=0;i<NUM_POINTS;i++){
        points[i].x=randomINT();
        points[i].y=randomINT();
        deposit(points[i].x, points[i].y,100.0);
    }
}

void updateAgents(){
    for(auto &a:agents){
        float ax=a.x+cos(a.angle)*sensor_distance;
        float ay=a.y+sin(a.angle)*sensor_distance;
        float lx=a.x+cos(a.angle+sensor_angle)*sensor_distance;
        float ly=a.y+sin(a.angle+sensor_angle)*sensor_distance;
        float rx=a.x+cos(a.angle-sensor_angle)*sensor_distance;
        float ry=a.y+sin(a.angle-sensor_angle)*sensor_distance;

        float sF=sampleTrail(ax,ay);
        float sL=sampleTrail(lx,ly);
        float sR=sampleTrail(rx,ry);

        if(sF>sL && sF>sR){
            // go straight
        } else if(sL>sR){
            a.angle+=turn_angle;
        } else if(sR>sL){
            a.angle-=turn_angle;
        } else {
            a.angle+=(uni01(rng)-0.5f)*0.2f;
        }

        a.x+=cos(a.angle)*step_size;
        a.y+=sin(a.angle)*step_size;
        a.angle += (uni01(rng) - 0.5f) * 0.5f;  // more exploration


        if(a.x<0){
            a.x=0;
            a.angle=M_PI-a.angle+ (uni01(rng)-0.5f)*0.5f;
        }
        if(a.x>=GRID_W){
            a.x=GRID_W-1;
            a.angle=M_PI-a.angle+ (uni01(rng)-0.5f)*0.5f;
        }
        if(a.y<0){
            a.y=0;
            a.angle = -a.angle + (uni01(rng)-0.5f)*0.5f;

        }
    if(a.y>=GRID_H){
        a.y=GRID_H-1;
        a.angle=-a.angle+ (uni01(rng)-0.5f)*0.5f;
    }

        deposit(a.x,a.y);
        
    }
    for(auto itr: points){
            deposit(itr.x,itr.y,100.0);
        }
}

void diffuse(){
    vector<float> newTrail = trail;

    for(int y=1; y<GRID_H-1; y++){
        for(int x=1; x<GRID_W-1; x++){
            float sum = 0.0f;
            for(int dy=-1; dy<=1; dy++){
                for(int dx=-1; dx<=1; dx++){
                    sum += trail[idx(x+dx, y+dy)];
                }
            }
            newTrail[idx(x,y)] = trail[idx(x,y)]*(1-diffusion_rate) + (sum/9.0f)*diffusion_rate;
        }
    }

    trail = newTrail;
}

void evaporate(){
    for(auto &v:trail){
        v*= (1.0f-evaporation);
    }
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);

    updateAgents();
    diffuse();
    evaporate();

    glBegin(GL_POINTS);

    for(int y=0;y<GRID_H;y++){
        for(int x=0;x<GRID_W;x++){
            float v=trail[idx(x,y)];
            if(v>0.01f){
                float c = std::min(1.0f,v*0.05f);
                glColor3f(c,c,c);
                glVertex2f((float)x/GRID_W*2-1, (float)y/GRID_H*2-1);
            }
        }
    }
    for(auto itr:points){
            glColor3f(1,0,0);
            glVertex2f((float)itr.x/GRID_W*2 - 1, (float)itr.y/GRID_H*2 - 1);

        }

    
    glEnd();
    
    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc,char**argv){
    srand(time(0));
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(WIN_W,WIN_H);
    glutCreateWindow("Slime Mold (GLUT)");

    glClearColor(0,0,0,1);
    glPointSize(2.0f);
    //glEnable(GL_POINT_SMOOTH);

    initAgents();
    assignPoints();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
