#pragma once

#include <array>
#include <string>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include "constants.h"

using namespace sf;
using namespace std;

class Map {
public:
    Map();
    void draw(RenderWindow& window);
    bool isWall(int x, int y) const;
    bool eatPellet(int x, int y);
    bool areAllPelletsEaten() const;

    const array<array<bool, MAP_WIDTH>, MAP_HEIGHT>& getPelletEaten() const { return pelletEaten; }
    void setPelletEaten(const array<array<bool, MAP_WIDTH>, MAP_HEIGHT>& newPelletEaten) { pelletEaten = newPelletEaten; }

private:
    array<string, MAP_HEIGHT> mapSketch;
    RectangleShape wall;
    array<array<bool, MAP_WIDTH>, MAP_HEIGHT> pelletEaten{};
    CircleShape pellet;

};
