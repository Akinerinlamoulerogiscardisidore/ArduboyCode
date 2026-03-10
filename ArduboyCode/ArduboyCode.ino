/*
 * ArduboyCode - Menu principal
 *
 * Contrôles du menu :
 *   HAUT / BAS  : naviguer dans la liste
 *   A           : lancer le jeu sélectionné
 *
 * Chaque jeu retourne au menu via le bouton B.
 */

#include <Arduboy2.h>
#include "Morpion.h"
#include "Pong.h"
#include "Memory.h"

Arduboy2 arduboy;

// ── États de l'application ───────────────────────────────────────────────────
enum class AppState : uint8_t { MENU, MORPION, PONG, MEMORY };
AppState appState = AppState::MENU;

// ── Jeux instanciés une seule fois ───────────────────────────────────────────
Morpion    morpion;
PongGame   pong;
MemoryGame memory;

// ── Menu ─────────────────────────────────────────────────────────────────────
static const uint8_t NUM_GAMES = 3;
uint8_t menuCursor = 0;

void updateMenu() {
    if (arduboy.justPressed(UP_BUTTON) && menuCursor > 0)
        menuCursor--;
    if (arduboy.justPressed(DOWN_BUTTON) && menuCursor < NUM_GAMES - 1)
        menuCursor++;

    if (arduboy.justPressed(A_BUTTON)) {
        switch (menuCursor) {
            case 0: morpion.init(); appState = AppState::MORPION; break;
            case 1: pong.init();    appState = AppState::PONG;    break;
            case 2: memory.init();  appState = AppState::MEMORY;  break;
        }
    }
}

static void printGameName(uint8_t i) {
    switch (i) {
        case 0: arduboy.print(F("Morpion")); break;
        case 1: arduboy.print(F("Pong"));    break;
        case 2: arduboy.print(F("Memory"));  break;
    }
}

void drawMenu() {
    arduboy.clear();
    arduboy.setTextSize(1);

    // Titre
    arduboy.setCursor(16, 2);
    arduboy.print(F("== ARDUBOY GAMES =="));

    // Liste des jeux
    for (uint8_t i = 0; i < NUM_GAMES; i++) {
        arduboy.setCursor(28, 18 + i * 14);
        arduboy.print(i == menuCursor ? F("> ") : F("  "));
        printGameName(i);
    }

    // Instructions
    arduboy.setCursor(14, 56);
    arduboy.print(F("HT/BS: Nav   A: Jouer"));
}

// ── Setup / Loop ─────────────────────────────────────────────────────────────
void setup() {
    arduboy.begin();
    arduboy.setFrameRate(30);
    randomSeed(analogRead(0)); // graine aléatoire depuis bruit analogique
    arduboy.clear();
}

void loop() {
    if (!arduboy.nextFrame()) return;

    arduboy.pollButtons();

    bool backToMenu = false;

    switch (appState) {
        case AppState::MENU:
            updateMenu();
            drawMenu();
            break;
        case AppState::MORPION:
            backToMenu = morpion.update(arduboy);
            morpion.draw(arduboy);
            break;
        case AppState::PONG:
            backToMenu = pong.update(arduboy);
            pong.draw(arduboy);
            break;
        case AppState::MEMORY:
            backToMenu = memory.update(arduboy);
            memory.draw(arduboy);
            break;
    }

    if (backToMenu) {
        appState  = AppState::MENU;
        menuCursor = 0;
    }

    arduboy.display();
}
