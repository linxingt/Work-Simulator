# Work-Simulator
> A pixel game written in C++, inspired by *Tapper*.

Un petit jeu développé dans le cadre d’un projet de **3e année en école d’ingénieur**, utilisant des bibliothèques fournies par les enseignants. Le code que j’ai principalement écrit se trouve dans le fichier `Eleve.cpp`.

---

## Objectif du jeu

Bienvenue dans le simulateur de travail !  
Vous incarnez un employé dans une usine. Un écran noir vous indique les matières premières nécessaires à la fabrication d’un produit final.

### Votre mission :
- Collecter les matières premières affichées.
- Une fois tous les éléments collectés, l’écran noir génère un produit final.
- Vous devez ensuite le récupérer et le déposer dans le stock.

### Récompenses et objectifs :
- Vous démarrez avec un salaire de départ (**2000 €**).
- Envoyer une matière première vous rapporte une prime.
- Stocker un produit final vous rapporte encore plus.
- Terminez **9 produits** ou atteignez **3000 €** pour gagner.
- Attention : si un produit final tombe au sol sans être ramassé, il se casse et vous perdez **500 €**.
- Si votre salaire tombe à **0 ou moins**, vous perdez.

---

## Structure du projet

Voici les fichiers présents dans ce projet :

- `Eleve.cpp` → le fichier principal que j'ai écrit
- `G2D*.cpp` / `G2D*.h` → bibliothèques graphiques fournies par les enseignants
- `V2.cpp` / `V2.h` → gestion des vecteurs (également fournies)
- `glut.h`, `glut32.dll`, `glut32.lib` → fichiers liés à OpenGL/GLUT (fournis par les enseignants)
- `G2D.sln`, `G2D.vcxproj`, etc. → fichiers Visual Studio pour compiler le projet

---

## Installation

1. **Téléchargez ou clonez mon dépôt**

2. **Décompressez** le projet si nécessaire.

3. **Ouvrez le projet sous Visual Studio Community** :

   * Double-cliquez sur le fichier `G2D.sln`.

5. **Lancez le jeu** :

   * Cliquez sur le bouton "Démarrer le débogage" (icône triangle vert).

---

## Comment jouer

* **Flèches directionnelles** : déplacer le personnage
* **C** : collecter une matière première ou un produit
* **D** : déposer une matière première ou un produit

---

## Fonctionnalités implémentées

Voici les fonctionnalités que j’ai développées dans `Eleve.cpp` :

* Mécanique de déplacement du personnage
* Génération aléatoire des produits finis et des matières premières nécessaires
* Affichage dynamique des commandes à l’écran
* Gestion des différents écrans du jeu (intro, jeu, fin)
* Système de salaire avec gains et pertes
* Gestion de la casse des produits non récupérés à temps
* États de victoire (produits complétés ou salaire atteint) et de défaite (salaire épuisé)

---

## Remerciements

* **Bibliothèques graphiques et techniques** fournies par les enseignants de **ESIEE**.
