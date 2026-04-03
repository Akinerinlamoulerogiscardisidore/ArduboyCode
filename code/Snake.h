// // #ifndef SNAKE_H
// // #define SNAKE_H

// // #include "Game.h"

// // // Direction du serpent
// // enum snakeDirection {
// //   SNAKE_UP,
// //   SNAKE_DOWN, 
// //   SNAKE_LEFT,
// //   SNAKE_RIGHT
// // };


// // struct Segment {
// //   int x, y;
// // };

// // class SnakeGame : public Game {
// //   private: 
// //     Segment snake[100];
// //     int snakeLength;
// //     snakeDirection direction;
// //     int appleX, appleY;
// //     bool needsRedraw;

// //     const int gridSize = 8;
// //     const int gridW = 240 / 8; // Soit 30 cases sur la longueur
// //     const int gridH = 110 / 8; // Soit 13 cases 
// //     const int scoreHeight = 2; // Définition de constante de taille pour le mapping de la zone de jeu

// //     unsigned long lastMove;
// //     const int moveInterval = 150; // vitesse entre chaque mouvement

// //     // Apparition aléatoire de la pomme
// //     void spawnApple() {
// //       appleX = random(0, gridW);
// //       appleY = random(scoreHeight, gridH);
// //     }

// //     // Déplacer le serpent selon la direction choisie
// //     void moveSnake() {
// //       if (millis() - lastMove < moveInterval) return;
// //       lastMove = millis();

// //       // 
// //       for (int i = snakeLength - 1; i > 0; i-- ) {
// //         snake[i] = snake[i - 1];
// //       }

// //       switch (direction) {
// //         case SNAKE_UP: snake[0].y -= 1; 
// //         break;
// //         case SNAKE_DOWN: snake[0].y += 1;
// //         break;
// //         case SNAKE_LEFT: snake[0].x -= 1;
// //         break;
// //         case SNAKE_RIGHT: snake[0].x += 1;
// //         break;

// //       }

// //     }

// //     // Checker le GameOver
// //     void checkCollision () {
      
// //       // Murs à ne pas dépasser
// //       if (snake[0].x < 0 || snake[0].x >= gridW ||
// //        snake[0].y < scoreHeight || snake[0].y >= gridH ) {
// //         state = GAME_OVER; 
// //         return;
// //        }

// //       // Corps (commence à 1 )
// //       for (int i = 1; i < snakeLength; i++) {
// //         if ( snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
// //           state = GAME_OVER;
// //           return;
// //         }
// //       }
// //     }

// //     // Checker si la pomme est bien mangé
// //     void checkApple() {
// //       if (snake[0].x == appleX && snake[0].y == appleY) {
// //         snakeLength ++; // Faire grandir le serpent
// //         score ++; // Augmenter le score
// //         spawnApple(); // Apparition de la nouvelle pomme
// //       }
// //     }

// //     public: 
// //       SnakeGame(TFT_eSPI* display) : Game(display) {}
       
// //       void init () override {
// //         snakeLength = 3;
// //         direction = SNAKE_RIGHT;
// //         lastMove = 0;
// //         score = 0;
// //         state = IN_PROGRESS;
// //         needsRedraw = true;

// //         for (int i = 0; i < snakeLength; i++) {
// //           snake[i].x = (gridW / 2) - i;
// //           snake[i].y = scoreHeight + (gridH / 2);
// //         }

// //         spawnApple();

// //       }

// //       void update(Buttons buttons) override {
// //         if (state == GAME_OVER) return;

// //         if (buttons.up && direction != SNAKE_DOWN) direction = SNAKE_UP;
// //         if (buttons.down && direction != SNAKE_UP ) direction = SNAKE_DOWN;
// //         if (buttons.right && direction != SNAKE_LEFT) direction = SNAKE_RIGHT;
// //         if (buttons.left && direction != SNAKE_RIGHT) direction = SNAKE_LEFT;

// //         moveSnake();
// //         checkCollision();
// //         checkApple();
// //       }

// //       void render() override {
// //         // Effacer UNIQUEMENT la zone de jeu (pas le score)
// //         screen->fillRect(0, gridSize * scoreHeight, 240, 135 - gridSize * scoreHeight, TFT_BLACK);

// //         // Score
// //         displayScore();

// //         // Apple
// //         screen->fillRect(
// //           appleX * gridSize,
// //           appleY * gridSize,
// //           gridSize - 1,
// //           gridSize - 1,
// //           TFT_RED
// //         );

// //         // Snake
// //         for (int i = 0; i < snakeLength; i++) {
// //           screen->fillRect(
// //             snake[i].x * gridSize,
// //             snake[i].y * gridSize,
// //             gridSize - 1,
// //             gridSize - 1,
// //             i == 0 ? TFT_WHITE : TFT_GREEN
// //           );
// //         }
// //       }

// //       virtual ~SnakeGame() {}
// // };
// // #endif


// #ifndef SNAKE_H
// #define SNAKE_H

// #include "Game.h"

// // Direction du serpent
// enum snakeDirection {
//   SNAKE_UP,
//   SNAKE_DOWN, 
//   SNAKE_LEFT,
//   SNAKE_RIGHT
// };

// struct Segment {
//   int x, y;
// };

// class SnakeGame : public Game {
//   private: 
//     Segment snake[100];
//     int snakeLength;
//     snakeDirection direction;
//     int appleX, appleY;

//     Segment lastTail;        // ← ancienne queue à effacer
//     bool moved;              // ← le serpent a bougé ce frame ?
//     bool appleEaten;         // ← pomme mangée ce frame ?
//     bool firstDraw;          // ← premier dessin complet ?
//     bool needsRedraw;        // ← Ajouté : flag pour redessiner

//     const int gridSize = 8;
//     const int gridW = 30;    // 240 / 8 = 30 (inchangé, déjà correct)
//     const int gridH = 14;    // Ajusté : (135 - 20) / 8 = 14 (utilise plus d'espace vertical)
//     const int offsetX = 0;   // Centré horizontalement : (240 - 30*8) / 2 = 0 (inchangé)
//     const int offsetY = 20;  // Ajusté : laisse 20px en haut pour le score (au lieu de 16)

//     unsigned long lastMove;
//     const int moveInterval = 150;

//     void spawnApple() {
//       appleX = random(0, gridW);
//       appleY = random(0, gridH);  // Ajusté : commence à 0 (pas scoreHeight, car offsetY gère l'espace)
//     }

//     void moveSnake() {
//       if (millis() - lastMove < moveInterval) return;
//       lastMove = millis();

//       lastTail = snake[snakeLength - 1];   // ← sauvegarder AVANT de bouger

//       for (int i = snakeLength - 1; i > 0; i--) {
//         snake[i] = snake[i - 1];
//       }

//       switch (direction) {
//         case SNAKE_UP:    snake[0].y -= 1; break;
//         case SNAKE_DOWN:  snake[0].y += 1; break;
//         case SNAKE_LEFT:  snake[0].x -= 1; break;
//         case SNAKE_RIGHT: snake[0].x += 1; break;
//       }

//       moved = true;   // ← signaler que le serpent a bougé
//     }

//     void checkCollision() {
//       if (snake[0].x < 0 || snake[0].x >= gridW ||
//           snake[0].y < 0 || snake[0].y >= gridH) {  // Ajusté : snake[0].y < 0 (offsetY gère l'espace score)
//         state = GAME_OVER;
//         return;
//       }
//       for (int i = 1; i < snakeLength; i++) {
//         if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
//           state = GAME_OVER;
//           return;
//         }
//       }
//     }

//     void checkApple() {
//       if (snake[0].x == appleX && snake[0].y == appleY) {
//         snakeLength++;
//         score++;
//         appleEaten = true;   // ← signaler que la pomme a été mangée
//         spawnApple();
//       }
//     }

//   public: 
//     SnakeGame(TFT_eSPI* display) : Game(display) {}
     
//     void init() override {
//       snakeLength = 3;
//       direction   = SNAKE_RIGHT;
//       lastMove    = 0;
//       score       = 0;
//       state       = IN_PROGRESS;
//       moved       = false;
//       appleEaten  = false;
//       firstDraw   = true;   // ← forcer un dessin complet au démarrage
//       needsRedraw = true;   // ← Ajouté : initialiser

//       for (int i = 0; i < snakeLength; i++) {
//         snake[i].x = (gridW / 2) - i;
//         snake[i].y = (gridH / 2);  // Ajusté : centré verticalement dans la grille
//       }

//       spawnApple();
//     }

//     void update(Buttons buttons) override {
//       if (state == GAME_OVER) return;

//       if (buttons.up    && direction != SNAKE_DOWN)  direction = SNAKE_UP;
//       if (buttons.down  && direction != SNAKE_UP)    direction = SNAKE_DOWN;
//       if (buttons.right && direction != SNAKE_LEFT)  direction = SNAKE_RIGHT;
//       if (buttons.left  && direction != SNAKE_RIGHT) direction = SNAKE_LEFT;

//       moved      = false;   // ← reset avant moveSnake
//       appleEaten = false;   // ← reset avant checkApple

//       moveSnake();
//       checkCollision();
//       checkApple();
//     }

//     void render() override {
//       if (!needsRedraw) return;
//       needsRedraw = false;

//       // Premier dessin : tout dessiner proprement
//       if (firstDraw) {
//         firstDraw = false;
//         screen->fillScreen(TFT_BLACK);
//         displayScore();  // Score avec nouvelle couleur
//         screen->fillRect(
//           offsetX + appleX * gridSize, offsetY + appleY * gridSize,  // Ajusté : ajouter offsetX/Y
//           gridSize - 1,
//           gridSize - 1,
//           TFT_RED
//         );
//         for (int i = 0; i < snakeLength; i++) {
//           screen->fillRect(
//             offsetX + snake[i].x * gridSize, offsetY + snake[i].y * gridSize,  // Ajusté : ajouter offsetX/Y
//             gridSize - 1,
//             gridSize - 1,
//             i == 0 ? TFT_WHITE : TFT_GREEN
//           );
//         }
//         return;
//       }

//       // ── Pas de mouvement → rien à redessiner ──
//       if (!moved) return;

//       // ── Effacer UNIQUEMENT l'ancienne queue ──
//       // (sauf si la pomme a été mangée — la queue grandit, rien à effacer)
//       if (!appleEaten) {
//         screen->fillRect(
//           offsetX + lastTail.x * gridSize, offsetY + lastTail.y * gridSize,  // Ajusté : ajouter offsetX/Y
//           gridSize - 1,
//           gridSize - 1,
//           TFT_BLACK   // ← 1 seule case noire, imperceptible !
//         );
//       }

//       // ── Redessiner la nouvelle tête ──
//       screen->fillRect(
//         offsetX + snake[0].x * gridSize, offsetY + snake[0].y * gridSize,  // Ajusté : ajouter offsetX/Y
//         gridSize - 1,
//         gridSize - 1,
//         TFT_WHITE
//       );

//       // ── Redessiner le 2e segment (était blanc, devient vert) ──
//       if (snakeLength > 1) {
//         screen->fillRect(
//           offsetX + snake[1].x * gridSize, offsetY + snake[1].y * gridSize,  // Ajusté : ajouter offsetX/Y
//           gridSize - 1,
//           gridSize - 1,
//           TFT_GREEN
//         );
//       }

//       // ── Redessiner la pomme si mangée (nouvelle position) ──
//       if (appleEaten) {
//         // Effacer toute la zone de jeu UNE SEULE FOIS (ajusté à offsetY)
//         screen->fillRect(
//           0,
//           offsetY,  // Ajusté : utiliser offsetY au lieu de gridSize * scoreHeight
//           240,
//           135 - offsetY,  // Ajusté : hauteur restante
//           TFT_BLACK
//         );

//         // Redessiner le serpent entier proprement
//         for (int i = 0; i < snakeLength; i++) {
//           screen->fillRect(
//             offsetX + snake[i].x * gridSize, offsetY + snake[i].y * gridSize,  // Ajusté : ajouter offsetX/Y
//             gridSize - 1,
//             gridSize - 1,
//             i == 0 ? TFT_WHITE : TFT_GREEN
//           );
//         }

//         // Redessiner la pomme à sa nouvelle position
//         screen->fillRect(
//           offsetX + appleX * gridSize, offsetY + appleY * gridSize,  // Ajusté : ajouter offsetX/Y
//           gridSize - 1,
//           gridSize - 1,
//           TFT_RED
//         );

//         // Mettre à jour le score
//         displayScore();
//       }
//     }

//     virtual ~SnakeGame() {}
// };

// #endif




#ifndef SNAKE_H
#define SNAKE_H

#include "Game.h"

// ════════════════════════════════════════════════
//  SNAKE — Mange les pommes sans toucher les murs
//  Boutons : HAUT / BAS / GAUCHE / DROITE
//  Game Over : collision mur ou corps
// ════════════════════════════════════════════════

// ── Grille de jeu ──
#define GRID_SIZE  8
#define GRID_W    (240 / GRID_SIZE)   // 30 colonnes
#define GRID_H    (135 / GRID_SIZE)   // 16 rangées

// ── Zone de jeu (laisse 2 rangées pour le score en haut) ──
#define ZONE_TOP   2    // ligne de départ (en cases)
#define ZONE_LEFT  1    // colonne de départ (bordure gauche)
#define ZONE_RIGHT (GRID_W - 1)   // colonne de fin   (bordure droite)
#define ZONE_BOT   (GRID_H - 1)   // ligne de fin     (bordure bas)

struct SnakePoint {
  int x, y;
};

class SnakeGame : public Game {
  private:

    SnakePoint snake[200];   // corps du serpent (max 200 segments)
    int        snakeLength;  // longueur actuelle
    SnakePoint food;         // position de la pomme

    int dirX, dirY;          // direction courante
    int nextDirX, nextDirY;  // direction demandée (bufferisée)

    unsigned long lastMove;  // timestamp du dernier déplacement
    int  moveDelay;          // intervalle entre deux mouvements (ms)
                             // démarre à 150ms, descend jusqu'à 60ms

    SnakePoint lastTail;     // ancienne queue (pour effacement ciblé)
    bool moved;              // a bougé ce frame ?
    bool ateFood;            // a mangé ce frame ?
    bool firstDraw;          // premier dessin complet ?

    // ────────────────────────────────────────────
    //  Générer la pomme dans le terrain,
    //  jamais sur le corps du serpent
    // ────────────────────────────────────────────
    void spawnFood() {
      bool valid = false;
      while (!valid) {
        food.x = random(ZONE_LEFT + 1, ZONE_RIGHT);
        food.y = random(ZONE_TOP  + 1, ZONE_BOT);
        valid = true;
        for (int i = 0; i < snakeLength; i++) {
          if (snake[i].x == food.x && snake[i].y == food.y) {
            valid = false;
            break;
          }
        }
      }
    }

    // ────────────────────────────────────────────
    //  Dessiner la pomme (cercle rouge avec reflet)
    // ────────────────────────────────────────────
    void drawFood(uint16_t color) {
      int cx = food.x * GRID_SIZE + GRID_SIZE / 2;
      int cy = food.y * GRID_SIZE + GRID_SIZE / 2;
      screen->fillCircle(cx, cy, GRID_SIZE / 2 - 1, color);
      if (color != TFT_BLACK) {
        // Petit reflet blanc en haut à gauche
        screen->drawPixel(cx - 1, cy - 1, TFT_WHITE);
      }
    }

    // ────────────────────────────────────────────
    //  Dessiner un segment du serpent
    //  tête = jaune, corps = vert dégradé
    // ────────────────────────────────────────────
    void drawSegment(int x, int y, int idx, uint16_t color) {
      // Un léger margin de 1px pour voir les segments séparés
      screen->fillRect(
        x * GRID_SIZE + 1,
        y * GRID_SIZE + 1,
        GRID_SIZE - 2,
        GRID_SIZE - 2,
        color
      );
      // Yeux sur la tête
      if (idx == 0 && color != TFT_BLACK) {
        int ex = x * GRID_SIZE;
        int ey = y * GRID_SIZE;
        if      (dirX == 1)  { screen->fillRect(ex+5, ey+1, 2, 2, TFT_BLACK); screen->fillRect(ex+5, ey+5, 2, 2, TFT_BLACK); }
        else if (dirX == -1) { screen->fillRect(ex+1, ey+1, 2, 2, TFT_BLACK); screen->fillRect(ex+1, ey+5, 2, 2, TFT_BLACK); }
        else if (dirY == -1) { screen->fillRect(ex+1, ey+1, 2, 2, TFT_BLACK); screen->fillRect(ex+5, ey+1, 2, 2, TFT_BLACK); }
        else                 { screen->fillRect(ex+1, ey+5, 2, 2, TFT_BLACK); screen->fillRect(ex+5, ey+5, 2, 2, TFT_BLACK); }
      }
    }

    // ────────────────────────────────────────────
    //  Dessiner les bordures du terrain
    // ────────────────────────────────────────────
    void drawBorders() {
      // Ligne du haut de la zone (sous le score)
      screen->drawFastHLine(0, ZONE_TOP * GRID_SIZE, 240, TFT_DARKGREY);
      // Bordure basse
      screen->drawFastHLine(0, ZONE_BOT * GRID_SIZE + GRID_SIZE - 1, 240, TFT_DARKGREY);
      // Bordure gauche
      screen->drawFastVLine(ZONE_LEFT * GRID_SIZE - 1, ZONE_TOP * GRID_SIZE, 135 - ZONE_TOP * GRID_SIZE, TFT_DARKGREY);
      // Bordure droite
      screen->drawFastVLine(ZONE_RIGHT * GRID_SIZE + GRID_SIZE, ZONE_TOP * GRID_SIZE, 135 - ZONE_TOP * GRID_SIZE, TFT_DARKGREY);
    }

    // ────────────────────────────────────────────
    //  Afficher le score en haut (zone réservée)
    // ────────────────────────────────────────────
    void drawScore() {
      screen->fillRect(0, 0, 240, ZONE_TOP * GRID_SIZE - 1, TFT_BLACK);
      screen->setTextColor(TFT_CYAN, TFT_BLACK);
      screen->setTextSize(1);
      screen->setCursor(4, 4);
      screen->print("SNAKE");
      screen->setTextColor(TFT_WHITE, TFT_BLACK);
      screen->setCursor(60, 4);
      screen->print("SCORE:");
      screen->print(score);
      screen->print("  ");
      // Vitesse
      screen->setTextColor(TFT_DARKGREY, TFT_BLACK);
      screen->setCursor(170, 4);
      screen->print("SPD:");
      screen->print((150 - moveDelay) / 5 + 1);
    }

    // ────────────────────────────────────────────
    //  Collision avec les murs ou le corps
    // ────────────────────────────────────────────
    bool isCollision(int x, int y) {
      // Murs
      if (x <= ZONE_LEFT  || x >= ZONE_RIGHT ||
          y <= ZONE_TOP   || y >= ZONE_BOT)    return true;
      // Corps (commence à 1, pas la tête)
      for (int i = 1; i < snakeLength; i++) {
        if (snake[i].x == x && snake[i].y == y) return true;
      }
      return false;
    }

  public:
    SnakeGame(TFT_eSPI* display) : Game(display) {}

    void init() override {
      snakeLength = 3;
      dirX = 1; dirY = 0;
      nextDirX = 1; nextDirY = 0;
      score     = 0;
      moveDelay = 150;
      lastMove  = 0;
      moved     = false;
      ateFood   = false;
      firstDraw = true;
      state     = IN_PROGRESS;

      // Position initiale au centre du terrain
      int startX = GRID_W / 2;
      int startY = ZONE_TOP + (ZONE_BOT - ZONE_TOP) / 2;
      for (int i = 0; i < snakeLength; i++) {
        snake[i].x = startX - i;
        snake[i].y = startY;
      }

      spawnFood();
    }

    void update(Buttons buttons) override {
      if (state == GAME_OVER) return;

      // ── Saisie de direction (bufferisée, pas de demi-tour) ──
      if (buttons.up    && dirY == 0) { nextDirX =  0; nextDirY = -1; }
      if (buttons.down  && dirY == 0) { nextDirX =  0; nextDirY =  1; }
      if (buttons.left  && dirX == 0) { nextDirX = -1; nextDirY =  0; }
      if (buttons.right && dirX == 0) { nextDirX =  1; nextDirY =  0; }

      moved   = false;
      ateFood = false;

      // ── Déplacement sur timer ──
      if (millis() - lastMove < (unsigned long)moveDelay) return;
      lastMove = millis();

      // Appliquer la direction bufferisée
      dirX = nextDirX;
      dirY = nextDirY;

      int newX = snake[0].x + dirX;
      int newY = snake[0].y + dirY;

      // ── Vérifier collision ──
      if (isCollision(newX, newY)) {
        state = GAME_OVER;
        return;
      }

      // ── Sauvegarder la queue AVANT de bouger ──
      lastTail = snake[snakeLength - 1];

      // ── Décaler le corps ──
      for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
      }
      snake[0].x = newX;
      snake[0].y = newY;
      moved = true;

      // ── Manger la pomme ──
      if (newX == food.x && newY == food.y) {
        snakeLength++;
        score += 10;
        ateFood = true;
        // Accélération progressive (max à SPD 19 = 60ms)
        if (moveDelay > 60) moveDelay -= 5;
        spawnFood();
      }
    }

    void render() override {

      // ── Premier dessin complet ──
      if (firstDraw) {
        firstDraw = false;
        screen->fillScreen(TFT_BLACK);
        drawBorders();
        drawScore();
        // Serpent entier
        for (int i = 0; i < snakeLength; i++) {
          uint16_t col = (i == 0) ? TFT_YELLOW : TFT_GREEN;
          drawSegment(snake[i].x, snake[i].y, i, col);
        }
        // Pomme
        drawFood(TFT_RED);
        return;
      }

      if (!moved) return;   // rien à redessiner

      // ── Effacement chirurgical ──
      if (!ateFood) {
        // Effacer uniquement l'ancienne queue
        drawSegment(lastTail.x, lastTail.y, -1, TFT_BLACK);
      }

      // Effacer l'ancienne tête (qui devient le 2e segment)
      // On redessine le 2e segment en vert (il était jaune)
      if (snakeLength > 1) {
        drawSegment(snake[1].x, snake[1].y, 1, TFT_GREEN);
      }

      // Dessiner la nouvelle tête en jaune
      drawSegment(snake[0].x, snake[0].y, 0, TFT_YELLOW);

      // ── Si pomme mangée : redessiner tout proprement ──
      if (ateFood) {
        // Effacer la zone de jeu une seule fois
        screen->fillRect(
          (ZONE_LEFT + 1) * GRID_SIZE,
          (ZONE_TOP  + 1) * GRID_SIZE,
          (ZONE_RIGHT - ZONE_LEFT - 1) * GRID_SIZE,
          (ZONE_BOT   - ZONE_TOP  - 1) * GRID_SIZE,
          TFT_BLACK
        );
        // Redessiner le serpent complet
        for (int i = snakeLength - 1; i >= 0; i--) {
          uint16_t col = (i == 0) ? TFT_YELLOW : TFT_GREEN;
          drawSegment(snake[i].x, snake[i].y, i, col);
        }
        // Nouvelle pomme
        drawFood(TFT_RED);
        // Redessiner les bordures (effacées par le fillRect)
        drawBorders();
        // Mettre à jour le score et la vitesse
        drawScore();
      }
    }

    virtual ~SnakeGame() {}
};

#endif