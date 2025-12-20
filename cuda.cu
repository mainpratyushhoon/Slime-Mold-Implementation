#include <glut.h>
#include <cuda.h>
#include <cuda_runtime.h>
#include <curand_kernel.h>
#include <chrono>
#include <vector>
#include <cmath>
#include <iostream>
#include <ctime>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define M_PI 3.1415926
using namespace std;

/* ---------------- Window ---------------- */

const int WIN_W = 800;
const int WIN_H = 800;
static int cnt = 0;
static auto last = std::chrono::high_resolution_clock::now();

/* ---------------- Simulation ---------------- */

const int NUM_AGENTS = 10000000;

/* Tunable parameters (HOST ONLY) */
float sensor_distance = 10.0f;
float sensor_angle    = 0.2f;
float turn_angle      = 0.3f;
float step_size       = 0.1f;
float deposit_amount  = 2.0f;
float evaporation     = 0.05f;
float diffusion_rate  = 0.1f;

/* ---------------- Grid ---------------- */

int GRID_W = 0;
int GRID_H = 0;

/* ---------------- GPU Buffers ---------------- */

float *d_ax, *d_ay, *d_angle;
float *d_trail, *d_trail_tmp;
int   *d_maze;
curandState *d_rng;

/* ---------------- Host Buffers ---------------- */

vector<int>   h_maze;
vector<float> h_trail;

/* ---------------- Utilities ---------------- */

__device__ __forceinline__
int idx(int x, int y, int W){
    return y * W + x;
}

/* ---------------- RNG Init ---------------- */

__global__ void initRNG(curandState* rng, unsigned seed){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < NUM_AGENTS)
        curand_init(seed, i, 0, &rng[i]);
}

/* ---------------- Agent Update ---------------- */

__global__ void updateAgentsKernel(
    float* ax, float* ay, float* angle,
    float* trail, int* maze,
    curandState* rng,
    int W, int H,
    float sensor_distance,
    float sensor_angle,
    float turn_angle,
    float step_size,
    float deposit_amount
){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i >= NUM_AGENTS) return;

    curandState local = rng[i];

    float x = ax[i];
    float y = ay[i];
    float a = angle[i];

    auto sample = [&](float sx, float sy){
        int xi = (int)sx;
        int yi = (int)sy;
        if (xi < 0 || xi >= W || yi < 0 || yi >= H) return 0.0f;
        if (maze[idx(xi, yi, W)]) return 0.0f;
        return trail[idx(xi, yi, W)];
    };

    float f = sample(x + cosf(a) * sensor_distance,
                     y + sinf(a) * sensor_distance);

    float l = sample(x + cosf(a + sensor_angle) * sensor_distance,
                     y + sinf(a + sensor_angle) * sensor_distance);

    float r = sample(x + cosf(a - sensor_angle) * sensor_distance,
                     y + sinf(a - sensor_angle) * sensor_distance);

    if (l > f && l > r)       a += turn_angle;
    else if (r > f && r > l)  a -= turn_angle;
    else                      a += (curand_uniform(&local) - 0.5f) * 0.2f;

    a += (curand_uniform(&local) - 0.5f) * 0.3f;

    float nx = x + cosf(a) * step_size;
    float ny = y + sinf(a) * step_size;

    int xi = (int)nx;
    int yi = (int)ny;

    if (xi >= 0 && xi < W && yi >= 0 && yi < H && !maze[idx(xi, yi, W)]) {
        x = nx;
        y = ny;
        atomicAdd(&trail[idx(xi, yi, W)], deposit_amount);
    } else {
        a += 3.1415926f * (curand_uniform(&local) - 0.5f);
    }

    ax[i] = x;
    ay[i] = y;
    angle[i] = a;
    rng[i] = local;
}

/* ---------------- Diffusion ---------------- */

__global__ void diffuseKernel(
    float* trail, float* out,
    int* maze, int W, int H,
    float diffusion_rate
){
    int x = blockIdx.x * blockDim.x + threadIdx.x;
    int y = blockIdx.y * blockDim.y + threadIdx.y;

    if (x <= 0 || x >= W - 1 || y <= 0 || y >= H - 1) return;
    if (maze[idx(x, y, W)]) return;

    float sum = 0.0f;
    int count = 0;

    for (int dy = -1; dy <= 1; dy++)
        for (int dx = -1; dx <= 1; dx++) {
            int id = idx(x + dx, y + dy, W);
            if (!maze[id]) {
                sum += trail[id];
                count++;
            }
        }

    int id = idx(x, y, W);
    out[id] = trail[id] * (1.0f - diffusion_rate)
            + (sum / count) * diffusion_rate;
}

/* ---------------- Evaporation ---------------- */

__global__ void evaporateKernel(
    float* trail,
    int n,
    float evaporation
){
    int i = blockIdx.x * blockDim.x + threadIdx.x;
    if (i < n)
        trail[i] *= (1.0f - evaporation);
}

/* ---------------- Map Loader ---------------- */

void loadMap(const char* filename){
    int w, h, n;
    unsigned char* img = stbi_load(filename, &w, &h, &n, 1);

    if (!img) {
        cout << "Failed to load map\n";
        exit(1);
    }

    GRID_W = w;
    GRID_H = h;

    h_maze.resize(w * h);

    for (int y = 0; y < h; y++)
        for (int x = 0; x < w; x++)
            h_maze[y * w + x] = img[y * w + x] > 128 ? 0 : 1;

    stbi_image_free(img);
}

/* ---------------- GPU Init ---------------- */

void initGPU(){
    size_t N = GRID_W * GRID_H;

    cudaMalloc(&d_ax, NUM_AGENTS * sizeof(float));
    cudaMalloc(&d_ay, NUM_AGENTS * sizeof(float));
    cudaMalloc(&d_angle, NUM_AGENTS * sizeof(float));
    cudaMalloc(&d_rng, NUM_AGENTS * sizeof(curandState));

    cudaMalloc(&d_trail,     N * sizeof(float));
    cudaMalloc(&d_trail_tmp, N * sizeof(float));
    cudaMalloc(&d_maze,      N * sizeof(int));

    cudaMemcpy(d_maze, h_maze.data(), N * sizeof(int), cudaMemcpyHostToDevice);
    cudaMemset(d_trail, 0, N * sizeof(float));

    vector<float> ax(NUM_AGENTS), ay(NUM_AGENTS), an(NUM_AGENTS);

    srand((unsigned)time(0));
    for (int i = 0; i < NUM_AGENTS; i++) {
        int x, y;
        do {
            x = rand() % GRID_W;
            y = rand() % GRID_H;
        } while (h_maze[y * GRID_W + x]);

        ax[i] = (float)x;
        ay[i] = (float)y;
        an[i] = ((float)rand() / RAND_MAX) * 2.0f * 3.1415926f;
    }

    cudaMemcpy(d_ax, ax.data(), NUM_AGENTS * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_ay, ay.data(), NUM_AGENTS * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_angle, an.data(), NUM_AGENTS * sizeof(float), cudaMemcpyHostToDevice);

    initRNG<<<(NUM_AGENTS + 255) / 256, 256>>>(d_rng, (unsigned)time(0));
}

/* ---------------- Display ---------------- */

void display(){
    glClear(GL_COLOR_BUFFER_BIT);

    updateAgentsKernel<<<(NUM_AGENTS + 255) / 256, 256>>>(
        d_ax, d_ay, d_angle,
        d_trail, d_maze, d_rng,
        GRID_W, GRID_H,
        sensor_distance,
        sensor_angle,
        turn_angle,
        step_size,
        deposit_amount
    );

    dim3 T(16, 16);
    dim3 B((GRID_W + 15) / 16, (GRID_H + 15) / 16);

    diffuseKernel<<<B, T>>>(
        d_trail, d_trail_tmp,
        d_maze, GRID_W, GRID_H,
        diffusion_rate
    );

    evaporateKernel<<<(GRID_W * GRID_H + 255) / 256, 256>>>(
        d_trail_tmp,
        GRID_W * GRID_H,
        evaporation
    );

    swap(d_trail, d_trail_tmp);

    h_trail.resize(GRID_W * GRID_H);
    cudaMemcpy(h_trail.data(), d_trail,
               GRID_W * GRID_H * sizeof(float),
               cudaMemcpyDeviceToHost);

    float maxv = 1e-6f;
    for (float v : h_trail) maxv = max(maxv, v);

    glBegin(GL_POINTS);
    for (int y = 0; y < GRID_H; y++)
        for (int x = 0; x < GRID_W; x++) {
            float v = h_trail[y * GRID_W + x] / maxv;
            if (v > 0.01f) {
                glColor3f(v, v, v);
                glVertex2f(
                    x / (float)GRID_W * 2 - 1,
                    y / (float)GRID_H * 2 - 1
                );
            }
        }
    glEnd();

    glutSwapBuffers();
    glutPostRedisplay();
    cnt++;

auto now = std::chrono::high_resolution_clock::now();
std::chrono::duration<double> elapsed = now - last;

if (elapsed.count() >= 1.0)
{
    cout << cnt / elapsed.count() << " FPS" << endl;
    cnt = 0;
    last = now;
}

}

/* ---------------- Main ---------------- */

int main(int argc, char** argv){
    loadMap("map.png");

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WIN_W, WIN_H);
    glutCreateWindow("GPU Slime Mold ADRP");

    glClearColor(0, 0, 0, 1);
    glPointSize(1.5f);

    initGPU();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
}
