# Arkanoid (Breakout Clone)

This project is a multi-level Arkanoid-style game developed in C++ using the SFML library.

## Features

* Multi-level system (dynamically loaded from .txt files)
* Smooth paddle and ball mechanics
* Brick breaking system with memory management
* Power-ups (Expand paddle, Multi-ball, Speed adjustments)
* Lives system and real-time score tracking
* Level transition, Game Over, and Victory screens

## Project Structure

```text
├── assets/
│   ├── fonts/          # UI typography
│   ├── levels/         # .txt matrix files for level design
│   └── textures/       # Game sprites (paddle, ball, bricks)
├── src/
│   ├── main.cpp        # Game loop and state management
│   ├── level.cpp       # File I/O and level parsing
│   ├── powerup.cpp     # Power-up spawning and logic
│   ├── level.hpp
│   ├── powerup.hpp
│   └── definitions.hpp
├── Makefile            # Build automation script
└── README.md


## Technologies

* C++
* SFML

## Progress

* [ ] Window creation
* [ ] Paddle movement
* [ ] Ball mechanics
* [ ] Collision system
* [ ] Bricks
* [ ] Level system
* [ ] Power-ups
* [ ] UI

## Build and Run

mingw32-make
mingw32-make run

## Author

Yaren Duman
