# Slime Mold Simulation

<div id="top"></div>

<!-- PROJECT SHIELDS -->
<div align="center">

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]

</div>

<br />
<div align="center">
  <a href="https://github.com/mainpratyushhoon/Slime-Mold-Implementation">
     <img width="340" alt="image" src="https://upload.wikimedia.org/wikipedia/commons/thumb/c/c9/Physarum_polycephalum_strain_LU352_-_4.jpg/1280px-Physarum_polycephalum_strain_LU352_-_4.jpg">
  </a>

  <h3 align="center">SLIME MOLD SIMULATION</h3>

  <p align="center">
    <i>Application of Swarm Intelligence</i>
  </p>
</div>


<details>
<summary>Table of Contents</summary>

- [About The Project](#about-the-project)
- [Features](#features)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
  - [Command-line Options](#command-line-options)
  - [Examples](#examples)
- [Map Feature](#map-feature)
- [GPU Acceleration](#gpu-acceleration)
- [Contact](#contact)
  - [Maintainer(s)](#maintainers)
  - [Creator(s)](#creators)
- [Additional documentation](#additional-documentation)

</details>

## About The Project

<div align="center">
  <a href="https://github.com/mainpratyushhoon/Slime-Mold-Implementation">
    <img width="50%" alt="image" src="https://github.com/mainpratyushhoon/Slime-Mold-Implementation/blob/main/working.png">
  </a>
</div>
<br>
This project simulates the behavior of slime mold (Physarum polycephalum) to approximate efficient transport networks between sets of points. The simulation models agents (plasmodia) that explore, deposit and follow pheromone trails to form minimal, robust networks similar to those observed in natural slime mold.

The goal of this repository is to provide a research / demonstration implementation that can be used to experiment with parameters, compare with brute-force optimal solutions, and visualize the resulting networks.

<p align="right">(<a href="#top">back to top</a>)</p>

## Features

- Agent-based slime mold simulation producing emergent transport networks
- Brute-force solver (separate script) for comparison
- Interactive OpenGL visualization
- Map support: load obstacle maps or custom layouts
- GPU acceleration (optional) to speed up pheromone diffusion and agent updates

<p align="right">(<a href="#top">back to top</a>)</p>

## Getting Started

To set up a local instance of the application, follow the steps below.

### Prerequisites

* C++ compiler (g++ / clang++)
* OpenGL + FreeGLUT (for visualization)
* Make or CMake (optional)
* Optional for GPU acceleration:
  - OpenCL or CUDA development libraries (depending on the GPU backend you choose)

Notes:
- The project uses FreeGLUT for windowing and OpenGL for rendering. On Windows the linker flags used previously were -lglu32 -lopengl32; on Linux use -lGL -lGLU -lglut (or link via your package manager).

<p align="right">(<a href="#top">back to top</a>)</p>

### Installation

1. Clone the repository
   ```sh
   git clone https://github.com/mainpratyushhoon/Slime-Mold-Implementation.git
   cd Slime-Mold-Implementation
   ```

2. Compile (CPU-only / default)
   ```sh
   g++ SlimeMain.cpp -lfreeglut -lglu32 -lopengl32 -o slime.exe
   ```
   (On Linux/macOS you may need to change the linker flags: `-lfreeglut -lGL -lGLU`)

3. Compile with GPU acceleration (optional)

   - If your build system uses a Makefile or CMakeLists, enable the GPU backend or add the OpenCL/CUDA linker flags. Example (OpenCL on Windows):
     ```sh
     g++ SlimeMain.cpp -lfreeglut -lglu32 -lopengl32 -lOpenCL -o slime_gpu.exe
     ```
   - If CUDA is used, compile with nvcc for CUDA kernels and link the resulting objects accordingly.

4. Run the binary (see usage below)

<p align="right">(<a href="#top">back to top</a>)</p>

## Usage

The simulation binary supports command-line options to configure behavior, enable GPU acceleration, and load map files.

### Command-line Options

- `--gpu` : Run the simulation using the GPU-accelerated update kernels (if the binary was built with GPU support).
- `--map <path>` : Load a map image or map file to use as obstacles/terrain. Supported formats: PNG, JPG, or a plain-text map format if provided in the repository.
- `--width <w>` `--height <h>` : Set simulation grid resolution.
- `--agents <n>` : Number of agents to spawn.
- `--help` : Show available options.

(Refer to the simulation source for the exact available flags and parameter names.)

### Examples

- Run default CPU simulation:
  ```sh
  ./slime.exe
  ```

- Run with GPU acceleration (binary must be built with GPU support):
  ```sh
  ./slime.exe --gpu
  ```

- Run with a custom map (image with obstacles):
  ```sh
  ./slime.exe --map maps/city_map.png --width 1024 --height 1024
  ```

- Combine GPU and map:
  ```sh
  ./slime.exe --gpu --map maps/maze.png
  ```

<p align="right">(<a href="#top">back to top</a>)</p>

## Map Feature

The map feature allows you to supply an external image or map file that defines obstacles, spawn points, and food/target locations.

- Obstacles: Dark pixels (near black) are treated as impassable terrain.
- Free space: Light pixels are traversable by agents.
- Marked points (optional): Colored pixels (for example bright red / green / blue) can be interpreted as special nodes such as start/goal locations or food sources. Check the source's map loader to confirm the exact color conventions.

Maps can be used to reproduce experiments (for testing real-world environments) or to design custom mazes and city layouts. Example map files are stored in the `maps/` directory if present.

<p align="right">(<a href="#top">back to top</a>)</p>

## GPU Acceleration

GPU acceleration speeds up the most expensive parts of the simulation (pheromone diffusion, decay, and agent updates) by running them on the GPU. This is especially helpful for large grid sizes or when using many agents.

- Backend options: The project can be built to use OpenCL or CUDA depending on your environment. Ensure you have the corresponding development headers and runtime installed.
- When running with `--gpu`, the program will attempt to initialize the GPU backend and fall back to CPU if initialization fails (if the binary includes a fallback).
- Performance notes: GPU acceleration gives the most benefit on large grid resolutions (e.g., 512x512 and above) and when the CPU single-threaded update becomes a bottleneck. For small maps the overhead of GPU setup may outweigh the benefit.

<p align="right">(<a href="#top">back to top</a>)</p>

## Contact

### Maintainer(s)

- [Pratyush Raj](https://github.com/mainpratyushhoon)

### Creator(s)

- [Pratyush Raj](https://github.com/mainpratyushhoon)

<p align="right">(<a href="#top">back to top</a>)</p>

## Additional documentation

- [License](/LICENSE)
- [Code of Conduct](/.github/CODE_OF_CONDUCT.md)
- [Security Policy](/.github/SECURITY.md)
- [Contribution Guidelines](/.github/CONTRIBUTING.md)

<!-- MARKDOWN LINKS & IMAGES -->

[contributors-shield]: https://img.shields.io/github/contributors/mainpratyushhoon/Slime-Mold-Implementation.svg?style=for-the-badge
[contributors-url]: https://github.com/mainpratyushhoon/Slime-Mold-Implementation/graphs/contributors
[forks-shield]: https://img.shields.io/github/forks/mainpratyushhoon/Slime-Mold-Implementation.svg?style=for-the-badge
[forks-url]: https://github.com/mainpratyushhoon/Slime-Mold-Implementation/network/members
[stars-shield]: https://img.shields.io/github/stars/mainpratyushhoon/Slime-Mold-Implementation.svg?style=for-the-badge
[stars-url]: https://github.com/mainpratyushhoon/Slime-Mold-Implementation/stargazers
[issues-shield]: https://img.shields.io/github/issues/mainpratyushhoon/Slime-Mold-Implementation.svg?style=for-the-badge
[issues-url]: https://github.com/mainpratyushhoon/Slime-Mold-Implementation/issues
[license-shield]: https://img.shields.io/github/license/mainpratyushhoon/Slime-Mold-Implementation.svg?style=for-the-badge
[license-url]: https://github.com/mainpratyushhoon/Slime-Mold-Implementation/blob/master/LICENSE
