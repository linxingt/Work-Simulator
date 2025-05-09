#pragma once
#include <cmath>
#include <string>
#include <iostream>
#include <vector>
#include "V2.h"
 
struct GameData;

struct Color
{
	float R, G, B, A;

	Color(float r, float g, float b)                        { R = r; G = g; B = b; A = 1; }
	Color(float r, float g, float b, float a)               { R = r; G = g; B = b; A = a; }

	const static Color Black, White, Red, Green, Blue, Magenta, Cyan, Yellow, Gray;  // use Color::Black
};

Color ColorFrom255(int r, int g, int b);
Color ColorFromHex(int hexCode);

 
enum class Key {   Unknown, A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,
	               LEFT, RIGHT, UP, DOWN, ENTER, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12   };


namespace G2D
{
	// key management 
	bool isKeyPressed(Key k);   // indique si la touche est enfonc�e (retourne true tant que le touche est appuy�e)
	bool keyHasBeenHit(Key k);  // retourne true si la touche a �t� appuy�e, retourne false si la touche reste enfonc�e

	// Main Graphic System Function

	void initWindow(V2 WindowSize, V2 WindowStartPos, std::string name);
	void Run(void logic(GameData& G), void Render(const GameData& G), GameData& G, int nbCalltoLogicPerSec, bool HideCursor);
	void clearScreen(Color c);
	void Show();                      
 
	// Mouse event
	void getMousePos(int &x, int & y);
	bool isMouseLeftButtonPressed();   // indique si le bouton gauche de la souris est enfonc�
	bool isMouseRightButtonPressed();
	bool isAnyMouseButtonPressed();
	bool detectLeftClick();            // d�tecte si le bouton gauche de la souris a �t� cliqu�
	bool detectRightClick();


	// Font
	void drawStringFontMono (V2 pos, std::string text, float fontSize = 20, float thickness = 3, Color c = Color::Black);
	void drawStringFontRoman(V2 pos, std::string text, float fontSize = 20, float thickness = 3, Color c = Color::Black);

	// Cr�e une texture � partir d'une description 1 lettre = 1 pixel/couleur 
	int initTextureFromString(V2 & Size, const std::string & Sprite);

	// Affichage d'un sprite
	// angleDef produit une rotation du sprite autours de son centre
	int  extractTextureFromPPM(const std::string & filepath, bool applyTransparency);
	void drawRectWithTexture(int IDtexture, V2 pos, V2 size, float angleDeg = 0);
	
	// Draw Geometry
	void setPixel(V2 P, Color c);
	void drawLine(V2 P1, V2 P2, Color c );
	void drawPolygon(std::vector<V2> & PointList, Color c, bool fill = false);
	void drawRectangle(V2 P1, V2 Size, Color c, bool fill = false);
	void drawCircle(V2 C, float r, Color c, bool fill = false);


	// Timing function
	double elapsedTimeFromStartSeconds();   // temps �coul� depuis le d�but du jeu
	bool   isOnPause();
}