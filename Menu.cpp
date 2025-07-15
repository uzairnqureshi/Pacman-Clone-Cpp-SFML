#include "Menu.h"
#include "constants.h"
#include <cstdint>
#include <iostream>
#include <filesystem>

using namespace std;
using namespace sf;

Menu::Menu() : selectedItem(0), gameStarted(false), loadGame(false),
fadeComplete(true), fadeAlpha(0.f), fadeSpeed(2.f), fadeInProgress(false) {
    if (!font.openFromFile("C:/Windows/Fonts/ariblk.ttf")) {
        throw runtime_error("Could not load font");
    }

    startNewGameText.setFillColor(Color::White);
    resumeGameText.setFillColor(Color::White);
    exitText.setFillColor(Color::White);
    fadeText.setFillColor(Color(255, 255, 255, 0));
    fadeText.setOrigin(Vector2f(200.f, 24.f));
    fadeText.setPosition(Vector2f(CELL_SIZE * MAP_WIDTH / 2.f, CELL_SIZE * MAP_HEIGHT / 2.f));
}

Menu::~Menu() {
    
}

void Menu::resetState() {
    gameStarted = false;
    loadGame = false;
    fadeInProgress = false;
    fadeComplete = true;
    fadeAlpha = 0.f;
    selectedItem = 0;
}

void Menu::draw(RenderWindow& window) {
    if (fadeInProgress) {
        updateFadeEffect();
        window.draw(fadeText);
        return;
    }

    float startY = CELL_SIZE * MAP_HEIGHT * 0.3f;

    startNewGameText.setOrigin(Vector2f(100.f, 20.f));
    startNewGameText.setPosition(Vector2f(CELL_SIZE * MAP_WIDTH / 2.f, startY));
    startNewGameText.setFillColor(selectedItem == 0 ? Color::Yellow : Color::White);
    window.draw(startNewGameText);

    resumeGameText.setOrigin(Vector2f(100.f, 20.f));
    resumeGameText.setPosition(Vector2f(CELL_SIZE * MAP_WIDTH / 2.f, startY + 60.f));
    resumeGameText.setFillColor(selectedItem == 1 ? Color::Yellow : Color::White);
    window.draw(resumeGameText);

    exitText.setOrigin(Vector2f(100.f, 20.f));
    exitText.setPosition(Vector2f(CELL_SIZE * MAP_WIDTH / 2.f, startY + 120.f));
    exitText.setFillColor(selectedItem == 2 ? Color::Yellow : Color::White);
    window.draw(exitText);
}

void Menu::handleInput(RenderWindow& window) {
    if (fadeInProgress) return;

    if (Keyboard::isKeyPressed(Keyboard::Key::Up)) {
        selectedItem = (selectedItem - 1 + MENU_ITEMS) % MENU_ITEMS;
    }
    else if (Keyboard::isKeyPressed(Keyboard::Key::Down)) {
        selectedItem = (selectedItem + 1) % MENU_ITEMS;
    }
    else if (Keyboard::isKeyPressed(Keyboard::Key::Enter)) {
        cout << "Menu item selected: " << selectedItem << endl;
        switch (selectedItem) {
        case 0:
            cout << "Starting new game..." << endl;
            gameStarted = true;
            loadGame = false;
            showFadeText("Gut Shyshtem Studios");
            break;
        case 1: 
            cout << "Attempting to resume game..." << endl;
            loadGame = true;
            gameStarted = true;
            showFadeText("Gut Shyshtem Studios");
            break;
        case 2: 
            cout << "Exiting game..." << endl;
            window.close();
            break;
        }
    }
}

void Menu::showFadeText(const string& text) {
    cout << "Showing fade text: " << text << endl;
    fadeText.setString(text);
    fadeText.setOrigin(Vector2f(200.f, 24.f));
    fadeText.setPosition(Vector2f(CELL_SIZE * MAP_WIDTH / 2.f, CELL_SIZE * MAP_HEIGHT / 2.f));
    fadeAlpha = 0.f;
    fadeInProgress = true;
    fadeComplete = false;
}

void Menu::updateFadeEffect() {
    if (!fadeInProgress) return;

    fadeAlpha += fadeSpeed;
    if (fadeAlpha >= 255.f) {
        fadeAlpha = 255.f;
        fadeInProgress = false;
        fadeComplete = true;
        cout << "Fade effect complete!" << endl;
    }

    fadeText.setFillColor(Color(255, 255, 255, static_cast<uint8_t>(fadeAlpha)));
}