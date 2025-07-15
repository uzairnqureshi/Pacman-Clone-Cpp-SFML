# Pacman - SFML C++ Game

> **Note:** This project was created as a second semester Computer Science OOP (Object-Oriented Programming) project.

## Overview
This is a classic Pacman game implemented in C++ using the SFML 3.0 library. The game features a playable Pacman, four ghosts with basic AI, collectible pellets, sound effects, a menu system, and win/lose conditions.

---

## Features
- **Classic Pacman Gameplay**: Navigate the maze, eat pellets, avoid ghosts.
- **Ghost AI**: Four ghosts with simple pursuit and random movement logic.
- **Win/Lose Conditions**: Win by eating all pellets, lose by running out of lives.
- **Menu System**: Start, load, and manage the game from a menu.
- **Sound Effects**: Pellet, death, win, and game start sounds (SFML audio).
- **Game State Save/Load**: Save and resume your progress.
- **Sprite and Image Support**: Pacman and ghosts can use PNG images or fallback to colored shapes.
- **FPS Display**: Console output for frame rate monitoring.

---

## Main Files & Functions
- **main.cpp**: Entry point, main game loop, event handling, sound, and UI.
- **map.h / map.cpp**: Map layout, pellet logic, drawing, and win condition check.
- **ghosts.h / ghosts.cpp**: Ghost class, AI movement, collision, and drawing (supports textures or colored fallback).
- **Menu.h / Menu.cpp**: Menu class for game start, load, and navigation.
- **constants.h**: Game constants (cell size, map dimensions, etc.).

### Key Classes/Functions
- `Map`: Handles maze, pellet state, drawing, and win check.
- `Ghost`: Handles ghost position, movement, AI, and drawing.
- `Menu`: Handles menu navigation and rendering.
- `GameState`: Struct for saving/loading game state.
- `checkCollision`: Utility for collision detection.

---

## Techniques Used
- **SFML 3.0**: For graphics, windowing, and audio.
- **Sprite Sheet & Texture Support**: Easily swap between images and shapes.
- **Sound Integration**: Uses `sf::SoundBuffer` and `sf::Sound` for effects.
- **Optional & Fallbacks**: Uses `std::optional` for sprite fallback logic.
- **Game State Serialization**: Binary save/load for progress.
- **Simple AI**: Ghosts use a mix of pursuit and random movement.
- **Event-Driven Input**: Keyboard and window event handling.
- **Auto-Save**: Periodic game state saving.

---

## Limitations
- **No Animated Sprites**: Pacman and ghosts use static images or shapes (no animation frames).
- **Basic Ghost AI**: Ghosts do not use advanced pathfinding (no A* or true Pacman AI).
- **No Power Pellets**: No super mode or ghost-eating mechanics.
- **No Background Music**: Only sound effects are supported.
- **Fixed Map**: Only one maze layout is available.
- **No High Score Table**: Scores are not persisted between sessions.
- **No Mobile/Touch Support**: Keyboard only.

---

## Future Enhancements
- **Animated Sprites**: Add sprite sheet animation for Pacman and ghosts.
- **Power Pellets**: Implement super mode and ghost-eating.
- **Advanced Ghost AI**: Use more sophisticated algorithms for ghost movement.
- **Multiple Maps**: Add support for different maze layouts.
- **Background Music**: Add looping music and volume controls.
- **High Score System**: Save and display top scores.
- **Settings Menu**: Allow users to customize controls, volume, and graphics.
- **Pause/Resume**: Add pause functionality.
- **Touch/Controller Support**: Expand input options.

---

## Credits
- **Programming**: [Uzair Qureshi](https://www.github.com/uzairnqureshi)
- **SFML Library**: [SFML Team](https://www.sfml-dev.org)
- **Sound Effects**: [Pixabay.com](https://pixabay.com/sound-effects/search/pac-man/)

---

> This project is open source and available on GitHub. Contributions and suggestions are welcome! 
