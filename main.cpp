#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFML/Graphics/CircleShape.hpp>
#include <SFML/Graphics/Font.hpp>
#include <optional>
#include <array>
#include <vector>
#include <cmath>
#include <random>
#include <chrono>
#include <thread>
#include <fstream>
#include <filesystem>
#include <SFML/Audio.hpp>

using namespace sf;
using namespace std;
namespace fs = filesystem;

#include "constants.h"
#include "Map.h"
#include "Ghosts.h"
#include "Menu.h"

bool checkCollision(const Vector2f& pos1, const Vector2f& pos2, float radius) {
    float dx = pos1.x - pos2.x;
    float dy = pos1.y - pos2.y;
    float distance = sqrt(dx * dx + dy * dy);
    return distance < radius * 2;
}

struct GameState {
    int score;
    int lives;
    Vector2f pacmanPosition;
    array<Vector2f, 4> ghostPositions;
    array<array<bool, MAP_WIDTH>, MAP_HEIGHT> pelletEaten;
    bool gameWon;
};

bool saveGameState(const GameState& state) {
    try {
        cout << "Attempting to save game state..." << endl;

        ofstream saveFile("pacman_save.dat", ios::binary);
        if (!saveFile) {
            cerr << "Error: Could not open save file for writing!" << endl;
            return false;
        }

        saveFile.write(reinterpret_cast<const char*>(&state.score), sizeof(state.score));
        saveFile.write(reinterpret_cast<const char*>(&state.lives), sizeof(state.lives));
        saveFile.write(reinterpret_cast<const char*>(&state.gameWon), sizeof(state.gameWon));

        saveFile.write(reinterpret_cast<const char*>(&state.pacmanPosition), sizeof(state.pacmanPosition));

        for (const auto& pos : state.ghostPositions) {
            saveFile.write(reinterpret_cast<const char*>(&pos), sizeof(pos));
        }

        for (const auto& row : state.pelletEaten) {
            saveFile.write(reinterpret_cast<const char*>(row.data()), row.size() * sizeof(bool));
        }

        saveFile.close();
        cout << "Game state saved successfully!" << endl;
        return true;
    }
    catch (const exception& e) {
        cerr << "Error saving game: " << e.what() << endl;
        return false;
    }
}

bool loadGameState(GameState& state) {
    try {
        cout << "Attempting to load game state..." << endl;

        if (!fs::exists("pacman_save.dat")) {
            cerr << "No save file found!" << endl;
            return false;
        }

        ifstream saveFile("pacman_save.dat", ios::binary);
        if (!saveFile) {
            cerr << "Error: Could not open save file for reading!" << endl;
            return false;
        }

        saveFile.read(reinterpret_cast<char*>(&state.score), sizeof(state.score));
        saveFile.read(reinterpret_cast<char*>(&state.lives), sizeof(state.lives));
        saveFile.read(reinterpret_cast<char*>(&state.gameWon), sizeof(state.gameWon));

        saveFile.read(reinterpret_cast<char*>(&state.pacmanPosition), sizeof(state.pacmanPosition));

        for (auto& pos : state.ghostPositions) {
            saveFile.read(reinterpret_cast<char*>(&pos), sizeof(pos));
        }

        for (auto& row : state.pelletEaten) {
            saveFile.read(reinterpret_cast<char*>(row.data()), row.size() * sizeof(bool));
        }

        saveFile.close();
        cout << "Game state loaded successfully!" << endl;
        cout << "Loaded score: " << state.score << endl;
        cout << "Loaded lives: " << state.lives << endl;
        cout << "Loaded Pacman position: (" << state.pacmanPosition.x << ", " << state.pacmanPosition.y << ")" << endl;
        return true;
    }
    catch (const exception& e) {
        cerr << "Error loading game: " << e.what() << endl;
        return false;
    }
}

int main() {
    Map map;
    int score = 0;
    int lives = 3; 
    bool gameOver = false;
    bool gameWon = false;

    RenderWindow window(VideoMode({ CELL_SIZE * MAP_WIDTH, CELL_SIZE * MAP_HEIGHT }), "Pac-Man - Let's Go!");
    window.setFramerateLimit(60);

    SoundBuffer pelletBuffer, deathBuffer, winBuffer, startBuffer;
    if (!pelletBuffer.loadFromFile("C:/Users/UrduTechZone/source/repos/PacManForReal/Audio/pellet.wav")) { std::cerr << "Failed to load pellet.wav\n"; }
    if (!deathBuffer.loadFromFile("C:/Users/UrduTechZone/source/repos/PacManForReal/Audio/death.wav")) { std::cerr << "Failed to load death.wav\n"; }
    if (!winBuffer.loadFromFile("C:/Users/UrduTechZone/source/repos/PacManForReal/Audio/win.wav")) { std::cerr << "Failed to load win.wav\n"; }
    if (!startBuffer.loadFromFile("C:/Users/UrduTechZone/source/repos/PacManForReal/Audio/start.wav")) { std::cerr << "Failed to load start.ogg\n"; }
    Sound pelletSound(pelletBuffer);
    Sound deathSound(deathBuffer);
    Sound winSound(winBuffer);
    Sound startSound(startBuffer);

    startSound.play();

    Menu menu;

    bool gameStarted = false;
    bool loadGame = false;

    Clock clock;
    float deltaTime = 0.f;

    Texture pacmanTexture;
    if (!pacmanTexture.loadFromFile("C:/Users/UrduTechZone/source/repos/PacManForReal/Images/pacman.png")) {
        cerr << "Error: Could not load pacman.png\n";
        return -1;
    }

    Sprite pacmanSprite(pacmanTexture);
    pacmanSprite.setScale(Vector2f(
        static_cast<float>(CELL_SIZE) / pacmanTexture.getSize().x,
        static_cast<float>(CELL_SIZE) / pacmanTexture.getSize().y
    ));
    pacmanSprite.setPosition(Vector2f(1 * CELL_SIZE, 1 * CELL_SIZE));

    array<Ghost, 4> ghosts = {
        Ghost(Vector2f(19 * CELL_SIZE, 10 * CELL_SIZE), Color::Red, "C:/Users/UrduTechZone/source/repos/PacManForReal/Images/red_ghost.png"),    // Red ghost
        Ghost(Vector2f(20 * CELL_SIZE, 10 * CELL_SIZE), Color::White, "C:/Users/UrduTechZone/source/repos/PacManForReal/Images/pink_ghost.png"),  // Pink ghost
        Ghost(Vector2f(21 * CELL_SIZE, 10 * CELL_SIZE), Color::Cyan, "C:/Users/UrduTechZone/source/repos/PacManForReal/Images/blue_ghost.png"),   // Blue ghost
        Ghost(Vector2f(22 * CELL_SIZE, 10 * CELL_SIZE), Color::Yellow, "C:/Users/UrduTechZone/source/repos/PacManForReal/Images/orange_ghost.png")  // Orange ghost
    };

    vector<Ghost*> ghostPtrs;
    for (auto& ghost : ghosts) {
        ghostPtrs.push_back(&ghost);
    }

    Font font;
    if (!font.openFromFile("C:/Windows/Fonts/ariblk.ttf")) {
        cerr << "Error: Could not load font!" << endl;
        return -1;
    }

    Text scoreText(font);
    scoreText.setString("Score: 0");
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(Color::White);
    scoreText.setPosition(Vector2f(10.f, 10.f));

    Text livesText(font);
    livesText.setString("Lives: 3");
    livesText.setCharacterSize(24);
    livesText.setFillColor(Color::White);
    livesText.setPosition(Vector2f(10.f, 40.f));

    Text gameOverText(font);
    gameOverText.setString("GAME OVER");
    gameOverText.setCharacterSize(48);
    gameOverText.setFillColor(Color::Red);

    Text winText(font);
    winText.setString("CONGRATULATIONS!\nYOU WIN!");
    winText.setCharacterSize(48);
    winText.setFillColor(Color::Green);

    int frameCount = 0;
    float fpsUpdateTime = 0.f;

    gameOverText.setOrigin(Vector2f(200.f, 24.f));
    gameOverText.setPosition(Vector2f(CELL_SIZE * MAP_WIDTH / 2.f, CELL_SIZE * MAP_HEIGHT / 2.f));

    winText.setOrigin(Vector2f(200.f, 24.f));  
    winText.setPosition(Vector2f(CELL_SIZE * MAP_WIDTH / 2.f, CELL_SIZE * MAP_HEIGHT / 2.f));

    Clock moveCooldown;
    const float moveDelay = 0.15f; 
    const float speed = CELL_SIZE;
    const float collisionRadius = CELL_SIZE / 2.f; 

    Vector2f currentDirection(0.f, 0.f);
    Vector2f startPosition(1 * CELL_SIZE, 1 * CELL_SIZE);

    Clock autoSaveTimer;
    const float autoSaveInterval = 60.f; 

    while (window.isOpen()) {
        while (const optional<Event> event = window.pollEvent()) {
            if (event->is<Event::Closed>()) {
                GameState saveState;
                saveState.score = score;
                saveState.lives = lives;
                saveState.gameWon = gameWon;
                saveState.pacmanPosition = pacmanSprite.getPosition();
                for (size_t i = 0; i < ghosts.size(); ++i) {
                    saveState.ghostPositions[i] = ghosts[i].getPosition();
                }
                saveState.pelletEaten = map.getPelletEaten();
                saveGameState(saveState);
                window.close();
            }
        }

        deltaTime = clock.restart().asSeconds();

        menu.handleInput(window);

        window.clear(Color::Black);

        if (menu.shouldLoadGame()) {
            cout << "Loading game..." << endl;
            GameState loadState;
            if (loadGameState(loadState)) {
                cout << "Game loaded successfully, applying state..." << endl;

                gameOver = false;
                gameWon = loadState.gameWon;
                score = loadState.score;
                lives = loadState.lives;

                pacmanSprite.setPosition(loadState.pacmanPosition);
                currentDirection = Vector2f(0.f, 0.f);
                moveCooldown.restart();

                for (size_t i = 0; i < ghosts.size(); ++i) {
                    ghosts[i].setPosition(loadState.ghostPositions[i]);
                    ghosts[i].reset();
                }

                map.setPelletEaten(loadState.pelletEaten);

                scoreText.setString("Score: " + to_string(score));
                livesText.setString("Lives: " + to_string(lives));

                autoSaveTimer.restart();
                clock.restart();  

                cout << "Game state applied successfully!" << endl;
            }
            else {
                cout << "Failed to load game, starting new game instead..." << endl;
                gameOver = false;
                gameWon = false;
                score = 0;
                lives = 3;

                pacmanSprite.setPosition(startPosition);
                currentDirection = Vector2f(0.f, 0.f);
                moveCooldown.restart();

                ghosts[0].setPosition(Vector2f(19 * CELL_SIZE, 10 * CELL_SIZE));
                ghosts[1].setPosition(Vector2f(20 * CELL_SIZE, 10 * CELL_SIZE));
                ghosts[2].setPosition(Vector2f(21 * CELL_SIZE, 10 * CELL_SIZE));
                ghosts[3].setPosition(Vector2f(22 * CELL_SIZE, 10 * CELL_SIZE));

                for (auto& ghost : ghosts) {
                    ghost.reset();
                }

                map.setPelletEaten(array<array<bool, MAP_WIDTH>, MAP_HEIGHT>{});

                scoreText.setString("Score: 0");
                livesText.setString("Lives: 3");

                autoSaveTimer.restart();
                clock.restart();  
            }
            menu.clearLoadFlag(); 
        }

        if (!menu.isGameStarted()) {
            menu.draw(window);
            window.display();
            continue;
        }

        if (!gameOver && !gameWon) {
            if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
                currentDirection = Vector2f(0.f, -1.f);
            }
            else if (Keyboard::isKeyPressed(Keyboard::Key::Down)) {
                currentDirection = Vector2f(0.f, 1.f);
            }
            else if (Keyboard::isKeyPressed(Keyboard::Key::Left)) {
                currentDirection = Vector2f(-1.f, 0.f);
            }
            else if (Keyboard::isKeyPressed(Keyboard::Key::Right)) {
                currentDirection = Vector2f(1.f, 0.f);
            }

            if (moveCooldown.getElapsedTime().asSeconds() >= moveDelay) {
                Vector2f currentPos = pacmanSprite.getPosition();
                int gridX = static_cast<int>(currentPos.x) / CELL_SIZE;
                int gridY = static_cast<int>(currentPos.y) / CELL_SIZE;

                int nextGridX = gridX + static_cast<int>(currentDirection.x);
                int nextGridY = gridY + static_cast<int>(currentDirection.y);

                if (!map.isWall(nextGridX, nextGridY)) {
                    pacmanSprite.move(Vector2f(currentDirection.x * speed, currentDirection.y * speed));
                    if (map.eatPellet(nextGridX, nextGridY)) {
                        score += 10;
                        scoreText.setString("Score: " + to_string(score));
                        pelletSound.play(); 
                    }
                    moveCooldown.restart();
                }
                else {
                    currentDirection = Vector2f(0.f, 0.f);
                }
            }

            for (auto& ghost : ghosts) {
                ghost.update(pacmanSprite.getPosition(), map, ghostPtrs);
            }

            Vector2f pacmanPos = pacmanSprite.getPosition() + Vector2f(CELL_SIZE / 2.f, CELL_SIZE / 2.f);
            for (const auto& ghost : ghosts) {
                Vector2f ghostPos = ghost.getPosition() + Vector2f(CELL_SIZE / 2.f, CELL_SIZE / 2.f);
                if (checkCollision(pacmanPos, ghostPos, collisionRadius)) {
                    lives--;
                    livesText.setString("Lives: " + to_string(lives));
                    deathSound.play();
                    if (lives <= 0) {
                        gameOver = true;
                    }
                    else {
                        pacmanSprite.setPosition(startPosition);
                        currentDirection = Vector2f(0.f, 0.f);
                        ghosts[0].setPosition(Vector2f(19 * CELL_SIZE, 10 * CELL_SIZE));
                        ghosts[1].setPosition(Vector2f(20 * CELL_SIZE, 10 * CELL_SIZE));
                        ghosts[2].setPosition(Vector2f(21 * CELL_SIZE, 10 * CELL_SIZE));
                        ghosts[3].setPosition(Vector2f(22 * CELL_SIZE, 10 * CELL_SIZE));
                        for (auto& ghost : ghosts) {
                            ghost.reset();
                        }
                    }
                    break;
                }
            }

            if (map.areAllPelletsEaten()) {
                gameWon = true;
                winSound.play();
            }

            if (autoSaveTimer.getElapsedTime().asSeconds() >= autoSaveInterval) {
                cout << "Auto-saving game..." << endl;
                GameState saveState;
                saveState.score = score;
                saveState.lives = lives;
                saveState.gameWon = gameWon;
                saveState.pacmanPosition = pacmanSprite.getPosition();
                for (size_t i = 0; i < ghosts.size(); ++i) {
                    saveState.ghostPositions[i] = ghosts[i].getPosition();
                }
                saveState.pelletEaten = map.getPelletEaten();
                if (saveGameState(saveState)) {
                    cout << "Auto-save completed successfully!" << endl;
                }
                else {
                    cout << "Auto-save failed!" << endl;
                }
                autoSaveTimer.restart();
            }
        }

        map.draw(window);
        window.draw(pacmanSprite);

        for (auto& ghost : ghosts) {
            ghost.draw(window);
        }

        window.draw(scoreText);
        window.draw(livesText);

        frameCount++;
        fpsUpdateTime += deltaTime;
        if (fpsUpdateTime >= 1.0f) {
            float currentFPS = frameCount / fpsUpdateTime;
            cout << "FPS: " << static_cast<int>(currentFPS) << endl;
            frameCount = 0;
            fpsUpdateTime = 0.f;
        }

        if (gameOver) {
            window.draw(gameOverText);
        }
        else if (gameWon) {
            window.draw(winText);
        }

        window.display();
    }

    return 0;
}
