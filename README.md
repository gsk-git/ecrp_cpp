# ESRO Game (ecrp_cpp)

**ESRO** is a C++-based game built using a modular “state machine” architecture. This repository (`ecrp_cpp`) holds the game engine code, state management, resources, and configuration needed to build and run the game.

---

## Table of Contents

- [About the Project](#about-the-project)  
- [Features](#features)  
- [Architecture](#architecture)  
- [Getting Started](#getting-started)  
  - [Prerequisites](#prerequisites)  
  - [Building Locally](#building-locally)  
  - [Running the Game](#running-the-game)  
- [Usage](#usage)  
- [Project Structure](#project-structure)  
- [Configuration](#configuration)  
- [Contributing](#contributing)  
- [Roadmap](#roadmap)  
- [License](#license)  
- [Acknowledgments](#acknowledgments)  

---

## About the Project

ESRO is a game developed in **C++**, designed around a flexible state machine. Each distinct “state” of the game (e.g., splash screen, main menu, pause, gameplay) is implemented as a separate module, making the code clean, maintainable, and extensible.

The goal of this project is to provide a simple but scalable game framework that can be extended for different kinds of games (2D, text-based, or even basic graphical ones). The codebase aims to be educational, modular, and easy to reason about.

---

## Features

- **State Management**: Uses states (e.g., `MainMenuState`, `SplashState`, `PauseState`) to switch between game screens cleanly.  
- **Config File**: A configuration module (`config.cpp` / `config.hpp`) allows easy setting of game parameters without recompiling core logic.  
- **Decoupled Game Logic**: Game logic is separated from UI or rendering logic, making it easier to extend or replace components.  
- **Cross-Platform Potential**: Though currently set up for a specific build environment, the architecture supports portability.  
- **Test Support**: Includes Google Test (via `vcpkg` packages) to write unit tests for game logic (if tests are added / maintained).

---

## Architecture

Here’s a high-level view of how the game is structured:

Main (main.cpp)
└── StateManager
├── IState (interface)
├── SplashState
├── MainMenuState
├── PauseState
└── OtherStates...
└── GameState (contains core game logic)
└── Config (config.cpp / config.hpp to hold configurable parameters)


- `StateManager` is responsible for switching between different states.
- Each state implements a common interface (`IState`) to ensure consistency.
- `GameState` handles the actual gameplay logic, independent of which state is active.
- `config.hpp` / `config.cpp` allows defining parameters like window size, game speed, file paths, etc.

---

## Getting Started

### Prerequisites

To build and run this project, you might need:

- A **C++17 (or above)** compatible compiler (GCC, Clang, MSVC, etc.)  
- **CMake** for building (if using cross-platform build system)  
- **vcpkg** (or another package manager) — currently used for dependencies like Google Test  
- A code editor or IDE (Visual Studio, VS Code, CLion, etc.)

---

### Building Locally

Here is a sample step-by-step to build the project:

1. **Clone the repository**  
     ```bash
     git clone https://github.com/gsk-git/ecrp_cpp.git
     cd ecrp_cpp
2. Example (assuming vcpkg is not yet bootstrapped)
     ```bash
     git clone https://github.com/microsoft/vcpkg.git
     ./vcpkg/bootstrap-vcpkg.sh
     ./vcpkg/vcpkg integrate install

3. Install Depndencies
     ./vcpkg/vcpkg install gtest
     SFML from webite
     Fastnoise2 from website



