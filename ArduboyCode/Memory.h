#pragma once
/*
 * Memory.h – Jeu de Memory (paires de cartes) pour Arduboy
 *
 * Contrôles :
 *   Flèches      : déplacer le curseur sur la grille 4×4
 *   A            : retourner la carte sélectionnée / rejouer après victoire
 *   B            : retour au menu (toujours actif)
 *
 * Règles :
 *   16 cartes (8 paires, valeurs 1–8).
 *   Retourner 2 cartes : si elles correspondent, elles restent visibles.
 *   Sinon elles se cachent après ~1,5 s.
 *   Gagner = toutes les paires trouvées.
 *   Le compteur de coups est affiché en permanence.
 */

#include <Arduboy2.h>

class MemoryGame {
public:
    // ── Initialisation ────────────────────────────────────────────────────────
    void init() {
        // Remplir le tableau avec 8 paires (valeurs 1–8)
        for (uint8_t i = 0; i < TOTAL_CARDS; i++) cards[i] = (i % NUM_PAIRS) + 1;

        // Mélange Fisher-Yates
        for (uint8_t i = TOTAL_CARDS - 1; i > 0; i--) {
            uint8_t j = random(i + 1);
            uint8_t tmp = cards[i]; cards[i] = cards[j]; cards[j] = tmp;
        }

        for (uint8_t i = 0; i < TOTAL_CARDS; i++) {
            faceUp[i]  = false;
            matched[i] = false;
        }

        cursorPos  = 0;
        firstCard  = -1;
        secondCard = -1;
        state      = SELECTING;
        pairsFound = 0;
        showDelay  = 0;
        moves      = 0;
    }

    // ── Mise à jour — retourne true = retour au menu ──────────────────────────
    bool update(Arduboy2& ab) {
        if (ab.justPressed(B_BUTTON)) return true;

        switch (state) {
            case SELECTING:    updateSelecting(ab); break;
            case SHOWING_PAIR: updateShowingPair();  break;
            case WIN:
                if (ab.justPressed(A_BUTTON)) init();
                break;
        }
        return false;
    }

    // ── Rendu ─────────────────────────────────────────────────────────────────
    void draw(Arduboy2& ab) {
        ab.clear();
        if (state == WIN) { drawWin(ab); return; }
        drawGrid(ab);
        drawCursor(ab);
        drawInfo(ab);
    }

private:
    // ── Constantes ────────────────────────────────────────────────────────────
    static const uint8_t TOTAL_CARDS = 16;
    static const uint8_t NUM_PAIRS   = 8;

    // Grille 4×4, cellule 12×12 px, espacement 3 px
    // Largeur totale : 4*12 + 3*3 = 57 px → début à x=5
    // Hauteur totale : 4*12 + 3*3 = 57 px → début à y=3
    static const int8_t  CARD_W  = 12;
    static const int8_t  CARD_H  = 12;
    static const int8_t  CARD_GAP = 3;
    static const int8_t  GRID_X  = 5;
    static const int8_t  GRID_Y  = 3;

    // Panneau d'info à droite (x ≥ 68)
    static const int8_t  INFO_X  = 68;

    enum State : uint8_t { SELECTING, SHOWING_PAIR, WIN };

    // ── État ──────────────────────────────────────────────────────────────────
    uint8_t cards[TOTAL_CARDS];
    bool    faceUp[TOTAL_CARDS];
    bool    matched[TOTAL_CARDS];
    int8_t  cursorPos;
    int8_t  firstCard, secondCard;
    State   state;
    uint8_t pairsFound;
    uint8_t showDelay;
    uint8_t moves;

    // ── Logique ───────────────────────────────────────────────────────────────
    void updateSelecting(Arduboy2& ab) {
        uint8_t col = cursorPos % 4;
        uint8_t row = cursorPos / 4;

        if (ab.justPressed(UP_BUTTON)    && row > 0) cursorPos -= 4;
        if (ab.justPressed(DOWN_BUTTON)  && row < 3) cursorPos += 4;
        if (ab.justPressed(LEFT_BUTTON)  && col > 0) cursorPos--;
        if (ab.justPressed(RIGHT_BUTTON) && col < 3) cursorPos++;

        if (ab.justPressed(A_BUTTON)) {
            // Ne peut pas retourner une carte déjà visible ou trouvée
            if (matched[cursorPos] || faceUp[cursorPos]) return;

            faceUp[cursorPos] = true;
            moves++;

            if (firstCard < 0) {
                firstCard = cursorPos;
            } else {
                secondCard = cursorPos;
                if (cards[firstCard] == cards[secondCard]) {
                    // Paire trouvée !
                    matched[firstCard] = true;
                    matched[secondCard] = true;
                    pairsFound++;
                    firstCard  = -1;
                    secondCard = -1;
                    if (pairsFound == NUM_PAIRS) state = WIN;
                } else {
                    // Pas de paire : afficher brièvement, puis cacher
                    showDelay = 45; // 45 frames ≈ 1,5 s à 30 fps
                    state     = SHOWING_PAIR;
                }
            }
        }
    }

    void updateShowingPair() {
        if (showDelay > 0) { showDelay--; return; }
        // Cacher les deux cartes non-appariées
        faceUp[firstCard]  = false;
        faceUp[secondCard] = false;
        firstCard  = -1;
        secondCard = -1;
        state      = SELECTING;
    }

    // ── Rendu ─────────────────────────────────────────────────────────────────
    void drawGrid(Arduboy2& ab) {
        for (uint8_t i = 0; i < TOTAL_CARDS; i++) {
            int16_t x = GRID_X + (i % 4) * (CARD_W + CARD_GAP);
            int16_t y = GRID_Y + (i / 4) * (CARD_H + CARD_GAP);

            ab.drawRect(x, y, CARD_W, CARD_H, WHITE);

            if (faceUp[i] || matched[i]) {
                // Afficher la valeur de la carte
                ab.setTextSize(1);
                ab.setCursor(x + 3, y + 2);
                ab.print(cards[i]);
                // Croix discrète sur les cartes trouvées
                if (matched[i]) {
                    ab.drawLine(x + 1, y + 1, x + CARD_W - 2, y + CARD_H - 2, WHITE);
                    ab.drawLine(x + CARD_W - 2, y + 1, x + 1, y + CARD_H - 2, WHITE);
                }
            } else {
                // Dos de carte : motif en croix
                int16_t mx = x + CARD_W / 2;
                int16_t my = y + CARD_H / 2;
                ab.drawPixel(mx, my,     WHITE);
                ab.drawPixel(mx - 2, my, WHITE);
                ab.drawPixel(mx + 2, my, WHITE);
                ab.drawPixel(mx, my - 2, WHITE);
                ab.drawPixel(mx, my + 2, WHITE);
            }
        }
    }

    void drawCursor(Arduboy2& ab) {
        if (state != SELECTING) return;
        int16_t x = GRID_X + (cursorPos % 4) * (CARD_W + CARD_GAP) - 1;
        int16_t y = GRID_Y + (cursorPos / 4) * (CARD_H + CARD_GAP) - 1;
        ab.drawRect(x, y, CARD_W + 2, CARD_H + 2, WHITE);
    }

    void drawInfo(Arduboy2& ab) {
        ab.setTextSize(1);

        ab.setCursor(INFO_X, 3);  ab.print(F("Paires"));
        ab.setCursor(INFO_X, 12); ab.print(pairsFound); ab.print(F("/8"));

        ab.setCursor(INFO_X, 26); ab.print(F("Coups"));
        ab.setCursor(INFO_X, 35); ab.print(moves);

        ab.setCursor(INFO_X, 50); ab.print(F("A:carte"));
        ab.setCursor(INFO_X, 58); ab.print(F("B:menu"));
    }

    void drawWin(Arduboy2& ab) {
        ab.fillRect(12, 14, 104, 36, BLACK);
        ab.drawRect(12, 14, 104, 36, WHITE);
        ab.setTextSize(1);

        ab.setCursor(26, 20);
        ab.print(F("Bravo ! Gagne en"));
        ab.setCursor(40, 30);
        ab.print(moves); ab.print(F(" coups !"));

        ab.setCursor(16, 42);
        ab.print(F("A: Rejouer   B: Menu"));
    }
};
