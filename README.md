<div id="top"></div>

<!-- PROJECT SHIELDS -->
<!-- https://www.markdownguide.org/basic-syntax/#reference-style-links-->
<div align="center">

[![Contributors][contributors-shield]][contributors-url]
[![Forks][forks-shield]][forks-url]
[![Stargazers][stars-shield]][stars-url]
[![Issues][issues-shield]][issues-url]
[![MIT License][license-shield]][license-url]

</div>

<!-- PROJECT LOGO -->
<br />
<!-- UPDATE -->
<div align="center">
  <a href="https://github.com/mainpratyushhoon/Slime-Mold-Implementation">
     <img width="340" alt="image" src="https://upload.wikimedia.org/wikipedia/commons/thumb/c/c9/Physarum_polycephalum_strain_LU352_-_4.jpg/1280px-Physarum_polycephalum_strain_LU352_-_4.jpg">
  </a>

  <h3 align="center">SLIME MOLD SIMULATION</h3>

  <p align="center">
  <!-- UPDATE -->
    <i>Applicaton of Swarm Intelligence</i>
  </p>
</div>


<!-- TABLE OF CONTENTS -->
<details>
<summary>Table of Contents</summary>

- [About The Project](#about-the-project)
- [Getting Started](#getting-started)
  - [Prerequisites](#prerequisites)
  - [Installation](#installation)
- [Usage](#usage)
- [Contact](#contact)
  - [Maintainer(s)](#maintainers)
  - [creators(s)](#creators)
- [Additional documentation](#additional-documentation)

</details>


<!-- ABOUT THE PROJECT -->
## About The Project
<!-- UPDATE -->
<div align="center">
  <a href="https://github.com/cgs-iitkgp/PROJECT_NAME">
    <img width="50%" alt="image" src="https://github.com/mainpratyushhoon/Slime-Mold-Implementation/blob/main/working.png">
  </a>
</div>
<br>
This project simulates the behavior of slime mold (Physarum polycephalum) to approximate efficient transport networks between randomly distributed points — similar to how slime mold finds optimal paths in nature. The simulation also allows users to draw custom mazes that the slime mold must adapt to while finding efficient routes, creating a dynamic, interactive exploration of bio-inspired pathfinding.
<br><br>
The goal of this project is to explore how biological systems can solve complex optimization problems. Slime mold naturally forms minimal, efficient, and robust transport networks, making it an excellent model for approximating shortest path or minimal spanning structures.<br>
<br>
This simulation replicates that behavior and compares the results with a brute-force approach that exhaustively computes all possible paths (with O(n!) time complexity). You can also draw obstacles or maze-like environments, and the simulated slime mold dynamically adapts — rerouting itself around barriers while maintaining efficient connectivity between nodes.

<p align="right">(<a href="#top">back to top</a>)</p>

## Getting Started

To set up a local instance of the application, follow the steps below.

### Prerequisites
The following dependencies are required to be installed for the project to function properly:
<!-- UPDATE -->
* OpenGL<br>
  For this particular project, we are using FreeGLUT. The pre-compiled version of OpenGL works just fine.

<p align="right">(<a href="#top">back to top</a>)</p>

### Installation

_Now that the environment has been set up and configured to properly compile and run the project, the next step is to install and configure the project locally on your system._
<!-- UPDATE -->
1. Clone the repository
   ```sh
   git clone https://github.com/mainpratyushhoon/Slime-Mold-Implementation.git
   ```
2. Compile the code
   ```sh
   g++ SlimeMain.cpp -lfreeglut -lglu32 -lopengl32 -o slime.exe
   ```
   
3. Execute the script
   ```sh
   ./slime.exe
   ```

<p align="right">(<a href="#top">back to top</a>)</p>


<!-- USAGE EXAMPLES -->
## Usage
<!-- UPDATE -->
To use the project, first run SlimeMold.cpp to start the main simulation. This file simulates the slime mold behavior that forms efficient networks between randomly generated points. Once running, you can interact with the simulation by drawing obstacles or maze patterns directly on the screen, and the slime mold will dynamically adapt its paths around them while maintaining connectivity. To evaluate performance, you can separately run bruteforce.py, which computes the exact optimal network using a recursive brute-force algorithm. It reports the total number of recursive calls and the time take. The bruteforce_plot.py plots how computation time increases as the number of points grows (from n = 3 to n = 10). Comparing the results from both files demonstrates how the slime mold model efficiently approximates near-optimal paths while avoiding the exponential complexity of brute-force search.

<div align="center">
  <a href="https://github.com/mainpratyushhoon/Slime-Mold-Implementation">
    <img width="50%" alt="image" src="https://github.com/mainpratyushhoon/Slime-Mold-Implementation/blob/main/maze.png">
  </a>
</div>

<p align="right">(<a href="#top">back to top</a>)</p>

## Contact

### Maintainer(s)

The currently active maintainer(s) of this project.

<!-- UPDATE -->
- [Pratyush Raj](https://github.com/mainpratyushhoon)

### Creator(s)

Honoring the original creator(s) and ideator(s) of this project.

<!-- UPDATE -->
- [Pratyush Raj](https://github.com/mainpratyushhoon)

<p align="right">(<a href="#top">back to top</a>)</p>

## Additional documentation

  - [License](/LICENSE)
  - [Code of Conduct](/.github/CODE_OF_CONDUCT.md)
  - [Security Policy](/.github/SECURITY.md)
  - [Contribution Guidelines](/.github/CONTRIBUTING.md)

<p align="right">(<a href="#top">back to top</a>)</p>

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