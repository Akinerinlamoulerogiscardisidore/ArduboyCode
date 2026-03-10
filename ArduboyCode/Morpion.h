#pragma once
/*
 * Morpion.h – Jeu de Morpion (Tic-Tac-Toe) pour Arduboy
 *
 * Contrôles :
 *   Flèches      : déplacer le curseur
 *   A            : poser une pièce / rejouer après game-over
 *   B            : retour au menu (toujours actif)
 *
 * Règles :
 *   Joueur = X   (humain)
 *   IA     = O   (heuristique simple : gagner > bloquer > centre > coin > autre)
 *
 * Bugs corrigés :
 *   - L'écran "game over" ne disparaît plus tout seul ; il faut appuyer sur A
 *     (rejouer) ou B (menu).
 *   - Le bouton B est vérifié en tout premier, avant toute autre logique, et
 *     fonctionne donc dans toutes les situations.
 *   - Anti-rebond assuré par arduboy.justPressed().
 */

#include <Arduboy2.h>

class Morpion {
public:
    // ── Initialisation ────────────────────────────────────────────────────────
    void init() {
        for (uint8_t r = 0; r < 3; r++)
            for (uint8_t c = 0; c < 3; c++)
                board[r][c] = 0;
        cursorRow    = 1;
        cursorCol    = 1;
        currentPlayer = PLAYER_HUMAN;
        state        = PLAYING;
        winner       = 0;
        scoreX       = 0;
        scoreO       = 0;
        aiDelay      = 0;
    }

    // ── Mise à jour — retourne true = retour au menu ──────────────────────────
    bool update(Arduboy2& ab) {
        // B retourne TOUJOURS au menu, quelle que soit la situation
        if (ab.justPressed(B_BUTTON)) return true;

        switch (state) {
            case PLAYING:   updatePlaying(ab); break;
            case GAME_OVER: updateGameOver(ab); break;
        }
        return false;
    }

    // ── Rendu ─────────────────────────────────────────────────────────────────
    void draw(Arduboy2& ab) {
        ab.clear();
        drawBoard(ab);
        drawPieces(ab);
        drawScore(ab);

        if (state == PLAYING) {
            drawCursor(ab);
            ab.setTextSize(1);
            ab.setCursor(0, 56);
            ab.print(F("A:Jouer  B:Menu"));
        } else {
            drawGameOver(ab);
        }
    }

private:
    // ── Constantes ────────────────────────────────────────────────────────────
    static const uint8_t PLAYER_HUMAN = 1;
    static const uint8_t PLAYER_AI    = 2;

    // Grille centrée : coin haut-gauche (38, 6), cellules 17×18 px
    static const int8_t  GRID_X  = 38;
    static const int8_t  GRID_Y  = 4;
    static const uint8_t CELL_W  = 17;
    static const uint8_t CELL_H  = 18;

    enum State : uint8_t { PLAYING, GAME_OVER };

    // ── État ──────────────────────────────────────────────────────────────────
    uint8_t board[3][3];   // 0 = vide, 1 = X, 2 = O
    int8_t  cursorRow, cursorCol;
    uint8_t currentPlayer;
    State   state;
    uint8_t winner;        // 0 = nul, 1 = X, 2 = O
    uint8_t scoreX, scoreO;
    uint8_t aiDelay;       // frames d'attente avant que l'IA joue

    // ── Logique de jeu ────────────────────────────────────────────────────────
    void updatePlaying(Arduboy2& ab) {
        if (currentPlayer == PLAYER_HUMAN) {
            if (ab.justPressed(UP_BUTTON)    && cursorRow > 0) cursorRow--;
            if (ab.justPressed(DOWN_BUTTON)  && cursorRow < 2) cursorRow++;
            if (ab.justPressed(LEFT_BUTTON)  && cursorCol > 0) cursorCol--;
            if (ab.justPressed(RIGHT_BUTTON) && cursorCol < 2) cursorCol++;

            if (ab.justPressed(A_BUTTON) && board[cursorRow][cursorCol] == 0) {
                board[cursorRow][cursorCol] = PLAYER_HUMAN;
                if (endOfTurn()) return;
                currentPlayer = PLAYER_AI;
                aiDelay = 20; // ~0,7 s : l'IA "réfléchit"
            }
        } else {
            // IA : attendre quelques frames pour simuler une réflexion
            if (aiDelay > 0) { aiDelay--; return; }
            aiMove();
            if (endOfTurn()) return;
            currentPlayer = PLAYER_HUMAN;
        }
    }

    // Appelé après chaque pose, retourne true si la partie est terminée
    bool endOfTurn() {
        uint8_t w = checkWinner();
        if (w != 0 || isBoardFull()) {
            winner = w;
            if (w == PLAYER_HUMAN) scoreX++;
            else if (w == PLAYER_AI) scoreO++;
            state = GAME_OVER;
            return true;
        }
        return false;
    }

    void updateGameOver(Arduboy2& ab) {
        // A = rejouer (plateau réinitialisé, scores conservés)
        if (ab.justPressed(A_BUTTON)) {
            for (uint8_t r = 0; r < 3; r++)
                for (uint8_t c = 0; c < 3; c++)
                    board[r][c] = 0;
            cursorRow     = 1;
            cursorCol     = 1;
            currentPlayer = PLAYER_HUMAN;
            winner        = 0;
            aiDelay       = 0;
            state         = PLAYING;
        }
    }

    // ── IA heuristique ────────────────────────────────────────────────────────
    void aiMove() {
        // 1. Gagner si possible
        if (tryPlace(PLAYER_AI)) return;
        // 2. Bloquer le joueur
        if (tryPlace(PLAYER_HUMAN)) return;
        // 3. Prendre le centre
        if (board[1][1] == 0) { board[1][1] = PLAYER_AI; return; }
        // 4. Prendre un coin
        static const uint8_t corners[4][2] = {{0,0},{0,2},{2,0},{2,2}};
        for (uint8_t i = 0; i < 4; i++)
            if (board[corners[i][0]][corners[i][1]] == 0) {
                board[corners[i][0]][corners[i][1]] = PLAYER_AI;
                return;
            }
        // 5. N'importe quelle case libre
        for (uint8_t r = 0; r < 3; r++)
            for (uint8_t c = 0; c < 3; c++)
                if (board[r][c] == 0) { board[r][c] = PLAYER_AI; return; }
    }

    // Essaye de placer PLAYER_AI dans la case gagnante/bloquante pour `player`
    bool tryPlace(uint8_t player) {
        for (uint8_t r = 0; r < 3; r++)
            for (uint8_t c = 0; c < 3; c++)
                if (board[r][c] == 0) {
                    board[r][c] = player;
                    if (checkWinner() == player) {
                        board[r][c] = PLAYER_AI; // placer O si c'était pour bloquer
                        return true;
                    }
                    board[r][c] = 0;
                }
        return false;
    }

    uint8_t checkWinner() {
        for (uint8_t i = 0; i < 3; i++) {
            if (board[i][0] && board[i][0]==board[i][1] && board[i][1]==board[i][2]) return board[i][0];
            if (board[0][i] && board[0][i]==board[1][i] && board[1][i]==board[2][i]) return board[0][i];
        }
        if (board[1][1]) {
            if (board[0][0]==board[1][1] && board[1][1]==board[2][2]) return board[1][1];
            if (board[0][2]==board[1][1] && board[1][1]==board[2][0]) return board[1][1];
        }
        return 0;
    }

    bool isBoardFull() {
        for (uint8_t r = 0; r < 3; r++)
            for (uint8_t c = 0; c < 3; c++)
                if (board[r][c] == 0) return false;
        return true;
    }

    // ── Rendu ─────────────────────────────────────────────────────────────────
    void drawBoard(Arduboy2& ab) {
        // Lignes verticales
        ab.drawLine(GRID_X + CELL_W,     GRID_Y, GRID_X + CELL_W,     GRID_Y + 3*CELL_H, WHITE);
        ab.drawLine(GRID_X + 2*CELL_W,   GRID_Y, GRID_X + 2*CELL_W,   GRID_Y + 3*CELL_H, WHITE);
        // Lignes horizontales
        ab.drawLine(GRID_X, GRID_Y + CELL_H,   GRID_X + 3*CELL_W, GRID_Y + CELL_H,   WHITE);
        ab.drawLine(GRID_X, GRID_Y + 2*CELL_H, GRID_X + 3*CELL_W, GRID_Y + 2*CELL_H, WHITE);
    }

    void drawPieces(Arduboy2& ab) {
        for (uint8_t r = 0; r < 3; r++) {
            for (uint8_t c = 0; c < 3; c++) {
                int16_t cx = GRID_X + c * CELL_W + CELL_W / 2;
                int16_t cy = GRID_Y + r * CELL_H + CELL_H / 2;
                if (board[r][c] == PLAYER_HUMAN) {
                    ab.drawLine(cx-5, cy-5, cx+5, cy+5, WHITE);
                    ab.drawLine(cx+5, cy-5, cx-5, cy+5, WHITE);
                } else if (board[r][c] == PLAYER_AI) {
                    ab.drawCircle(cx, cy, 5, WHITE);
                }
            }
        }
    }

    void drawCursor(Arduboy2& ab) {
        int16_t x = GRID_X + cursorCol * CELL_W + 2;
        int16_t y = GRID_Y + cursorRow * CELL_H + 2;
        ab.drawRect(x, y, CELL_W - 4, CELL_H - 4, WHITE);
    }

    void drawScore(Arduboy2& ab) {
        ab.setTextSize(1);
        ab.setCursor(2, 4);  ab.print(F("X:")); ab.print(scoreX);
        ab.setCursor(2, 16); ab.print(F("O:")); ab.print(scoreO);
    }

    void drawGameOver(Arduboy2& ab) {
        ab.fillRect(12, 16, 104, 32, BLACK);
        ab.drawRect(12, 16, 104, 32, WHITE);
        ab.setTextSize(1);

        // Ligne 1 : résultat
        ab.setCursor(34, 21);
        if (winner == PLAYER_HUMAN)     ab.print(F("Vous gagnez !"));
        else if (winner == PLAYER_AI)   ab.print(F("  L'IA gagne !"));
        else                            ab.print(F("  Match nul !"));

        // Ligne 2 : instructions (toujours visibles)
        ab.setCursor(16, 35);
        ab.print(F("A: Rejouer   B: Menu"));
    }
};
