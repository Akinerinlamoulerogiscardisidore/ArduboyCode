// #ifndef MEMORY_H
// #define MEMORY_H

// #include "Game.h"

// // ════════════════════════════════════════════════
// //  MEMORY — Retourner les paires de cartes
// //  Boutons : HAUT/BAS/GAUCHE/DROITE curseur, A retourner
// //  Game Over : toutes les paires trouvées (victoire)
// // ════════════════════════════════════════════════

// class MemoryGame : public Game {
//   private:

//     // ── Grille de cartes ──
//     const int COLS  = 4;    // colonnes
//     const int ROWS  = 3;    // rangées (= 12 cartes = 6 paires)
//     const int cardW = 50;   // largeur d'une carte
//     const int cardH = 36;   // hauteur d'une carte
//     const int offX  = 10;   // décalage gauche
//     const int offY  = 18;   // décalage haut

//     // ── Valeurs des cartes (6 paires de 0 à 5) ──
//     int  cards[3][4];        // valeur de chaque carte
//     bool flipped[3][4];      // retournée ?
//     bool matched[3][4];      // trouvée (paire validée) ?

//     // ── Curseur ──
//     int curCol, curRow;      // position du curseur
//     int prevCurCol, prevCurRow;

//     // ── Logique de jeu ──
//     int  firstCol,  firstRow;   // première carte retournée
//     bool waitingSecond;          // attend la 2e carte ?
//     int  pairsFound;             // paires trouvées
//     bool showingPair;            // affiche 2 cartes retournées ?
//     unsigned long showStart;     // timestamp d'affichage

//     bool needsRedraw;

//     // ── Couleurs des valeurs ──
//     uint16_t cardColors[6] = {
//       TFT_RED, TFT_GREEN, TFT_BLUE,
//       TFT_YELLOW, TFT_CYAN, TFT_MAGENTA
//     };

//     // ────────────────────────────────────────────
//     //  Mélanger les cartes (Fisher-Yates shuffle)
//     // ────────────────────────────────────────���───
//     void shuffleCards() {
//       int flat[12];
//       // Créer les paires
//       for (int i = 0; i < 6; i++) { flat[i] = i; flat[i+6] = i; }
//       // Mélanger
//       for (int i = 11; i > 0; i--) {
//         int j = random(0, i + 1);
//         int tmp = flat[i]; flat[i] = flat[j]; flat[j] = tmp;
//       }
//       // Remplir la grille
//       for (int r = 0; r < ROWS; r++)
//         for (int c = 0; c < COLS; c++)
//           cards[r][c] = flat[r * COLS + c];
//     }

//     // ────────────────────────────────────────────
//     //  Dessiner une carte
//     // ────────────────────────────────────────────
//     void drawCard(int col, int row) {
//       int x = offX + col * (cardW + 2);
//       int y = offY + row * (cardH + 2);

//       if (matched[row][col]) {
//         // Paire trouvée : fond vert discret
//         screen->fillRoundRect(x, y, cardW, cardH, 4, TFT_DARKGREY);
//         screen->drawRoundRect(x, y, cardW, cardH, 4, TFT_GREEN);
//       }
//       else if (flipped[row][col]) {
//         // Carte retournée : afficher la couleur/valeur
//         screen->fillRoundRect(x, y, cardW, cardH, 4, cardColors[cards[row][col]]);
//         screen->setTextColor(TFT_WHITE, cardColors[cards[row][col]]);
//         screen->setTextSize(2);
//         screen->setCursor(x + cardW/2 - 6, y + cardH/2 - 8);
//         screen->print(cards[row][col] + 1);
//       }
//       else {
//         // Carte cachée : dos bleu
//         screen->fillRoundRect(x, y, cardW, cardH, 4, TFT_NAVY);
//         screen->drawRoundRect(x, y, cardW, cardH, 4, TFT_WHITE);
//         // Motif dos de carte
//         screen->drawRoundRect(x+3, y+3, cardW-6, cardH-6, 3, TFT_BLUE);
//       }

//       // Curseur : bordure jaune
//       if (col == curCol && row == curRow) {
//         screen->drawRect(x - 1, y - 1, cardW + 2, cardH + 2, TFT_YELLOW);
//       }
//     }

//   public:
//     MemoryGame(TFT_eSPI* display) : Game(display) {}

//     void init() override {
//       curCol        = 0;
//       curRow        = 0;
//       prevCurCol    = 0;
//       prevCurRow    = 0;
//       firstCol      = -1;
//       firstRow      = -1;
//       waitingSecond = false;
//       pairsFound    = 0;
//       showingPair   = false;
//       score         = 0;
//       state         = IN_PROGRESS;
//       needsRedraw   = true;

//       for (int r = 0; r < ROWS; r++)
//         for (int c = 0; c < COLS; c++) {
//           flipped[r][c] = false;
//           matched[r][c] = false;
//         }

//       shuffleCards();
//     }

//     void update(Buttons buttons) override {
//       if (state == GAME_OVER) return;

//       // ── Attente entre 2 cartes retournées ──
//       if (showingPair) {
//         if (millis() - showStart > 1000) {
//           showingPair = false;
//           // Cacher les cartes si pas de paire
//           if (cards[firstRow][firstCol] != cards[curRow][curCol]) {
//             flipped[firstRow][firstCol] = false;
//             flipped[curRow][curCol]     = false;
//           }
//           waitingSecond = false;
//           firstCol = -1;
//           firstRow = -1;
//           needsRedraw = true;
//         }
//         return;   // bloquer les inputs pendant l'affichage
//       }

//       bool changed = false;

//       // ── Déplacement curseur ──
//       prevCurCol = curCol;
//       prevCurRow = curRow;

//       if (buttons.upPressed    && curRow > 0)        { curRow--;  changed = true; }
//       if (buttons.downPressed  && curRow < ROWS - 1) { curRow++;  changed = true; }
//       if (buttons.leftPressed  && curCol > 0)        { curCol--;  changed = true; }
//       if (buttons.rightPressed && curCol < COLS - 1) { curCol++;  changed = true; }

//       // ── Retourner une carte ──
//       if (buttons.aPressed && !flipped[curRow][curCol] && !matched[curRow][curCol]) {
//         flipped[curRow][curCol] = true;
//         changed = true;

//         if (!waitingSecond) {
//           // Première carte
//           firstCol      = curCol;
//           firstRow      = curRow;
//           waitingSecond = true;
//         } else {
//           // Deuxième carte — vérifier la paire
//           showingPair = true;
//           showStart   = millis();

//           if (cards[firstRow][firstCol] == cards[curRow][curCol]) {
//             // Paire trouvée !
//             matched[firstRow][firstCol] = true;
//             matched[curRow][curCol]     = true;
//             pairsFound++;
//             score += 10;

//             if (pairsFound == ROWS * COLS / 2) {
//               state = GAME_OVER;   // toutes les paires trouvées !
//             }
//           }
//         }
//       }

//       if (changed) needsRedraw = true;
//     }

//     void render() override {
//       if (!needsRedraw) return;
//       needsRedraw = false;

//       screen->fillScreen(TFT_BLACK);

//       // ── Dessiner toutes les cartes ──
//       for (int r = 0; r < ROWS; r++)
//         for (int c = 0; c < COLS; c++)
//           drawCard(c, r);

//       // ── Score et paires ──
//       screen->setTextColor(TFT_WHITE, TFT_BLACK);
//       screen->setTextSize(1);
//       screen->setCursor(2, 4);
//       screen->print("PAIRES:");
//       screen->print(pairsFound);
//       screen->print("/6");
//     }

//     virtual ~MemoryGame() {}
// };

// #endif


#ifndef MEMORY_H
#define MEMORY_H

#include "Game.h"

// ════════════════════════════════════════════════
//  MEMORY — Retourner les paires de cartes
//  Boutons : HAUT/BAS/GAUCHE/DROITE curseur, A retourner
//  Game Over : toutes les paires trouvées (victoire)
// ════════════════════════════════════════════════

class MemoryGame : public Game {
  private:

    // ── Grille de cartes ──
    const int COLS  = 4;    // colonnes
    const int ROWS  = 3;    // rangées (= 12 cartes = 6 paires)
    const int cardW = 50;   // largeur d'une carte
    const int cardH = 36;   // hauteur d'une carte
    const int offX  = 10;   // décalage gauche
    const int offY  = 18;   // décalage haut

    // ── Valeurs des cartes (6 paires de 0 à 5) ──
    int  cards[3][4];        // valeur de chaque carte
    bool flipped[3][4];      // retournée ?
    bool matched[3][4];      // trouvée (paire validée) ?

    // ── Curseur ──
    int curCol, curRow;      // position du curseur
    int prevCurCol, prevCurRow;

    // ── Logique de jeu ──
    int  firstCol,  firstRow;   // première carte retournée
    bool waitingSecond;          // attend la 2e carte ?
    int  pairsFound;             // paires trouvées
    bool showingPair;            // affiche 2 cartes retournées ?
    unsigned long showStart;     // timestamp d'affichage

    bool needsRedraw;
    bool firstDraw;           // premier dessin complet
    bool dirtyCards[3][4];    // cartes à redessiner individuellement

    // ── Couleurs des valeurs ──
    uint16_t cardColors[6] = {
      TFT_RED, TFT_GREEN, TFT_BLUE,
      TFT_YELLOW, TFT_CYAN, TFT_MAGENTA
    };

    // Marquer une carte comme à redessiner
    void markDirty(int col, int row) {
      if (row >= 0 && row < ROWS && col >= 0 && col < COLS)
        dirtyCards[row][col] = true;
    }

    // Tout marquer comme sale (premier dessin)
    void markAllDirty() {
      for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
          dirtyCards[r][c] = true;
    }
    // ────────────────────────────────────────���───
    void shuffleCards() {
      int flat[12];
      // Créer les paires
      for (int i = 0; i < 6; i++) { flat[i] = i; flat[i+6] = i; }
      // Mélanger
      for (int i = 11; i > 0; i--) {
        int j = random(0, i + 1);
        int tmp = flat[i]; flat[i] = flat[j]; flat[j] = tmp;
      }
      // Remplir la grille
      for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
          cards[r][c] = flat[r * COLS + c];
    }

    // ────────────────────────────────────────────
    //  Dessiner une carte
    // ────────────────────────────────────────────
    void drawCard(int col, int row) {
      int x = offX + col * (cardW + 2);
      int y = offY + row * (cardH + 2);

      if (matched[row][col]) {
        // Paire trouvée : fond vert discret
        screen->fillRoundRect(x, y, cardW, cardH, 4, TFT_DARKGREY);
        screen->drawRoundRect(x, y, cardW, cardH, 4, TFT_GREEN);
      }
      else if (flipped[row][col]) {
        // Carte retournée : afficher la couleur/valeur
        screen->fillRoundRect(x, y, cardW, cardH, 4, cardColors[cards[row][col]]);
        screen->setTextColor(TFT_WHITE, cardColors[cards[row][col]]);
        screen->setTextSize(2);
        screen->setCursor(x + cardW/2 - 6, y + cardH/2 - 8);
        screen->print(cards[row][col] + 1);
      }
      else {
        // Carte cachée : dos bleu
        screen->fillRoundRect(x, y, cardW, cardH, 4, TFT_NAVY);
        screen->drawRoundRect(x, y, cardW, cardH, 4, TFT_WHITE);
        // Motif dos de carte
        screen->drawRoundRect(x+3, y+3, cardW-6, cardH-6, 3, TFT_BLUE);
      }

      // Curseur : bordure jaune
      if (col == curCol && row == curRow) {
        screen->drawRect(x - 1, y - 1, cardW + 2, cardH + 2, TFT_YELLOW);
      }
    }

  public:
    MemoryGame(TFT_eSPI* display) : Game(display) {}

    void init() override {
      curCol        = 0;
      curRow        = 0;
      prevCurCol    = 0;
      prevCurRow    = 0;
      firstCol      = -1;
      firstRow      = -1;
      waitingSecond = false;
      pairsFound    = 0;
      showingPair   = false;
      score         = 0;
      state         = IN_PROGRESS;
      needsRedraw   = true;
      firstDraw     = true;

      for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++) {
          flipped[r][c]   = false;
          matched[r][c]   = false;
          dirtyCards[r][c] = false;
        }

      shuffleCards();
    }

    void update(Buttons buttons) override {
      if (state == GAME_OVER || state == GAME_WIN) return;

      // ── Attente entre 2 cartes retournées ──
      if (showingPair) {
        if (millis() - showStart > 1000) {
          showingPair = false;
          // Cacher les cartes si pas de paire
          if (cards[firstRow][firstCol] != cards[curRow][curCol]) {
            flipped[firstRow][firstCol] = false;
            flipped[curRow][curCol]     = false;
            markDirty(firstCol, firstRow);
            markDirty(curCol,   curRow);
          }
          waitingSecond = false;
          firstCol = -1;
          firstRow = -1;
          needsRedraw = true;
        }
        return;   // bloquer les inputs pendant l'affichage
      }

      // ── Déplacement curseur ──
      int oldCol = curCol;
      int oldRow = curRow;

      if (buttons.upPressed    && curRow > 0)        curRow--;
      if (buttons.downPressed  && curRow < ROWS - 1) curRow++;
      if (buttons.leftPressed  && curCol > 0)        curCol--;
      if (buttons.rightPressed && curCol < COLS - 1) curCol++;

      // Si le curseur a bougé : redessiner SEULEMENT les 2 cartes concernées
      if (oldCol != curCol || oldRow != curRow) {
        markDirty(oldCol, oldRow);   // ancienne position (effacer la bordure jaune)
        markDirty(curCol, curRow);   // nouvelle position (ajouter la bordure jaune)
        needsRedraw = true;
      }

      // ── Retourner une carte ──
      if (buttons.aPressed && !flipped[curRow][curCol] && !matched[curRow][curCol]) {
        flipped[curRow][curCol] = true;
        markDirty(curCol, curRow);
        needsRedraw = true;

        if (!waitingSecond) {
          firstCol      = curCol;
          firstRow      = curRow;
          waitingSecond = true;
        } else {
          // Deuxième carte — vérifier la paire
          showingPair = true;
          showStart   = millis();

          if (cards[firstRow][firstCol] == cards[curRow][curCol]) {
            // Paire trouvée !
            matched[firstRow][firstCol] = true;
            matched[curRow][curCol]     = true;
            markDirty(firstCol, firstRow);
            markDirty(curCol,   curRow);
            pairsFound++;
            score += 10;

            if (pairsFound == ROWS * COLS / 2) {
              state = GAME_WIN;
            }
          }
        }
      }
    }

    void render() override {
      if (!needsRedraw) return;
      needsRedraw = false;

      // ── Premier dessin : fond + toutes les cartes + score ──
      if (firstDraw) {
        firstDraw = false;
        screen->fillScreen(TFT_BLACK);
        markAllDirty();   // toutes les cartes à dessiner

        // Score initial
        screen->setTextColor(TFT_WHITE, TFT_BLACK);
        screen->setTextSize(1);
        screen->setCursor(2, 4);
        screen->print("PAIRES:0/6");
      }

      // ── Redessiner uniquement les cartes marquées ──
      for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
          if (dirtyCards[r][c]) {
            dirtyCards[r][c] = false;
            drawCard(c, r);
          }
        }
      }

      // ── Score et paires ──
      screen->setTextColor(TFT_WHITE, TFT_BLACK);
      screen->setTextSize(1);
      screen->setCursor(2, 4);
      screen->print("PAIRES:");
      screen->print(pairsFound);
      screen->print("/6");
    }

    virtual ~MemoryGame() {}
};

#endif