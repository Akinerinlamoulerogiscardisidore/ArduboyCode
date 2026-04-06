#include <TFT_eSPI.h>
#include <EEPROM.h>
#include <string.h>
#include <Bluepad32.h>


// Bibliothèques console 
#include "Game.h"
#include "Menu.h"
#include "setting.h"

//  Bibliothèques des jeux 
#include "Snake.h"
#include "Morpion.h"
#include "Pong.h"
#include "platformer.h"
#include "MemoryNumber.h"
/*#include "FlappyBird.h"
#include "Tetris.h"
#include "SpaceInvaders.h"
#include "DinoRun.h"
#include "Maze.h"
#include "Memory.h"*/

//  Broches des boutons
#define BTN_UP     25
#define BTN_DOWN   26
#define BTN_LEFT   27
#define BTN_RIGHT  32
#define BTN_A      13
#define BTN_B      15
#define BUZZER_PIN 17
#define VIBRATION_PIN 16   // ou une autre broche PWM/disponible

Buttons buttons;

//États de la console
enum stateConsole {
  STARTING,
  MENU,
  PLAYING,
  GAMEOVER,
  SETTINGS,
  WINNER
};

int currentGameId = -1;
int currentWinner = 0;
bool useGamepad = true; // global

// Variables globales
TFT_eSPI screen = TFT_eSPI();
Menu menu(&screen);
Settings settings(&screen);  // crée l'objet settings avec l'écran TFT
stateConsole consoleState = STARTING;
Game* currentGame = nullptr;
int highScore = 0;
bool menuFirstDraw = true;

// === Images des jeux (remplace ces tableaux par des vrais bitmaps RGB565 plus tard) ===
#include "snake_img.h";
#include "morpion_img.h";
#include "pong_img.h";
#include "platformer_img.h";
#include "memoryNumber_img.h";


struct GameCard {
  const char* name;
  const uint16_t* image;
  uint16_t color; // couleur placeholder si pas d'image
};

// 4 jeux pour ton exemple
GameCard gameCards[5] = {
  {"Snake", snake_img, TFT_RED},
  {"Morpion", morpion_img, TFT_CYAN},
  {"Pong", pong_img, TFT_YELLOW},
  {"Platformer", platformer_img, TFT_PINK},
  {"MemoryNumber", memoryNumber_img, TFT_MAGENTA}
};

// Index du jeu sélectionné
int selectedIndex = 0;
int scrollOffset = 0;           // premier jeu affiché à gauche
const int visibleGames = 2;     // nombre d'icônes visibles (ajustable selon largeur)
int gameCount;                  // sera initialisé dans setup()

const char* getGameDesc(int id) {
  switch(id) {
    case 0: return "Mange les pommes!";
    case 1: return "2 joueurs - A pour jouer!";
    case 2: return "Bat l'IA en 5 points!";
    case 3: return "Saute sur les platformes!";
    case 4: return "Memorise 5 chiffres!";
    default: return "Nouveau jeu!";
  }
}
// Objet manette
ControllerPtr myController = nullptr;

// Callbacks Bluepad32
void onConnectedController(ControllerPtr ctl) {
    myController = ctl;
    Serial.println("Manette connectée !");
}

void onDisconnectedController(ControllerPtr ctl) {
    if (myController == ctl) {
        myController = nullptr;
        Serial.println("Manette déconnectée.");
    }
}
void renderMenuHorizontal() {
  screen.fillScreen(TFT_BLACK);

  int centerY = screen.height() / 2;
  int spacing = 60;               // espace entre les deux icônes
  int iconW = 90, iconH = 70;    // icônes plus grandes
  int startX = (240 - (visibleGames * iconW + (visibleGames-1) * spacing)) / 2;

  for (int i = 0; i < visibleGames; i++) {
    int idx = scrollOffset + i;
    if (idx >= gameCount) break;

    int x = startX + i * (iconW + spacing);
    int y = centerY - iconH/2;
    int w = iconW, h = iconH;

    if (idx == selectedIndex) {
      w = iconW + 10;
      h = iconH + 10;
      x -= 5;
      y -= 5;
      screen.fillRoundRect(x+2, y+2, w, h, 8, TFT_DARKGREY);
    }

    if (gameCards[idx].image != nullptr) {
      screen.pushImage(x, y, iconW, iconH, gameCards[idx].image);
    } else {
      screen.fillRoundRect(x, y, w, h, 8, gameCards[idx].color);
    }
    screen.drawRoundRect(x, y, w, h, 8, TFT_WHITE);

    // Deux premières lettres
    screen.setTextColor(TFT_WHITE);
    screen.setTextSize(1);
    char firstTwo[3] = {gameCards[idx].name[0], gameCards[idx].name[1], '\0'};
    screen.setCursor(x + w/2 - 10, y + h/2 - 4);
    screen.print(firstTwo);

    // Nom complet sous l'icône
    int len = strlen(gameCards[idx].name);
    screen.setCursor(x + w/2 - (len * 3) / 2, y + h + 2);
    screen.print(gameCards[idx].name);
  }

  // Flèche sous le jeu sélectionné (s'il est visible)
  int visibleIndex = selectedIndex - scrollOffset;
  if (visibleIndex >= 0 && visibleIndex < visibleGames) {
    int arrowX = startX + visibleIndex * (iconW + spacing) + iconW/2 - 5;
    screen.fillTriangle(arrowX, centerY + iconH/2 + 5,
                        arrowX + 5, centerY + iconH/2 + 11,
                        arrowX - 5, centerY + iconH/2 + 11,
                        TFT_YELLOW);
  }

  // Description
  screen.setTextColor(TFT_WHITE);
  screen.setTextSize(1);
  screen.setCursor(10, 115);
  screen.print(getGameDesc(selectedIndex));

  // Indicateurs de scroll
  screen.setTextColor(TFT_DARKGREY);
  screen.setCursor(10, 125);
  screen.print("LEFT/RIGHT  select");
  if (scrollOffset > 0) {
    screen.setCursor(180, 125);
    screen.print("<<");
  }
  if (scrollOffset + visibleGames < gameCount) {
    screen.setCursor(210, 125);
    screen.print(">>");
  }
}


//  Lecture des boutons
//  Calcule les fronts montants (pressed)
//  = vrai uniquement le frame où le bouton
//    vient d'être appuyé
// Variables pour la vibration non-bloquante
static unsigned long vibEndTime = 0;
static bool vibActive = false;

void readButtons() {
     // Gestion de la fin de vibration (non-bloquante)
    if (vibActive && millis() >= vibEndTime) {
        digitalWrite(VIBRATION_PIN, LOW);
        vibActive = false;
    }
    if (useGamepad) {
        BP32.update();
    }// Si une manette est connectée, on l'utilise
    if (myController && myController->isConnected()) {
        // Mappage des touches PS4/PS5 → tes boutons
        buttons.up    = myController->dpad() == DPAD_UP;
        buttons.down  = myController->dpad() == DPAD_DOWN;
        buttons.left  = myController->dpad() == DPAD_LEFT;
        buttons.right = myController->dpad() == DPAD_RIGHT;
        buttons.a     = myController->a();      // Croix
        buttons.b     = myController->b();      // Rond

        // Fronts montants (à recalculer)
        static bool prevUp, prevDown, prevLeft, prevRight, prevA, prevB;
        buttons.upPressed    = buttons.up    && !prevUp;
        buttons.downPressed  = buttons.down  && !prevDown;
        buttons.leftPressed  = buttons.left  && !prevLeft;
        buttons.rightPressed = buttons.right && !prevRight;
        buttons.aPressed     = buttons.a     && !prevA;
        buttons.bPressed     = buttons.b     && !prevB;

        prevUp    = buttons.up;
        prevDown  = buttons.down;
        prevLeft  = buttons.left;
        prevRight = buttons.right;
        prevA     = buttons.a;
        prevB     = buttons.b;
    }
    else {
      // lire les boutons
      buttons.up    = !digitalRead(BTN_UP);
      buttons.down  = !digitalRead(BTN_DOWN);
      buttons.left  = !digitalRead(BTN_LEFT);
      buttons.right = !digitalRead(BTN_RIGHT);
      buttons.a     = !digitalRead(BTN_A);
      buttons.b     = !digitalRead(BTN_B);

      // fronts montants
      static bool prevUp = false, prevDown = false, prevLeft = false, prevRight = false, prevA = false, prevB = false;
      buttons.upPressed    = buttons.up    && !prevUp;
      buttons.downPressed  = buttons.down  && !prevDown;
      buttons.leftPressed  = buttons.left  && !prevLeft;
      buttons.rightPressed = buttons.right && !prevRight;
      buttons.aPressed     = buttons.a     && !prevA;
      buttons.bPressed     = buttons.b     && !prevB;

      prevUp = buttons.up;
      prevDown = buttons.down;
      prevLeft = buttons.left;
      prevRight = buttons.right;
      prevA = buttons.a;
      prevB = buttons.b;
    }
  // sons pour tous les boutons
  if (buttons.upPressed || buttons.downPressed || buttons.leftPressed || buttons.rightPressed) {
        if (settings.soundOn) tone(BUZZER_PIN, 1000, 50);
        if (settings.vibrationOn && !vibActive) {
            digitalWrite(VIBRATION_PIN, HIGH);
            vibEndTime = millis() + 30;   // 30 ms de vibration
            vibActive = true;
        }
    }
    else if (buttons.aPressed) {
        if (settings.soundOn) tone(BUZZER_PIN, 1500, 100);
        if (settings.vibrationOn && !vibActive) {
            digitalWrite(VIBRATION_PIN, HIGH);
            vibEndTime = millis() + 50;
            vibActive = true;
        }
    }
    else if (buttons.bPressed) {
        if (settings.soundOn) tone(BUZZER_PIN, 440, 100);
        if (settings.vibrationOn && !vibActive) {
            digitalWrite(VIBRATION_PIN, HIGH);
            vibEndTime = millis() + 40;
            vibActive = true;
        }
    }
  // passage MENU → SETTINGS uniquement si on est dans MENU et appuie B
  if (buttons.bPressed && consoleState == MENU){
    consoleState = SETTINGS;
    settings.settingsIndex = 0;
    settings.resetNavTime();   // ← FORCER un délai déjà écoulé
    settings.needsRedraw = true;
    // réinitialiser les fronts montants pour Settings
    buttons.upPressed = false;
    buttons.downPressed = false;
    buttons.leftPressed = false;
    buttons.rightPressed = false;
    buttons.aPressed = false;
    buttons.bPressed = false;
  }
}
/////////
/*unsigned long lastTime = 0;     // dernière action
const unsigned long interval = 100; // 500 ms
static bool menuDrawn = false;*/
///////

//  Écran de démarrage
//  Affiché une seule fois au boot
void showBoot() {
  screen.fillScreen(TFT_BLACK);

  // --- Paramètres pluie ---
  const char letters[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
  const int maxLetters = 200;
  char falling[maxLetters];
  int xPos[maxLetters];
  int yPos[maxLetters];
  int speed[maxLetters];
  String finalWord = "ARDUBOY";
  int centerX = screen.width() / 2;
  int centerY = screen.height() / 2;

  // Initialisation
  for (int i = 0; i < maxLetters; i++) {
    falling[i] = letters[random(strlen(letters))];
    xPos[i] = random(screen.width());
    yPos[i] = random(-screen.height(), 0);
    speed[i] = random(1, 5);
  }

  // Pluie de lettres
  for (int t = 0; t < 200; t++) {
    screen.fillScreen(TFT_BLACK);
    for (int i = 0; i < maxLetters; i++) {
      screen.setTextColor(TFT_GREEN);
      screen.setTextSize(1);
      screen.drawChar(falling[i], xPos[i], yPos[i], 1);
      yPos[i] += speed[i];
      if (yPos[i] > screen.height()) {
        yPos[i] = 0;
        xPos[i] = random(screen.width());
        falling[i] = letters[random(strlen(letters))];
        speed[i] = random(1, 5);
        tone(BUZZER_PIN, random(800, 1200), 20);
      }
    }
    delay(30);
  }

  // Condensation au centre (avec limite de pas)
  for (int step = 0; step < 60; step++) {
    screen.fillScreen(TFT_BLACK);
    bool allCentered = true;
    for (int i = 0; i < maxLetters; i++) {
      // Rapprochement progressif
      xPos[i] += (centerX - xPos[i]) / 10;
      yPos[i] += (centerY - yPos[i]) / 10;
      screen.setTextColor(TFT_GREEN);
      screen.setTextSize(1);
      screen.drawChar(falling[i], xPos[i], yPos[i], 1);
      if (abs(xPos[i] - centerX) > 2 || abs(yPos[i] - centerY) > 2)
        allCentered = false;
    }
    tone(BUZZER_PIN, random(600, 900), 20);
    delay(30);
    if (allCentered) break;
  }

  // Efface les lettres résiduelles
  screen.fillScreen(TFT_BLACK);

  // Affiche le mot final
  screen.setTextColor(TFT_CYAN);
  screen.setTextSize(3);
  screen.setCursor(centerX - finalWord.length() * 9, centerY - 8);
  screen.print(finalWord);
  delay(1000);

  // Passe au menu et l'affiche immédiatement
  consoleState = MENU;
  renderMenuHorizontal();
}
bool backgroundDrawn = false;


//  Écran GAME OVER
//  Affiche le score + meilleur score
//  Sauvegarde le meilleur score en EEPROM
//  Appelé UNE SEULE FOIS depuis case GAMEOVER

void showGameOver() {
 /* if (settings.vibrationOn) {
  digitalWrite(VIBRATION_PIN, HIGH);
  delay(200);
  digitalWrite(VIBRATION_PIN, LOW);
  delay(100);
  digitalWrite(VIBRATION_PIN, HIGH);
  delay(200);
  digitalWrite(VIBRATION_PIN, LOW);
}*/
if (settings.soundOn){
  tone(BUZZER_PIN, 400, 200);
  tone(BUZZER_PIN, 300, 200);
  tone(BUZZER_PIN, 200, 400);
}
  highScore = EEPROM.read(0);

  if (currentGame->getScore() > highScore) {
    highScore = currentGame->getScore();
    EEPROM.write(0, highScore);
    EEPROM.commit();
  }


  int lastScore = currentGame->getScore();

  // Fond sombre stylé
  screen.fillScreen(TFT_BLACK);

  // Cadre central
  screen.drawRoundRect(20, 10, 200, 115, 8, TFT_RED);

  // Titre
  screen.setTextColor(TFT_RED);
  screen.setTextSize(3);
  screen.setCursor(40, 20);
  screen.print("GAME OVER");

  // Score
  screen.setTextColor(TFT_WHITE);
  screen.setTextSize(2);
  screen.setCursor(40, 60);
  screen.print("Score: ");
  screen.print(lastScore);

  // High score
  screen.setTextColor(TFT_YELLOW);
  screen.setCursor(40, 85);
  screen.print("Best: ");
  screen.print(highScore);

  // Boutons
  screen.setTextColor(TFT_GREEN);
  screen.setTextSize(1);
  screen.setCursor(35, 110);
  screen.print("[A] Retry   [B] Menu");
}


void showWinner() {
    /* if (settings.vibrationOn) {
  digitalWrite(VIBRATION_PIN, HIGH);
  delay(200);
  digitalWrite(VIBRATION_PIN, LOW);
  delay(100);
  digitalWrite(VIBRATION_PIN, HIGH);
  delay(200);
  digitalWrite(VIBRATION_PIN, LOW);
}*/
if (settings.soundOn){
  tone(BUZZER_PIN, 600, 200);
  tone(BUZZER_PIN, 800, 200);
  tone(BUZZER_PIN, 1000, 400);
}
  highScore = EEPROM.read(0);
  if (currentGame->getScore() > highScore) {
    highScore = currentGame->getScore();
    EEPROM.write(0, highScore);
    EEPROM.commit();
  }

  int lastScore = currentGame->getScore();

  screen.fillScreen(TFT_BLACK);
  screen.drawRoundRect(20, 10, 200, 115, 8, TFT_GREEN);

  screen.setTextColor(TFT_GREEN);
  screen.setTextSize(3);
if (currentGameId == 1 && currentWinner != 0) {
        // Morpion : afficher le gagnant
        screen.setCursor(30, 20);
        screen.print("JOUEUR ");
        screen.print(currentWinner);
        screen.print(" GAGNE !");
    } else {
        screen.setCursor(50, 20);
        screen.print("WINNER !");
    }

    // Affichage score (si pertinent)
    screen.setTextColor(TFT_WHITE);
    screen.setTextSize(2);
    screen.setCursor(40, 60);
    screen.print("Score: ");
    screen.print(currentGame->getScore());

    screen.setTextColor(TFT_YELLOW);
    screen.setCursor(40, 85);
    screen.print("Best: ");
    screen.print(highScore);

    screen.setTextColor(TFT_GREEN);
    screen.setTextSize(1);
    screen.setCursor(35, 110);
    screen.print("[A] Retry   [B] Menu");


}
//  Lancement d'un jeu
//  Supprime l'ancien jeu, crée le nouveau,
//  l'initialise et passe en état PLAYING

void launchGame(int gameId) {
  if (gameId == 1) useGamepad = false;
  else useGamepad = true;
  if (currentGame != nullptr) {
    delete currentGame;
    currentGame = nullptr;
  }

  switch (gameId) {
    case 0: currentGame = new SnakeGame(&screen);         break;
    case 1: currentGame = new MorpionGame(&screen);       break;
    case 2: currentGame = new PongGame(&screen);          break;
    case 3: currentGame = new PlatformerGame(&screen);     break;
    case 4: currentGame = new MemoryNumberGame(&screen); break;
    /*case 4: currentGame = new FlappyBirdGame(&screen);    break;
    case 5: currentGame = new TetrisGame(&screen);        break;
    case 6: currentGame = new SpaceInvadersGame(&screen); break;
    case 7: currentGame = new DinoRunGame(&screen);       break;
    case 8: currentGame = new MazeGame(&screen);          break;
    case 9: currentGame = new MemoryGame(&screen);        break;*/
  }

  if (currentGame != nullptr) {
    currentGame->init();
    consoleState = PLAYING;
  }
}


//  SETUP
void setup() {
randomSeed(analogRead(0));
  screen.setSwapBytes(true);
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
  pinMode(VIBRATION_PIN, OUTPUT);
  digitalWrite(VIBRATION_PIN, LOW);
  ledcSetup(0, 2000, 8);   // canal 0, 2 kHz, résolution 8 bits
  ledcAttachPin(BUZZER_PIN, 0);

  EEPROM.begin(16);
  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.forgetBluetoothKeys(); // facultatif : efface les anciens appairages

  menu.addGame("Snake ",          "Mange les pommes!",        "", 0, TFT_RED);
  menu.addGame("Morpion ",        "2 joueurs - A pour jouer!","", 1, TFT_CYAN);
  menu.addGame("Pong ",           "Bat l'IA en 5 points!",    "", 2, TFT_YELLOW);
  menu.addGame("Platformer",       "Saute sur les platformes!", "", 3, TFT_PINK);
  menu.addGame("MemoryNum", "Rapide: memorise 5 chiffres!", "", 4, TFT_MAGENTA);/*
  menu.addGame("Flappy Bird",    "Evite les tuyaux!",        "", 4);
  menu.addGame("Tetris",         "Aligne les lignes!",       "", 5);
  menu.addGame("Space Invaders", "Detruit les envahisseurs!", "", 6);
  menu.addGame("Dino Run",       "Saute les cactus!",        "", 7);
  menu.addGame("Labyrinthe",     "Trouve la sortie!",        "", 8);
  menu.addGame("Memory",         "Trouve les paires!",       "", 9);
  */
  gameCount = sizeof(gameCards) / sizeof(gameCards[0]);
  showBoot();
}


//  LOOP
//  Machine à états de la console :
//  STARTING → MENU → PLAYING → GAMEOVER/winner → MENU
void loop() {
  unsigned long now = millis(); // temps actuel en ms
  readButtons();

  switch (consoleState) {

    // ── Démarrage ──
    case STARTING:
      break;

    // ── Menu principal ──
  
  case MENU: {
  static unsigned long lastNav = 0;
  unsigned long nowMenu = millis();

  if (nowMenu - lastNav > 200) {
    if (buttons.leftPressed) {
      if (selectedIndex > 0) {
        selectedIndex--;
        if (selectedIndex < scrollOffset) {
          scrollOffset = selectedIndex;
        }
        renderMenuHorizontal();
      }
      lastNav = nowMenu;
    }
    if (buttons.rightPressed) {
      if (selectedIndex < gameCount - 1) {
        selectedIndex++;
        if (selectedIndex >= scrollOffset + visibleGames) {
          scrollOffset = selectedIndex - visibleGames + 1;
        }
        renderMenuHorizontal();
      }
      lastNav = nowMenu;
    }
  }

  static bool firstDraw = true;
  if (firstDraw) {
    renderMenuHorizontal();
    firstDraw = false;
  }

  if (buttons.aPressed) {
    currentGameId = selectedIndex;
    launchGame(currentGameId);
    firstDraw = true;
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
        }
        else if (currentGame->isWinner()) {
            // Récupérer le gagnant pour Morpion
            if (currentGameId == 1) { // Morpion
                MorpionGame* morpion = (MorpionGame*)currentGame;
                currentWinner = morpion->getWinner();
            } else {
                currentWinner = 1; // pour les autres jeux, c'est le joueur
            }
            consoleState = WINNER;
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
    useGamepad = true; // réactive le gamepad pour le menu et autres jeux
    gameOverDrawn = false;
    consoleState = MENU;
    menuFirstDraw = true;
    renderMenuHorizontal();
  }
  break;
}
  case WINNER: {
  static bool winnerDrawn = false;
  if (!winnerDrawn) {
    showWinner();        // ← ici showWinner() au lieu de showGameOver()
    winnerDrawn = true;
  }
  if (buttons.aPressed) {
    winnerDrawn = false;
    launchGame(currentGameId);
  }
  if (buttons.bPressed) {
     useGamepad = true; // réactive le gamepad pour le menu et autres jeux
    winnerDrawn = false;
    consoleState = MENU;
    menu.forceRedraw();
  }
  break;
  }
   case SETTINGS: {
    int oldState = consoleState;
    settings.navigate(buttons, (int&)consoleState);
    if (consoleState == MENU) {
        // On vient de quitter Settings → afficher le menu tout de suite
        renderMenuHorizontal();
    } else {
        // Sinon, on reste dans Settings → mettre à jour l'affichage
        settings.render();
    }
    break;
  }
  }

  delay(1);
}