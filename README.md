![Game screenshot](res/esro1.png)
# ESRO Game (ecrp_cpp)

**ESRO** is a C++-based game built using a modular “state machine” architecture. This repository (`ecrp_cpp`) holds the game engine code, state management, resources, and configuration needed to build and run the game.

---

## Table of Contents

- [About the Project](#about-the-project)  
- [Features](#features)

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
