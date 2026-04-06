#ifndef MORPION_H
#define MORPION_H

#include "Game.h"

#ifndef VIBRATION_PIN
#define VIBRATION_PIN 16   // doit correspondre à la broche utilisée dans ArduBoy.ino
#endif
extern Settings settings;

class MorpionGame : public Game {
private:
    // ===== MODES =====
    enum Mode { MODE_MENU, MODE_AI_SELECT, MODE_GAME };
    Mode mode;

    enum AILevel { AI_EASY, AI_HARD };
    AILevel aiLevel;

    int menuIndex;
    bool vsAI;
    bool iaPending;
    int winner;   // 0 = pas de vainqueur, 1 = joueur1, 2 = joueur2

    // ===== JEU =====i
    int board[3][3];
    int cursorX, cursorY;
    int currentPlayer;
    int result;

    // ===== RENDU =====
    bool needsRedraw;
    bool firstDraw;          // ← AJOUTÉ
    unsigned long lastAnim;
    int animPhase;

    const int cellSize = 36;
    const int gridX = (240 - 36 * 3) / 2;
    const int gridY = (135 - 36 * 3) / 2 + 10;

    // ===== PAUSE =====
    bool paused;
    int pauseIndex;
    unsigned long lastNavTime;
    const int navDelay = 200;
    int lastMoveRow, lastMoveCol;

    // ===== CURSEUR =====
    int prevCursorX = -1, prevCursorY = -1;

    // ===== LOGIQUE =====
    int checkWinner() {
        for (int i = 0; i < 3; i++) {
            if (board[i][0] && board[i][0] == board[i][1] && board[i][1] == board[i][2]) return board[i][0];
            if (board[0][i] && board[0][i] == board[1][i] && board[1][i] == board[2][i]) return board[0][i];
        }
        if (board[0][0] && board[0][0] == board[1][1] && board[1][1] == board[2][2]) return board[0][0];
        if (board[0][2] && board[0][2] == board[1][1] && board[1][1] == board[2][0]) return board[0][2];
        return 0;
    }

    bool checkDraw() {
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                if (board[r][c] == 0) return false;
        return true;
    }

    void resetGame() {
        winner = 0;
        for (int r = 0; r < 3; r++)
            for (int c = 0; c < 3; c++)
                board[r][c] = 0;

        cursorX = 1;
        cursorY = 1;
        currentPlayer = 1;
        result = 0;

        prevCursorX = -1;
        prevCursorY = -1;
        firstDraw = true;     // force tout redessiner au prochain render
        needsRedraw = true;
        iaPending = false;
        if (vsAI && currentPlayer == 2) {
        iaPending = true;   // l'IA jouera au prochain update()
        }
    }

            // ===== IA AMÉLIORÉE =====
    void playAI_Easy() {
        // Aléatoire pur : ne réfléchit pas du tout
        int r, c;
        do {
            r = random(0, 3);
            c = random(0, 3);
        } while (board[r][c] != 0);
        board[r][c] = 2;
    }

    int evaluateBoard() {
        int winner = checkWinner();
        if (winner == 2) return 10;   // L'IA gagne
        if (winner == 1) return -10;  // Le joueur gagne
        return 0;
    }

    int minimax(int depth, bool isMaximizing) {
        int score = evaluateBoard();
        if (score == 10 || score == -10) return score;
        if (checkDraw()) return 0;

        if (isMaximizing) {
            int best = -1000;
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 3; c++) {
                    if (board[r][c] == 0) {
                        board[r][c] = 2; // IA
                        best = max(best, minimax(depth + 1, false));
                        board[r][c] = 0;
                    }
                }
            }
            return best;
        } else {
            int best = 1000;
            for (int r = 0; r < 3; r++) {
                for (int c = 0; c < 3; c++) {
                    if (board[r][c] == 0) {
                        board[r][c] = 1; // Joueur
                        best = min(best, minimax(depth + 1, true));
                        board[r][c] = 0;
                    }
                }
            }
            return best;
        }
    }

    void playAI_Hard() {
    // Algorithme minimax : l'IA joue le coup optimal
    int bestVal = -1000;
    int bestRow = -1, bestCol = -1;
    for (int r = 0; r < 3; r++) {
        for (int c = 0; c < 3; c++) {
            if (board[r][c] == 0) {
                board[r][c] = 2;
                int moveVal = minimax(0, false);
                board[r][c] = 0;
                if (moveVal > bestVal) {
                    bestVal = moveVal;
                    bestRow = r;
                    bestCol = c;
                }
            }
        }
    }
    if (bestRow != -1) board[bestRow][bestCol] = 2;
}

    
    // ===== DESSIN D'UNE CASE (SANS LE CURSEUR) =====
    void drawCell(int col, int row) {
        int x = gridX + col * cellSize;
        int y = gridY + row * cellSize;
        screen->drawRoundRect(x, y, cellSize, cellSize, 6, TFT_DARKGREY);
        // Si la case contient un symbole, on le dessine
        if (board[row][col] == 1) {
            screen->drawLine(x + 6, y + 6, x + cellSize - 6, y + cellSize - 6, TFT_RED);
            screen->drawLine(x + cellSize - 6, y + 6, x + 6, y + cellSize - 6, TFT_RED);
        }
        else if (board[row][col] == 2) {
            int cx = x + cellSize / 2;
            int cy = y + cellSize / 2;
            screen->drawCircle(cx, cy, cellSize / 2 - 6, TFT_BLUE);
            screen->drawCircle(cx, cy, cellSize / 2 - 8, TFT_CYAN);
        }
    }

    // ===== REDESSINER UN SYMBOLE (X ou O) À UN ENDROIT PRÉCIS =====
    void drawSymbol(int col, int row) {
        // Efface d'abord la zone (fond + contour)
        int x = gridX + col * cellSize;
        int y = gridY + row * cellSize;
        screen->fillRect(x+1, y+1, cellSize-2, cellSize-2, TFT_LIGHTGREY);
        screen->drawRoundRect(x, y, cellSize, cellSize, 6, TFT_DARKGREY);
        // Puis redessine le symbole si présent
        if (board[row][col] == 1) {
            screen->drawLine(x + 6, y + 6, x + cellSize - 6, y + cellSize - 6, TFT_RED);
            screen->drawLine(x + cellSize - 6, y + 6, x + 6, y + cellSize - 6, TFT_RED);
        }
        else if (board[row][col] == 2) {
            int cx = x + cellSize / 2;
            int cy = y + cellSize / 2;
            screen->drawCircle(cx, cy, cellSize / 2 - 6, TFT_BLUE);
            screen->drawCircle(cx, cy, cellSize / 2 - 8, TFT_CYAN);
        }
    }

    // ===== CURSEUR =====
    void renderCursor() {
        // efface l'ancien curseur
        if (prevCursorX != -1) {
            int px = gridX + prevCursorX * cellSize;
            int py = gridY + prevCursorY * cellSize;
            // redessiner la case sous l'ancien curseur (sans le curseur)
            drawSymbol(prevCursorX, prevCursorY);
        }

        // dessine le nouveau curseur
        int cx = gridX + cursorX * cellSize;
        int cy = gridY + cursorY * cellSize;
        uint16_t col = (animPhase == 0) ? TFT_ORANGE : TFT_YELLOW;
        screen->drawRoundRect(cx + 2, cy + 2, cellSize - 4, cellSize - 4, 6, col);

        prevCursorX = cursorX;
        prevCursorY = cursorY;
    }

    // ===== MENUS =====
    void renderMenu() {
        screen->fillScreen(TFT_LIGHTGREY);
        screen->setTextSize(2);
        screen->setCursor(60, 20);
        screen->setTextColor(TFT_BLACK);
        screen->print("MORPION");

        const char* opt[2] = { "J1 vs J2", "J1 vs IA" };
        for (int i = 0; i < 2; i++) {
            screen->setCursor(60, 70 + i * 25);
            screen->setTextColor((i == menuIndex) ? TFT_YELLOW : TFT_BLACK);
            if (i == menuIndex) screen->print("> "); else screen->print("  ");
            screen->print(opt[i]);
        }
    }

    void renderAIMenu() {
        screen->fillScreen(TFT_LIGHTGREY);
        screen->setTextSize(2);
        screen->setCursor(70, 20);
        screen->setTextColor(TFT_BLACK);
        screen->print("IA");

        const char* opt[2] = { "EASY", "HARD" };
        for (int i = 0; i < 2; i++) {
            screen->setCursor(80, 70 + i * 25);
            screen->setTextColor((i == menuIndex) ? TFT_YELLOW : TFT_BLACK);
            if (i == menuIndex) screen->print("> "); else screen->print("  ");
            screen->print(opt[i]);
        }
    }

    void renderPauseMenu() {
        // Dessiner un rectangle semi-transparent (simulé) sans effacer tout l'écran
        screen->fillRoundRect(40, 40, 160, 67, 8, TFT_DARKGREY);
        screen->drawRoundRect(40, 40, 160, 67, 8, TFT_WHITE);

        screen->setTextColor(TFT_WHITE);
        screen->setCursor(80, 50);
        screen->print("PAUSE");

        const char* options[3] = { "Reprendre", "Restart", "Quitter" };
        for (int i = 0; i < 3; i++) {
            screen->setCursor(70, 65 + i * 15);
            if (i == pauseIndex) {
                screen->setTextColor(TFT_YELLOW);
                screen->print("> ");
            } else {
                screen->setTextColor(TFT_WHITE);
                screen->print("  ");
            }
            screen->print(options[i]);
        }
    }

public:
    MorpionGame(TFT_eSPI* display) : Game(display) {}

    int getWinner() { return winner; }
    
    void init() override {
        Serial.println("Morpion init start");
        mode = MODE_MENU;
        menuIndex = 0;
        paused = false;
        pauseIndex = 0;
        lastAnim = 0;
        animPhase = 0;
        state = IN_PROGRESS;
        needsRedraw = true;
        firstDraw = true;
        lastMoveRow = -1;
        lastMoveCol = -1;
        result = 0;   // ← AJOUTER
        winner = 0;   // ← AJOUTER
        Serial.println("Morpion init end");
    }
    bool isWinner() override {
        return (result != 0);   // victoire de n'importe qui
        }
    void update(Buttons b) override {
        Serial.println("Morpion update start");
        unsigned long now = millis();

        // curseur animé
        if (now - lastAnim > 200) {
            animPhase = (animPhase + 1) % 2;
            lastAnim = now;
            needsRedraw = true;
        }

        // ===== MENU =====
        if (mode == MODE_MENU) {
            if (now - lastNavTime > navDelay) {
                if (b.upPressed || b.downPressed) {
                    menuIndex = (menuIndex + 1) % 2;
                    lastNavTime = now;
                    needsRedraw = true;
                }
                if (b.aPressed) {
                    if (menuIndex == 0) {
                        vsAI = false;
                        mode = MODE_GAME;
                        resetGame();
                    } else {
                        mode = MODE_AI_SELECT;
                        menuIndex = 0;
                    }
                    needsRedraw = true;
                }
            }
            return;
        }

        // ===== MENU IA =====
        if (mode == MODE_AI_SELECT) {
            if (now - lastNavTime > navDelay) {
                if (b.upPressed || b.downPressed) {
                    menuIndex = (menuIndex + 1) % 2;
                    lastNavTime = now;
                    needsRedraw = true;
                }
                if (b.aPressed) {
                    vsAI = true;
                    aiLevel = (menuIndex == 0) ? AI_EASY : AI_HARD;
                    mode = MODE_GAME;
                    resetGame();
                    needsRedraw = true;
                }
                if (b.bPressed) {
                    mode = MODE_MENU;
                    needsRedraw = true;
                }
            }
            return;
        }

        // ===== PAUSE =====
        if (b.bPressed) {
            paused = !paused;
            needsRedraw = true;
        }
        if (paused) {
            if (now - lastNavTime > navDelay) {
                if (b.upPressed) {
                    pauseIndex = (pauseIndex + 2) % 3;
                    lastNavTime = now;
                    needsRedraw = true;
                }
                if (b.downPressed) {
                    pauseIndex = (pauseIndex + 1) % 3;
                    lastNavTime = now;
                    needsRedraw = true;
                }
                if (b.aPressed) {
                    if (pauseIndex == 0) {
                        paused = false;
                    } else if (pauseIndex == 1) {
                        resetGame();
                        paused = false;
                    } else if (pauseIndex == 2) {
                        state = GAME_OVER;
                        paused = false;
                    }
                    needsRedraw = true;
                }
            }
            return;
        }

        if (vsAI && iaPending && !paused && state == IN_PROGRESS) {
        iaPending = false;
        if (aiLevel == AI_EASY) playAI_Easy();
        else playAI_Hard();
        result = checkWinner();
        if (result || checkDraw()) {
            state = GAME_OVER;
            needsRedraw = true;
            return;
        }
        currentPlayer = 1;  // retour au joueur
        needsRedraw = true;
        return; // important : on sort pour que le rendu s'effectue
        }

        // ===== INPUT =====
        if (b.upPressed && cursorY > 0) cursorY--;
        else if (b.downPressed && cursorY < 2) cursorY++;
        else if (b.leftPressed && cursorX > 0) cursorX--;
        else if (b.rightPressed && cursorX < 2) cursorX++;
        else if (b.aPressed && board[cursorY][cursorX] == 0) {
            board[cursorY][cursorX] = currentPlayer;
            lastMoveRow = cursorY;
            lastMoveCol = cursorX;

            result = checkWinner();
            if (result != 0) winner = result;   // stocke le gagnant
            if (result || checkDraw()) {
                state = GAME_OVER;
                needsRedraw = true;
                return;
            }
            currentPlayer = (currentPlayer == 1) ? 2 : 1;
            needsRedraw = true;

            // Si c'est au tour de l'IA, on la programme pour le prochain cycle
            if (vsAI && currentPlayer == 2) {
                iaPending = true;
            }
            return; // sortir pour permettre l'affichage du coup
        }

        needsRedraw = true;
        Serial.println("Morpion update end");
    }

    void render() override {
        Serial.println("Morpion render start");
        if (!needsRedraw) return;
        needsRedraw = false;

        // Première fois : on dessine toute la grille
        if (firstDraw) {
            screen->fillScreen(TFT_LIGHTGREY);
            for (int r = 0; r < 3; r++)
                for (int c = 0; c < 3; c++)
                    drawCell(c, r);
            firstDraw = false;
        }

        // Si un coup a été joué, redessiner uniquement la case concernée
        if (lastMoveRow != -1) {
            drawSymbol(lastMoveCol, lastMoveRow);
            lastMoveRow = -1;
        }

        // Gérer le curseur (efface l'ancien, dessine le nouveau)
        renderCursor();

        // Afficher le menu pause par-dessus si nécessaire
        if (paused) {
            renderPauseMenu();
        }

        // Si on est dans les menus principaux, on les dessine (ils remplacent tout)
        if (mode == MODE_MENU) {
            renderMenu();
        } else if (mode == MODE_AI_SELECT) {
            renderAIMenu();
        }
        Serial.println("Morpion render end");
    }
};

#endif