# Procedural Grass Generation

A high-performance C++ engine built with **openFrameworks** for the real-time procedural generation of organic 3D plant life. The project utilizes advanced geometric algorithms to move beyond simple randomization, creating mathematically sound and visually stable 3D meshes.

## 📽 Demo Video
[![Watch the Demo](https://img.youtube.com/vi/sy8bFXakerA/maxresdefault.jpg)](https://www.youtube.com/watch?v=sy8bFXakerA)
*(Click the image to play the video on YouTube)*

## 🛠 Technical Highlights

### Parallel Transport Frames (Bishop Frame)
To solve the common "twisting" artifacts and gimbal lock issues associated with traditional Frenet-Serret frames, this project implements the **Parallel Transport Frame** algorithm.
* **Implementation**: Calculates local coordinate frames (Tangent, Normal, Binormal) by rotating the previous frame's normal into the current tangent via the shortest path.
* **Impact**: Enables the generation of smooth, non-self-intersecting tubular meshes for stems and branches regardless of the curve's complexity.

### Procedural Mesh Construction
Rather than using pre-made assets, every piece of geometry is generated at runtime:
* **Cubic Bézier Spines**: Uses 3-point and 4-point Bézier formulas to calculate smooth centerlines for organic growth patterns.
* **Dynamic Tapering**: Implements a linear radius interpolation from base to tip, creating realistic stem and branch thickness.
* **Memory Optimization**: Consolidates separate component meshes—stems, leaves, branches, and seeds—into a single optimized vertex buffer for efficient GPU rendering.

### Field Generation & Complexity
The system supports two distinct field generation modes:
1. **Static Instance Field**: Rapidly populates a scene by offsetting a single high-detail mesh.
2. **True Procedural Field**: Generates 1,000+ unique, randomized plant models on the fly, demonstrating the efficiency of the underlying generation logic.

## 🚀 Key Features
* **Seed-Based Randomization**: Uses a deterministic seed to control all procedural generation, ensuring reproducible results.
* **Real-time Performance**: Maintains a high frame rate (60 FPS) even with high-density field generation.
* **Mesh Export**: Supports exporting generated models to `.ply` format for further use in 3D suites like Blender or Maya.

## ⌨️ Controls
| Key | Action |
| :--- | :--- |
| **R** | Generate a new procedural plant model |
| **G** | Toggle "Single Model" field mode |
| **T** | Generate a field of 1,000+ unique procedural plants |
| **C** | Toggle between Solid and Wireframe view |
| **X** | Export current plant to `bin/data/grassSample.ply` |
| **A** | Toggle Axis display |

## 📦 Setup & Requirements
* **Environment**: Developed using Visual Studio 2022.
* **Framework**: Requires [openFrameworks v0.12.1+](https://openframeworks.cc/).

* **Language Standard**: C++11 or later.
