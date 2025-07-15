#pragma once

#include <SFML/Graphics.hpp>
#include <string>

using namespace sf;

class Menu {
public:
    Menu();
    ~Menu();
    void draw(RenderWindow& window);
    void handleInput(RenderWindow& window);
    bool isGameStarted() const { return gameStarted && !fadeInProgress; }
    bool shouldLoadGame() const { return loadGame; }
    void showFadeText(const string& text);
    bool isFading() const { return fadeInProgress; }
    bool isFadeComplete() const { return fadeComplete; }
    void resetState();
    void clearLoadFlag() { loadGame = false; }

private:
    static const int MENU_ITEMS = 3;
    Font font;
    Text startNewGameText{ font, "Start New Game", 40 };
    Text resumeGameText{ font, "Resume Game", 40 };
    Text exitText{ font, "Exit", 40 };
    Text fadeText{ font, "Gut Shyshtem Studios", 50 };
    int selectedItem{ 0 };
    bool gameStarted{ false };
    bool loadGame{ false };
    bool fadeComplete{ true };

    float fadeAlpha{ 0.f };
    float fadeSpeed{ 2.f };
    bool fadeInProgress{ false };

    void updateFadeEffect();
};