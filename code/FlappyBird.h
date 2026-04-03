#ifndef FLAPPYBIRD_H
#define FLAPPYBIRD_H

#include "Game.h"

// ════════════════════════════════════════════════
//  FLAPPY BIRD — Éviter les tuyaux en sautant
//  Bouton : A pour sauter
//  Game Over : collision avec un tuyau ou le sol
// ════════════════════════════════════════════════

// ── Structure d'une paire de tuyaux ──
struct Pipe {
  int  x;        // position X du tuyau
  int  gapY;     // centre du trou entre les 2 tuyaux
  bool passed;   // le joueur est-il passé ?
};

class FlappyBirdGame : public Game {
  private:

    // ── Oiseau ──
    float birdY;               // position Y (float pour la physique)
    float birdVel;             // vitesse verticale
    const int birdX    = 40;   // position X fixe
    const int birdSize = 8;    // taille de l'oiseau
    const float gravity  = 0.4;  // gravité appliquée chaque frame
    const float jumpForce = -4.5; // force du saut (négatif = vers le haut)

    // ── Tuyaux ──
    static const int PIPE_COUNT = 3;  // nombre de tuyaux simultanés
    Pipe pipes[PIPE_COUNT];
    const int pipeW   = 16;    // largeur d'un tuyau
    const int gapSize = 38;    // taille du trou entre les tuyaux
    const int pipeSpd = 2;     // vitesse de défilement

    // ── Rendu optimisé ──
    float prevBirdY;
    int   prevPipes[PIPE_COUNT];   // anciennes positions X des tuyaux
    bool  firstDraw;

    // ── Timing ──
    unsigned long lastUpdate;
    const int updateInterval = 16;

    // ────────────────────────────────────────────
    //  Initialiser un tuyau à une position X donnée
    // ────────────────────────────────────────────
    void initPipe(int index, int x) {
      pipes[index].x      = x;
      pipes[index].gapY   = random(30, 100);  // trou à hauteur aléatoire
      pipes[index].passed = false;
    }

    // ────────────────────────────────────────────
    //  Dessiner un tuyau complet (haut + bas)
    // ────────────────────────────────────────────
    void drawPipe(int x, int gapY, uint16_t color) {
      // Tuyau du haut
      screen->fillRect(x, 0, pipeW, gapY - gapSize/2, color);
      // Tuyau du bas
      screen->fillRect(x, gapY + gapSize/2, pipeW, 135 - (gapY + gapSize/2), color);
    }

  public:
    FlappyBirdGame(TFT_eSPI* display) : Game(display) {}

    void init() override {
      birdY     = 67.0;
      birdVel   = 0.0;
      score     = 0;
      state     = IN_PROGRESS;
      firstDraw = true;
      lastUpdate = 0;

      // Espacer les tuyaux régulièrement
      for (int i = 0; i < PIPE_COUNT; i++) {
        initPipe(i, 240 + i * 90);
      }
    }

    void update(Buttons buttons) override {
      if (state == GAME_OVER) return;
      if (millis() - lastUpdate < updateInterval) return;
      lastUpdate = millis();

      prevBirdY = birdY;
      for (int i = 0; i < PIPE_COUNT; i++) prevPipes[i] = pipes[i].x;

      // ── Physique de l'oiseau ──
      if (buttons.aPressed) birdVel = jumpForce;  // saut
      birdVel += gravity;                          // gravité
      birdY   += birdVel;

      // ── Sol et plafond ──
      if (birdY <= 0 || birdY >= 135 - birdSize) {
        state = GAME_OVER;
        return;
      }

      // ── Déplacement et collision tuyaux ──
      for (int i = 0; i < PIPE_COUNT; i++) {
        pipes[i].x -= pipeSpd;

        // Recycler le tuyau quand il sort à gauche
        if (pipes[i].x < -pipeW) {
          initPipe(i, 240);
        }

        // Compter le score (tuyau passé)
        if (!pipes[i].passed && pipes[i].x + pipeW < birdX) {
          pipes[i].passed = true;
          score++;
        }

        // Collision avec le tuyau
        if (birdX + birdSize > pipes[i].x &&
            birdX < pipes[i].x + pipeW) {
          if ((int)birdY < pipes[i].gapY - gapSize/2 ||
              (int)birdY + birdSize > pipes[i].gapY + gapSize/2) {
            state = GAME_OVER;
            return;
          }
        }
      }
    }

    void render() override {

      if (firstDraw) {
        firstDraw = false;
        screen->fillScreen(TFT_BLACK);

        // Dessiner tous les tuyaux
        for (int i = 0; i < PIPE_COUNT; i++) {
          drawPipe(pipes[i].x, pipes[i].gapY, TFT_GREEN);
        }

        // Dessiner l'oiseau
        screen->fillRect(birdX, (int)birdY, birdSize, birdSize, TFT_YELLOW);

        // Score
        screen->setTextColor(TFT_WHITE, TFT_BLACK);
        screen->setTextSize(2);
        screen->setCursor(100, 4);
        screen->print(score);
        return;
      }

      // ── Effacer ancienne position oiseau ──
      screen->fillRect(birdX, (int)prevBirdY, birdSize, birdSize, TFT_BLACK);

      // ── Effacer et redessiner les tuyaux ──
      for (int i = 0; i < PIPE_COUNT; i++) {
        if (prevPipes[i] != pipes[i].x) {
          drawPipe(prevPipes[i], pipes[i].gapY, TFT_BLACK);  // effacer
          drawPipe(pipes[i].x,   pipes[i].gapY, TFT_GREEN);  // redessiner
        }
      }

      // ── Dessiner nouvelle position oiseau ──
      screen->fillRect(birdX, (int)birdY, birdSize, birdSize, TFT_YELLOW);

      // ── Score ──
      screen->setTextColor(TFT_WHITE, TFT_BLACK);
      screen->setTextSize(2);
      screen->setCursor(100, 4);
      screen->print(score);
      screen->print("  ");
    }

    virtual ~FlappyBirdGame() {}
};

#endif