#ifndef SNAKE_H
#define SNAKE_H

#include "Game.h"
// En haut de Snake.h, après #include "Game.h"

#ifndef VIBRATION_PIN
#define VIBRATION_PIN 16   // doit correspondre à la broche utilisée dans ArduBoy.ino
#endif
extern Settings settings;

enum snakeDirection {
  SNAKE_UP,
  SNAKE_DOWN,
  SNAKE_LEFT,
  SNAKE_RIGHT
};

struct Segment {
  int x, y;
};

class SnakeGame : public Game {
  private:
    Segment snake[100];
    int snakeLength;
    snakeDirection direction;
    int appleX, appleY;

    Segment lastTail;
    bool moved;
    bool appleEaten;
    bool firstDraw;
    bool needsRedraw;

    const int gridSize = 8;
    const int gridW = 240 / 8;        // 30
    const int gridH = (135 - 20) / 8; // 14 (car 20px pour le score)
    const int scoreHeight = 20;       // hauteur de la zone de score en pixels
    const int gameAreaY = 20;         // début de la zone de jeu

    unsigned long lastMove;
    int moveInterval = 150;  // deviendra variable pour la vitesse
    
    bool paused;
    int pauseIndex;
    unsigned long lastNavTime;
    const int navDelay = 200;

    // Variables pour l'animation de fond
    int bgOffset = 0;
    unsigned long lastBgAnim = 0;
    bool isSpecialBg = false;  // fond spécial après score 10

    void spawnApple() {
      // Éviter de générer la pomme sur le serpent
      bool onSnake;
      do {
        onSnake = false;
        appleX = random(0, gridW);
        appleY = random(0, gridH);
        for (int i = 0; i < snakeLength; i++) {
          if (snake[i].x == appleX && snake[i].y == appleY) {
            onSnake = true;
            break;
          }
        }
      } while (onSnake);
    }

    void moveSnake() {
      if (millis() - lastMove < moveInterval) return;
      lastMove = millis();

      lastTail = snake[snakeLength - 1];

      for (int i = snakeLength - 1; i > 0; i--) {
        snake[i] = snake[i - 1];
      }

      switch (direction) {
        case SNAKE_UP:    snake[0].y -= 1; break;
        case SNAKE_DOWN:  snake[0].y += 1; break;
        case SNAKE_LEFT:  snake[0].x -= 1; break;
        case SNAKE_RIGHT: snake[0].x += 1; break;
      }

      moved = true;
    }

    void checkCollision() {
      // Collision avec les bords de la zone de jeu
      if (snake[0].x < 0 || snake[0].x >= gridW ||
          snake[0].y < 0 || snake[0].y >= gridH) {
        state = GAME_OVER;
        return;
      }
      // Collision avec soi-même
      for (int i = 1; i < snakeLength; i++) {
        if (snake[0].x == snake[i].x && snake[0].y == snake[i].y) {
          state = GAME_OVER;
          return;
        }
      }
    }

    void checkApple() {
      if (snake[0].x == appleX && snake[0].y == appleY) {
       if (settings.vibrationOn && !vibrationActive) {
        digitalWrite(VIBRATION_PIN, HIGH);
        vibrationEndTime = millis() + 60;
        vibrationActive = true;
      }
        snakeLength++;
        score++;
        
        // Son différent selon le score
        if (score >= 10) {
          if (settings.soundOn) tone(17, 1200, 80);
          // Activer le fond spécial si ce n'est pas déjà fait
          if (!isSpecialBg) {
            isSpecialBg = true;
            moveInterval = 100;  // accélère le jeu
          }
        } else {
          if (settings.soundOn) tone(17, 800, 60);tone(17, 800, 60);     // son normal
        }
        
        appleEaten = true;
        spawnApple();
        
        // Augmenter la vitesse progressivement
        if (moveInterval > 80 && score % 5 == 0) {
          moveInterval -= 5;
        }
      }
    }

    // Dessiner le fond (normal ou spécial SNK)
    void drawBackground() {
      if (!isSpecialBg) {
        // Fond noir simple
        screen->fillRect(0, gameAreaY, 240, 135 - gameAreaY, TFT_BLACK);
      } else {
        // Fond animé style SNK : dégradé de rouge/orange avec lignes mouvantes
        for (int y = gameAreaY; y < 135; y++) {
          uint16_t color = screen->color565(
            200 + (bgOffset + y) % 50,
            50 + (bgOffset + y) % 80,
            0
          );
          screen->drawFastHLine(0, y, 240, color);
        }
        // Dessiner des lignes horizontales défilantes
        for (int i = 0; i < 8; i++) {
          int yLine = (bgOffset + i * 15) % (135 - gameAreaY) + gameAreaY;
          screen->drawFastHLine(0, yLine, 240, TFT_YELLOW);
        }
      }
    }

  public:
    SnakeGame(TFT_eSPI* display) : Game(display) {}
    unsigned long vibrationEndTime = 0;
    bool vibrationActive = false;

    void init() override {
      snakeLength = 3;
      direction   = SNAKE_RIGHT;
      lastMove    = 0;
      score       = 0;
      state       = IN_PROGRESS;
      moved       = false;
      appleEaten  = false;
      firstDraw   = true;
      paused      = false;
      pauseIndex  = 0;
      needsRedraw = true;
      lastNavTime = 0;
      moveInterval = 150;
      isSpecialBg = false;
      bgOffset = 0;

      // Position initiale du serpent
      for (int i = 0; i < snakeLength; i++) {
        snake[i].x = (gridW / 2) - i;
        snake[i].y = gridH / 2;
      }

      spawnApple();
    }

    void update(Buttons buttons) override {
      if (state == GAME_OVER) return;

      unsigned long now = millis();

      // Animation du fond
    if (!paused && (now - lastBgAnim > 50)) {
        bgOffset++;
        lastBgAnim = now;
        needsRedraw = true;
      }
      if (vibrationActive && millis() >= vibrationEndTime) {
      digitalWrite(VIBRATION_PIN, LOW);
      vibrationActive = false;
    }
      // Gestion de la pause
      if (buttons.bPressed) {
        paused = !paused;
        needsRedraw = true;
        return;
      }

      if (paused) {
        
        if (now - lastNavTime > navDelay) {
          if (buttons.upPressed) {
            pauseIndex = (pauseIndex + 2) % 3;
            lastNavTime = now;
            needsRedraw = true;
          }
          if (buttons.downPressed) {
            pauseIndex = (pauseIndex + 1) % 3;
            lastNavTime = now;
            needsRedraw = true;
          }
          if (buttons.aPressed) {
            if (pauseIndex == 0) {
              paused = false;
              needsRedraw = true;
              firstDraw = true;   // ← ajoute cette ligne
            } else if (pauseIndex == 1) {
              init();
            } else if (pauseIndex == 2) {
              state = GAME_OVER;
            }
            lastNavTime = now;
          }
        }
        return;
      }

      // Mouvement
      if (buttons.upPressed    && direction != SNAKE_DOWN)  direction = SNAKE_UP;
      if (buttons.downPressed  && direction != SNAKE_UP)    direction = SNAKE_DOWN;
      if (buttons.rightPressed && direction != SNAKE_LEFT)  direction = SNAKE_RIGHT;
      if (buttons.leftPressed  && direction != SNAKE_RIGHT) direction = SNAKE_LEFT;

      moved = false;
      appleEaten = false;

      moveSnake();
      checkCollision();
      checkApple();

      if (moved || appleEaten) {
        needsRedraw = true;
      }
    }
  void render() override {
  if (!needsRedraw && !firstDraw ) return;
  needsRedraw = false;

    // Gestion de la pause (affichage par-dessus sans tout effacer)
  if (paused) {
        // écran noir stable
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

 
  if (firstDraw) {
    firstDraw = false;
    screen->fillScreen(TFT_BLACK);
    // Dessiner le score une fois pour toutes
    screen->fillRect(0, 0, 240, 20, TFT_CYAN);
    screen->setTextColor(TFT_WHITE, TFT_CYAN);
    screen->setTextSize(2);
    screen->setCursor(2, 2);
    screen->print("SCORE = ");
    screen->print(score);
    // Dessiner la pomme et le serpent initial
    screen->fillRect(appleX * gridSize, gameAreaY + appleY * gridSize, gridSize-1, gridSize-1, TFT_RED);
    for (int i=0; i<snakeLength; i++) {
      screen->fillRect(snake[i].x * gridSize, gameAreaY + snake[i].y * gridSize, gridSize-1, gridSize-1, i==0 ? TFT_WHITE : TFT_GREEN);
    }
    return;
  }

 // --- Effacement ciblé ---
  if (!appleEaten) {
    // effacer l'ancienne queue
    screen->fillRect(lastTail.x * gridSize, gameAreaY + lastTail.y * gridSize, gridSize-1, gridSize-1, TFT_BLACK);
  }

  // Redessiner la nouvelle tête (blanche)
  screen->fillRect(snake[0].x * gridSize, gameAreaY + snake[0].y * gridSize, gridSize-1, gridSize-1, TFT_WHITE);
  // Redessiner le second segment (vert)
  if (snakeLength > 1) {
    screen->fillRect(snake[1].x * gridSize, gameAreaY + snake[1].y * gridSize, gridSize-1, gridSize-1, TFT_GREEN);
  }

  // Si pomme mangée, effacer l'ancienne pomme et mettre à jour le score
  if (appleEaten) {
    screen->fillRect(appleX * gridSize, gameAreaY + appleY * gridSize, gridSize-1, gridSize-1, TFT_RED); // en fait on va redessiner la nouvelle
    // Mettre à jour l'affichage du score (car le score a changé)
    screen->fillRect(0, 0, 240, 20, TFT_CYAN);
    screen->setTextColor(TFT_WHITE, TFT_CYAN);
    screen->setCursor(2, 2);
    screen->print("SCORE = ");
    screen->print(score);
    // Redessiner la nouvelle pomme
    screen->fillRect(appleX * gridSize, gameAreaY + appleY * gridSize, gridSize-1, gridSize-1, TFT_RED);
  }

  // Si le fond spécial est activé, tu peux l'animer sans tout effacer
  // mais c'est plus complexe : il faudrait ne redessiner que les parties du fond qui changent
  // ou accepter un effacement périodique (moins grave)
}
    virtual ~SnakeGame() {}
};

#endif