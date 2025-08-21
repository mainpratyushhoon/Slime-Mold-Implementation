#include <glut.h>
#include <vector>
#include <cmath>
#include <random>
#include <iostream>
using namespace std;
const int WIN_W = 800;
const int WIN_H = 800;
const int NUM_AGENTS = 1000;

struct Agent {
    float x, y;
    float angle;
};


vector<Agent> agents;
vector<float> trail;  // 2D trail map
int GRID_W = 150, GRID_H = 150;

mt19937 rng(time(0));
uniform_real_distribution<float> uni01(0.0f,1.0f);

// parameters
float sensor_distance = 3.0f;
float sensor_angle = 0.5f;
float turn_angle = 0.01f;
float step_size = 0.1f;
float deposit_amount = 1.0f;
float evaporation = 0.01f;

inline int idx(int x,int y){ return y*GRID_W + x; }

void initAgents(){
    agents.resize(NUM_AGENTS);
    for(int i=0;i<NUM_AGENTS;i++){
        agents[i].x = GRID_W/2+(uni01(rng)-0.5f)*0.2f;
        agents[i].y = GRID_H/2+(uni01(rng)-0.5f)*0.2f;
        agents[i].angle = uni01(rng)*2*M_PI;
    }
    trail.assign(GRID_W*GRID_H,0.0f);
}

float sampleTrail(float x,float y){
    int xi = (int)x; int yi = (int)y;
    if(xi<0||xi>=GRID_W||yi<0||yi>=GRID_H) return 0.0f;
    return trail[idx(xi,yi)];
}

void deposit(float x,float y){
    int xi=(int)x; int yi=(int)y;
    if(xi<0||xi>=GRID_W||yi<0||yi>=GRID_H) return;
    trail[idx(xi,yi)] += deposit_amount;
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

        if(a.x<0){
            a.x=0;
            a.angle=M_PI-a.angle;
        }
        if(a.x>=GRID_W){
            a.x=GRID_W-1;
            a.angle=M_PI-a.angle;
        }
        if(a.y<0){
            a.y=0;
            a.angle=-a.angle;
        }
    if(a.y>=GRID_H){
        a.y=GRID_H-1;
        a.angle=-a.angle;
    }

        deposit(a.x,a.y);
    }
}

void evaporate(){
    for(auto &v:trail){
        v*= (1.0f-evaporation);
    }
}





void display(){
    glClear(GL_COLOR_BUFFER_BIT);

    updateAgents();
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
    glEnd();
    
    glutSwapBuffers();
    glutPostRedisplay();
}

int main(int argc,char**argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB);
    glutInitWindowSize(WIN_W,WIN_H);
    glutCreateWindow("Slime Mold (GLUT)");

    glClearColor(0,0,0,1);
    glPointSize(2.0f);
    glEnable(GL_POINT_SMOOTH);

    initAgents();

    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
