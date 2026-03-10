#pragma once
/*
 * Pong.h – Jeu de Pong pour Arduboy
 *
 * Contrôles :
 *   HAUT / BAS   : déplacer la raquette gauche (joueur)
 *   A            : rejouer après game-over
 *   B            : retour au menu (toujours actif)
 *
 * Règles :
 *   Premier à 7 points gagne.
 *   La raquette droite est contrôlée par une IA simple.
 *   La balle accélère légèrement à chaque rebond sur une raquette.
 */

#include <Arduboy2.h>

class PongGame {
public:
    // ── Initialisation ────────────────────────────────────────────────────────
    void init() {
        paddleL = (HEIGHT - PADDLE_H) / 2;
        paddleR = (HEIGHT - PADDLE_H) / 2;
        scoreL  = 0;
        scoreR  = 0;
        state   = PLAYING;
        winner  = 0;
        resetBall(1); // balle vers la droite au début
    }

    // ── Mise à jour — retourne true = retour au menu ──────────────────────────
    bool update(Arduboy2& ab) {
        if (ab.justPressed(B_BUTTON)) return true;

        if (state == GAME_OVER) {
            if (ab.justPressed(A_BUTTON)) init();
            return false;
        }

        // ── Joueur : raquette gauche ──────────────────────────────────────────
        if (ab.pressed(UP_BUTTON)   && paddleL > 0)               paddleL--;
        if (ab.pressed(DOWN_BUTTON) && paddleL < HEIGHT - PADDLE_H) paddleL++;

        // ── IA : raquette droite (se déplace uniquement quand la balle arrive) ─
        if (ballDX > 0) {
            int16_t center = paddleR + PADDLE_H / 2;
            if (ballY + BALL_H / 2 < center && paddleR > 0)                paddleR--;
            else if (ballY + BALL_H / 2 > center && paddleR < HEIGHT - PADDLE_H) paddleR++;
        }

        moveBall();
        checkScoring();

        return false;
    }

    // ── Rendu ─────────────────────────────────────────────────────────────────
    void draw(Arduboy2& ab) {
        ab.clear();

        if (state == GAME_OVER) { drawGameOver(ab); return; }

        // Ligne centrale pointillée
        for (int16_t y = 0; y < HEIGHT; y += 4)
            ab.drawPixel(WIDTH / 2, y, WHITE);

        // Raquettes
        ab.fillRect(PADDLE_X_L, paddleL, PADDLE_W, PADDLE_H, WHITE);
        ab.fillRect(PADDLE_X_R, paddleR, PADDLE_W, PADDLE_H, WHITE);

        // Balle
        ab.fillRect(ballX, ballY, BALL_W, BALL_H, WHITE);

        // Scores
        ab.setTextSize(1);
        ab.setCursor(52, 2); ab.print(scoreL);
        ab.setCursor(70, 2); ab.print(scoreR);

        // Instruction
        ab.setCursor(42, 56);
        ab.print(F("B: Menu"));
    }

private:
    // ── Constantes ────────────────────────────────────────────────────────────
    static const int16_t PADDLE_W  = 3;
    static const int16_t PADDLE_H  = 14;
    static const int16_t PADDLE_X_L = 4;
    static const int16_t PADDLE_X_R = WIDTH - PADDLE_W - 4; // 121
    static const int16_t BALL_W    = 3;
    static const int16_t BALL_H    = 3;
    static const uint8_t WIN_SCORE = 7;

    enum State : uint8_t { PLAYING, GAME_OVER };

    // ── État ──────────────────────────────────────────────────────────────────
    int16_t paddleL, paddleR;
    int16_t ballX, ballY;
    int16_t ballDX, ballDY; // vitesse signée en 1/2 pixel (divisée à l'affichage)
    uint8_t scoreL, scoreR;
    State   state;
    uint8_t winner; // 1 = joueur, 2 = IA

    // ── Physique ──────────────────────────────────────────────────────────────
    void resetBall(int8_t dir) {
        ballX  = WIDTH  / 2 - BALL_W / 2;
        ballY  = HEIGHT / 2 - BALL_H / 2;
        ballDX = dir * 2;
        ballDY = 1;
    }

    void moveBall() {
        ballX += ballDX;
        ballY += ballDY;

        // Rebond haut / bas
        if (ballY <= 0)                  { ballY = 0;                  ballDY = -ballDY; }
        if (ballY + BALL_H >= HEIGHT)    { ballY = HEIGHT - BALL_H;    ballDY = -ballDY; }

        // Collision raquette gauche (balle arrive de droite → ballDX < 0)
        if (ballDX < 0 &&
            ballX < PADDLE_X_L + PADDLE_W &&
            ballX + BALL_W > PADDLE_X_L   &&
            ballY + BALL_H > paddleL       &&
            ballY < paddleL + PADDLE_H)
        {
            ballX  = PADDLE_X_L + PADDLE_W;
            ballDX = abs(ballDX);
            ballDY = calcAngle(ballY, paddleL);
        }

        // Collision raquette droite (balle arrive de gauche → ballDX > 0)
        if (ballDX > 0 &&
            ballX + BALL_W >= PADDLE_X_R &&
            ballX < PADDLE_X_R + PADDLE_W &&
            ballY + BALL_H > paddleR       &&
            ballY < paddleR + PADDLE_H)
        {
            ballX  = PADDLE_X_R - BALL_W;
            ballDX = -abs(ballDX);
            ballDY = calcAngle(ballY, paddleR);
        }
    }

    // Calcule l'angle de rebond en fonction de la position sur la raquette
    int16_t calcAngle(int16_t by, int16_t paddle) {
        int16_t rel = (by + BALL_H / 2) - (paddle + PADDLE_H / 2);
        // rel : -7..+7 → ballDY : -3..+3
        int16_t angle = rel / 3;
        if (angle == 0) angle = (ballDY >= 0) ? 1 : -1;
        return angle;
    }

    void checkScoring() {
        if (ballX + BALL_W < 0) {
            // Balle sortie à gauche → IA marque
            scoreR++;
            resetBall(-1); // repart vers le joueur (perdant)
        }
        if (ballX > WIDTH) {
            // Balle sortie à droite → joueur marque
            scoreL++;
            resetBall(1); // repart vers l'IA (perdant)
        }

        if (scoreL >= WIN_SCORE || scoreR >= WIN_SCORE) {
            winner = (scoreL >= WIN_SCORE) ? 1 : 2;
            state  = GAME_OVER;
        }
    }

    // ── Rendu ─────────────────────────────────────────────────────────────────
    void drawGameOver(Arduboy2& ab) {
        ab.setTextSize(1);
        ab.fillRect(14, 15, 100, 34, BLACK);
        ab.drawRect(14, 15, 100, 34, WHITE);

        ab.setCursor(28, 21);
        if (winner == 1) ab.print(F("Vous gagnez !"));
        else             ab.print(F("L'IA gagne !"));

        ab.setCursor(30, 31);
        ab.print(scoreL); ab.print(F("  -  ")); ab.print(scoreR);

        ab.setCursor(16, 41);
        ab.print(F("A: Rejouer   B: Menu"));
    }
};
