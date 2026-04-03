// // #ifndef MAZE_H
// // #define MAZE_H

// // #include "Game.h"

// // // ════════════════════════════════════════════════
// // //  MAZE — Trouver la sortie du labyrinthe
// // //  Boutons : HAUT/BAS/GAUCHE/DROITE pour se déplacer
// // //  Game Over : atteindre la sortie (victoire !)
// // // ════════════════════════════════════════════════

// // class MazeGame : public Game {
// //   private:

// //     // ── Dimensions de la grille ──
// //     const int COLS  = 15;   // colonnes du labyrinthe
// //     const int ROWS  = 9;    // rangées du labyrinthe
// //     const int CELL  = 14;   // taille d'une case en pixels
// //     const int offX  = 9;    // décalage X pour centrer (240 - 15*14) / 2
// //     const int offY  = 16;   // décalage Y (place pour le score)

// //     // ── Grille : 1 = mur, 0 = chemin ──
// //     // Labyrinthe pré-défini (modifiable)
// //     const int mazeMap[9][15] = {
// //       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
// //       {1,0,0,0,1,0,0,0,0,0,1,0,0,0,1},
// //       {1,0,1,0,1,0,1,1,1,0,1,0,1,0,1},
// //       {1,0,1,0,0,0,0,0,1,0,0,0,1,0,1},
// //       {1,0,1,1,1,1,1,0,1,1,1,0,1,0,1},
// //       {1,0,0,0,0,0,1,0,0,0,1,0,0,0,1},
// //       {1,1,1,0,1,0,1,0,1,0,1,1,1,0,1},
// //       {1,0,0,0,1,0,0,0,1,0,0,0,0,0,0},  // ← sortie à droite (col 14)
// //       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
// //     };

// //     // ── Joueur ──
// //     int playerCol, playerRow;   // position dans la grille
// //     int prevCol,   prevRow;     // ancienne position
// //     bool needsRedraw;

// //     // ── Timer ──
// //     unsigned long startTime;    // pour calculer le temps de résolution

// //   public:
// //     MazeGame(TFT_eSPI* display) : Game(display) {}

// //     void init() override {
// //       playerCol  = 1;       // départ en haut à gauche
// //       playerRow  = 1;
// //       prevCol    = 1;
// //       prevRow    = 1;
// //       score      = 0;
// //       state      = IN_PROGRESS;
// //       needsRedraw = true;
// //       startTime  = millis();
// //     }

// //     void update(Buttons buttons) override {
// //       if (state == GAME_OVER) return;

// //       int newCol = playerCol;
// //       int newRow = playerRow;

// //       // ── Déplacement ──
// //       if (buttons.upPressed)    newRow--;
// //       if (buttons.downPressed)  newRow++;
// //       if (buttons.leftPressed)  newCol--;
// //       if (buttons.rightPressed) newCol++;

// //       // ── Vérifier si le mouvement est possible ──
// //       if (newCol != playerCol || newRow != playerRow) {
// //         // Sortie du labyrinthe ?
// //         if (newCol >= COLS || newRow >= ROWS || newCol < 0 || newRow < 0) {
// //           // Victoire !
// //           score = max(0, 999 - (int)((millis() - startTime) / 1000));
// //           state = GAME_OVER;
// //           return;
// //         }
// //         // Chemin libre (pas un mur) ?
// //         if (mazeMap[newRow][newCol] == 0) {
// //           prevCol   = playerCol;
// //           prevRow   = playerRow;
// //           playerCol = newCol;
// //           playerRow = newRow;
// //           needsRedraw = true;
// //         }
// //       }
// //     }

// //     void render() override {
// //       if (!needsRedraw) return;
// //       needsRedraw = false;

// //       // Premier dessin : tout le labyrinthe
// //       static bool fullDraw = true;
// //       if (fullDraw) {
// //         fullDraw = false;
// //         screen->fillScreen(TFT_BLACK);

// //         // Dessiner les murs
// //         for (int r = 0; r < ROWS; r++) {
// //           for (int c = 0; c < COLS; c++) {
// //             if (mazeMap[r][c] == 1) {
// //               screen->fillRect(
// //                 offX + c * CELL, offY + r * CELL,
// //                 CELL, CELL, TFT_BLUE
// //               );
// //             }
// //           }
// //         }

// //         // Marquer la sortie
// //         screen->fillRect(offX + 14 * CELL, offY + 7 * CELL, CELL, CELL, TFT_YELLOW);

// //         // Titre
// //         screen->setTextColor(TFT_WHITE, TFT_BLACK);
// //         screen->setTextSize(1);
// //         screen->setCursor(2, 4);
// //         screen->print("TROUVE LA SORTIE !");
// //       }

// //       // ── Effacer ancienne position joueur ──
// //       screen->fillRect(
// //         offX + prevCol * CELL + 1, offY + prevRow * CELL + 1,
// //         CELL - 2, CELL - 2,
// //         TFT_BLACK
// //       );

// //       // ── Dessiner nouvelle position joueur ──
// //       screen->fillRect(
// //         offX + playerCol * CELL + 2, offY + playerRow * CELL + 2,
// //         CELL - 4, CELL - 4,
// //         TFT_RED
// //       );
// //     }

// //     virtual ~MazeGame() {
// //       // Réinitialiser fullDraw pour la prochaine partie
// //     }
// // };

// // #endif

// #ifndef MAZE_H
// #define MAZE_H

// #include "Game.h"

// // ════════════════════════════════════════════════
// //  MAZE — Trouver la sortie du labyrinthe
// //  Boutons : HAUT/BAS/GAUCHE/DROITE pour se déplacer
// //  Game Over : atteindre la sortie (victoire !)
// // ════════════════════════════════════════════════

// class MazeGame : public Game {
//   private:

//     // ── Dimensions de la grille ──
//     const int COLS  = 15;   // colonnes du labyrinthe
//     const int ROWS  = 9;    // rangées du labyrinthe
//     const int CELL  = 14;   // taille d'une case en pixels
//     const int offX  = 9;    // décalage X pour centrer (240 - 15*14) / 2
//     const int offY  = 16;   // décalage Y (place pour le score)

//     // ── Grille : 1 = mur, 0 = chemin ──
//     // Labyrinthe pré-défini (modifiable)
//     const int mazeMap[9][15] = {
//       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
//       {1,0,0,0,1,0,0,0,0,0,1,0,0,0,1},
//       {1,0,1,0,1,0,1,1,1,0,1,0,1,0,1},
//       {1,0,1,0,0,0,0,0,1,0,0,0,1,0,1},
//       {1,0,1,1,1,1,1,0,1,1,1,0,1,0,1},
//       {1,0,0,0,0,0,1,0,0,0,1,0,0,0,1},
//       {1,1,1,0,1,0,1,0,1,0,1,1,1,0,1},
//       {1,0,0,0,1,0,0,0,1,0,0,0,0,0,0},  // ← sortie à droite (col 14)
//       {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
//     };

//     // ── Joueur ──
//     int playerCol, playerRow;   // position dans la grille
//     int prevCol,   prevRow;     // ancienne position
//     bool needsRedraw;

//     bool firstDraw;             // premier dessin complet ?

//     // ── Timer ──
//     unsigned long startTime;    // pour calculer le temps de résolution

//   public:
//     MazeGame(TFT_eSPI* display) : Game(display) {}

//     void init() override {
//       playerCol  = 1;       // départ en haut à gauche
//       playerRow  = 1;
//       prevCol    = 1;
//       prevRow    = 1;
//       score      = 0;
//       state      = IN_PROGRESS;
//       needsRedraw = true;
//       firstDraw  = true;        // ← forcer dessin complet à chaque nouvelle partie
//       startTime  = millis();
//     }

//     void update(Buttons buttons) override {
//       if (state == GAME_OVER) return;

//       int newCol = playerCol;
//       int newRow = playerRow;

//       // ── Déplacement ──
//       if (buttons.upPressed)    newRow--;
//       if (buttons.downPressed)  newRow++;
//       if (buttons.leftPressed)  newCol--;
//       if (buttons.rightPressed) newCol++;

//       // ── Vérifier si le mouvement est possible ──
//       if (newCol != playerCol || newRow != playerRow) {
//         // Sortie du labyrinthe ?
//         if (newCol >= COLS || newRow >= ROWS || newCol < 0 || newRow < 0) {
//           // Victoire !
//           score = max(0, 999 - (int)((millis() - startTime) / 1000));
//           state = GAME_OVER;
//           return;
//         }
//         // Chemin libre (pas un mur) ?
//         if (mazeMap[newRow][newCol] == 0) {
//           prevCol   = playerCol;
//           prevRow   = playerRow;
//           playerCol = newCol;
//           playerRow = newRow;
//           needsRedraw = true;
//         }
//       }
//     }

//     void render() override {
//       if (!needsRedraw) return;
//       needsRedraw = false;

//       // Premier dessin : tout le labyrinthe
//       if (firstDraw) {
//         firstDraw = false;
//         screen->fillScreen(TFT_BLACK);

//         // Dessiner les murs
//         for (int r = 0; r < ROWS; r++) {
//           for (int c = 0; c < COLS; c++) {
//             if (mazeMap[r][c] == 1) {
//               screen->fillRect(
//                 offX + c * CELL, offY + r * CELL,
//                 CELL, CELL, TFT_BLUE
//               );
//             }
//           }
//         }

//         // Marquer la sortie
//         screen->fillRect(offX + 14 * CELL, offY + 7 * CELL, CELL, CELL, TFT_YELLOW);

//         // Titre
//         screen->setTextColor(TFT_WHITE, TFT_BLACK);
//         screen->setTextSize(1);
//         screen->setCursor(2, 4);
//         screen->print("TROUVE LA SORTIE !");
//       }

//       // ── Effacer ancienne position joueur ──
//       screen->fillRect(
//         offX + prevCol * CELL + 1, offY + prevRow * CELL + 1,
//         CELL - 2, CELL - 2,
//         TFT_BLACK
//       );

//       // ── Dessiner nouvelle position joueur ──
//       screen->fillRect(
//         offX + playerCol * CELL + 2, offY + playerRow * CELL + 2,
//         CELL - 4, CELL - 4,
//         TFT_RED
//       );
//     }

//     virtual ~MazeGame() {}
// };

// #endif


#ifndef MAZE_H
#define MAZE_H

#include "Game.h"

// ════════════════════════════════════════════════
//  MAZE — Trouver la sortie du labyrinthe
//  Boutons : HAUT/BAS/GAUCHE/DROITE pour se déplacer
//  Game Over : atteindre la sortie (victoire !)
// ════════════════════════════════════════════════

class MazeGame : public Game {
  private:

    // ── Dimensions de la grille ──
    const int COLS  = 15;   // colonnes du labyrinthe
    const int ROWS  = 9;    // rangées du labyrinthe
    const int CELL  = 14;   // taille d'une case en pixels
    const int offX  = 9;    // décalage X pour centrer (240 - 15*14) / 2
    const int offY  = 16;   // décalage Y (place pour le score)

    // ── Grille : 1 = mur, 0 = chemin ──
    // Labyrinthe pré-défini (modifiable)
    const int mazeMap[9][15] = {
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
      {1,0,0,0,1,0,0,0,0,0,1,0,0,0,1},
      {1,0,1,0,1,0,1,1,1,0,1,0,1,0,1},
      {1,0,1,0,0,0,0,0,1,0,0,0,1,0,1},
      {1,0,1,1,1,1,1,0,1,1,1,0,1,0,1},
      {1,0,0,0,0,0,1,0,0,0,1,0,0,0,1},
      {1,1,1,0,1,0,1,0,1,0,1,1,1,0,1},
      {1,0,0,0,1,0,0,0,1,0,0,0,0,0,0},  // ← sortie à droite (col 14)
      {1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
    };

    // ── Joueur ──
    int playerCol, playerRow;   // position dans la grille
    int prevCol,   prevRow;     // ancienne position
    bool needsRedraw;

    bool firstDraw;             // premier dessin complet ?

    // ── Timer ──
    unsigned long startTime;    // pour calculer le temps de résolution

  public:
    MazeGame(TFT_eSPI* display) : Game(display) {}

    void init() override {
      playerCol  = 1;       // départ en haut à gauche
      playerRow  = 1;
      prevCol    = 1;
      prevRow    = 1;
      score      = 0;
      state      = IN_PROGRESS;
      needsRedraw = true;
      firstDraw  = true;        // ← forcer dessin complet à chaque nouvelle partie
      startTime  = millis();
    }

    void update(Buttons buttons) override {
      if (state == GAME_OVER) return;

      int newCol = playerCol;
      int newRow = playerRow;

      // ── Déplacement ──
      if (buttons.upPressed)    newRow--;
      if (buttons.downPressed)  newRow++;
      if (buttons.leftPressed)  newCol--;
      if (buttons.rightPressed) newCol++;

      // ── Vérifier si le mouvement est possible ──
      if (newCol != playerCol || newRow != playerRow) {
        // Sortie du labyrinthe ?
        if (newCol >= COLS || newRow >= ROWS || newCol < 0 || newRow < 0) {
          // Victoire !
          score = max(0, 999 - (int)((millis() - startTime) / 1000));
          state = GAME_WIN;
          return;
        }
        // Chemin libre (pas un mur) ?
        if (mazeMap[newRow][newCol] == 0) {
          prevCol   = playerCol;
          prevRow   = playerRow;
          playerCol = newCol;
          playerRow = newRow;
          needsRedraw = true;
        }
      }
    }

    void render() override {
      if (!needsRedraw) return;
      needsRedraw = false;

      // Premier dessin : tout le labyrinthe
      if (firstDraw) {
        firstDraw = false;
        screen->fillScreen(TFT_BLACK);

        // Dessiner les murs
        for (int r = 0; r < ROWS; r++) {
          for (int c = 0; c < COLS; c++) {
            if (mazeMap[r][c] == 1) {
              screen->fillRect(
                offX + c * CELL, offY + r * CELL,
                CELL, CELL, TFT_BLUE
              );
            }
          }
        }

        // Marquer la sortie
        screen->fillRect(offX + 14 * CELL, offY + 7 * CELL, CELL, CELL, TFT_YELLOW);

        // Titre
        screen->setTextColor(TFT_WHITE, TFT_BLACK);
        screen->setTextSize(1);
        screen->setCursor(2, 4);
        screen->print("TROUVE LA SORTIE !");
      }

      // ── Effacer ancienne position joueur ──
      screen->fillRect(
        offX + prevCol * CELL + 1, offY + prevRow * CELL + 1,
        CELL - 2, CELL - 2,
        TFT_BLACK
      );

      // ── Dessiner nouvelle position joueur ──
      screen->fillRect(
        offX + playerCol * CELL + 2, offY + playerRow * CELL + 2,
        CELL - 4, CELL - 4,
        TFT_RED
      );
    }

    virtual ~MazeGame() {}
};

#endif