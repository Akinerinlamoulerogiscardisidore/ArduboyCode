#ifndef DINORUN_H
#define DINORUN_H

#include "Game.h"

// ════════════════════════════════════════════════
//  DINO RUN — Sauter par-dessus les cactus
//  Bouton : A pour sauter
//  Game Over : collision avec un cactus
// ════════════════════════════════════════════════

// ── Structure d'un obstacle ──
struct Cactus {
  int  x;        // position X
  int  w, h;     // largeur et hauteur (variable)
  bool active;   // actif ?
};

class DinoRunGame : public Game {
  private:

    // ── Dino ──
    const int dinoX   = 20;    // position X fixe
    float dinoY;               // position Y (float pour physique)
    float dinoVel;             // vitesse verticale
    const int dinoW   = 12;    // largeur
    const int dinoH   = 16;    // hauteur
    const int groundY = 110;   // position Y du sol
    bool isOnGround;           // le dino est-il au sol ?

    const float gravity   = 0.5;   // gravité
    const float jumpForce = -7.0;  // force de saut

    // ── Cactus ──
    static const int MAX_CACTUS = 3;
    Cactus cactus[MAX_CACTUS];
    int  gameSpeed;            // vitesse de défilement (augmente avec le score)

    // ── Sol ──
    int groundOffset;          // décalage du sol animé

    // ── Rendu ──
    float prevDinoY;
    bool  firstDraw;

    // ── Timing ──
    unsigned long lastUpdate;
    unsigned long lastSpawn;
    const int updateInterval = 16;

    // ────────────────────────────────────────────
    //  Spawner un cactus aléatoire
    // ────────────────────────────────────────────
    void spawnCactus(int index) {
      cactus[index].x      = 260 + random(0, 80);
      cactus[index].w      = 8 + random(0, 8);
      cactus[index].h      = 12 + random(0, 12);
      cactus[index].active = true;
    }

    // ────────────────────────────────────────────
    //  Dessiner le dino (pixel art simple)
    // ────────────────────────────────────────────
    void drawDino(int x, int y, uint16_t color) {
      screen->fillRect(x,       y,          dinoW - 2, dinoH - 4, color);  // corps
      screen->fillRect(x + 2,   y - 4,      dinoW - 4, 4,         color);  // tête
      screen->fillRect(x + dinoW - 2, y,    4,         4,         color);  // queue
    }

  public:
    DinoRunGame(TFT_eSPI* display) : Game(display) {}

    void init() override {
      dinoY        = groundY - dinoH;
      dinoVel      = 0;
      isOnGround   = true;
      score        = 0;
      gameSpeed    = 3;
      groundOffset = 0;
      state        = IN_PROGRESS;
      firstDraw    = true;
      lastUpdate   = 0;
      lastSpawn    = 0;

      for (int i = 0; i < MAX_CACTUS; i++) cactus[i].active = false;
      spawnCactus(0);
    }

    void update(Buttons buttons) override {
      if (state == GAME_OVER) return;
      if (millis() - lastUpdate < updateInterval) return;
      lastUpdate = millis();

      prevDinoY = dinoY;

      // ── Saut ──
      if (buttons.aPressed && isOnGround) {
        dinoVel    = jumpForce;
        isOnGround = false;
      }

      // ── Physique ──
      dinoVel += gravity;
      dinoY   += dinoVel;

      // ── Sol ──
      if (dinoY >= groundY - dinoH) {
        dinoY      = groundY - dinoH;
        dinoVel    = 0;
        isOnGround = true;
      }

      // ── Score et vitesse ──
      score++;
      if (score % 100 == 0 && gameSpeed < 8) gameSpeed++;

      // ── Défilement sol ──
      groundOffset = (groundOffset + gameSpeed) % 16;

      // ── Spawn cactus ──
      bool anyActive = false;
      for (int i = 0; i < MAX_CACTUS; i++) if (cactus[i].active) anyActive = true;
      if (!anyActive || (millis() - lastSpawn > 2000 && random(0, 100) < 5)) {
        for (int i = 0; i < MAX_CACTUS; i++) {
          if (!cactus[i].active) { spawnCactus(i); lastSpawn = millis(); break; }
        }
      }

      // ── Déplacement et collision cactus ──
      for (int i = 0; i < MAX_CACTUS; i++) {
        if (!cactus[i].active) continue;

        // Effacer ancienne position
        screen->fillRect(cactus[i].x, groundY - cactus[i].h, cactus[i].w, cactus[i].h, TFT_BLACK);

        cactus[i].x -= gameSpeed;

        // Recycler si sorti
        if (cactus[i].x + cactus[i].w < 0) {
          cactus[i].active = false;
          continue;
        }

        // Collision
        if (dinoX + dinoW - 2 > cactus[i].x &&
            dinoX < cactus[i].x + cactus[i].w &&
            (int)dinoY + dinoH > groundY - cactus[i].h) {
          state = GAME_OVER;
          return;
        }

        // Dessiner nouvelle position
        screen->fillRect(cactus[i].x, groundY - cactus[i].h, cactus[i].w, cactus[i].h, TFT_GREEN);
      }
    }

    void render() override {
      if (firstDraw) {
        firstDraw = false;
        screen->fillScreen(TFT_BLACK);
        screen->drawLine(0, groundY, 240, groundY, TFT_WHITE);
        drawDino(dinoX, (int)dinoY, TFT_WHITE);
        return;
      }

      // ── Effacer ancien dino ──
      drawDino(dinoX, (int)prevDinoY, TFT_BLACK);

      // ── Redessiner le sol animé ──
      screen->drawLine(0, groundY, 240, groundY, TFT_WHITE);

      // ── Redessiner le dino ──
      drawDino(dinoX, (int)dinoY, TFT_WHITE);

      // ── Score ──
      screen->setTextColor(TFT_WHITE, TFT_BLACK);
      screen->setTextSize(1);
      screen->setCursor(180, 4);
      screen->print(score / 10);
      screen->print("m  ");
    }

    virtual ~DinoRunGame() {}
};

#endif