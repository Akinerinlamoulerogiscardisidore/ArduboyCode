#ifndef PONG_H
#define PONG_H

#include "Game.h"

#ifndef VIBRATION_PIN
#define VIBRATION_PIN 16   // doit correspondre à la broche utilisée dans ArduBoy.ino
#endif
 
extern Settings settings;

class PongGame : public Game {
private:
    // Raquette joueur
    int paddleY;
    const int paddleX = 8;
    const int paddleH = 24;
    const int paddleW = 4;
    const int paddleSpd = 3;

    // Raquette IA
    int aiY;
    const int aiX = 228;
    const int aiSpd = 2;

    // Balle
    int ballX, ballY;
    float velX, velY;
    const int ballSize = 4;
    int lastHit; // 0=joueur, 1=IA

    // Score
    int playerScore;
    int aiScore;

    // Rendu optimisé
    int prevBallX, prevBallY;
    int prevPaddleY;
    int prevAiY;
    bool firstDraw;

    // Timing
    unsigned long lastUpdate;
    const int updateInterval = 16; // ~60 fps

    // Pause
    bool paused;
    int pauseIndex;
    unsigned long lastNavTime;
    const int navDelay = 200; // ms entre chaque "pas" dans menu

    // Redraw
    bool needsRedraw;

    // --- Fonctions internes ---
    void resetBall() {
        ballX = 120;
        ballY = 67;
        velX = (random(0, 2) == 0) ? 2.0 : -2.0;
        velY = (random(0, 2) == 0) ? 1.5 : -1.5;
    }

    void updateAI() {
        if (ballY > aiY + paddleH / 2) aiY += aiSpd;
        else if (ballY < aiY - paddleH / 2) aiY -= aiSpd;
        aiY = constrain(aiY, paddleH / 2, 135 - paddleH / 2);
    }

public:
    PongGame(TFT_eSPI* display) : Game(display) {}

    void init() override {
        paddleY = 67;
        aiY = 67;
        playerScore = 0;
        aiScore = 0;
        score = 0;
        state = IN_PROGRESS;
        firstDraw = true;
        lastUpdate = 0;
        lastHit = 0;
        paused = false;
        pauseIndex = 0;
        lastNavTime = 0;
        needsRedraw = true;
        resetBall();
    }
    void clearPauseMenu() {
    screen->fillRect(40, 30, 160, 80, TFT_BLACK); // juste la zone du menu
    }
    void update(Buttons buttons) override {
        if (state == GAME_OVER) return;

        unsigned long now = millis();
        if (buttons.bPressed) {
        paused = true;
        needsRedraw = true;
        return; // ne pas continuer la logique du jeu
        }

              // --- Gestion pause ---
        if (paused) {
          if (now - lastNavTime > navDelay) {
              // Navigation dans le menu
              if (buttons.up) {
                  pauseIndex = (pauseIndex + 2) % 3; // remonte
                  lastNavTime = now;
                  needsRedraw = true;
              }
              if (buttons.down) {
                  pauseIndex = (pauseIndex + 1) % 3; // descend
                  lastNavTime = now;
                  needsRedraw = true;
              }

              // Sélection
              if (buttons.aPressed) {
                  if (pauseIndex == 0) {       // Reprendre
                      paused = false;
                      // Efface juste la zone du menu sans toucher au reste
                      screen->fillRect(40, 30, 160, 80, TFT_BLACK);
                      needsRedraw = true;      // redessine la balle et les raquettes si nécessaire
                  }
                  else if (pauseIndex == 1) {  // Restart
                      init();
                  }
                  else if (pauseIndex == 2) {  // Quitter
                      state = GAME_OVER;
                  }
              }
          }
        return; // on ne fait pas la logique du jeu tant que le menu est actif
      }

        // --- Logique normale ---
        if (now - lastUpdate < updateInterval) return;
        lastUpdate = now;

        // Sauvegarde anciennes positions
        prevBallX = ballX;
        prevBallY = ballY;
        prevPaddleY = paddleY;
        prevAiY = aiY;

        // Déplacement joueur
        if (buttons.up) paddleY -= paddleSpd;
        if (buttons.down) paddleY += paddleSpd;
        paddleY = constrain(paddleY, paddleH / 2, 135 - paddleH / 2);

        // Déplacement IA
        updateAI();

        // Déplacement balle
        ballX += (int)velX;
        ballY += (int)velY;

        // Rebond murs
        if (ballY <= 0 || ballY >= 135) velY = -velY;

        // Collision raquette joueur
        if (ballX <= paddleX + paddleW &&
            ballY >= paddleY - paddleH / 2 &&
            ballY <= paddleY + paddleH / 2) {
            velX = abs(velX) + 0.1;
            velY = (ballY - paddleY) * 0.15;
            lastHit = 0;
        }

        // Collision raquette IA
        if (ballX >= aiX - paddleW &&
            ballY >= aiY - paddleH / 2 &&
            ballY <= aiY + paddleH / 2) {
            velX = -(abs(velX) + 0.1);
            velY = (ballY - aiY) * 0.15;
            lastHit = 1;
        }

        // Point marqué
        if (ballX < 0) {
            aiScore++;
            if (aiScore >= 5) { state = GAME_OVER; score = playerScore; }
            resetBall();
        }
        if (ballX > 240) {
            playerScore++;
            score = playerScore;
            if (playerScore >= 5) state = GAME_OVER;
            resetBall();
        }

        needsRedraw = true;
    }

    void render() override {
    if (!needsRedraw && !firstDraw) return;
    needsRedraw = false;

    // -------- MODE PAUSE --------
    if (paused) {
        // écran noir stable
        screen->fillScreen(TFT_BLACK);

        screen->setTextSize(1);

        // boîte
        screen->fillRoundRect(40, 40, 160, 67, 8, TFT_WHITE);
        screen->drawRoundRect(40, 40, 160, 67, 8, TFT_BLACK);

        // titre
        screen->setTextColor(TFT_BLACK);
        screen->setCursor(80, 50);
        screen->print("PAUSE");

        // options
        const char* options[3] = {"Reprendre", "Restart", "Quitter"};
        for(int i=0; i<3; i++){
            screen->setCursor(70, 65 + i*15);

            if(i==pauseIndex){
                screen->setTextColor(TFT_YELLOW);
                screen->print("> ");
            } else {
                screen->setTextColor(TFT_BLACK);
                screen->print("  ");
            }

            screen->print(options[i]);
        }

        return; // 🔴 CRUCIAL
    }

    // -------- JEU NORMAL --------

    if (firstDraw) {
        firstDraw = false;
        screen->fillScreen(TFT_BLACK);
    } else {
        // effacer anciennes positions
        screen->fillRect(prevBallX, prevBallY, ballSize, ballSize, TFT_BLACK);
        screen->fillRect(paddleX, prevPaddleY - paddleH / 2, paddleW, paddleH, TFT_BLACK);
        screen->fillRect(aiX, prevAiY - paddleH / 2, paddleW, paddleH, TFT_BLACK);
    }

    // raquettes
    screen->fillRect(paddleX, paddleY - paddleH / 2, paddleW, paddleH, TFT_BLUE);
    screen->fillRect(aiX, aiY - paddleH / 2, paddleW, paddleH, TFT_RED);

    // balle
    uint16_t ballColor = (lastHit == 0) ? TFT_BLUE : TFT_RED;
    screen->fillRect(ballX, ballY, ballSize, ballSize, ballColor);

    // score
    screen->setTextColor(TFT_WHITE, TFT_BLUE);
    screen->setTextSize(2);
    screen->setCursor(80, 4);
    screen->print(playerScore);

    screen->setTextColor(TFT_WHITE, TFT_RED);
    screen->setCursor(148, 4);
    screen->print(aiScore);
}

    virtual ~PongGame() {}
};

#endif