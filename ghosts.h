#pragma once

#include <SFML/Graphics.hpp>
#include <random>
#include <vector>
#include <optional>
#include "constants.h"
#include "Map.h"

using namespace sf;

class Ghost {
public:
    Ghost(const Vector2f& startPos, const Color& color, const string& texturePath = "");
    void update(const Vector2f& pacmanPos, const Map& map, const vector<Ghost*>& otherGhosts);
    void draw(RenderWindow& window);
    const Vector2f& getPosition() const { return position; }
    FloatRect getBounds() const {
        if (sprite.has_value()) {
            return sprite->getGlobalBounds();
        }
        else {
            return fallbackShape.getGlobalBounds();
        }
    }
    void setPosition(const Vector2f& newPos) { position = newPos; }
    void reset();

private:
    optional<Sprite> sprite;
    Texture texture;
    Vector2f position;
    Vector2f direction;
    Color ghostColor;
    float speed;
    float moveTimer;
    const float moveDelay = 0.2f;
    Clock moveCooldown;
    mt19937 rng;
    bool textureLoaded;
    CircleShape fallbackShape; 

    Vector2f chooseNewDirection(const Vector2f& pacmanPos, const Map& map);
    bool isValidMove(const Vector2f& newPos, const Map& map) const;
    bool wouldCollideWithGhosts(const Vector2f& newPos, const vector<Ghost*>& otherGhosts) const;
};