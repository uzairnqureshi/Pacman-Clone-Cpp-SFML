#include "Map.h"
#include <SFML/Graphics/CircleShape.hpp>

Map::Map() {
    wall.setSize(Vector2f(CELL_SIZE, CELL_SIZE));
    wall.setFillColor(Color::Blue);

    mapSketch = {
        "########################################",
        "#......................................#",
        "#.######.########.#############.######.#",
        "#......................................#",
        "#.######.########.#############.######.#",
        "#......................................#",
        "#.######.########.#############.######.#",
        "#......................................#",
        "#.######.########.#############.######.#",
        "#......................................#",
        "#.######.########.#############.######.#",
        "#......................................#",
        "#.######.########.#############.######.#",
        "#......................................#",
        "#.######.########.#############.######.#",
        "#......................................#",
        "#.######.########.#############.######.#",
        "#......................................#",
        "#.######.########.#############.######.#",
        "#......................................#",
        "########################################"
    };

    pellet.setRadius(CELL_SIZE / 6.f);
    pellet.setFillColor(Color::Yellow);

    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            pelletEaten[y][x] = (mapSketch[y][x] != '.');
        }
    }
}

void Map::draw(RenderWindow& window) {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (mapSketch[y][x] == '#') {
                wall.setPosition(Vector2f(x * CELL_SIZE, y * CELL_SIZE));
                window.draw(wall);
            }
            else if (mapSketch[y][x] == '.' && !pelletEaten[y][x]) {
                pellet.setPosition(Vector2f(
                    x * CELL_SIZE + (CELL_SIZE / 2.f) - pellet.getRadius(),
                    y * CELL_SIZE + (CELL_SIZE / 2.f) - pellet.getRadius()
                ));
                window.draw(pellet);
            }
        }
    }
}

bool Map::isWall(int x, int y) const {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
        return true;
    return mapSketch[y][x] == '#';
}

bool Map::eatPellet(int x, int y) {
    if (x < 0 || x >= MAP_WIDTH || y < 0 || y >= MAP_HEIGHT)
        return false;
    if (mapSketch[y][x] == '.' && !pelletEaten[y][x]) {
        pelletEaten[y][x] = true;
        return true;
    }
    return false;
}

bool Map::areAllPelletsEaten() const {
    for (int y = 0; y < MAP_HEIGHT; y++) {
        for (int x = 0; x < MAP_WIDTH; x++) {
            if (mapSketch[y][x] == '.' && !pelletEaten[y][x]) {
                return false;
            }
        }
    }
    return true;
}
