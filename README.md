# ArduboyCode 🎮

Collection de mini-jeux pour la console **Arduboy** (ATmega32U4, écran OLED 128×64 px, 6 boutons).

---

## Présentation du projet

ArduboyCode est un sketch Arduino multi-jeux doté d'un **menu principal** permettant de lancer l'un des jeux disponibles. L'architecture est conçue pour être facilement extensible : chaque jeu est encapsulé dans son propre fichier d'en-tête (`.h`) et expose une interface minimale (`init`, `update`, `draw`).

---

## Matériel requis

| Élément | Description |
|---------|-------------|
| Console | [Arduboy](https://www.arduboy.com/) (ou clone compatible) |
| Bibliothèque | [Arduboy2](https://github.com/MLXXXp/Arduboy2) ≥ 6.0 |
| IDE | Arduino IDE ≥ 1.8 ou Arduino IDE 2.x |

---

## Jeux implémentés

### 1. Morpion (Tic-Tac-Toe)

Jeu classique à deux camps : le **joueur** joue les X, l'**IA** joue les O.

| Contrôle | Action |
|----------|--------|
| ↑ ↓ ← → | Déplacer le curseur |
| **A** | Poser une pièce / Rejouer après game-over |
| **B** | Retour au menu *(toujours actif)* |

* L'IA applique l'heuristique : **gagner → bloquer → centre → coin → libre**.
* Les scores X / O sont conservés entre les parties.
* L'écran « game-over » reste affiché jusqu'à appui sur **A** ou **B** (bug corrigé).

---

### 2. Pong

Pong classique joueur contre IA ; premier à **7 points** gagne.

| Contrôle | Action |
|----------|--------|
| ↑ ↓ | Déplacer la raquette gauche (joueur) |
| **A** | Rejouer après game-over |
| **B** | Retour au menu *(toujours actif)* |

* La raquette droite (IA) ne réagit que lorsque la balle se dirige vers elle.
* L'angle de rebond dépend de l'endroit où la balle touche la raquette.

---

### 3. Memory

Jeu de paires sur une grille **4 × 4** (8 paires, valeurs 1–8).

| Contrôle | Action |
|----------|--------|
| ↑ ↓ ← → | Déplacer le curseur |
| **A** | Retourner la carte sélectionnée / Rejouer après victoire |
| **B** | Retour au menu *(toujours actif)* |

* Les cartes non-appariées se cachent automatiquement après ~1,5 s.
* Le compteur de coups est affiché en permanence sur le panneau de droite.
* Le plateau est mélangé aléatoirement à chaque nouvelle partie.

---

## Organisation du code

```
ArduboyCode/              ← répertoire du dépôt
├── README.md
└── ArduboyCode/          ← dossier du sketch Arduino
    ├── ArduboyCode.ino   ← point d'entrée : setup(), loop(), menu principal
    ├── Morpion.h         ← classe Morpion  (Tic-Tac-Toe)
    ├── Pong.h            ← classe PongGame (Pong)
    └── Memory.h          ← classe MemoryGame (Memory)
```

### Interface commune de chaque jeu

Chaque jeu expose trois méthodes publiques :

```cpp
void init();                     // (ré)initialise l'état du jeu
bool update(Arduboy2& ab);       // logique : retourne true → retour menu
void draw(Arduboy2& ab);         // rendu dans le buffer d'affichage
```

Le menu principal appelle ces méthodes dans `loop()`, après `arduboy.pollButtons()` et `arduboy.nextFrame()`.

### Ajouter un nouveau jeu

1. Créer `MonJeu.h` dans `ArduboyCode/` avec la classe exposant `init`, `update`, `draw`.
2. Inclure `#include "MonJeu.h"` dans `ArduboyCode.ino`.
3. Ajouter une entrée dans `GAME_NAMES`, un `case` dans `updateMenu()` et dans `loop()`.

---

## Comment compiler et flasher

1. Installer la bibliothèque **Arduboy2** via le gestionnaire de bibliothèques de l'IDE Arduino.
2. Ouvrir `ArduboyCode/ArduboyCode.ino` dans l'IDE Arduino.
3. Sélectionner la carte **Arduboy** (ou *Arduino Leonardo* si absente).
4. Téléverser (`Ctrl+U`).

---

## TODO / Améliorations prévues

- [ ] **Nouveaux jeux variés**
  - [ ] Casse-briques (Breakout)
  - [ ] Snake
  - [ ] Flappy Bird simplifié
- [ ] **Morpion** — améliorer l'IA avec un algorithme minimax pour une difficulté maximale
- [ ] **Pong** — ajouter un mode 2 joueurs (raquette droite contrôlée par ← →)
- [ ] **Memory** — ajouter plusieurs niveaux de difficulté (grilles 2×2 à 4×4)
- [ ] **Menu principal** — ajouter des icônes / sprites pour chaque jeu
- [ ] **Gestion des high-scores** — sauvegarder en EEPROM
- [ ] **Musique / effets sonores** — exploiter le buzzer intégré de l'Arduboy
- [ ] **Fluidité générale** — profiler et optimiser les routines de rendu si nécessaire

---

## Licence

Projet libre — à adapter et redistribuer librement.
