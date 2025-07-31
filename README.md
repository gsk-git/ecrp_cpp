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

## 📸 Screenshots

> *Coming soon: Add sample in-game screenshots here*

---

## 🧠 Design Concepts

- **std::optional for Player**: Player is only created when the game starts — uses `std::optional` to avoid dangling references.
- **Lazy chunk loading**: Generates chunks *only when needed*, based on player movement direction.
- **SFML VertexArray**: Minimizes draw calls by batching tile rendering.
- **Config-driven world**: Game behavior is driven by settings in a centralized config file.

---

## 🤝 Contributing

Pull requests are welcome!  
If you want to suggest a feature, open an issue or fork the project and create a PR.

**To contribute:**

1. Fork the repo
2. Create your feature branch (`git checkout -b feature/my-feature`)
3. Commit your changes (`git commit -am 'Add some feature'`)
4. Push to the branch (`git push origin feature/my-feature`)
5. Create a pull request

---

## 📜 License

This project is licensed under the GPL 3.0 License.

---

## 🙌 Acknowledgments

- [SFML](https://www.sfml-dev.org/)
- [Ken Perlin](https://mrl.cs.nyu.edu/~perlin/) for the noise algorithm
- Open-source game developers for inspiration and ideas

---

## 📌 Future Plans

- [ ] Add NPCs / Entities
- [ ] Implement basic AI
- [ ] World saving/loading
- [ ] Collision and physics
- [ ] UI and inventory system
- [ ] Sound and music support


