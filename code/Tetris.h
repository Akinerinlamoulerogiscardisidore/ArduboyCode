#ifndef TETRIS_H
#define TETRIS_H

#include "Game.h"

// ════════════════════════════════════════════════
//  TETRIS — Empiler les pièces et compléter des lignes
//  Boutons : GAUCHE/DROITE déplacer, HAUT tourner,
//            BAS accélérer, A rotation rapide
//  Game Over : la grille est pleine
// ════════════════════════════════════════════════

class TetrisGame : public Game {
  private:

    // ── Dimensions de la grille ──
    const int COLS     = 12;    // colonnes de jeu
    const int ROWS     = 17;    // rangées de jeu
    const int CELL     = 7;     // taille d'une case en pixels
    const int offsetX  = 52;    // décalage gauche pour centrer
    const int offsetY  = 16;    // décalage haut (place pour le score)

    // ── Grille de jeu : 0=vide, sinon = couleur ──
    uint16_t grid[17][12];

    // ── Les 7 pièces Tetris (tétrominos) ──
    // Chaque pièce = 4 coordonnées {x,y} relatives
    const int pieces[7][4][2] = {
      {{0,0},{1,0},{2,0},{3,0}},   // I — cyan
      {{0,0},{0,1},{1,0},{1,1}},   // O — jaune
      {{1,0},{0,1},{1,1},{2,1}},   // T — violet
      {{0,0},{1,0},{1,1},{2,1}},   // S — vert
      {{1,0},{2,0},{0,1},{1,1}},   // Z — rouge
      {{0,0},{0,1},{1,1},{2,1}},   // J — bleu
      {{2,0},{0,1},{1,1},{2,1}},   // L — orange
    };

    // ── Couleurs des pièces ──
    const uint16_t pieceColors[7] = {
      TFT_CYAN, TFT_YELLOW, TFT_MAGENTA,
      TFT_GREEN, TFT_RED, TFT_BLUE, TFT_ORANGE
    };

    // ── Pièce courante ──
    int  curPiece;       // index du tétromino actuel (0-6)
    int  curColor;       // couleur associée
    int  curX, curY;     // position dans la grille
    int  rotation;       // rotation actuelle (0-3)
    int  curShape[4][2]; // coordonnées calculées avec rotation

    // ── Timing ──
    unsigned long lastFall;    // dernier déplacement automatique vers le bas
    int  fallInterval;         // intervalle de chute (diminue avec le score)
    bool needsRedraw;

    // ────────────────────────────────────────────
    //  Appliquer la rotation à une pièce
    //  On utilise la formule de rotation 2D :
    //  rot 90° : (x,y) → (y,-x) → (y, maxX-x)
    // ────────────────────────────────────────────
    void applyRotation(int piece, int rot, int shape[4][2]) {
      for (int i = 0; i < 4; i++) {
        int x = pieces[piece][i][0];
        int y = pieces[piece][i][1];
        for (int r = 0; r < rot; r++) {
          int tmp = x;
          x = y;
          y = 3 - tmp;
        }
        shape[i][0] = x;
        shape[i][1] = y;
      }
    }

    // ────────────────────────────────────────────
    //  Vérifier si une position est valide
    //  (dans la grille ET cases vides)
    // ────────────────────────────────────────────
    bool isValid(int px, int py, int shape[4][2]) {
      for (int i = 0; i < 4; i++) {
        int nx = px + shape[i][0];
        int ny = py + shape[i][1];
        if (nx < 0 || nx >= COLS || ny >= ROWS) return false;
        if (ny >= 0 && grid[ny][nx] != TFT_BLACK) return false;
      }
      return true;
    }

    // ────────────────────────────────────────────
    //  Fixer la pièce dans la grille
    // ─────────────────────────────���──────────────
    void lockPiece() {
      for (int i = 0; i < 4; i++) {
        int nx = curX + curShape[i][0];
        int ny = curY + curShape[i][1];
        if (ny >= 0) grid[ny][nx] = curColor;
      }
    }

    // ────────────────────────────────────────────
    //  Vérifier et supprimer les lignes complètes
    // ────────────────────────────────────────────
    void clearLines() {
      int cleared = 0;
      for (int r = ROWS - 1; r >= 0; r--) {
        bool full = true;
        for (int c = 0; c < COLS; c++) {
          if (grid[r][c] == TFT_BLACK) { full = false; break; }
        }
        if (full) {
          cleared++;
          // Décaler toutes les lignes au-dessus vers le bas
          for (int row = r; row > 0; row--) {
            for (int c = 0; c < COLS; c++) {
              grid[row][c] = grid[row-1][c];
            }
          }
          // Vider la ligne du haut
          for (int c = 0; c < COLS; c++) grid[0][c] = TFT_BLACK;
          r++;  // revérifier la même ligne
        }
      }
      // Score : 1 ligne = 10pts, 2 = 30pts, 3 = 60pts, 4 = 100pts
      if (cleared > 0) {
        score += cleared * cleared * 10;
        needsRedraw = true;
        // Accélérer la chute selon le score
        fallInterval = max(100, 500 - score * 2);
      }
    }

    // ────────────────────────────────────────────
    //  Spawner une nouvelle pièce aléatoire
    // ────────────────────────────────────────────
    void spawnPiece() {
      curPiece = random(0, 7);
      curColor = pieceColors[curPiece];
      curX     = COLS / 2 - 2;
      curY     = 0;
      rotation = 0;
      applyRotation(curPiece, rotation, curShape);

      // Game Over si la pièce n'a pas de place
      if (!isValid(curX, curY, curShape)) {
        state = GAME_OVER;
      }
    }

    // ────────────────────────────────────────────
    //  Dessiner une case de la grille à l'écran
    // ────────────────────────────────────────────
    void drawCell(int col, int row, uint16_t color) {
      screen->fillRect(
        offsetX + col * CELL,
        offsetY + row * CELL,
        CELL - 1, CELL - 1,
        color
      );
    }

  public:
    TetrisGame(TFT_eSPI* display) : Game(display) {}

    void init() override {
      // Vider la grille
      for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
          grid[r][c] = TFT_BLACK;

      score        = 0;
      state        = IN_PROGRESS;
      fallInterval = 500;
      needsRedraw  = true;
      lastFall     = 0;
      spawnPiece();
    }

    void update(Buttons buttons) override {
      if (state == GAME_OVER) return;

      int  tmpShape[4][2];
      bool moved = false;

      // ── Déplacement gauche ──
      if (buttons.leftPressed) {
        if (isValid(curX - 1, curY, curShape)) { curX--; moved = true; }
      }

      // ── Déplacement droite ──
      if (buttons.rightPressed) {
        if (isValid(curX + 1, curY, curShape)) { curX++; moved = true; }
      }

      // ── Rotation ──
      if (buttons.aPressed || buttons.upPressed) {
        int newRot = (rotation + 1) % 4;
        applyRotation(curPiece, newRot, tmpShape);
        if (isValid(curX, curY, tmpShape)) {
          rotation = newRot;
          applyRotation(curPiece, rotation, curShape);
          moved = true;
        }
      }

      // ── Chute automatique + accélération avec BAS ──
      int interval = buttons.down ? 50 : fallInterval;
      if (millis() - lastFall >= (unsigned long)interval) {
        lastFall = millis();
        if (isValid(curX, curY + 1, curShape)) {
          curY++;
          moved = true;
        } else {
          // La pièce atterrit
          lockPiece();
          clearLines();
          spawnPiece();
          needsRedraw = true;
        }
      }

      if (moved) needsRedraw = true;
    }

    void render() override {
      if (!needsRedraw) return;
      needsRedraw = false;

      // Fond de la zone de jeu
      screen->fillRect(offsetX, offsetY, COLS * CELL, ROWS * CELL, TFT_BLACK);

      // ── Dessiner la grille fixe ──
      for (int r = 0; r < ROWS; r++)
        for (int c = 0; c < COLS; c++)
          if (grid[r][c] != TFT_BLACK)
            drawCell(c, r, grid[r][c]);

      // ── Dessiner la pièce courante ──
      for (int i = 0; i < 4; i++)
        drawCell(curX + curShape[i][0], curY + curShape[i][1], curColor);

      // ── Bordures de la zone de jeu ──
      screen->drawRect(offsetX - 1, offsetY - 1, COLS * CELL + 2, ROWS * CELL + 2, TFT_WHITE);

      // ── Score ──
      screen->fillRect(0, 0, 50, 16, TFT_BLACK);
      screen->setTextColor(TFT_WHITE, TFT_BLACK);
      screen->setTextSize(1);
      screen->setCursor(2, 4);
      screen->print("SC:");
      screen->print(score);
    }

    virtual ~TetrisGame() {}
};

#endif