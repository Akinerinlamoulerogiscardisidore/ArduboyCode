#ifndef SPACEINVADERS_H
#define SPACEINVADERS_H

#include "Game.h"

// ════════════════════════════════════════════════
//  SPACE INVADERS — Détruire les envahisseurs
//  Boutons : GAUCHE/DROITE déplacer, A tirer
//  Game Over : envahisseurs atteignent le bas
//              ou le joueur est touché
// ════════════════════════════════════════════════

// ── Structure d'un envahisseur ──
struct Invader {
  int  x, y;    // position
  bool alive;   // vivant ?
};

// ── Structure d'un missile ──
struct Bullet {
  int  x, y;    // position
  bool active;  // actif ?
  bool isEnemy; // missile ennemi ou joueur ?
};

class SpaceInvadersGame : public Game {
  private:

    // ── Envahisseurs ──
    const int ROWS_INV  = 3;    // rangées d'envahisseurs
    const int COLS_INV  = 8;    // colonnes d'envahisseurs
    Invader invaders[3][8];
    int  invaderCount;          // nombre d'envahisseurs vivants
    int  invaderDirX;           // direction de déplacement (+1 ou -1)
    int  invaderOffX;           // décalage global des envahisseurs
    const int invW = 12;        // largeur d'un envahisseur
    const int invH = 8;         // hauteur d'un envahisseur
    const int invSpacing = 26;  // espacement entre envahisseurs

    // ── Joueur ──
    int playerX;                // position X du joueur
    const int playerY = 120;    // position Y fixe (bas)
    const int playerW = 12;     // largeur du vaisseau
    const int playerSpd = 3;    // vitesse

    // ── Missiles ──
    static const int MAX_BULLETS = 5;
    Bullet bullets[MAX_BULLETS];
    const int bulletSpd = 5;    // vitesse des missiles joueur
    const int enemyBulletSpd = 2; // vitesse des missiles ennemis

    // ── Timing ──
    unsigned long lastInvaderMove;  // dernier mouvement des envahisseurs
    unsigned long lastEnemyShot;    // dernier tir ennemi
    int  invaderMoveInterval;       // intervalle (accélère quand il en reste peu)
    bool needsRedraw;

    // ────────────────────────────────────────────
    //  Dessiner un envahisseur (pixel art simple)
    // ────────────────────────────────────────────
    void drawInvader(int x, int y, uint16_t color) {
      screen->fillRect(x + 2, y,     invW - 4, 2, color);  // tête
      screen->fillRect(x,     y + 2, invW,     4, color);  // corps
      screen->fillRect(x,     y + 3, 2,        2, color);  // patte gauche
      screen->fillRect(x + invW - 2, y + 3, 2, 2, color); // patte droite
    }

    // ────────────────────────────────────────────
    //  Tir aléatoire d'un ennemi vivant
    // ────────────────────────────────────────────
    void enemyShoot() {
      if (millis() - lastEnemyShot < 1500) return;
      lastEnemyShot = millis();

      // Choisir un envahisseur vivant au hasard
      int attempts = 0;
      while (attempts < 20) {
        int r = random(0, ROWS_INV);
        int c = random(0, COLS_INV);
        if (invaders[r][c].alive) {
          // Trouver un slot de missile libre
          for (int i = 0; i < MAX_BULLETS; i++) {
            if (!bullets[i].active && bullets[i].isEnemy) {
              bullets[i].x       = invaders[r][c].x + invW / 2;
              bullets[i].y       = invaders[r][c].y + invH;
              bullets[i].active  = true;
              return;
            }
          }
          break;
        }
        attempts++;
      }
    }

  public:
    SpaceInvadersGame(TFT_eSPI* display) : Game(display) {}

    void init() override {
      playerX             = 120;
      invaderDirX         = 1;
      invaderOffX         = 0;
      score               = 0;
      state               = IN_PROGRESS;
      invaderCount        = ROWS_INV * COLS_INV;
      invaderMoveInterval = 400;
      needsRedraw         = true;
      lastInvaderMove     = 0;
      lastEnemyShot       = 0;

      // Initialiser les envahisseurs en grille
      for (int r = 0; r < ROWS_INV; r++) {
        for (int c = 0; c < COLS_INV; c++) {
          invaders[r][c].x     = 10 + c * invSpacing;
          invaders[r][c].y     = 16 + r * 18;
          invaders[r][c].alive = true;
        }
      }

      // Initialiser les missiles
      for (int i = 0; i < MAX_BULLETS; i++) {
        bullets[i].active  = false;
        bullets[i].isEnemy = (i >= 2);  // 2 missiles joueur, 3 ennemis
      }
    }

    void update(Buttons buttons) override {
      if (state == GAME_OVER) return;

      bool changed = false;

      // ── Déplacement joueur ──
      if (buttons.left  && playerX > playerW / 2)       { playerX -= playerSpd; changed = true; }
      if (buttons.right && playerX < 240 - playerW / 2) { playerX += playerSpd; changed = true; }

      // ── Tir joueur ──
      if (buttons.aPressed) {
        for (int i = 0; i < 2; i++) {  // slots joueur = 0 et 1
          if (!bullets[i].active) {
            bullets[i].x      = playerX;
            bullets[i].y      = playerY;
            bullets[i].active = true;
            break;
          }
        }
      }

      // ── Tir ennemi ──
      enemyShoot();

      // ── Déplacement des missiles ──
      for (int i = 0; i < MAX_BULLETS; i++) {
        if (!bullets[i].active) continue;

        // Effacer ancienne position
        screen->fillRect(bullets[i].x - 1, bullets[i].y - 2, 2, 4, TFT_BLACK);

        if (bullets[i].isEnemy) bullets[i].y += enemyBulletSpd;
        else                    bullets[i].y -= bulletSpd;

        // Missile sorti de l'écran
        if (bullets[i].y < 0 || bullets[i].y > 135) {
          bullets[i].active = false;
          continue;
        }

        // Missile joueur touche un envahisseur
        if (!bullets[i].isEnemy) {
          for (int r = 0; r < ROWS_INV; r++) {
            for (int c = 0; c < COLS_INV; c++) {
              if (!invaders[r][c].alive) continue;
              if (bullets[i].x >= invaders[r][c].x &&
                  bullets[i].x <= invaders[r][c].x + invW &&
                  bullets[i].y >= invaders[r][c].y &&
                  bullets[i].y <= invaders[r][c].y + invH) {
                // Effacer l'envahisseur
                screen->fillRect(invaders[r][c].x, invaders[r][c].y, invW, invH, TFT_BLACK);
                invaders[r][c].alive = false;
                bullets[i].active   = false;
                invaderCount--;
                score += 10;
                invaderMoveInterval = max(80, 400 - invaderCount * 15);
                needsRedraw = true;
                if (invaderCount == 0) { state = GAME_OVER; return; }
              }
            }
          }
        }

        // Missile ennemi touche le joueur
        if (bullets[i].isEnemy &&
            bullets[i].x >= playerX - playerW / 2 &&
            bullets[i].x <= playerX + playerW / 2 &&
            bullets[i].y >= playerY) {
          state = GAME_OVER;
          return;
        }

        // Dessiner nouvelle position
        screen->fillRect(bullets[i].x - 1, bullets[i].y - 2, 2, 4,
          bullets[i].isEnemy ? TFT_RED : TFT_WHITE);
        changed = true;
      }

      // ── Déplacement des envahisseurs ──
      if (millis() - lastInvaderMove >= (unsigned long)invaderMoveInterval) {
        lastInvaderMove = millis();

        // Effacer tous les envahisseurs
        for (int r = 0; r < ROWS_INV; r++)
          for (int c = 0; c < COLS_INV; c++)
            if (invaders[r][c].alive)
              screen->fillRect(invaders[r][c].x, invaders[r][c].y, invW, invH, TFT_BLACK);

        // Vérifier si rebond nécessaire
        bool hitWall = false;
        for (int r = 0; r < ROWS_INV; r++) {
          for (int c = 0; c < COLS_INV; c++) {
            if (!invaders[r][c].alive) continue;
            int nx = invaders[r][c].x + invaderDirX * 3;
            if (nx < 0 || nx + invW > 240) { hitWall = true; break; }
          }
        }

        if (hitWall) {
          invaderDirX = -invaderDirX;
          // Descendre d'une rangée
          for (int r = 0; r < ROWS_INV; r++)
            for (int c = 0; c < COLS_INV; c++)
              invaders[r][c].y += 8;
        } else {
          for (int r = 0; r < ROWS_INV; r++)
            for (int c = 0; c < COLS_INV; c++)
              invaders[r][c].x += invaderDirX * 3;
        }

        // Game Over si envahisseurs atteignent le joueur
        for (int r = 0; r < ROWS_INV; r++)
          for (int c = 0; c < COLS_INV; c++)
            if (invaders[r][c].alive && invaders[r][c].y + invH >= playerY)
              { state = GAME_OVER; return; }

        needsRedraw = true;
      }

      if (changed) needsRedraw = true;
    }

    void render() override {
      if (!needsRedraw) return;
      needsRedraw = false;

      // ── Dessiner les envahisseurs vivants ──
      for (int r = 0; r < ROWS_INV; r++) {
        for (int c = 0; c < COLS_INV; c++) {
          if (invaders[r][c].alive) {
            uint16_t col = (r == 0) ? TFT_GREEN : (r == 1) ? TFT_CYAN : TFT_MAGENTA;
            drawInvader(invaders[r][c].x, invaders[r][c].y, col);
          }
        }
      }

      // ── Dessiner le joueur ──
      screen->fillRect(playerX - playerW/2, playerY, playerW, 8, TFT_BLACK);
      screen->fillRect(playerX - playerW/2, playerY, playerW, 8, TFT_GREEN);

      // ── Score ──
      screen->setTextColor(TFT_WHITE, TFT_BLACK);
      screen->setTextSize(1);
      screen->setCursor(2, 4);
      screen->print("SCORE:");
      screen->print(score);
      screen->print("  ");
    }

    virtual ~SpaceInvadersGame() {}
};

#endif