// // #ifndef PONG_H
// // #define PONG_H

// // #include "Game.h"

// // //  PONG — Jeu de raquette solo contre l'IA
// // //  Boutons : HAUT / BAS pour bouger la raquette
// // //  Game Over : si la balle passe derrière ta raquette

// // class PongGame : public Game {
// //   private:

// //     // Raquette du joueur (gauche)
// //     int paddleY;               // position Y du centre de la raquette
// //     const int paddleX   = 8;   // position X fixe (côté gauche)
// //     const int paddleH   = 24;  // hauteur de la raquette en pixels
// //     const int paddleW   = 4;   // largeur de la raquette en pixels
// //     const int paddleSpd = 3;   // vitesse de déplacement en pixels/frame

// //     //Raquette de l'IA (droite)
// //     int aiY;                   // position Y du centre de la raquette IA
// //     const int aiX     = 228;   // position X fixe (côté droit)
// //     const int aiSpd   = 2;     // vitesse de l'IA (plus lente que le joueur)

// //     // Balle
// //     int   ballX, ballY;        // position de la balle
// //     float velX,  velY;         // vitesse de la balle (float pour précision)
// //     const int ballSize = 4;    // taille de la balle en pixels

// //     // Score
// //     int playerScore;           // score du joueur
// //     int aiScore;               // score de l'IA

// //     // Rendu optimisé
// //     int   prevBallX,  prevBallY;   // ancienne position balle
// //     int   prevPaddleY;             // ancienne position raquette joueur
// //     int   prevAiY;                 // ancienne position raquette IA
// //     bool  firstDraw;               // premier dessin complet ?

// //     //Timing
// //     unsigned long lastUpdate;      // dernier update de la balle
// //     const int updateInterval = 16; // ~60fps

// //     //  Reset la balle au centre après un point
// //     void resetBall() {
// //       ballX = 120;
// //       ballY = 67;
// //       // Direction aléatoire à chaque reset
// //       velX = (random(0, 2) == 0) ? 2.0 : -2.0;
// //       velY = (random(0, 2) == 0) ? 1.5 : -1.5;
// //     }

// //     //  Déplacement de l'IA — suit la balle
// //     //  mais avec une vitesse limitée (aiSpd)
// //     void updateAI() {
// //       if (ballY > aiY + paddleH / 2) aiY += aiSpd;
// //       else if (ballY < aiY - paddleH / 2) aiY -= aiSpd;
// //       // Limiter l'IA dans l'écran
// //       aiY = constrain(aiY, paddleH / 2, 135 - paddleH / 2);
// //     }

// //   public:
// //     PongGame(TFT_eSPI* display) : Game(display) {}

// //     //  Initialisation — appelée au démarrage
// //     //  et à chaque nouvelle partie

// //     void init() override {
// //       paddleY      = 67;
// //       aiY          = 67;
// //       playerScore  = 0;
// //       aiScore      = 0;
// //       score        = 0;
// //       state        = IN_PROGRESS;
// //       firstDraw    = true;
// //       lastUpdate   = 0;
// //       resetBall();
// //     }


// //     //  Update — logique du jeu à chaque frame
// //     void update(Buttons buttons) override {
// //       if (state == GAME_OVER) return;
// //       if (millis() - lastUpdate < updateInterval) return;
// //       lastUpdate = millis();

// //       // Sauvegarder les anciennes positions pour l'effacement
// //       prevBallX   = ballX;
// //       prevBallY   = ballY;
// //       prevPaddleY = paddleY;
// //       prevAiY     = aiY;

// //       //Déplacement raquette joueur
// //       if (buttons.up)   paddleY -= paddleSpd;
// //       if (buttons.down) paddleY += paddleSpd;
// //       paddleY = constrain(paddleY, paddleH / 2, 135 - paddleH / 2);

// //       // Déplacement IA
// //       updateAI();

// //       // Déplacement balle
// //       ballX += (int)velX;
// //       ballY += (int)velY;

// //       // Rebond murs haut/bas
// //       if (ballY <= 0 || ballY >= 135) velY = -velY;

// //       // Collision raquette joueur 
// //       if (ballX <= paddleX + paddleW &&
// //           ballY >= paddleY - paddleH / 2 &&
// //           ballY <= paddleY + paddleH / 2) {
// //         velX = abs(velX) + 0.1; // accélérer légèrement
// //         // Angle selon où frappe la balle sur la raquette
// //         velY = (ballY - paddleY) * 0.15;
// //       }

// //       // Collision raquette IA
// //       if (ballX >= aiX - paddleW &&
// //           ballY >= aiY - paddleH / 2 &&
// //           ballY <= aiY + paddleH / 2) {
// //         velX = -(abs(velX) + 0.1);
// //         velY = (ballY - aiY) * 0.15;
// //       }

// //       // Point marqué 
// //       if (ballX < 0) {
// //         // L'IA marque
// //         aiScore++;
// //         if (aiScore >= 5) { state = GAME_OVER; score = playerScore; return; }
// //         resetBall();
// //       }
// //       if (ballX > 240) {
// //         // Le joueur marque
// //         playerScore++;
// //         score = playerScore;
// //         if (playerScore >= 5) { state = GAME_OVER; return; }
// //         resetBall();
// //       }
// //     }


// //     //  Render — affichage optimisé
// //     //  On efface uniquement les anciens pixels
// //     void render() override {

// //       // Premier dessin : tout afficher
// //       if (firstDraw) {
// //         firstDraw = false;
// //         screen->fillScreen(TFT_BLACK);

// //         // Ligne centrale pointillée
// //         for (int y = 0; y < 135; y += 8) {
// //           screen->fillRect(119, y, 2, 4, TFT_DARKGREY);
// //         }

// //         // Score
// //         screen->setTextColor(TFT_WHITE, TFT_BLACK);
// //         screen->setTextSize(2);
// //         screen->setCursor(80, 4);
// //         screen->print(playerScore);
// //         screen->setCursor(148, 4);
// //         screen->print(aiScore);

// //         // Raquettes
// //         screen->fillRect(paddleX, paddleY - paddleH/2, paddleW, paddleH, TFT_WHITE);
// //         screen->fillRect(aiX,     aiY    - paddleH/2, paddleW, paddleH, TFT_WHITE);

// //         // Balle
// //         screen->fillRect(ballX, ballY, ballSize, ballSize, TFT_WHITE);
// //         return;
// //       }

// //       // Effacer ancienne balle
// //       screen->fillRect(prevBallX, prevBallY, ballSize, ballSize, TFT_BLACK);

// //       // Effacer ancienne raquette joueur
// //       screen->fillRect(paddleX, prevPaddleY - paddleH/2, paddleW, paddleH, TFT_BLACK);

// //       // Effacer ancienne raquette IA
// //       screen->fillRect(aiX, prevAiY - paddleH/2, paddleW, paddleH, TFT_BLACK);

// //       // Dessiner nouvelle balle
// //       screen->fillRect(ballX, ballY, ballSize, ballSize, TFT_WHITE);

// //       // Dessiner nouvelle raquette joueur
// //       screen->fillRect(paddleX, paddleY - paddleH/2, paddleW, paddleH, TFT_WHITE);

// //       // Dessiner nouvelle raquette IA
// //       screen->fillRect(aiX, aiY - paddleH/2, paddleW, paddleH, TFT_WHITE);

// //       // Mettre à jour le score si changé
// //       screen->setTextColor(TFT_WHITE, TFT_BLACK);
// //       screen->setTextSize(2);
// //       screen->setCursor(80, 4);
// //       screen->print(playerScore);
// //       screen->setCursor(148, 4);
// //       screen->print(aiScore);
// //     }

// //     virtual ~PongGame() {}
// // };

// // #endif


// #ifndef PONG_H
// #define PONG_H

// #include "Game.h"

// //  PONG — Jeu de raquette solo contre l'IA
// //  Boutons : HAUT / BAS pour bouger la raquette
// //  Game Over : si la balle passe derrière ta raquette

// class PongGame : public Game {
//   private:

//     // Raquette du joueur (gauche)
//     int paddleY;               // position Y du centre de la raquette
//     const int paddleX   = 8;   // position X fixe (côté gauche)
//     const int paddleH   = 24;  // hauteur de la raquette en pixels
//     const int paddleW   = 4;   // largeur de la raquette en pixels
//     const int paddleSpd = 3;   // vitesse de déplacement en pixels/frame

//     //Raquette de l'IA (droite)
//     int aiY;                   // position Y du centre de la raquette IA
//     const int aiX     = 228;   // position X fixe (côté droit)
//     int   aiSpd;               // vitesse de l'IA (augmente avec le score joueur)
//     const int aiSpdMax = 4;    // vitesse max de l'IA

//     // Balle
//     int   ballX, ballY;        // position de la balle
//     float velX,  velY;         // vitesse de la balle (float pour précision)
//     const int   ballSize = 4;  // taille de la balle en pixels
//     const float ballMaxV = 5.0; // vitesse max de la balle (évite accélération infinie)

//     // Score
//     int playerScore;           // score du joueur
//     int aiScore;               // score de l'IA

//     // Rendu optimisé
//     int   prevBallX,  prevBallY;   // ancienne position balle
//     int   prevPaddleY;             // ancienne position raquette joueur
//     int   prevAiY;                 // ancienne position raquette IA
//     bool  firstDraw;               // premier dessin complet ?

//     //Timing
//     unsigned long lastUpdate;      // dernier update de la balle
//     const int updateInterval = 16; // ~60fps

//     //  Reset la balle au centre après un point
//     void resetBall() {
//       ballX = 120;
//       ballY = 67;
//       // Direction aléatoire à chaque reset
//       velX = (random(0, 2) == 0) ? 2.0 : -2.0;
//       velY = (random(0, 2) == 0) ? 1.5 : -1.5;
//     }

//     //  Déplacement de l'IA — suit la balle
//     //  mais avec une vitesse limitée (aiSpd)
//     void updateAI() {
//       if (ballY > aiY + paddleH / 2) aiY += aiSpd;
//       else if (ballY < aiY - paddleH / 2) aiY -= aiSpd;
//       // Limiter l'IA dans l'écran
//       aiY = constrain(aiY, paddleH / 2, 135 - paddleH / 2);
//     }

//   public:
//     PongGame(TFT_eSPI* display) : Game(display) {}

//     //  Initialisation — appelée au démarrage
//     //  et à chaque nouvelle partie

//     void init() override {
//       paddleY      = 67;
//       aiY          = 67;
//       playerScore  = 0;
//       aiScore      = 0;
//       score        = 0;
//       aiSpd        = 1;      // IA très lente au départ
//       state        = IN_PROGRESS;
//       firstDraw    = true;
//       lastUpdate   = 0;
//       resetBall();
//     }


//     //  Update — logique du jeu à chaque frame
//     void update(Buttons buttons) override {
//       if (state == GAME_OVER) return;
//       if (millis() - lastUpdate < updateInterval) return;
//       lastUpdate = millis();

//       // Sauvegarder les anciennes positions pour l'effacement
//       prevBallX   = ballX;
//       prevBallY   = ballY;
//       prevPaddleY = paddleY;
//       prevAiY     = aiY;

//       //Déplacement raquette joueur
//       if (buttons.up)   paddleY -= paddleSpd;
//       if (buttons.down) paddleY += paddleSpd;
//       paddleY = constrain(paddleY, paddleH / 2, 135 - paddleH / 2);

//       // Déplacement IA
//       updateAI();

//       // Déplacement balle
//       ballX += (int)velX;
//       ballY += (int)velY;

//       // Rebond murs haut/bas
//       if (ballY <= 0 || ballY >= 135) velY = -velY;

//       // Collision raquette joueur 
//       if (ballX <= paddleX + paddleW &&
//           ballY >= paddleY - paddleH / 2 &&
//           ballY <= paddleY + paddleH / 2) {
//         // Accélération légère mais plafonnée
//         float newVX = abs(velX) + 0.1f;
//         velX = min(newVX, ballMaxV);
//         // Angle selon où frappe la balle sur la raquette
//         velY = (ballY - paddleY) * 0.15;
//       }

//       // Collision raquette IA
//       if (ballX >= aiX - paddleW &&
//           ballY >= aiY - paddleH / 2 &&
//           ballY <= aiY + paddleH / 2) {
//         float newVX = abs(velX) + 0.1f;
//         velX = -min(newVX, ballMaxV);
//         velY = (ballY - aiY) * 0.15;
//       }

//       // Point marqué 
//       if (ballX < 0) {
//         // L'IA marque
//         aiScore++;
//         if (aiScore >= 5) { state = GAME_OVER; score = playerScore; return; }
//         resetBall();
//       }
//       if (ballX > 240) {
//         // Le joueur marque
//         playerScore++;
//         score = playerScore;
//         // ── Difficulté progressive : l'IA s'améliore à chaque point ──
//         // 0 pt → aiSpd=1, 1pt → 2, 2pts → 2, 3pts → 3, 4pts → 4
//         aiSpd = constrain(1 + playerScore, 1, aiSpdMax);
//         if (playerScore >= 5) { state = GAME_WIN; return; }  // Victoire joueur !
//         resetBall();
//       }
//     }


//     //  Render — affichage optimisé
//     //  On efface uniquement les anciens pixels
//     void render() override {

//       // Premier dessin : tout afficher
//       if (firstDraw) {
//         firstDraw = false;
//         screen->fillScreen(TFT_BLACK);

//         // Ligne centrale pointillée
//         for (int y = 0; y < 135; y += 8) {
//           screen->fillRect(119, y, 2, 4, TFT_DARKGREY);
//         }

//         // Score
//         screen->setTextColor(TFT_WHITE, TFT_BLACK);
//         screen->setTextSize(2);
//         screen->setCursor(80, 4);
//         screen->print(playerScore);
//         screen->setCursor(148, 4);
//         screen->print(aiScore);

//         // Raquettes
//         screen->fillRect(paddleX, paddleY - paddleH/2, paddleW, paddleH, TFT_WHITE);
//         screen->fillRect(aiX,     aiY    - paddleH/2, paddleW, paddleH, TFT_WHITE);

//         // Balle
//         screen->fillRect(ballX, ballY, ballSize, ballSize, TFT_WHITE);
//         return;
//       }

//       // Effacer ancienne balle
//       screen->fillRect(prevBallX, prevBallY, ballSize, ballSize, TFT_BLACK);

//       // Effacer ancienne raquette joueur
//       screen->fillRect(paddleX, prevPaddleY - paddleH/2, paddleW, paddleH, TFT_BLACK);

//       // Effacer ancienne raquette IA
//       screen->fillRect(aiX, prevAiY - paddleH/2, paddleW, paddleH, TFT_BLACK);

//       // Dessiner nouvelle balle
//       screen->fillRect(ballX, ballY, ballSize, ballSize, TFT_WHITE);

//       // Dessiner nouvelle raquette joueur
//       screen->fillRect(paddleX, paddleY - paddleH/2, paddleW, paddleH, TFT_WHITE);

//       // Dessiner nouvelle raquette IA
//       screen->fillRect(aiX, aiY - paddleH/2, paddleW, paddleH, TFT_WHITE);

//       // Mettre à jour le score si changé
//       screen->setTextColor(TFT_WHITE, TFT_BLACK);
//       screen->setTextSize(2);
//       screen->setCursor(80, 4);
//       screen->print(playerScore);
//       screen->setCursor(148, 4);
//       screen->print(aiScore);
//     }

//     virtual ~PongGame() {}
// };

// #endif



#ifndef PONG_H
#define PONG_H

#include "Game.h"

//  PONG — Jeu de raquette solo contre l'IA
//  Boutons : HAUT / BAS pour bouger la raquette
//  Game Over : si la balle passe derrière ta raquette

class PongGame : public Game {
  private:

    // Raquette du joueur (gauche)
    int paddleY;               // position Y du centre de la raquette
    const int paddleX   = 8;   // position X fixe (côté gauche)
    const int paddleH   = 24;  // hauteur de la raquette en pixels
    const int paddleW   = 4;   // largeur de la raquette en pixels
    const int paddleSpd = 3;   // vitesse de déplacement en pixels/frame

    //Raquette de l'IA (droite)
    int aiY;                    // position Y du centre de la raquette IA
    const int aiX      = 228;   // position X fixe (côté droit)
    const int aiSpd    = 3;     // vitesse fixe de l'IA en pixels
    unsigned long lastAiMove;   // timestamp du dernier mouvement IA
    int  aiMoveInterval;        // intervalle entre chaque mouvement (ms)
                                // grand = IA lente, petit = IA rapide
    // Difficulté progressive par palier de score joueur :
    //   0 pt  → 120ms  (IA très lente, facile à battre)
    //   1 pt  →  90ms
    //   2 pts →  65ms
    //   3 pts →  45ms
    //   4 pts →  30ms  (IA rapide mais toujours battable)

    // Balle
    int   ballX, ballY;         // position de la balle
    float velX,  velY;          // vitesse de la balle (float pour précision)
    const int   ballSize = 4;   // taille de la balle en pixels
    const float ballMaxV = 5.0; // plafond de vitesse (évite accélération infinie)

    // Score
    int playerScore;           // score du joueur
    int aiScore;               // score de l'IA

    // Rendu optimisé
    int   prevBallX,  prevBallY;   // ancienne position balle
    int   prevPaddleY;             // ancienne position raquette joueur
    int   prevAiY;                 // ancienne position raquette IA
    bool  firstDraw;               // premier dessin complet ?

    //Timing
    unsigned long lastUpdate;      // dernier update de la balle
    const int updateInterval = 16; // ~60fps

    //  Reset la balle au centre après un point
    void resetBall() {
      ballX = 120;
      ballY = 67;
      // Direction aléatoire à chaque reset
      velX = (random(0, 2) == 0) ? 2.0 : -2.0;
      velY = (random(0, 2) == 0) ? 1.5 : -1.5;
    }

    // ────────────────────────────────────────────
    //  Déplacement de l'IA
    //  Inspiré du pattern timer + demi-terrain :
    //  → l'IA ne réagit que quand la balle est dans son camp
    //  → elle ne bouge que toutes les aiMoveInterval ms
    //  → résultat : battable au début, difficile à la fin
    // ────────────────────────────────────────────
    void updateAI() {
      // L'IA reste passive si la balle va vers le joueur
      if (ballX <= 120) return;

      // Timer : l'IA ne peut pas réagir plus vite que son intervalle
      if (millis() - lastAiMove < (unsigned long)aiMoveInterval) return;
      lastAiMove = millis();

      // Suivi simple : monter ou descendre vers le centre de la balle
      if (ballY < aiY - paddleH / 2 && aiY > paddleH / 2) {
        aiY -= aiSpd;
      } else if (ballY > aiY + paddleH / 2 && aiY < 135 - paddleH / 2) {
        aiY += aiSpd;
      }
    }

  public:
    PongGame(TFT_eSPI* display) : Game(display) {}

    //  Initialisation — appelée au démarrage
    //  et à chaque nouvelle partie

    void init() override {
      paddleY         = 67;
      aiY             = 67;
      playerScore     = 0;
      aiScore         = 0;
      score           = 0;
      aiMoveInterval  = 120;   // IA très lente au départ
      lastAiMove      = 0;
      state           = IN_PROGRESS;
      firstDraw       = true;
      lastUpdate      = 0;
      resetBall();
    }


    //  Update — logique du jeu à chaque frame
    void update(Buttons buttons) override {
      if (state == GAME_OVER) return;
      if (millis() - lastUpdate < updateInterval) return;
      lastUpdate = millis();

      // Sauvegarder les anciennes positions pour l'effacement
      prevBallX   = ballX;
      prevBallY   = ballY;
      prevPaddleY = paddleY;
      prevAiY     = aiY;

      //Déplacement raquette joueur
      if (buttons.up)   paddleY -= paddleSpd;
      if (buttons.down) paddleY += paddleSpd;
      paddleY = constrain(paddleY, paddleH / 2, 135 - paddleH / 2);

      // Déplacement IA
      updateAI();

      // Déplacement balle
      ballX += (int)velX;
      ballY += (int)velY;

      // Rebond murs haut/bas
      if (ballY <= 0 || ballY >= 135) velY = -velY;
      

      // Collision raquette joueur 
      if (ballX <= paddleX + paddleW &&
          ballY >= paddleY - paddleH / 2 &&
          ballY <= paddleY + paddleH / 2) {
        // Accélération légère mais plafonnée
        float newVX = abs(velX) + 0.1f;
        velX = min(newVX, ballMaxV);
        // Angle selon où frappe la balle sur la raquette
        velY = (ballY - paddleY) * 0.15;
      }

      // Collision raquette IA
      if (ballX >= aiX - paddleW &&
          ballY >= aiY - paddleH / 2 &&
          ballY <= aiY + paddleH / 2) {
        float newVX = abs(velX) + 0.1f;
        velX = -min(newVX, ballMaxV);
        velY = (ballY - aiY) * 0.15;
      }

      // Point marqué 
      if (ballX < 0) {
        // L'IA marque
        aiScore++;
        if (aiScore >= 5) { state = GAME_OVER; score = playerScore; return; }
        resetBall();
      }
      if (ballX > 240) {
        // Le joueur marque — l'IA accélère sa réaction
        playerScore++;
        score = playerScore;
        // Intervalle IA : 120 → 90 → 65 → 45 → 30ms
        const int intervals[] = {120, 90, 65, 45, 30};
        aiMoveInterval = intervals[constrain(playerScore, 0, 4)];
        if (playerScore >= 5) { state = GAME_WIN; return; }
        resetBall();
      }
    }


    //  Render — affichage optimisé
    //  On efface uniquement les anciens pixels
    void render() override {

      // Premier dessin : tout afficher
      if (firstDraw) {
        firstDraw = false;
        screen->fillScreen(TFT_BLACK);

        // Ligne centrale pointillée
        for (int y = 0; y < 135; y += 8) {
          screen->fillRect(119, y, 2, 4, TFT_DARKGREY);
        }

        // Score
        screen->setTextColor(TFT_WHITE, TFT_BLACK);
        screen->setTextSize(2);
        screen->setCursor(80, 4);
        screen->print(playerScore);
        screen->setCursor(148, 4);
        screen->print(aiScore);

        // Raquettes
        screen->fillRect(paddleX, paddleY - paddleH/2, paddleW, paddleH, TFT_WHITE);
        screen->fillRect(aiX,     aiY    - paddleH/2, paddleW, paddleH, TFT_WHITE);

        // Balle
        screen->fillRect(ballX, ballY, ballSize, ballSize, TFT_WHITE);
        return;
      }

      // Effacer ancienne balle
      screen->fillRect(prevBallX, prevBallY, ballSize, ballSize, TFT_BLACK);

      // Effacer ancienne raquette joueur
      screen->fillRect(paddleX, prevPaddleY - paddleH/2, paddleW, paddleH, TFT_BLACK);

      // Effacer ancienne raquette IA
      screen->fillRect(aiX, prevAiY - paddleH/2, paddleW, paddleH, TFT_BLACK);

      // Dessiner nouvelle balle
      screen->fillRect(ballX, ballY, ballSize, ballSize, TFT_WHITE);

      // Dessiner nouvelle raquette joueur
      screen->fillRect(paddleX, paddleY - paddleH/2, paddleW, paddleH, TFT_WHITE);

      // Dessiner nouvelle raquette IA
      screen->fillRect(aiX, aiY - paddleH/2, paddleW, paddleH, TFT_WHITE);

      // Mettre à jour le score si changé
      screen->setTextColor(TFT_WHITE, TFT_BLACK);
      screen->setTextSize(2);
      screen->setCursor(80, 4);
      screen->print(playerScore);
      screen->setCursor(148, 4);
      screen->print(aiScore);
    }

    virtual ~PongGame() {}
};

#endif