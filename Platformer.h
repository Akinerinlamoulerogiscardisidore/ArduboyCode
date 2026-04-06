#ifndef PLATFORMER_H
#define PLATFORMER_H

#include "Game.h"

#ifndef VIBRATION_PIN
#define VIBRATION_PIN 16
#endif

extern Settings settings;

struct Platform {
    int x, y, w;
    bool isTrampoline;   // rebond fort
    bool isBreakable;    // disparaît après rebond
    bool hasObstacle;    // obstacle mortel
};

class PlatformerGame : public Game {
private:
    int playerX, playerY;
    int playerVy;
    const int playerW = 8, playerH = 8;

    const int gravity = 1;
    const int jumpSpeed = -6;
    const int trampolineSpeed = -12;

    static const int NUM_PLATFORMS = 7;
    Platform platforms[NUM_PLATFORMS];

    bool firstDraw;
    bool moved;

    const int screenW = 240;
    const int screenH = 135;

    int score;
    int level;

    // Variables pour le fond animé
    int bgHue;
    unsigned long lastBgUpdate;

    void initPlatforms() {
        for (int i = 0; i < NUM_PLATFORMS; i++) {
            platforms[i].w = random(30, 60);
            platforms[i].x = random(0, screenW - platforms[i].w);
            platforms[i].y = i * (screenH / NUM_PLATFORMS);
            platforms[i].isTrampoline = false;
            platforms[i].isBreakable = false;
            platforms[i].hasObstacle = false;
        }

        // Plateforme de départ sous joueur
        platforms[0].y = screenH - 10;
        platforms[0].w = 60;
        platforms[0].x = (screenW / 2) - 30;
        // On place le joueur sur cette plateforme
        playerX = platforms[0].x + platforms[0].w/2 - playerW/2;
        playerY = platforms[0].y - playerH;
    }

    void updatePlatforms() {
        if (playerY < screenH / 2) {
            int dy = (screenH / 2) - playerY;
            playerY = screenH / 2;

            for (int i = 0; i < NUM_PLATFORMS; i++) {
                platforms[i].y += dy;

                if (platforms[i].y > screenH) {
                    platforms[i].y = 0;
                    platforms[i].x = random(0, screenW - platforms[i].w);
                    platforms[i].isTrampoline = (random(0, 100) < 10);  // 10% trampoline
                    platforms[i].isBreakable = (random(0, 100) < 10);   // 10% cassable
                    platforms[i].hasObstacle = (random(0, 100) < 20);   // 20% obstacle
                    score++;
                    // Ajustement du niveau : vitesse augmente à partir de 50 points
                    if (score >= 50 && score < 100) level = 2;
                    else if (score >= 100) level = 3;
                    else level = 1;
                }
            }
        }
    }

    void movePlayer(Buttons buttons) {
        // Vitesse horizontale progressive (max 5)
        int speed = 3 + (level-1);
        if (speed > 5) speed = 5;
        if (buttons.left && playerX > 0) playerX -= speed;
        if (buttons.right && playerX < screenW - playerW) playerX += speed;

        playerVy += gravity;
        playerY += playerVy;

        if (playerVy > 0) {
            for (int i = 0; i < NUM_PLATFORMS; i++) {
                if (playerX + playerW > platforms[i].x &&
                    playerX < platforms[i].x + platforms[i].w &&
                    playerY + playerH >= platforms[i].y &&
                    playerY + playerH <= platforms[i].y + 6) {

                    playerY = platforms[i].y - playerH;
                    playerVy = 0;

                    // Son d'atterrissage
                    if (settings.soundOn) tone(17, 400, 30);

                    if (platforms[i].isTrampoline) {
                        playerVy = trampolineSpeed;
                        if (settings.soundOn) tone(17, 1200, 50);
                    }

                    if (platforms[i].isBreakable) {
                        platforms[i].y = screenH + 10;
                        platforms[i].isBreakable = false;
                    }

                    if (platforms[i].hasObstacle) {
                        state = GAME_OVER;
                        return;
                    }

                    break;
                }
            }
        }

        // Saut volontaire (touche A) seulement si le joueur est sur une plateforme
        if (buttons.aPressed && playerVy == 0 && playerY + playerH >= screenH-10) {
            playerVy = jumpSpeed;
            if (settings.soundOn) tone(17, 600, 40);
        }

        if (playerY > screenH) {
            state = GAME_OVER;
        }
    }

    // Dessine un fond dégradé animé
    void drawBackground() {
        for (int y = 0; y < screenH; y++) {
            uint16_t color = screen->color565(
                100 + (bgHue + y/2) % 100,
                50 + (bgHue + y/3) % 80,
                150 + (bgHue + y/4) % 100
            );
            screen->drawFastHLine(0, y, screenW, color);
        }
    }

public:
    PlatformerGame(TFT_eSPI* display) : Game(display) {}

    void init() override {
        playerX = screenW / 2;
        playerY = screenH - 20;
        playerVy = 0;
        firstDraw = true;
        moved = true;
        score = 0;
        level = 1;
        bgHue = 0;
        lastBgUpdate = 0;

        initPlatforms();
        state = IN_PROGRESS;
    }

    void update(Buttons buttons) override {
        if (state == GAME_OVER) return;

        unsigned long now = millis();
        if (now - lastBgUpdate > 30) {
            bgHue = (bgHue + 2) % 360;
            lastBgUpdate = now;
            moved = true;
        }

        moved = true;
        movePlayer(buttons);
        updatePlatforms();
    }

    void render() override {
        if (!moved) return;

        drawBackground();  // Fond coloré animé

        // Dessiner les plateformes
        for (int i = 0; i < NUM_PLATFORMS; i++) {
            uint16_t color = TFT_DARKGREY;
            if (platforms[i].isTrampoline) color = TFT_CYAN;
            else if (platforms[i].isBreakable) color = TFT_ORANGE;
            screen->fillRect(platforms[i].x, platforms[i].y, platforms[i].w, 4, color);
            screen->drawRect(platforms[i].x, platforms[i].y, platforms[i].w, 4, TFT_WHITE);

            // Dessiner un obstacle si présent
            if (platforms[i].hasObstacle) {
                int obsX = platforms[i].x + platforms[i].w/2 - 4;
                int obsY = platforms[i].y - 5;
                screen->fillRect(obsX, obsY, 8, 5, TFT_RED);
                screen->fillRect(obsX+3, obsY-3, 2, 3, TFT_RED);
            }
        }

        // Joueur
        screen->fillRect(playerX, playerY, playerW, playerH, TFT_GREEN);

        // Affichage des scores
        screen->setTextColor(TFT_WHITE, TFT_BLACK);
        screen->setCursor(5, 5);
        screen->print("Score: ");
        screen->print(score);
        screen->setCursor(5, 20);
        screen->print("Level: ");
        screen->print(level);

        moved = false;
    }

    virtual ~PlatformerGame() {}
};

#endif