// #include <TFT_eSPI.h>
// #include <EEPROM.h>

// // Bibliothèques console 
// #include "Game.h"
// #include "Menu.h"

// //  Bibliothèques des jeux 
// #include "Snake.h"
// #include "Morpion.h"
// #include "Pong.h"
// #include "Breakout.h"
// #include "FlappyBird.h"
// #include "Tetris.h"
// #include "SpaceInvaders.h"
// #include "DinoRun.h"
// #include "Maze.h"
// #include "Memory.h"

// //  Broches des boutons
// #define BTN_UP     25
// #define BTN_DOWN   26
// #define BTN_LEFT   27
// #define BTN_RIGHT  32
// #define BTN_A      13
// #define BTN_B      15
// #define BUZZER_PIN 17

// Buttons buttons;

// //États de la console
// enum stateConsole {
//   STARTING,
//   MENU,
//   PLAYING,
//   GAMEOVER
// };

// int currentGameId = -1;

// // Variables globales
// TFT_eSPI screen = TFT_eSPI();
// Menu menu(&screen);
// stateConsole consoleState = STARTING;
// Game* currentGame = nullptr;
// int highScore = 0;

// //  Lecture des boutons
// //  Calcule les fronts montants (pressed)
// //  = vrai uniquement le frame où le bouton
// //    vient d'être appuyé

// void readButtons() {
//   bool prevUp    = buttons.up;
//   bool prevDown  = buttons.down;
//   bool prevLeft  = buttons.left;
//   bool prevRight = buttons.right;
//   bool prevA     = buttons.a;
//   bool prevB     = buttons.b;

//   buttons.up    = !digitalRead(BTN_UP);
//   buttons.down  = !digitalRead(BTN_DOWN);
//   buttons.left  = !digitalRead(BTN_LEFT);
//   buttons.right = !digitalRead(BTN_RIGHT);
//   buttons.a     = !digitalRead(BTN_A);
//   buttons.b     = !digitalRead(BTN_B);

//   buttons.upPressed    = buttons.up    && !prevUp;
//   buttons.downPressed  = buttons.down  && !prevDown;
//   buttons.leftPressed  = buttons.left  && !prevLeft;
//   buttons.rightPressed = buttons.right && !prevRight;
//   buttons.aPressed     = buttons.a     && !prevA;
//   buttons.bPressed     = buttons.b     && !prevB;
// }


// //  Écran de démarrage
// //  Affiché une seule fois au boot
// void showBoot() {
//   screen.fillScreen(TFT_BLACK);

//   screen.drawRect(10, 10, 220, 80, TFT_CYAN);
//   screen.drawRect(12, 12, 216, 76, TFT_CYAN);

//   screen.setTextColor(TFT_CYAN, TFT_BLACK);
//   screen.setTextSize(3);
//   screen.setCursor(30, 35);
//   screen.print(" ARDUBOY ");

//   screen.setTextSize(1);
//   screen.setTextColor(TFT_WHITE, TFT_BLACK);
//   screen.setCursor(55, 60);
//   screen.print("UNE CONSOLE DE JEUX");

//   // Son de démarrage
//   tone(BUZZER_PIN, 523, 150); delay(150);
//   tone(BUZZER_PIN, 659, 150); delay(150);
//   tone(BUZZER_PIN, 784, 300); delay(300);

//   // Barre de chargement
//   screen.drawRect(20, 100, 200, 15, TFT_WHITE);
//   for (int i = 0; i <= 100; i += 2) {
//     int largeur = (i * 196) / 100;
//     screen.fillRect(22, 102, largeur, 10, TFT_GREEN);
//     delay(30);
//   }

//   delay(500);
//   consoleState = MENU;
// }


// //  Écran GAME OVER
// //  Affiche le score + meilleur score
// //  Sauvegarde le meilleur score en EEPROM
// //  Appelé UNE SEULE FOIS depuis case GAMEOVER

// void showGameOver() {
//   highScore = EEPROM.read(0);

//   if (currentGame->getScore() > highScore) {
//     highScore = currentGame->getScore();
//     EEPROM.write(0, highScore);
//     EEPROM.commit();
//   }

//   int lastScore = currentGame->getScore();

//   screen.fillScreen(TFT_BLACK);

//   screen.setTextColor(TFT_RED, TFT_BLACK);
//   screen.setTextSize(3);
//   screen.setCursor(30, 15);
//   screen.print("GAME OVER !");

//   screen.setTextColor(TFT_WHITE, TFT_BLACK);
//   screen.setTextSize(2);
//   screen.setCursor(20, 55);
//   screen.print("SCORE  : ");
//   screen.print(lastScore);

//   screen.setTextColor(TFT_YELLOW, TFT_BLACK);
//   screen.setCursor(25, 75);
//   screen.print("MEILLEUR: ");
//   screen.print(highScore);

//   screen.setTextColor(TFT_GREEN, TFT_BLACK);
//   screen.setTextSize(1);
//   screen.setCursor(20, 110);
//   screen.print("[A] Rejouer  [B] Menu");
// }


// //  Lancement d'un jeu
// //  Supprime l'ancien jeu, crée le nouveau,
// //  l'initialise et passe en état PLAYING

// void launchGame(int gameId) {
//   if (currentGame != nullptr) {
//     delete currentGame;
//     currentGame = nullptr;
//   }

//   switch (gameId) {
//     case 0: currentGame = new SnakeGame(&screen);         break;
//     case 1: currentGame = new MorpionGame(&screen);       break;
//     case 2: currentGame = new PongGame(&screen);          break;
//     case 3: currentGame = new BreakoutGame(&screen);      break;
//     case 4: currentGame = new FlappyBirdGame(&screen);    break;
//     case 5: currentGame = new TetrisGame(&screen);        break;
//     case 6: currentGame = new SpaceInvadersGame(&screen); break;
//     case 7: currentGame = new DinoRunGame(&screen);       break;
//     case 8: currentGame = new MazeGame(&screen);          break;
//     case 9: currentGame = new MemoryGame(&screen);        break;
//   }

//   if (currentGame != nullptr) {
//     currentGame->init();
//     consoleState = PLAYING;
//   }
// }


// //  SETUP
// void setup() {
//   screen.init();
//   screen.setRotation(1);
//   screen.fillScreen(TFT_BLACK);

//   pinMode(BTN_UP,    INPUT_PULLUP);
//   pinMode(BTN_DOWN,  INPUT_PULLUP);
//   pinMode(BTN_LEFT,  INPUT_PULLUP);
//   pinMode(BTN_RIGHT, INPUT_PULLUP);
//   pinMode(BTN_A,     INPUT_PULLUP);
//   pinMode(BTN_B,     INPUT_PULLUP);
//   pinMode(BUZZER_PIN, OUTPUT);

//   EEPROM.begin(16);

//   menu.addGame("Snake",          "Mange les pommes!",        "", 0);
//   menu.addGame("Morpion",        "2 joueurs - A pour jouer!","", 1);
//   menu.addGame("Pong",           "Bat l'IA en 5 points!",    "", 2);
//   menu.addGame("Breakout",       "Casse toutes les briques!", "", 3);
//   menu.addGame("Flappy Bird",    "Evite les tuyaux!",        "", 4);
//   menu.addGame("Tetris",         "Aligne les lignes!",       "", 5);
//   menu.addGame("Space Invaders", "Detruit les envahisseurs!", "", 6);
//   menu.addGame("Dino Run",       "Saute les cactus!",        "", 7);
//   menu.addGame("Labyrinthe",     "Trouve la sortie!",        "", 8);
//   menu.addGame("Memory",         "Trouve les paires!",       "", 9);

//   showBoot();
// }


// //  LOOP
// //  Machine à états de la console :
// //  STARTING → MENU → PLAYING → GAMEOVER → MENU
// void loop() {
//   readButtons();

//   switch (consoleState) {

//     // ── Démarrage ──
//     case STARTING:
//       break;

//     // ── Menu principal ──
//     case MENU: {
//       menu.update(buttons);
//       menu.render();

//       if (buttons.aPressed) {
//         currentGameId = menu.getSelectedId();
//         launchGame(currentGameId);
//       }
//       break;
//     }

//     // ── Jeu en cours ──
//     case PLAYING: {
//       if (currentGame != nullptr) {
//         currentGame->update(buttons);
//         currentGame->render();
//         if (currentGame->isGameOver()) {
//           consoleState = GAMEOVER;
//         }
//       }
//       break;
//     }

//     // ── Game Over ──
//     case GAMEOVER: {
//       static bool gameOverDrawn = false;

//       if (!gameOverDrawn) {
//         showGameOver();        // dessiné UNE SEULE FOIS
//         gameOverDrawn = true;
//       }

//       if (buttons.aPressed) {
//         gameOverDrawn = false;
//         launchGame(currentGameId);
//       }
//       if (buttons.bPressed) {
//         gameOverDrawn = false;
//         consoleState = MENU;
//         menu.forceRedraw();
//       }
//       break;
//     }
//   }

//   delay(1);
// }


// #include <TFT_eSPI.h>
// #include <EEPROM.h>

// // Bibliothèques console 
// #include "Game.h"
// #include "Menu.h"

// //  Bibliothèques des jeux 
// #include "Snake.h"
// #include "Morpion.h"
// #include "Pong.h"
// #include "Breakout.h"
// #include "FlappyBird.h"
// #include "Tetris.h"
// #include "SpaceInvaders.h"
// #include "DinoRun.h"
// #include "Maze.h"
// #include "Memory.h"

// //  Broches des boutons
// #define BTN_UP     25
// #define BTN_DOWN   26
// #define BTN_LEFT   27
// #define BTN_RIGHT  32
// #define BTN_A      13
// #define BTN_B      15
// #define BUZZER_PIN 17

// Buttons buttons;

// //États de la console
// enum stateConsole {
//   STARTING,
//   MENU,
//   PLAYING,
//   GAMEOVER,
//   GAMEWIN      // victoire pour les jeux avec une fin heureuse
// };

// int currentGameId = -1;

// // Variables globales
// TFT_eSPI screen = TFT_eSPI();
// Menu menu(&screen);
// stateConsole consoleState = STARTING;
// Game* currentGame = nullptr;

// // ── High scores par jeu (10 jeux × 4 bytes = 40 bytes EEPROM) ──
// // Adresse EEPROM du jeu N = N * 4
// #define EEPROM_SIZE     40
// #define SCORE_ADDR(id)  ((id) * 4)

// int getHighScore(int gameId) {
//   int val = 0;
//   EEPROM.get(SCORE_ADDR(gameId), val);
//   // Si EEPROM jamais écrite (valeur corrompue), on retourne 0
//   if (val < 0 || val > 99999) val = 0;
//   return val;
// }

// void saveHighScore(int gameId, int newScore) {
//   int current = getHighScore(gameId);
//   if (newScore > current) {
//     EEPROM.put(SCORE_ADDR(gameId), newScore);
//     EEPROM.commit();
//   }
// }

// //  Lecture des boutons
// //  Calcule les fronts montants (pressed)
// //  = vrai uniquement le frame où le bouton
// //    vient d'être appuyé

// void readButtons() {
//   bool prevUp    = buttons.up;
//   bool prevDown  = buttons.down;
//   bool prevLeft  = buttons.left;
//   bool prevRight = buttons.right;
//   bool prevA     = buttons.a;
//   bool prevB     = buttons.b;

//   buttons.up    = !digitalRead(BTN_UP);
//   buttons.down  = !digitalRead(BTN_DOWN);
//   buttons.left  = !digitalRead(BTN_LEFT);
//   buttons.right = !digitalRead(BTN_RIGHT);
//   buttons.a     = !digitalRead(BTN_A);
//   buttons.b     = !digitalRead(BTN_B);

//   buttons.upPressed    = buttons.up    && !prevUp;
//   buttons.downPressed  = buttons.down  && !prevDown;
//   buttons.leftPressed  = buttons.left  && !prevLeft;
//   buttons.rightPressed = buttons.right && !prevRight;
//   buttons.aPressed     = buttons.a     && !prevA;
//   buttons.bPressed     = buttons.b     && !prevB;
// }


// //  Écran de démarrage
// //  Affiché une seule fois au boot
// void showBoot() {
//   screen.fillScreen(TFT_BLACK);

//   screen.drawRect(10, 10, 220, 80, TFT_CYAN);
//   screen.drawRect(12, 12, 216, 76, TFT_CYAN);

//   screen.setTextColor(TFT_CYAN, TFT_BLACK);
//   screen.setTextSize(3);
//   screen.setCursor(30, 35);
//   screen.print(" ARDUBOY ");

//   screen.setTextSize(1);
//   screen.setTextColor(TFT_WHITE, TFT_BLACK);
//   screen.setCursor(55, 60);
//   screen.print("UNE CONSOLE DE JEUX");

//   // Son de démarrage
//   tone(BUZZER_PIN, 523, 150); delay(150);
//   tone(BUZZER_PIN, 659, 150); delay(150);
//   tone(BUZZER_PIN, 784, 300); delay(300);

//   // Barre de chargement
//   screen.drawRect(20, 100, 200, 15, TFT_WHITE);
//   for (int i = 0; i <= 100; i += 2) {
//     int largeur = (i * 196) / 100;
//     screen.fillRect(22, 102, largeur, 10, TFT_GREEN);
//     delay(30);
//   }

//   delay(500);
//   consoleState = MENU;
// }


// //  Écran GAME OVER
// //  Affiche le score + meilleur score
// //  Sauvegarde le meilleur score en EEPROM
// //  Appelé UNE SEULE FOIS depuis case GAMEOVER

// void showGameOver() {
//   int lastScore = currentGame->getScore();
//   int highScore = getHighScore(currentGameId);

//   // Sauvegarder si nouveau record
//   if (lastScore > highScore) {
//     highScore = lastScore;
//     saveHighScore(currentGameId, lastScore);
//   }

//   screen.fillScreen(TFT_BLACK);

//   screen.setTextColor(TFT_RED, TFT_BLACK);
//   screen.setTextSize(3);
//   screen.setCursor(30, 15);
//   screen.print("GAME OVER !");

//   screen.setTextColor(TFT_WHITE, TFT_BLACK);
//   screen.setTextSize(2);
//   screen.setCursor(20, 55);
//   screen.print("SCORE  : ");
//   screen.print(lastScore);

//   screen.setTextColor(TFT_YELLOW, TFT_BLACK);
//   screen.setCursor(25, 75);
//   screen.print("MEILLEUR: ");
//   screen.print(highScore);

//   screen.setTextColor(TFT_GREEN, TFT_BLACK);
//   screen.setTextSize(1);
//   screen.setCursor(20, 110);
//   screen.print("[A] Rejouer  [B] Menu");
// }


// //  Écran GAME WIN
// //  Affiché quand le joueur remporte la partie
// //  (Pong, Morpion J1, Maze, Memory, Breakout)

// void showGameWin() {
//   int lastScore = currentGame->getScore();
//   int highScore = getHighScore(currentGameId);

//   if (lastScore > highScore) {
//     highScore = lastScore;
//     saveHighScore(currentGameId, lastScore);
//   }

//   screen.fillScreen(TFT_BLACK);

//   // Cadre doré
//   screen.drawRect(8,  8,  224, 90, TFT_YELLOW);
//   screen.drawRect(10, 10, 220, 86, TFT_YELLOW);

//   screen.setTextColor(TFT_YELLOW, TFT_BLACK);
//   screen.setTextSize(2);
//   screen.setCursor(35, 18);
//   screen.print("** VICTOIRE ! **");

//   screen.setTextColor(TFT_WHITE, TFT_BLACK);
//   screen.setTextSize(2);
//   screen.setCursor(20, 48);
//   screen.print("SCORE  : ");
//   screen.print(lastScore);

//   screen.setTextColor(TFT_CYAN, TFT_BLACK);
//   screen.setCursor(20, 68);
//   screen.print("RECORD : ");
//   screen.print(highScore);

//   screen.setTextColor(TFT_GREEN, TFT_BLACK);
//   screen.setTextSize(1);
//   screen.setCursor(20, 110);
//   screen.print("[A] Rejouer  [B] Menu");
// }


// //  Lancement d'un jeu
// //  Supprime l'ancien jeu, crée le nouveau,
// //  l'initialise et passe en état PLAYING

// void launchGame(int gameId) {
//   if (currentGame != nullptr) {
//     delete currentGame;
//     currentGame = nullptr;
//   }

//   switch (gameId) {
//     case 0: currentGame = new SnakeGame(&screen);         break;
//     case 1: currentGame = new MorpionGame(&screen);       break;
//     case 2: currentGame = new PongGame(&screen);          break;
//     case 3: currentGame = new BreakoutGame(&screen);      break;
//     case 4: currentGame = new FlappyBirdGame(&screen);    break;
//     case 5: currentGame = new TetrisGame(&screen);        break;
//     case 6: currentGame = new SpaceInvadersGame(&screen); break;
//     case 7: currentGame = new DinoRunGame(&screen);       break;
//     case 8: currentGame = new MazeGame(&screen);          break;
//     case 9: currentGame = new MemoryGame(&screen);        break;
//   }

//   if (currentGame != nullptr) {
//     currentGame->init();
//     consoleState = PLAYING;
//   }
// }


// //  SETUP
// void setup() {
//   screen.init();
//   screen.setRotation(1);
//   screen.fillScreen(TFT_BLACK);

//   pinMode(BTN_UP,    INPUT_PULLUP);
//   pinMode(BTN_DOWN,  INPUT_PULLUP);
//   pinMode(BTN_LEFT,  INPUT_PULLUP);
//   pinMode(BTN_RIGHT, INPUT_PULLUP);
//   pinMode(BTN_A,     INPUT_PULLUP);
//   pinMode(BTN_B,     INPUT_PULLUP);
//   pinMode(BUZZER_PIN, OUTPUT);

//   EEPROM.begin(EEPROM_SIZE);

//   menu.addGame("Snake",          "Mange les pommes!",        "", 0);
//   menu.addGame("Morpion",        "2 joueurs - A pour jouer!","", 1);
//   menu.addGame("Pong",           "Bat l'IA en 5 points!",    "", 2);
//   menu.addGame("Breakout",       "Casse toutes les briques!", "", 3);
//   menu.addGame("Flappy Bird",    "Evite les tuyaux!",        "", 4);
//   menu.addGame("Tetris",         "Aligne les lignes!",       "", 5);
//   menu.addGame("Space Invaders", "Detruit les envahisseurs!", "", 6);
//   menu.addGame("Dino Run",       "Saute les cactus!",        "", 7);
//   menu.addGame("Labyrinthe",     "Trouve la sortie!",        "", 8);
//   menu.addGame("Memory",         "Trouve les paires!",       "", 9);

//   showBoot();
// }


// //  LOOP
// //  Machine à états de la console :
// //  STARTING → MENU → PLAYING → GAMEOVER → MENU
// void loop() {
//   readButtons();

//   switch (consoleState) {

//     // ── Démarrage ──
//     case STARTING:
//       break;

//     // ── Menu principal ──
//     case MENU: {
//       menu.update(buttons);
//       menu.render();

//       if (buttons.aPressed) {
//         currentGameId = menu.getSelectedId();
//         launchGame(currentGameId);
//       }
//       break;
//     }

//     // ── Jeu en cours ──
//     case PLAYING: {
//       if (currentGame != nullptr) {
//         currentGame->update(buttons);
//         currentGame->render();
//         if (currentGame->isGameOver()) {
//           consoleState = GAMEOVER;
//         } else if (currentGame->isGameWon()) {
//           consoleState = GAMEWIN;
//         }
//       }
//       break;
//     }

//     // ── Game Over ──
//     case GAMEOVER: {
//       static bool gameOverDrawn = false;

//       if (!gameOverDrawn) {
//         showGameOver();
//         gameOverDrawn = true;
//       }

//       if (buttons.aPressed) {
//         gameOverDrawn = false;
//         launchGame(currentGameId);
//       }
//       if (buttons.bPressed) {
//         gameOverDrawn = false;
//         consoleState = MENU;
//         menu.forceRedraw();
//       }
//       break;
//     }

//     // ── Game Win ──
//     case GAMEWIN: {
//       static bool gameWinDrawn = false;

//       if (!gameWinDrawn) {
//         showGameWin();
//         gameWinDrawn = true;
//       }

//       if (buttons.aPressed) {
//         gameWinDrawn = false;
//         launchGame(currentGameId);
//       }
//       if (buttons.bPressed) {
//         gameWinDrawn = false;
//         consoleState = MENU;
//         menu.forceRedraw();
//       }
//       break;
//     }
//   }

//   delay(1);
// }


#include <TFT_eSPI.h>
#include <EEPROM.h>

// Bibliothèques de la console 
#include "Game.h"
#include "Menu.h"

//  Bibliothèques des jeux 
#include "Snake.h"
#include "Morpion.h"
#include "Pong.h"
#include "Breakout.h"
#include "FlappyBird.h"
#include "Tetris.h"
#include "SpaceInvaders.h"
#include "DinoRun.h"
#include "Maze.h"
#include "Memory.h"

//  Broches des boutons
#define BTN_UP     25
#define BTN_DOWN   26
#define BTN_LEFT   27
#define BTN_RIGHT  32
#define BTN_A      13
#define BTN_B      15
#define BUZZER_PIN 17

Buttons buttons;

//États de la console
enum stateConsole {
  STARTING,
  MENU,
  PLAYING,
  GAMEOVER,
  GAMEWIN      // victoire pour les jeux avec une fin heureuse
};

int currentGameId = -1;

// Variables globales
TFT_eSPI screen = TFT_eSPI();
Menu menu(&screen);
stateConsole consoleState = STARTING;
Game* currentGame = nullptr;

// High scores par jeu (10 jeux × 4 bytes = 40 bytes EEPROM) 
// Adresse EEPROM du jeu N = N * 4
#define EEPROM_SIZE     40
#define SCORE_ADDR(id)  ((id) * 4)

int getHighScore(int gameId) {
  int val = 0;
  EEPROM.get(SCORE_ADDR(gameId), val);
  // Si EEPROM jamais écrite (valeur corrompue), on retourne 0
  if (val < 0 || val > 99999) val = 0;
  return val;
}

void saveHighScore(int gameId, int newScore) {
  int current = getHighScore(gameId);
  if (newScore > current) {
    EEPROM.put(SCORE_ADDR(gameId), newScore);
    EEPROM.commit();
  }
}

//  Lecture des boutons
//  Calcule les fronts montants (pressed)
//  = vrai uniquement le frame où le bouton
//    vient d'être appuyé

void readButtons() {
  bool prevUp    = buttons.up;
  bool prevDown  = buttons.down;
  bool prevLeft  = buttons.left;
  bool prevRight = buttons.right;
  bool prevA     = buttons.a;
  bool prevB     = buttons.b;

  buttons.up    = !digitalRead(BTN_UP);
  buttons.down  = !digitalRead(BTN_DOWN);
  buttons.left  = !digitalRead(BTN_LEFT);
  buttons.right = !digitalRead(BTN_RIGHT);
  buttons.a     = !digitalRead(BTN_A);
  buttons.b     = !digitalRead(BTN_B);

  buttons.upPressed    = buttons.up    && !prevUp;
  buttons.downPressed  = buttons.down  && !prevDown;
  buttons.leftPressed  = buttons.left  && !prevLeft;
  buttons.rightPressed = buttons.right && !prevRight;
  buttons.aPressed     = buttons.a     && !prevA;
  buttons.bPressed     = buttons.b     && !prevB;
}


//  Animation sourire
//  Dessinée avant l'écran de démarrage ARDUBOY
//  Cercle → yeux → sourire → fondu vers le titre

void showSmiley() {
  screen.fillScreen(TFT_BLACK);

  const int cx   = 120;   // centre X de l'écran
  const int cy   = 67;    // centre Y de l'écran
  const int r    = 50;    // rayon du visage

  // // ── 1. Dessin progressif du cercle (visage) ──
  // // On trace le cercle degré par degré pour un effet "qui se dessine"
  // for (int angle = 0; angle <= 360; angle += 3) {
  //   float rad = angle * 3.14159 / 180.0;
  //   int x = cx + (int)(r * cos(rad));
  //   int y = cy + (int)(r * sin(rad));
  //   screen.fillCircle(x, y, 2, TFT_YELLOW);
  //   delay(5);
  // }

  // // ── 2. Apparition des yeux (clignotement) ──
  // delay(100);
  // for (int blink = 0; blink < 2; blink++) {
  //   // Œil gauche
  //   screen.fillCircle(cx - 16, cy - 14, 5, TFT_WHITE);
  //   // Œil droit
  //   screen.fillCircle(cx + 16, cy - 14, 5, TFT_WHITE);
  //   delay(180);
  //   // Cligner (effacer)
  //   screen.fillCircle(cx - 16, cy - 14, 5, TFT_BLACK);
  //   screen.fillCircle(cx + 16, cy - 14, 5, TFT_BLACK);
  //   delay(80);
  // }
  // // Yeux définitifs avec pupilles
  // screen.fillCircle(cx - 16, cy - 14, 5, TFT_WHITE);
  // screen.fillCircle(cx + 16, cy - 14, 5, TFT_WHITE);
  // screen.fillCircle(cx - 15, cy - 13, 2, TFT_BLACK);  // pupille gauche
  // screen.fillCircle(cx + 17, cy - 13, 2, TFT_BLACK);  // pupille droite

  // ── 3. Dessin progressif du sourire ──
  // Arc de cercle entre 20° et 160° (bas du visage)
  // rayon du sourire = 28px, centré légèrement en bas
  delay(150);
  const int sr = 26;    // rayon du sourire
  const int sy = cy + 8; // centre Y du sourire (décalé vers le bas)
  for (int angle = 20; angle <= 160; angle += 4) {
    float rad = angle * 3.14159 / 180.0;
    int x = cx + (int)(sr * cos(rad));
    int y = sy + (int)(sr * sin(rad));
    screen.fillCircle(x, y, 2, TFT_YELLOW);
    delay(18);
  }

  // // ── 4. Petites joues roses ──
  // delay(100);
  // for (int i = 0; i < 3; i++) {
  //   screen.fillCircle(cx - 32, cy + 8, 6, TFT_PINK);
  //   screen.fillCircle(cx + 32, cy + 8, 6, TFT_PINK);
  //   delay(120);
  //   screen.fillCircle(cx - 32, cy + 8, 6, TFT_BLACK);
  //   screen.fillCircle(cx + 32, cy + 8, 6, TFT_BLACK);
  //   delay(80);
  // }
  // screen.fillCircle(cx - 32, cy + 8, 6, TFT_PINK);
  // screen.fillCircle(cx + 32, cy + 8, 6, TFT_PINK);

  // ── 5. Son joyeux ──
  tone(BUZZER_PIN, 523, 80); delay(90);
  tone(BUZZER_PIN, 659, 80); delay(90);
  tone(BUZZER_PIN, 784, 80); delay(90);
  tone(BUZZER_PIN, 1047, 200); delay(250);

  // ── 6. Pause finale avant transition ──
  delay(600);

  // ── 7. Fondu vers le noir (transition douce) ──
  // On remplit progressivement par des cercles concentriques noirs
  for (int fr = r + 4; fr >= 0; fr -= 3) {
    screen.fillCircle(cx, cy, fr, TFT_BLACK);
    delay(18);
  }
  screen.fillScreen(TFT_BLACK);
  delay(200);
}


//  Écran de démarrage
//  Affiché une seule fois au boot
void showBoot() {
  // Animation sourire avant le titre
  showSmiley();

  screen.fillScreen(TFT_BLACK);

  screen.drawRect(10, 10, 220, 80, TFT_CYAN);
  screen.drawRect(12, 12, 216, 76, TFT_CYAN);

  screen.setTextColor(TFT_CYAN, TFT_BLACK);
  screen.setTextSize(3);
  screen.setCursor(30, 35);
  screen.print(" ARDUBOY ");

  screen.setTextSize(1);
  screen.setTextColor(TFT_WHITE, TFT_BLACK);
  screen.setCursor(55, 60);
  screen.print("UNE CONSOLE DE JEUX");

  // Son de démarrage (après le sourire)
  tone(BUZZER_PIN, 784, 150); delay(150);
  tone(BUZZER_PIN, 1047, 300); delay(300);

  // Barre de chargement
  screen.drawRect(20, 100, 200, 15, TFT_WHITE);
  for (int i = 0; i <= 100; i += 2) {
    int largeur = (i * 196) / 100;
    screen.fillRect(22, 102, largeur, 10, TFT_GREEN);
    delay(30);
  }

  delay(500);
  consoleState = MENU;
}


//  Écran GAME OVER
//  Affiche le score + meilleur score
//  Sauvegarde le meilleur score en EEPROM
//  Appelé UNE SEULE FOIS depuis case GAMEOVER

void showGameOver() {
  int lastScore = currentGame->getScore();
  int highScore = getHighScore(currentGameId);

  // Sauvegarder si nouveau record
  if (lastScore > highScore) {
    highScore = lastScore;
    saveHighScore(currentGameId, lastScore);
  }

  screen.fillScreen(TFT_BLACK);

  screen.setTextColor(TFT_RED, TFT_BLACK);
  screen.setTextSize(3);
  screen.setCursor(30, 15);
  screen.print("GAME OVER !");

  screen.setTextColor(TFT_WHITE, TFT_BLACK);
  screen.setTextSize(2);
  screen.setCursor(20, 55);
  screen.print("SCORE  : ");
  screen.print(lastScore);

  screen.setTextColor(TFT_YELLOW, TFT_BLACK);
  screen.setCursor(25, 75);
  screen.print("MEILLEUR: ");
  screen.print(highScore);

  screen.setTextColor(TFT_GREEN, TFT_BLACK);
  screen.setTextSize(1);
  screen.setCursor(20, 110);
  screen.print("[A] Rejouer  [B] Menu");
}


//  Écran GAME WIN
//  Affiché quand le joueur remporte la partie
//  (Pong, Morpion J1, Maze, Memory, Breakout)

void showGameWin() {
  int lastScore = currentGame->getScore();
  int highScore = getHighScore(currentGameId);

  if (lastScore > highScore) {
    highScore = lastScore;
    saveHighScore(currentGameId, lastScore);
  }

  screen.fillScreen(TFT_BLACK);

  // Cadre doré
  screen.drawRect(8,  8,  224, 90, TFT_YELLOW);
  screen.drawRect(10, 10, 220, 86, TFT_YELLOW);

  screen.setTextColor(TFT_YELLOW, TFT_BLACK);
  screen.setTextSize(2);
  screen.setCursor(35, 18);
  screen.print("** VICTOIRE ! **");

  screen.setTextColor(TFT_WHITE, TFT_BLACK);
  screen.setTextSize(2);
  screen.setCursor(20, 48);
  screen.print("SCORE  : ");
  screen.print(lastScore);

  screen.setTextColor(TFT_CYAN, TFT_BLACK);
  screen.setCursor(20, 68);
  screen.print("RECORD : ");
  screen.print(highScore);

  screen.setTextColor(TFT_GREEN, TFT_BLACK);
  screen.setTextSize(1);
  screen.setCursor(20, 110);
  screen.print("[A] Rejouer  [B] Menu");
}


//  Lancement d'un jeu
//  Supprime l'ancien jeu, crée le nouveau,
//  l'initialise et passe en état PLAYING

void launchGame(int gameId) {
  if (currentGame != nullptr) {
    delete currentGame;
    currentGame = nullptr;
  }

  switch (gameId) {
    case 0: currentGame = new SnakeGame(&screen);         break;
    case 1: currentGame = new MorpionGame(&screen);       break;
    case 2: currentGame = new PongGame(&screen);          break;
    case 3: currentGame = new BreakoutGame(&screen);      break;
    case 4: currentGame = new FlappyBirdGame(&screen);    break;
    case 5: currentGame = new TetrisGame(&screen);        break;
    case 6: currentGame = new SpaceInvadersGame(&screen); break;
    case 7: currentGame = new DinoRunGame(&screen);       break;
    case 8: currentGame = new MazeGame(&screen);          break;
    case 9: currentGame = new MemoryGame(&screen);        break;
  }

  if (currentGame != nullptr) {
    currentGame->init();
    consoleState = PLAYING;
  }
}


//  SETUP
void setup() {
  screen.init();
  screen.setRotation(1);
  screen.fillScreen(TFT_BLACK);

  pinMode(BTN_UP,    INPUT_PULLUP);
  pinMode(BTN_DOWN,  INPUT_PULLUP);
  pinMode(BTN_LEFT,  INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_A,     INPUT_PULLUP);
  pinMode(BTN_B,     INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);

  EEPROM.begin(EEPROM_SIZE);

  menu.addGame("Snake",          "Mange les pommes!",        "", 0);
  menu.addGame("Morpion",        "2 joueurs - A pour jouer!","", 1);
  menu.addGame("Pong",           "Bat l'IA en 5 points!",    "", 2);
  menu.addGame("Breakout",       "Casse toutes les briques!", "", 3);
  menu.addGame("Flappy Bird",    "Evite les tuyaux!",        "", 4);
  menu.addGame("Tetris",         "Aligne les lignes!",       "", 5);
  menu.addGame("Space Invaders", "Detruit les envahisseurs!", "", 6);
  menu.addGame("Dino Run",       "Saute les cactus!",        "", 7);
  menu.addGame("Labyrinthe",     "Trouve la sortie!",        "", 8);
  menu.addGame("Memory",         "Trouve les paires!",       "", 9);

  showBoot();
}


//  LOOP
//  Machine à états de la console :
//  STARTING → MENU → PLAYING → GAMEOVER → MENU
void loop() {
  readButtons();

  switch (consoleState) {

    // ── Démarrage ──
    case STARTING:
      break;

    // ── Menu principal ──
    case MENU: {
      menu.update(buttons);
      menu.render();

      if (buttons.aPressed) {
        currentGameId = menu.getSelectedId();
        launchGame(currentGameId);
      }
      break;
    }

    // ── Jeu en cours ──
    case PLAYING: {
      if (currentGame != nullptr) {
        currentGame->update(buttons);
        currentGame->render();
        if (currentGame->isGameOver()) {
          consoleState = GAMEOVER;
        } else if (currentGame->isGameWon()) {
          consoleState = GAMEWIN;
        }
      }
      break;
    }

    // ── Game Over ──
    case GAMEOVER: {
      static bool gameOverDrawn = false;

      if (!gameOverDrawn) {
        showGameOver();
        gameOverDrawn = true;
      }

      if (buttons.aPressed) {
        gameOverDrawn = false;
        launchGame(currentGameId);
      }
      if (buttons.bPressed) {
        gameOverDrawn = false;
        consoleState = MENU;
        menu.forceRedraw();
      }
      break;
    }

    // ── Game Win ──
    case GAMEWIN: {
      static bool gameWinDrawn = false;

      if (!gameWinDrawn) {
        showGameWin();
        gameWinDrawn = true;
      }

      if (buttons.aPressed) {
        gameWinDrawn = false;
        launchGame(currentGameId);
      }
      if (buttons.bPressed) {
        gameWinDrawn = false;
        consoleState = MENU;
        menu.forceRedraw();
      }
      break;
    }
  }

  delay(1);
}