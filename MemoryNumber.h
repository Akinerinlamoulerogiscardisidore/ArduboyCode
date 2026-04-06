#ifndef MEMORY_NUMBER_H
#define MEMORY_NUMBER_H

#include "Game.h"

extern Settings settings;

class MemoryNumberGame : public Game {
private:
    enum GamePhase { COUNTDOWN, SHOW_SEQ, INPUT_PHASE, RESULT };
    GamePhase phase;

    int sequence[5];
    int userInput[5];
    int inputIndex;
    int cursorX, cursorY;
    unsigned long phaseStartTime;
    unsigned long countdownStart;
    int countdownValue;
    bool seqDisplayed;
    bool needsRedraw;  // ajouté

    const int gridCols = 3;
    const int gridRows = 3;
    const int cellW = 40;
    const int cellH = 40;
    int gridX, gridY;
    int zeroX, zeroY;

    const int inputTimeLimit = 10000;

    void generateSequence() {
        for (int i = 0; i < 5; i++) {
            sequence[i] = random(0, 10);
        }
    }

    void resetGame() {
        generateSequence();
        for (int i = 0; i < 5; i++) userInput[i] = -1;
        inputIndex = 0;
        cursorX = 0; cursorY = 0;
        phase = COUNTDOWN;
        countdownValue = 3;
        phaseStartTime = millis();
        countdownStart = millis();
        seqDisplayed = false;
        needsRedraw = true;
    }

    int getSelectedNumber() {
        if (cursorX == 3 && cursorY == 1) return 0;
        return cursorY * gridCols + cursorX + 1;
    }

    void drawNumberPad() {
        for (int row = 0; row < gridRows; row++) {
            for (int col = 0; col < gridCols; col++) {
                int num = row * gridCols + col + 1;
                int x = gridX + col * cellW;
                int y = gridY + row * cellH;
                if (cursorX == col && cursorY == row) {
                    screen->fillRoundRect(x, y, cellW, cellH, 5, TFT_YELLOW);
                    screen->setTextColor(TFT_BLACK);
                } else {
                    screen->fillRoundRect(x, y, cellW, cellH, 5, TFT_DARKGREY);
                    screen->setTextColor(TFT_WHITE);
                }
                screen->drawRoundRect(x, y, cellW, cellH, 5, TFT_WHITE);
                screen->setTextSize(2);
                screen->setCursor(x + cellW/2 - 8, y + cellH/2 - 8);
                screen->print(num);
            }
        }
        int x0 = zeroX, y0 = zeroY;
        if (cursorX == 3 && cursorY == 1) {
            screen->fillRoundRect(x0, y0, cellW, cellH, 5, TFT_YELLOW);
            screen->setTextColor(TFT_BLACK);
        } else {
            screen->fillRoundRect(x0, y0, cellW, cellH, 5, TFT_DARKGREY);
            screen->setTextColor(TFT_WHITE);
        }
        screen->drawRoundRect(x0, y0, cellW, cellH, 5, TFT_WHITE);
        screen->setCursor(x0 + cellW/2 - 8, y0 + cellH/2 - 8);
        screen->print("0");
    }

    void drawInputArea() {
        int startX = 20, startY = 20, boxW = 30, boxH = 30;
        screen->setTextSize(2);
        for (int i = 0; i < inputIndex; i++) {
            int x = startX + i * (boxW + 5);
            screen->fillRect(x, startY, boxW, boxH, TFT_BLUE);
            screen->drawRect(x, startY, boxW, boxH, TFT_WHITE);
            screen->setCursor(x + 10, startY + 8);
            screen->setTextColor(TFT_WHITE);
            screen->print(userInput[i]);
        }
        if (inputIndex < 5) {
            int x = startX + inputIndex * (boxW + 5);
            screen->drawRect(x, startY, boxW, boxH, TFT_YELLOW);
        }
    }

public:
    MemoryNumberGame(TFT_eSPI* display) : Game(display) {
        gridX = (240 - (gridCols * cellW)) / 2;
        gridY = 70;
        zeroX = gridX + cellW;
        zeroY = gridY + gridRows * cellH + 10;
    }

    void init() override {
        resetGame();
        state = IN_PROGRESS;
        needsRedraw = true;
    }

    void update(Buttons buttons) override {
        if (state != IN_PROGRESS) return;

        unsigned long now = millis();

        if (phase == COUNTDOWN) {
            if (now - countdownStart >= 1000) {
                countdownValue--;
                countdownStart = now;
                if (countdownValue <= 0) {
                    phase = SHOW_SEQ;
                    phaseStartTime = now;
                }
                needsRedraw = true;
            }
            return;
        }

        if (phase == SHOW_SEQ) {
            if (!seqDisplayed) {
                seqDisplayed = true;
                phaseStartTime = now;
                needsRedraw = true;
            } else if (now - phaseStartTime >= 1000) {
                phase = INPUT_PHASE;
                phaseStartTime = now;
                needsRedraw = true;
            }
            return;
        }

        if (phase == INPUT_PHASE) {
            if (now - phaseStartTime > inputTimeLimit) {
                state = GAME_OVER;
                return;
            }

            // Navigation
            if (buttons.leftPressed) {
                if (cursorX > 0) cursorX--;
                else if (cursorX == 0 && cursorY == 1) { cursorX = 2; cursorY = 1; }
                needsRedraw = true;
            }
            if (buttons.rightPressed) {
                if (cursorX < gridCols-1) cursorX++;
                else if (cursorX == 2 && cursorY == 1) { cursorX = 3; cursorY = 1; }
                else if (cursorX == 3 && cursorY == 1) cursorX = 2;
                needsRedraw = true;
            }
            if (buttons.upPressed) {
                if (cursorY > 0) cursorY--;
                needsRedraw = true;
            }
            if (buttons.downPressed) {
                if (cursorY < gridRows-1) cursorY++;
                else if (cursorY == 2 && cursorX == 1) { cursorX = 3; cursorY = 1; }
                needsRedraw = true;
            }

            if (buttons.aPressed) {
                int num = getSelectedNumber();
                userInput[inputIndex] = num;
                inputIndex++;
                if (settings.soundOn) tone(17, 1000, 50);
                needsRedraw = true;
                if (inputIndex == 5) {
                    bool correct = true;
                    for (int i = 0; i < 5; i++) {
                        if (userInput[i] != sequence[i]) {
                            correct = false;
                            break;
                        }
                    }
                    if (correct) {
                        score = 5;
                    }
                    state = GAME_OVER;
                }
            }
            return;
        }
    }

    bool isWinner() override {
        if (state == GAME_OVER && inputIndex == 5) {
            for (int i = 0; i < 5; i++) if (userInput[i] != sequence[i]) return false;
            return true;
        }
        return false;
    }

    void render() override {
        if (!needsRedraw && phase != COUNTDOWN && phase != SHOW_SEQ && phase != INPUT_PHASE) return;
        needsRedraw = false;

        screen->fillScreen(TFT_BLACK);
        screen->setTextSize(2);

        if (phase == COUNTDOWN) {
            int centerX = 120, centerY = 67;
            screen->setTextColor(TFT_WHITE);
            screen->setTextSize(4);
            if (countdownValue > 0) {
                screen->setCursor(centerX-15, centerY-20);
                screen->print(countdownValue);
            } else {
                screen->setCursor(centerX-30, centerY-20);
                screen->print("GO!");
            }
            return;
        }

        if (phase == SHOW_SEQ) {
            int startX = 20;
            int boxW = 35;
            for (int i = 0; i < 5; i++) {
                int x = startX + i * (boxW + 5);
                screen->fillRect(x, 50, boxW, 50, TFT_DARKGREY);
                screen->drawRect(x, 50, boxW, 50, TFT_WHITE);
                screen->setTextSize(2);
                screen->setCursor(x + 12, 68);
                screen->setTextColor(TFT_YELLOW);
                screen->print(sequence[i]);
            }
            return;
        }

        if (phase == INPUT_PHASE) {
            drawInputArea();
            drawNumberPad();
            unsigned long remaining = (phaseStartTime + inputTimeLimit - millis()) / 1000;
            screen->setTextSize(1);
            screen->setCursor(180, 5);
            screen->setTextColor(TFT_RED);
            screen->print("Time: ");
            screen->print(remaining);
            return;
        }
    }
};

#endif