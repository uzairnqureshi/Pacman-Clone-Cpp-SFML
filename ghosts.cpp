#include "Ghosts.h"
#include <random>
#include <chrono>
#include <iostream>

Ghost::Ghost(const Vector2f& startPos, const Color& color, const string& texturePath)
    : position(startPos), direction(0.f, 0.f), ghostColor(color), speed(CELL_SIZE), moveTimer(0.f), textureLoaded(false) {

    fallbackShape.setRadius(CELL_SIZE / 2.f);
    fallbackShape.setFillColor(color);
    fallbackShape.setPosition(position);

    if (!texturePath.empty()) {
        if (texture.loadFromFile(texturePath)) {
            sprite.emplace(texture); 
            sprite->setScale(Vector2f(
                static_cast<float>(CELL_SIZE) / texture.getSize().x,
                static_cast<float>(CELL_SIZE) / texture.getSize().y
            ));
            sprite->setPosition(position);
            textureLoaded = true;
        }
        else {
            cerr << "Warning: Could not load ghost texture from " << texturePath << ". Using colored shape instead." << endl;
        }
    }

    unsigned seed = chrono::system_clock::now().time_since_epoch().count();
    rng.seed(seed);
}

void Ghost::reset() {
    direction = Vector2f(0.f, 0.f);
    moveTimer = 0.f;
    moveCooldown.restart();
    if (sprite.has_value()) {
        sprite->setPosition(position);
    }
}

void Ghost::update(const Vector2f& pacmanPos, const Map& map, const vector<Ghost*>& otherGhosts) {
    if (moveCooldown.getElapsedTime().asSeconds() >= moveDelay) {
        Vector2f newDirection = chooseNewDirection(pacmanPos, map);
        Vector2f nextPos = position + Vector2f(newDirection.x * CELL_SIZE, newDirection.y * CELL_SIZE);

        if (isValidMove(nextPos, map) && !wouldCollideWithGhosts(nextPos, otherGhosts)) {
            position = nextPos;
            direction = newDirection;
        }

        moveCooldown.restart();
    }
}

void Ghost::draw(RenderWindow& window) {
    if (textureLoaded && sprite.has_value()) {
        sprite->setPosition(position);
        window.draw(*sprite);
    }
    else {
        fallbackShape.setPosition(position);
        window.draw(fallbackShape);
    }
}

bool Ghost::wouldCollideWithGhosts(const Vector2f& newPos, const vector<Ghost*>& otherGhosts) const {
    float ghostSize = CELL_SIZE;

    for (const auto& otherGhost : otherGhosts) {
        if (otherGhost != this) {
            Vector2f otherPos = otherGhost->getPosition();

            float dx = newPos.x - otherPos.x;
            float dy = newPos.y - otherPos.y;
            float distance = sqrt(dx * dx + dy * dy);

            if (distance < ghostSize) {
                return true;
            }
        }
    }
    return false;
}

Vector2f Ghost::chooseNewDirection(const Vector2f& pacmanPos, const Map& map) {
    Vector2f directionToPacman = pacmanPos - position;

    float length = sqrt(directionToPacman.x * directionToPacman.x +
        directionToPacman.y * directionToPacman.y);
    if (length > 0) {
        directionToPacman.x /= length;
        directionToPacman.y /= length;
    }

    uniform_real_distribution<float> dist(0.f, 1.f);
    if (dist(rng) < 0.7f) {
        Vector2f possibleDirections[] = {
            Vector2f(1.f, 0.f),   // Right
            Vector2f(-1.f, 0.f),  // Left
            Vector2f(0.f, 1.f),   // Down
            Vector2f(0.f, -1.f)   // Up
        };

        vector<Vector2f> validDirections;
        for (const auto& dir : possibleDirections) {
            Vector2f nextPos = position + Vector2f(dir.x * CELL_SIZE, dir.y * CELL_SIZE);
            if (isValidMove(nextPos, map)) {
                validDirections.push_back(dir);
            }
        }

        if (!validDirections.empty()) {
            Vector2f bestDirection = validDirections[0];
            float bestDot = -1.f;

            for (const auto& dir : validDirections) {
                float dot = dir.x * directionToPacman.x + dir.y * directionToPacman.y;
                if (dot > bestDot) {
                    bestDot = dot;
                    bestDirection = dir;
                }
            }

            return bestDirection;
        }
    }

    Vector2f possibleDirections[] = {
        Vector2f(1.f, 0.f),   // Right
        Vector2f(-1.f, 0.f),  // Left
        Vector2f(0.f, 1.f),   // Down
        Vector2f(0.f, -1.f)   // Up
    };

    shuffle(begin(possibleDirections), end(possibleDirections), rng);

    for (const auto& dir : possibleDirections) {
        Vector2f nextPos = position + Vector2f(dir.x * CELL_SIZE, dir.y * CELL_SIZE);
        if (isValidMove(nextPos, map)) {
            return dir;
        }
    }

    return Vector2f(0.f, 0.f);
}

bool Ghost::isValidMove(const Vector2f& newPos, const Map& map) const {
    int gridX = static_cast<int>(newPos.x) / CELL_SIZE;
    int gridY = static_cast<int>(newPos.y) / CELL_SIZE;
    return !map.isWall(gridX, gridY);
}