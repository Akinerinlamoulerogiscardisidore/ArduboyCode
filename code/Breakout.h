// #ifndef BREAKOUT_H
// #define BREAKOUT_H

// #include "Game.h"

// // ════════════════════════════════════════════════
// //  BREAKOUT — Casser les briques avec une balle
// //  Boutons : GAUCHE / DROITE pour la raquette
// //  Game Over : si la balle tombe en bas
// // ════════════════════════════════════════════════

// // ── Structure d'une brique ──
// struct Brick {
//   bool  alive;   // est-elle encore vivante ?
//   int   x, y;    // position en pixels
// };

// class BreakoutGame : public Game {
//   private:

//     // ── Dimensions des briques ──
//     const int COLS      = 10;   // nombre de colonnes
//     const int ROWS      = 4;    // nombre de rangées
//     const int brickW    = 22;   // largeur d'une brique
//     const int brickH    = 8;    // hauteur d'une brique
//     const int brickOffX = 2;    // décalage gauche
//     const int brickOffY = 20;   // décalage haut (laisser place au score)

//     Brick bricks[4][10];        // grille de briques [rangée][colonne]
//     int   bricksLeft;           // briques restantes

//     // ── Raquette ──
//     int paddleX;                // position X centre de la raquette
//     const int paddleY   = 125;  // position Y fixe (bas de l'écran)
//     const int paddleW   = 30;   // largeur de la raquette
//     const int paddleH   = 4;    // hauteur de la raquette
//     const int paddleSpd = 4;    // vitesse de déplacement

//     // ── Balle ──
//     int   ballX, ballY;         // position de la balle
//     float velX,  velY;          // vitesse
//     const int ballSize = 4;     // taille

//     // ── Rendu optimisé ──
//     int  prevBallX,   prevBallY;
//     int  prevPaddleX;
//     bool firstDraw;

//     // ── Timing ──
//     unsigned long lastUpdate;
//     const int updateInterval = 12;

//     // ────────────────────────────────────────────
//     //  Initialiser les briques en grille
//     // ────────────────────────────────────────────
//     void initBricks() {
//       bricksLeft = 0;
//       for (int r = 0; r < ROWS; r++) {
//         for (int c = 0; c < COLS; c++) {
//           bricks[r][c].alive = true;
//           bricks[r][c].x = brickOffX + c * (brickW + 2);
//           bricks[r][c].y = brickOffY + r * (brickH + 2);
//           bricksLeft++;
//         }
//       }
//     }

//     // ────────────────────────────────────────────
//     //  Couleur d'une rangée — dégradé arc-en-ciel
//     // ────────────────────────────────────────────
//     uint16_t rowColor(int row) {
//       switch (row) {
//         case 0: return TFT_RED;
//         case 1: return TFT_ORANGE;
//         case 2: return TFT_YELLOW;
//         case 3: return TFT_GREEN;
//         default: return TFT_WHITE;
//       }
//     }

//   public:
//     BreakoutGame(TFT_eSPI* display) : Game(display) {}

//     void init() override {
//       paddleX      = 120;
//       ballX        = 120;
//       ballY        = 100;
//       velX         = 2.0;
//       velY         = -2.5;
//       score        = 0;
//       state        = IN_PROGRESS;
//       firstDraw    = true;
//       lastUpdate   = 0;
//       initBricks();
//     }

//     void update(Buttons buttons) override {
//       if (state == GAME_OVER) return;
//       if (millis() - lastUpdate < updateInterval) return;
//       lastUpdate = millis();

//       prevBallX   = ballX;
//       prevBallY   = ballY;
//       prevPaddleX = paddleX;

//       // ── Déplacement raquette ──
//       if (buttons.left)  paddleX -= paddleSpd;
//       if (buttons.right) paddleX += paddleSpd;
//       paddleX = constrain(paddleX, paddleW/2, 240 - paddleW/2);

//       // ── Déplacement balle ──
//       ballX += (int)velX;
//       ballY += (int)velY;

//       // ── Rebonds murs ──
//       if (ballX <= 0 || ballX >= 240 - ballSize) velX = -velX;
//       if (ballY <= 0) velY = abs(velY);

//       // ── Collision raquette ──
//       if (ballY + ballSize >= paddleY &&
//           ballX + ballSize >= paddleX - paddleW/2 &&
//           ballX <= paddleX + paddleW/2) {
//         velY = -abs(velY);
//         // Angle selon impact sur la raquette
//         velX = (ballX - paddleX) * 0.1;
//       }

//       // ── Game Over : balle tomb��e ──
//       if (ballY > 135) { state = GAME_OVER; return; }

//       // ── Collision briques ──
//       for (int r = 0; r < ROWS; r++) {
//         for (int c = 0; c < COLS; c++) {
//           if (!bricks[r][c].alive) continue;

//           int bx = bricks[r][c].x;
//           int by = bricks[r][c].y;

//           if (ballX + ballSize >= bx && ballX <= bx + brickW &&
//               ballY + ballSize >= by && ballY <= by + brickH) {

//             // Effacer la brique
//             screen->fillRect(bx, by, brickW, brickH, TFT_BLACK);
//             bricks[r][c].alive = false;
//             bricksLeft--;
//             score++;
//             velY = -velY;   // rebond vertical

//             // Gagné si plus de briques
//             if (bricksLeft == 0) { state = GAME_OVER; return; }
//           }
//         }
//       }
//     }

//     void render() override {

//       if (firstDraw) {
//         firstDraw = false;
//         screen->fillScreen(TFT_BLACK);

//         // Dessiner toutes les briques
//         for (int r = 0; r < ROWS; r++) {
//           for (int c = 0; c < COLS; c++) {
//             if (bricks[r][c].alive) {
//               screen->fillRect(
//                 bricks[r][c].x, bricks[r][c].y,
//                 brickW - 1, brickH - 1,
//                 rowColor(r)
//               );
//             }
//           }
//         }

//         // Score
//         screen->setTextColor(TFT_WHITE, TFT_BLACK);
//         screen->setTextSize(1);
//         screen->setCursor(2, 4);
//         screen->print("SCORE:");
//         screen->print(score);

//         // Raquette
//         screen->fillRect(paddleX - paddleW/2, paddleY, paddleW, paddleH, TFT_CYAN);

//         // Balle
//         screen->fillRect(ballX, ballY, ballSize, ballSize, TFT_WHITE);
//         return;
//       }

//       // ── Effacer ancienne balle et raquette ──
//       screen->fillRect(prevBallX,   prevBallY,            ballSize, ballSize, TFT_BLACK);
//       screen->fillRect(prevPaddleX - paddleW/2, paddleY,  paddleW,  paddleH,  TFT_BLACK);

//       // ── Dessiner nouvelle balle et raquette ──
//       screen->fillRect(ballX,   ballY,                    ballSize, ballSize, TFT_WHITE);
//       screen->fillRect(paddleX - paddleW/2, paddleY,      paddleW,  paddleH,  TFT_CYAN);

//       // ── Score ──
//       screen->setTextColor(TFT_WHITE, TFT_BLACK);
//       screen->setTextSize(1);
//       screen->setCursor(2, 4);
//       screen->print("SCORE:");
//       screen->print(score);
//       screen->print("  ");
//     }

//     virtual ~BreakoutGame() {}
// };

// #endif


#ifndef BREAKOUT_H
#define BREAKOUT_H

#include "Game.h"

// ════════════════════════════════════════════════
//  BREAKOUT — Casser les briques avec une balle
//  Boutons : GAUCHE / DROITE pour la raquette
//  Game Over : si la balle tombe en bas
// ════════════════════════════════════════════════

// ── Structure d'une brique ──
struct Brick {
  bool  alive;   // est-elle encore vivante ?
  int   x, y;    // position en pixels
};

class BreakoutGame : public Game {
  private:

    // ── Dimensions des briques ──
    const int COLS      = 10;   // nombre de colonnes
    const int ROWS      = 4;    // nombre de rangées
    const int brickW    = 22;   // largeur d'une brique
    const int brickH    = 8;    // hauteur d'une brique
    const int brickOffX = 2;    // décalage gauche
    const int brickOffY = 20;   // décalage haut (laisser place au score)

    Brick bricks[4][10];        // grille de briques [rangée][colonne]
    int   bricksLeft;           // briques restantes

    // ── Raquette ──
    int paddleX;                // position X centre de la raquette
    const int paddleY   = 125;  // position Y fixe (bas de l'écran)
    const int paddleW   = 30;   // largeur de la raquette
    const int paddleH   = 4;    // hauteur de la raquette
    const int paddleSpd = 4;    // vitesse de déplacement

    // ── Balle ──
    int   ballX, ballY;         // position de la balle
    float velX,  velY;          // vitesse
    const int ballSize = 4;     // taille

    // ── Rendu optimisé ──
    int  prevBallX,   prevBallY;
    int  prevPaddleX;
    bool firstDraw;

    // ── Timing ──
    unsigned long lastUpdate;
    const int updateInterval = 12;

    // ────────────────────────────────────────────
    //  Initialiser les briques en grille
    // ────────────────────────────────────────────
    void initBricks() {
      bricksLeft = 0;
      for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
          bricks[r][c].alive = true;
          bricks[r][c].x = brickOffX + c * (brickW + 2);
          bricks[r][c].y = brickOffY + r * (brickH + 2);
          bricksLeft++;
        }
      }
    }

    // ────────────────────────────────────────────
    //  Couleur d'une rangée — dégradé arc-en-ciel
    // ────────────────────────────────────────────
    uint16_t rowColor(int row) {
      switch (row) {
        case 0: return TFT_RED;
        case 1: return TFT_ORANGE;
        case 2: return TFT_YELLOW;
        case 3: return TFT_GREEN;
        default: return TFT_WHITE;
      }
    }

  public:
    BreakoutGame(TFT_eSPI* display) : Game(display) {}

    void init() override {
      paddleX      = 120;
      ballX        = 120;
      ballY        = 100;
      velX         = 2.0;
      velY         = -2.5;
      score        = 0;
      state        = IN_PROGRESS;
      firstDraw    = true;
      lastUpdate   = 0;
      initBricks();
    }

    void update(Buttons buttons) override {
      if (state == GAME_OVER) return;
      if (millis() - lastUpdate < updateInterval) return;
      lastUpdate = millis();

      prevBallX   = ballX;
      prevBallY   = ballY;
      prevPaddleX = paddleX;

      // ── Déplacement raquette ──
      if (buttons.left)  paddleX -= paddleSpd;
      if (buttons.right) paddleX += paddleSpd;
      paddleX = constrain(paddleX, paddleW/2, 240 - paddleW/2);

      // ── Déplacement balle ──
      ballX += (int)velX;
      ballY += (int)velY;

      // ── Rebonds murs ──
      if (ballX <= 0 || ballX >= 240 - ballSize) velX = -velX;
      if (ballY <= 0) velY = abs(velY);

      // ── Collision raquette ──
      if (ballY + ballSize >= paddleY &&
          ballX + ballSize >= paddleX - paddleW/2 &&
          ballX <= paddleX + paddleW/2) {
        velY = -abs(velY);
        // Angle selon impact sur la raquette
        velX = (ballX - paddleX) * 0.1;
      }

      // ── Game Over : balle tomb��e ──
      if (ballY > 135) { state = GAME_OVER; return; }

      // ── Collision briques ──
      for (int r = 0; r < ROWS; r++) {
        for (int c = 0; c < COLS; c++) {
          if (!bricks[r][c].alive) continue;

          int bx = bricks[r][c].x;
          int by = bricks[r][c].y;

          if (ballX + ballSize >= bx && ballX <= bx + brickW &&
              ballY + ballSize >= by && ballY <= by + brickH) {

            // Effacer la brique
            screen->fillRect(bx, by, brickW, brickH, TFT_BLACK);
            bricks[r][c].alive = false;
            bricksLeft--;
            score++;
            velY = -velY;   // rebond vertical

            // Gagné si plus de briques
            if (bricksLeft == 0) { state = GAME_WIN; return; }
          }
        }
      }
    }

    void render() override {

      if (firstDraw) {
        firstDraw = false;
        screen->fillScreen(TFT_BLACK);

        // Dessiner toutes les briques
        for (int r = 0; r < ROWS; r++) {
          for (int c = 0; c < COLS; c++) {
            if (bricks[r][c].alive) {
              screen->fillRect(
                bricks[r][c].x, bricks[r][c].y,
                brickW - 1, brickH - 1,
                rowColor(r)
              );
            }
          }
        }

        // Score
        screen->setTextColor(TFT_WHITE, TFT_BLACK);
        screen->setTextSize(1);
        screen->setCursor(2, 4);
        screen->print("SCORE:");
        screen->print(score);

        // Raquette
        screen->fillRect(paddleX - paddleW/2, paddleY, paddleW, paddleH, TFT_CYAN);

        // Balle
        screen->fillRect(ballX, ballY, ballSize, ballSize, TFT_WHITE);
        return;
      }

      // ── Effacer ancienne balle et raquette ──
      screen->fillRect(prevBallX,   prevBallY,            ballSize, ballSize, TFT_BLACK);
      screen->fillRect(prevPaddleX - paddleW/2, paddleY,  paddleW,  paddleH,  TFT_BLACK);

      // ── Dessiner nouvelle balle et raquette ──
      screen->fillRect(ballX,   ballY,                    ballSize, ballSize, TFT_WHITE);
      screen->fillRect(paddleX - paddleW/2, paddleY,      paddleW,  paddleH,  TFT_CYAN);

      // ── Score ──
      screen->setTextColor(TFT_WHITE, TFT_BLACK);
      screen->setTextSize(1);
      screen->setCursor(2, 4);
      screen->print("SCORE:");
      screen->print(score);
      screen->print("  ");
    }

    virtual ~BreakoutGame() {}
};

#endif