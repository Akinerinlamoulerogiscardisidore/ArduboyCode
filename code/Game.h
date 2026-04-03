// #ifndef GAME_H
// #define GAME_H

// #include <TFT_eSPI.h>

// // Structure des boutons
// struct Buttons {
//   bool up, down, left, right, a, b;

//   // Fronts montants (vrai UNE SEULE FOIS par appui)
//   bool upPressed, downPressed;
//   bool leftPressed, rightPressed;
//   bool aPressed, bPressed;
// };

// // Informations sur l'état du jeu
// enum stateGame {
//   START,
//   IN_PROGRESS,
//   GAME_OVER
// };

// // Implémentation de la classe Game
// class Game {
//   protected:
//     TFT_eSPI* screen;
//     int score;
//     stateGame state;

//   public:
//     // Constructeur
//     Game(TFT_eSPI* display) {
//       screen = display;
//       score = 0;
//       state = START;
//     }

//     // Fonctions virtuelles pures
//     virtual void init() = 0;
//     virtual void update(Buttons buttons) = 0;
//     virtual void render() = 0; 

//     // Fonctions communes 
//     bool isGameOver() {
//       return state == GAME_OVER;
//     }

//     int getScore() {
//       return score;
//     }

//     void displayScore() {
//       screen->fillRect(0, 0, 240, 20, TFT_CYAN);
//       screen->setTextColor(TFT_WHITE, TFT_CYAN);
//       screen->setTextSize(2);
//       screen->setCursor(2, 2);
//       screen->print("SCORE = ");
//       screen->print(score);

//       /* A revoir */
//     }

//     virtual ~Game(){}

// };

// #endif


#ifndef GAME_H
#define GAME_H

#include <TFT_eSPI.h>

// Structure des boutons
struct Buttons {
  bool up, down, left, right, a, b;

  // Fronts montants (vrai UNE SEULE FOIS par appui)
  bool upPressed, downPressed;
  bool leftPressed, rightPressed;
  bool aPressed, bPressed;
};

// Informations sur l'état du jeu
enum stateGame {
  START,
  IN_PROGRESS,
  GAME_OVER,
  GAME_WIN     // victoire (Pong, Morpion, Maze, Memory, Breakout...)
};

// Implémentation de la classe Game
class Game {
  protected:
    TFT_eSPI* screen;
    int score;
    stateGame state;

  public:
    // Constructeur
    Game(TFT_eSPI* display) {
      screen = display;
      score = 0;
      state = START;
    }

    // Fonctions virtuelles pures
    virtual void init() = 0;
    virtual void update(Buttons buttons) = 0;
    virtual void render() = 0; 

    // Fonctions communes 
    bool isGameOver() {
      return state == GAME_OVER;
    }

    bool isGameWon() {
      return state == GAME_WIN;
    }

    int getScore() {
      return score;
    }

    void displayScore() {
      screen->fillRect(0, 0, 240, 20, TFT_CYAN);
      screen->setTextColor(TFT_WHITE, TFT_CYAN);
      screen->setTextSize(2);
      screen->setCursor(2, 2);
      screen->print("SCORE = ");
      screen->print(score);

      /* A revoir */
    }

    virtual ~Game(){}

};

#endif