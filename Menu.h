#ifndef MENU_H
#define MENU_H

#include <TFT_eSPI.h>
#include "Game.h"

struct MenuItem {
  String name;
  String description;
  String emoji;
  int gameId;
  uint16_t colorGame;
};

class Menu {
private:
    TFT_eSPI* screen;
    MenuItem items[10];
    int itemCount;
    int selectedIndex;
    int scrollOffset;
    bool needsRedraw;

    const int VISIBLE = 4;

public:
    unsigned long lastNavTime = 0;
    const unsigned long navDelay = 200;

    Menu(TFT_eSPI* display) {
      screen = display;
      itemCount = 0;
      selectedIndex = 0;
      scrollOffset = 0;
      needsRedraw = true;
    }

    void addGame(String name, String description, String emoji, int gameId, uint16_t colorGame) {
      if (itemCount < 10) {
        items[itemCount].name = name;
        items[itemCount].description = description;
        items[itemCount].emoji = emoji;
        items[itemCount].gameId = gameId;
        items[itemCount].colorGame = colorGame;
        itemCount++;
      }
    }

    int update(Buttons buttons) {
      unsigned long now = millis();
      static unsigned long lastNavigation = 0;

      if (buttons.down && (now - lastNavigation > navDelay)) {
        selectedIndex++;
        if (selectedIndex >= itemCount) {
          selectedIndex = 0;
          scrollOffset = 0;
        } else if (selectedIndex >= scrollOffset + VISIBLE) {
          scrollOffset = selectedIndex - VISIBLE + 1;
        }
        needsRedraw = true;
        lastNavigation = now;
      }

      if (buttons.up && (now - lastNavigation > navDelay)) {
        selectedIndex--;
        if (selectedIndex < 0) {
          selectedIndex = itemCount - 1;
          scrollOffset = max(0, itemCount - VISIBLE);
        } else if (selectedIndex < scrollOffset) {
          scrollOffset = selectedIndex;
        }
        needsRedraw = true;
        lastNavigation = now;
      }

      return -1;
    }

    int getSelectedId() {
      return items[selectedIndex].gameId;
    }

    void render() {
      if (!needsRedraw) return;
      needsRedraw = false;

      // Fond dégradé léger
      for (int y = 0; y < 135; y++) {
        screen->drawFastHLine(0, y, 240, screen->color565(10 + y/6, 10 + y/6, 30 + y/3));
      }

      // Titre
      screen->setTextColor(TFT_CYAN, TFT_BLACK);
      screen->setTextSize(2);
      screen->setCursor(50, 2);
      screen->print("  ARDUBOY");

      // Scroll haut
      if (scrollOffset > 0) {
        screen->setTextColor(TFT_WHITE, TFT_BLACK);
        screen->setTextSize(1);
        screen->setCursor(112, 18);
        screen->print("^  plus haut");
      }

      for (int i = 0; i < VISIBLE; i++) {
        int idx = scrollOffset + i;
        if (idx >= itemCount) break;
        int y = 26 + (i * 24);

        if (idx == selectedIndex) {
          // Fond arrondi pour sélection
          screen->fillRoundRect(0, y, 240, 22, 4, TFT_DARKGREY);

          // Flèche animée
          if ((millis() / 300) % 2 == 0) {
            screen->setTextColor(items[idx].colorGame);
            screen->setTextSize(1);
            screen->setCursor(2, y + 7);
            screen->print(">>");
          }

          // Nom du jeu
          screen->setTextSize(3);
          screen->setTextColor(items[idx].colorGame);
          screen->setCursor(20, y + 3);
          screen->print(items[idx].name);

          // Barre séparation
          screen->drawLine(0, y + 5, 239, y + 5, TFT_WHITE);

          // Indicateur couleur
          screen->fillRect(5, 126, 6, 7, items[idx].colorGame);

        } else {
          // Item non sélectionné
          screen->setTextColor(TFT_LIGHTGREY, TFT_BLACK);
          screen->setTextSize(3);
          screen->setCursor(20, y + 3);
          screen->print(items[idx].name);

          // Barre séparation
          screen->drawLine(0, y + 5, 239, y + 5, TFT_WHITE);
        }
      }

      // Scroll bas
      if (scrollOffset + VISIBLE < itemCount) {
        screen->setTextColor(TFT_WHITE, TFT_BLACK);
        screen->setTextSize(1);
        screen->setCursor(112, 122);
        screen->print("v  plus bas");
      }

      // Compteur
      screen->setTextColor(TFT_DARKGREY, TFT_BLACK);
      screen->setTextSize(1);
      screen->setCursor(195, 126);
      screen->print(selectedIndex + 1);
      screen->print("/");
      screen->print(itemCount);

      // Aide boutons
      screen->setTextColor(TFT_DARKGREY, TFT_BLACK);
      screen->setTextSize(1);
      screen->setCursor(15, 126);
      screen->print("[A] Jouer [^/v] Naviguer");
    }

    void forceRedraw() {
      needsRedraw = true;
    }
};

#endif