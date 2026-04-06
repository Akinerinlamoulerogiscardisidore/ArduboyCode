#ifndef SETTING_H
#define SETTING_H

#include <TFT_eSPI.h>
#include "Game.h"

class Settings {
public:
    Settings(TFT_eSPI* tft) {
        screen = tft;
        settingsIndex = 0;
        soundOn = true;
        vibrationOn = true;
        lastNavTime = 0;
        needsRedraw = true;   // ← nouveau
    }
    void resetNavTime() {
    lastNavTime = millis() - 201;
    }
    // Navigation sans valeur de retour (modifie consoleState directement)
    void navigate(const Buttons& buttons, int &consoleState) {
    unsigned long now = millis();
    if (now - lastNavTime < 200) return;

    bool changed = false;

    if (buttons.upPressed) {
        settingsIndex = (settingsIndex - 1 + 3) % 3;
        changed = true;
    }
    else if (buttons.downPressed) {
        settingsIndex = (settingsIndex + 1) % 3;
        changed = true;
    }
    else if (buttons.leftPressed || buttons.rightPressed || buttons.aPressed) {
        if (settingsIndex == 0) {
            soundOn = !soundOn;
            // éventuellement jouer un son de confirmation
        }
        else if (settingsIndex == 1) {
            vibrationOn = !vibrationOn;
        }
        else if (settingsIndex == 2) {
            consoleState = 1; // retour MENU
            needsRedraw = true;
            return; // sortie immédiate, pas de render ici
        }
        changed = true;
    }
    else if (buttons.bPressed) {
        consoleState = 1;
        needsRedraw = true;
        return;
    }

    if (changed) {
        lastNavTime = now;
        needsRedraw = true;
    }
    }
    void render() {
        if (!needsRedraw) return;
        needsRedraw = false;
        screen->fillScreen(TFT_BLACK);
        screen->setTextSize(2);

        // Titre
        screen->setTextColor(TFT_CYAN, TFT_BLACK);
        screen->setCursor(60, 5);
        screen->print("SETTINGS");

        // Option Son
        screen->setCursor(20, 40);
        screen->setTextColor(settingsIndex == 0 ? TFT_YELLOW : TFT_WHITE, TFT_BLACK);
        screen->print("Sound: ");
        screen->print(soundOn ? "ON" : "OFF");

        // Option Vibration
        screen->setCursor(20, 70);
        screen->setTextColor(settingsIndex == 1 ? TFT_YELLOW : TFT_WHITE, TFT_BLACK);
        screen->print("Vibration: ");
        screen->print(vibrationOn ? "ON" : "OFF");

        // Retour
        screen->setCursor(20, 100);
        screen->setTextColor(settingsIndex == 2 ? TFT_YELLOW : TFT_WHITE, TFT_BLACK);
        screen->print("Back");

        // Aide
        screen->setTextSize(1);
        screen->setCursor(10, 125);
        screen->setTextColor(TFT_DARKGREY, TFT_BLACK);
        screen->print("UP/DOWN  select   A/LEFT/RIGHT  toggle   B  back");
    }

    // Variables publiques
    int settingsIndex;
    bool soundOn;
    bool vibrationOn;
    bool needsRedraw;   // ← nouveau
private:
    TFT_eSPI* screen;
    unsigned long lastNavTime;
};

#endif