# 🕹️ Tile World Game (C++ / SFML)

Welcome to the **Tile World Game**, a procedurally generated 2D tile-based world built using **C++** and **SFML**. Inspired by sandbox games like Minecraft, this project demonstrates chunk-based world generation, Perlin noise terrain, dynamic camera systems, and modular architecture — all from the ground up.

---

## 📦 Features

- 🧱 **Chunk-Based World System**  
  World is divided into chunks to allow efficient loading/unloading of large tile maps.

- 🌍 **Procedural Terrain Generation**  
  Terrain is generated using **Perlin noise**, enabling infinite and natural-looking landscapes.

- 🎥 **Camera System**  
  Smooth camera that follows the player and handles world-space to screen-space transformations.

- 🧭 **Directional Chunk Generation**  
  Chunks are generated only in the direction the player is moving, to improve performance and reduce lag.

- 🎨 **Textured Tile Rendering**  
  Optimized tile rendering using **`sf::VertexArray`** for fast GPU batching.

- 👾 **Player Movement & Control**  
  Player character moves across the world grid using keyboard input.

- ⚙️ **Modular Configuration**  
  Game constants and settings are cleanly organized in `config.hpp`/`config.cpp`.

---

## 🚀 Getting Started

### ✅ Prerequisites

- **C++17 or newer**
- [SFML 3.0+](https://www.sfml-dev.org/download.php)
- A C++ compiler (MSVC, g++, clang++)
- Visual Studio or any modern C++ IDE

### 📦 Building the Project

#### Option 1: Visual Studio

1. Open the `.sln` file in Visual Studio.
2. Ensure SFML is correctly linked via project settings.
3. Build and run (`Ctrl + F5`).

#### Option 2: Command Line (CMake example)

> *Coming soon*

---

## 📁 Project Structure

