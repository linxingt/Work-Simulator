#pragma warning( disable : 4996 ) 


#include <cstdlib>
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include <algorithm>  // pour std::replace
#include "G2D.h"
#include <set>

#define VALWIN 3000
#define NBRPF 9

#define SALAIREINIT 2000
#define PFVALUE 500

#define NBRTYPEMP 3
#define MPNEEDMIN 9
#define MPNEEDMAX 15

#define MPMAXCMD 30

using namespace std;

void printMap(const map<string, int>& m, const string& name) {
	cout << "Contenu de " << name << ":" << endl;
	for (const auto& pair : m) {
		cout << "  " << pair.first << ": " << pair.second << endl;
	}
	cout << endl;
}


V2 finR0 = V2(280, 495);
V2 finR1 = V2(280, 375);
V2 finR2 = V2(280, 255);
V2 finR3 = V2(280, 135);

V2 debutR0 = V2(120, 495);
V2 debutR1 = V2(120, 375);
V2 debutR2 = V2(120, 255);
V2 debutR3 = V2(120, 135);

enum ECRAN { ECRAN_ACCEUIL, ECRAN_PAUSE, ECRAN_JEU, ECRAN_GAME_OVER };
ECRAN ecran = ECRAN::ECRAN_ACCEUIL;  // Initialise avec l'écran de jeu ou accueil

V2 posWord = V2(250, 300);
V2 posSentence = V2(120, 300);

V2 posEcran0 = V2(45, 495);
V2 posEcran1 = V2(45, 375);
V2 posEcran2 = V2(45, 255);
V2 posEcran3 = V2(45, 135);
V2 posEcran[4] = { posEcran0,posEcran1,posEcran2,posEcran3 };

V2 posMp0 = V2(480, 130);
V2 posMp1 = V2(480, 250);
V2 posMp2 = V2(480, 370);
V2 posStock = V2(480, 490);
V2 posZoneNom[4] = { posMp0,posMp1,posMp2,posStock };
string posZoneNomStr[4] = { "Stock Pf","Mp2","Mp1","Mp0" };

bool refreshCmd = false;
bool refreshMpRoute = false;
bool refreshPfRoute = false;

bool msgDepotPfFrist = false;
bool msgPfTombe = false;
float timerMsgPfTombe = 0.0f; // en secondes

struct _Heros
{
	string texture =
		"[RRR  ]"
		"[RRWR ]"
		"[RRR  ]"
		"[YY   ]"
		"[YYY  ]"
		"[YY YG]"
		"[GG   ]"
		"[CC   ]"
		"[CC   ]"
		"[C C  ]"
		"[C C  ]";

	string textureWalk =
		"[RRR  ]"
		"[RRWR ]"
		"[RRR  ]"
		"[YY YG]"
		"[YYY  ]"
		"[YY   ]"
		"[GG   ]"
		"[CC   ]"
		"[CC   ]"
		"[ C   ]"
		"[ C   ]";

	V2 Size;
	int IdTex;
	int IdTexIdle;
	int IdTexWalk;
	int salaire = SALAIREINIT;
	int primePerPf = 100;
	int primePerMP = 2;
	int pfAlreadyDepose = 0;

	bool hasMP0 = false;
	bool hasMP1 = false;
	bool hasMP2 = false;
	int alreadyHasMpNum = NULL;
	bool hasPF = false;
	string pfNom = "";
	int nbrObjHas = 0;


	bool isNearRoute = false;
	int routeNearIndex = NULL;
	bool isWin = false;
	V2 Pos = V2(450, 300);
};


struct _MatiereP
{
	string texture =
		"[          W          ]"
		"[         WWW         ]"
		"[        WWWWW        ]"
		"[        WWWWW        ]"
		"[        WWWWW        ]"
		"[        WWWWW        ]";


	V2 Size;
	int IdTex;
	V2 Pos;
	string mpNom;

	_MatiereP(string pNom) {
		mpNom = pNom;
	}
	void move() {
		Pos = Pos + V2(-1, 0);//tjr vers gauche
	}
	_MatiereP() {}
};

struct _ProduitF
{
	string texture =
		"[      ]"
		"[  RRR ]"
		"[  RRR ]"
		"[ CCCCC]"
		"[ WWWWW]"
		"[ WWWWW]"
		"[ WWWWW]"
		"[ CCCCC]"
		"[ RCCCR]"
		"[ RRRRR]"
		"[      ]";

	V2 Pos;
	V2 Size;
	int IdTex;
	string pfNom;
	int value = PFVALUE;
	bool estCollecte = false;
	bool estTombe = false;

	void move() {
		Pos = Pos + V2(0.5, 0);//tjr vers drite
	}

	_ProduitF(string pNom) {
		pfNom = pNom;
	}

	_ProduitF() {}
};


struct GameData
{

	string Map =
		"MMMMMMMMMMMMMMM"
		"MMM     0    MM"
		"MEE++++++    MM"
		"MMM     0   AAM"
		"MMM     0   AAM"
		"MEE------    MM"
		"MMM     0   BBM"
		"MMM     0   BBM"
		"MEE******    MM"
		"MMM     0   CCM"
		"MMM     0   CCM"
		"MEE//////    MM"
		"MMM     0   SSM"
		"MMM     0   SSM"
		"MMMMMMMMMMMMMMM";

	// indique la présence d'un mur à la case (x,y)
	bool Mur(int x, int y) const { return Map[(15 - y - 1) * 15 + x] == 'M'; }  // les fonctions const ne modifient pas les données
	bool Ecran(int x, int y) const { return Map[(15 - y - 1) * 15 + x] == 'E'; }
	bool EnRoute0(int x, int y) const { return Map[(15 - y - 1) * 15 + x] == '+'; }
	bool EnRoute1(int x, int y) const { return Map[(15 - y - 1) * 15 + x] == '-'; }
	bool EnRoute2(int x, int y) const { return Map[(15 - y - 1) * 15 + x] == '*'; }
	bool EnRoute3(int x, int y) const { return Map[(15 - y - 1) * 15 + x] == '/'; }
	bool EnRoute(int x, int y) const { return EnRoute0(x, y) || EnRoute1(x, y) || EnRoute2(x, y) || EnRoute3(x, y);}
	bool BlocageInvisible(int x, int y) const { return Map[(15 - y - 1) * 15 + x] == '0'; }
	bool hasBlocage(int x, int y) const { return Mur(x, y) || Ecran(x, y) || EnRoute(x, y) || BlocageInvisible(x, y); }
	bool MP0(int x, int y) const { return Map[(15 - y - 1) * 15 + x] == 'A'; }
	bool MP1(int x, int y) const { return Map[(15 - y - 1) * 15 + x] == 'B'; }
	bool MP2(int x, int y) const { return Map[(15 - y - 1) * 15 + x] == 'C'; }
	bool Distributeur(int x, int y) const { return MP0(x, y) || MP1(x, y) || MP2(x, y); }
	bool Stockage(int x, int y) const { return Map[(15 - y - 1) * 15 + x] == 'S'; }


	int Lpix = 40;  // largeur en pixels des cases du labyrinthe
	_Heros Heros;   // data du héros

	map<string, _MatiereP> AllTypeMp;
	map<string, _ProduitF> ProduitsFinalsAFaire;
	map<string, map<string, int>> ProduitsFinalsComposants;

	map<string, int> allMpBsn;
	map<string, int> allMpCollecte;
	map<int, string> cmdSurEcran;

	vector<_MatiereP> mpEnR0;
	vector<_MatiereP> mpEnR1;
	vector<_MatiereP> mpEnR2;
	vector<_MatiereP> mpEnR3;
						   
	vector<_ProduitF> pfEnR0;
	vector<_ProduitF> pfEnR1;
	vector<_ProduitF> pfEnR2;
	vector<_ProduitF> pfEnR3;

	int frameCounter = 0;	// pr animation
	int frameSwitch = 10;

	double startTime = 0.0;
	double timeLeft = 0.0;


	void cmdGenerate(int numCmd) {
		if (allMpBsn.empty()) return;

		auto it = allMpBsn.begin();
		advance(it, rand() % allMpBsn.size());

		string nomMp = it->first;
		int maxQty = it->second;

		int nbrMpRequis = (maxQty == 1) ? 1 : maxQty > MPMAXCMD ? (rand() % MPMAXCMD) + 1 : (rand() % maxQty) + 1;

		cmdSurEcran[numCmd] = nomMp + " " + to_string(nbrMpRequis);
	}

	void pfGenerate() {
		// Vérification production en cours
		if (!pfEnR0.empty() || !pfEnR1.empty() || !pfEnR2.empty() || !pfEnR3.empty()) return;

		// Trouver TOUS les PFs possibles
		vector<string> possiblePFs;
		for (const auto& pf : ProduitsFinalsComposants) {

			bool canGenerate = true;
			for (const auto& mpForOne : pf.second) {
				if (allMpCollecte[mpForOne.first] < mpForOne.second) {
					canGenerate = false;
					break;
				}
			}
			if (canGenerate) possiblePFs.push_back(pf.first);
		}

		// Choisir un PF aléatoire parmi les possibles
		if (!possiblePFs.empty()) {
			string nomPf = possiblePFs[rand() % possiblePFs.size()];

			cout << "=== AVANT GENERATION ===" << endl;
			printMap(allMpCollecte, "allMpCollecte");

			// Retirer les composants
			for (const auto& mpForSpecifOne : ProduitsFinalsComposants[nomPf]) {
				allMpCollecte[mpForSpecifOne.first] -= mpForSpecifOne.second;
			}

			// Créer et placer le PF
			_ProduitF newPf = ProduitsFinalsAFaire[nomPf];
			int routeIndex = rand() % 4;
			newPf.Pos = (routeIndex == 0) ? debutR0 : (routeIndex == 1) ? debutR1 : (routeIndex == 2) ? debutR2 : debutR3;

			auto& targetRoute = (routeIndex == 0) ? pfEnR0 : (routeIndex == 1) ? pfEnR1 : (routeIndex == 2) ? pfEnR2 : pfEnR3;
			targetRoute.push_back(newPf);

			refreshPfRoute = true;
			cout << "=== APRES GENERATION ===" << endl;
			cout << "PF généré: " << nomPf << " sur route " << routeIndex << endl;
			printMap(allMpCollecte, "allMpCollecte");
		}
	}

	void assetInit() {
		for (auto& mp : AllTypeMp) {
			(mp.first == "Mp0") ? replace(mp.second.texture.begin(), mp.second.texture.end(), 'W', 'R') : // Remplace 'W' par 'R'
				(mp.first == "Mp1") ? replace(mp.second.texture.begin(), mp.second.texture.end(), 'W', 'C') :
				NULL;
			mp.second.IdTex = G2D::initTextureFromString(mp.second.Size, mp.second.texture);
			mp.second.Size = mp.second.Size * 2; // on peut zoomer la taille du sprite
		}

		for (auto& pf : ProduitsFinalsAFaire) {
			pf.second.IdTex = G2D::initTextureFromString(pf.second.Size, pf.second.texture);
			if (pf.second.IdTex == 0) { // Vérifiez que la texture est bien chargée
				cerr << "Erreur chargement texture pour " << pf.first << endl;
			}
			pf.second.Size = pf.second.Size * 2; // on peut zoomer la taille du sprite
			ProduitsFinalsAFaire[pf.first] = pf.second;
		}
	}

	void init() {
		//mettre a zero les valeurs

		Heros.salaire = SALAIREINIT;
		Heros.pfAlreadyDepose = 0;
		Heros.Pos = V2(450, 300);
		Heros.isWin = false;

		ProduitsFinalsComposants.clear();

		allMpBsn.clear();
		allMpCollecte.clear();
		cmdSurEcran.clear();

		mpEnR0.clear();
		mpEnR1.clear();
		mpEnR2.clear();
		mpEnR3.clear();

		pfEnR0.clear();
		pfEnR1.clear();
		pfEnR2.clear();
		pfEnR3.clear();

		//init all type of MP
		for (int i = 0; i < NBRTYPEMP; i++) {
			string nom = "Mp" + to_string(i);
			_MatiereP mp(nom);
			if (mp.texture.empty()) {
				mp.texture = "[default texture]"; // Valeur par défaut
			}
			AllTypeMp[mp.mpNom] = mp;
		}
		//init all composant of PF
		for (int i = 0; i < NBRPF; i++) {
			string vNomPf = "PF" + to_string(i);
			_ProduitF pf = _ProduitF(vNomPf);
			ProduitsFinalsAFaire[pf.pfNom] = pf;

			map<string, int> composants;
			set<string> typesUtilises;

			// Boucle jusqu’à avoir au moins 2 types différents
			while (typesUtilises.size() < 2) {
				composants.clear();
				typesUtilises.clear();
				int nbrTypeMpRequis = rand() % NBRTYPEMP + 3; //car commence a 3 pr eviter un peu 1 seul mp

				for (int j = 0; j < nbrTypeMpRequis; j++) {
					int indexType = rand() % NBRTYPEMP;
					int quantite = rand() % (MPNEEDMAX - MPNEEDMIN + 1) + MPNEEDMIN;
					string nomMp = "Mp" + to_string(indexType);
					composants[nomMp] += quantite;
					typesUtilises.insert(nomMp);
				}
			}
			ProduitsFinalsComposants[vNomPf] = composants;
		}
		//init all cmd prepare for ecran
		for (auto& ProduitFinalComposants : ProduitsFinalsComposants) {
			for (auto& ComposantPF : ProduitFinalComposants.second) {
				allMpBsn[ComposantPF.first] += ComposantPF.second;
			}
		}

		for (int i = 0; i < 4; i++) {
			cmdGenerate(i);
		}
		assetInit();
	}

	GameData() {
		init();

		for (const auto& pf : ProduitsFinalsComposants) {
			cout << "Composants pour " << pf.first << ":" << endl;
			printMap(pf.second, "composants");
		}
	}

};



void Render(const GameData& G)  // const ref => garantit que l'on ne modifie pas les donnes de G
{
	if (ecran == ECRAN::ECRAN_ACCEUIL)
	{
		G2D::clearScreen(Color::White);

		V2 btnPos = V2(150, 135);
		V2 btnSize = V2(450, 50);
		Color btnColor = Color::Blue;

		G2D::drawStringFontMono(V2(15, 550), "Vous etes un employe dans une usine.", 13.0f, 1.0f, Color::Black);
		G2D::drawStringFontMono(V2(15, 530), "Un ecran noir affichera les matiere premiere necessaires.", 13.0f, 1.0f, Color::Black);

		G2D::drawStringFontMono(V2(15, 500), "Lorsque vous avez collecte tous les mp d’un produit final,", 13.0f, 1.0f, Color::Black);
		G2D::drawStringFontMono(V2(15, 480), "cote l ecran noir vous livrera ce produit.", 13.0f, 1.0f, Color::Black);

		G2D::drawStringFontMono(V2(15, 450), "Votre role est de collecter le produit final ", 13.0f, 1.0f, Color::Black);
		G2D::drawStringFontMono(V2(15, 430), "et de le deposer dans le stock.", 13.0f, 1.0f, Color::Black);

		G2D::drawStringFontMono(V2(15, 400), "Votre salaire de depart est de " + to_string(SALAIREINIT), 13.0f, 1.0f, Color::Black);
		G2D::drawStringFontMono(V2(15, 380), "Envoyer une matiere premiere vous rapporte " + to_string(G.Heros.primePerMP), 13.0f, 1.0f, Color::Black);
		G2D::drawStringFontMono(V2(15, 360), "stocker un produit final vous rapporte " + to_string(G.Heros.primePerPf), 13.0f, 1.0f, Color::Black);

		G2D::drawStringFontMono(V2(15, 330), "Completez " + to_string(NBRPF) + " produits OU atteignez un salaire compris " + to_string(VALWIN), 13.0f, 1.0f, Color::Black);
		G2D::drawStringFontMono(V2(15, 310), "pour gagner la partie.", 13.0f, 1.0f, Color::Black);

		G2D::drawStringFontMono(V2(15, 280), "Chaque produit a une valeur de " + to_string(PFVALUE), 13.0f, 1.0f, Color::Black);
		G2D::drawStringFontMono(V2(15, 260), "S il n est pas recupere a temps, il tombe et se casse : ", 13.0f, 1.0f, Color::Black);
		G2D::drawStringFontMono(V2(15, 240), "vous perdez " + to_string(PFVALUE), 13.0f, 1.0f, Color::Black);

		G2D::drawStringFontMono(V2(15, 210), "Si votre salaire tombe a 0 ou moins, vous perdez.", 13.0f, 1.0f, Color::Black);

		G2D::drawRectangle(btnPos, btnSize, btnColor, true);
		G2D::drawStringFontMono(btnPos + V2(15, 20), "appuyez ENTREZ pour commencer", 14.0f, 1.5f, Color::White);

		G2D::drawStringFontMono(V2(85, 100), "Bienvenue dans le simulateur de travail !", 14.0f, 1.5f, Color::Black);

	}

	else if (ecran == ECRAN::ECRAN_PAUSE)
	{
		if (G2D::isOnPause()) {
			G2D::drawStringFontRoman(posWord, "PAUSE", 30.0f, 4.0f, Color::White); // Texte en grand
		}
	}

	else if (ecran == ECRAN::ECRAN_GAME_OVER)
	{
		G2D::clearScreen(Color::White);

		(G.Heros.isWin) ? G2D::drawStringFontRoman(V2(300, 400), "WIN") :
			G2D::drawStringFontRoman(V2(300, 400), "LOSS");

		G2D::drawStringFontMono(V2(350, 500), to_string(G.timeLeft));
	}

	else
	{
		G2D::clearScreen(Color::Gray);

		for (int x = 0; x < 15; x++)
		{
			for (int y = 0; y < 15; y++)
			{
				int xx = x * G.Lpix;
				int yy = y * G.Lpix;

				if (G.Mur(x, y) || G.EnRoute(x, y) || G.Ecran(x, y) || G.Distributeur(x, y) || G.Stockage(x, y))
				{
					Color vc = G.Mur(x, y) ? Color::Blue : G.EnRoute(x, y) ? Color::Yellow : G.Ecran(x, y) ? Color::Black : G.MP0(x, y) ? Color::Red : G.MP1(x, y) ? Color::Cyan : G.MP2(x, y) ? Color::White : Color::Green;
					G2D::drawRectangle(V2(xx, yy), V2(G.Lpix, G.Lpix), vc, true);
				}
			}
		}

		// affichage du héro avec boite englobante et zoom x 2
		G2D::drawRectangle(G.Heros.Pos, G.Heros.Size, Color::Red);
		G2D::drawRectWithTexture(G.Heros.IdTex, G.Heros.Pos, G.Heros.Size);

		//affichage de nom de zone
		for (int i = 0; i < 4;i++) {
			G2D::drawStringFontMono(posZoneNom[i], posZoneNomStr[i], 13.0f, 1.0f, Color::White);
		}

		//afficher la progression de l'objectif
		string phrase = "Salaire :" + to_string(G.Heros.salaire) + " / " + to_string(VALWIN) + "      Produits collectés :" + to_string(G.Heros.pfAlreadyDepose) + " / " + to_string(NBRPF);
		G2D::drawStringFontMono(V2(15, 580), phrase, 14.0f, 1.5f, Color::White);
		
		//affichage de la cmd
		if (!G.Heros.isWin || refreshCmd) {
			for (int i = 0; i < 4; i++) {
				if (G.cmdSurEcran.count(i) > 0) { // Vérifier que la clé existe
					string cmd = G.cmdSurEcran.at(i);
					Color c = Color::White;

					if (cmd.find("Mp0") != string::npos) c = Color::Red;
					else if (cmd.find("Mp1") != string::npos) c = Color::Cyan;
					else if (cmd.find("Mp2") != string::npos) c = Color::White;

					G2D::drawStringFontMono(posEcran[i], cmd, 15.0f, 1.0f, c);
				}
			}
			refreshCmd = false;
		}

		//affichage de la matiere premiere en route
		if (!G.mpEnR0.empty() || !G.mpEnR1.empty() || !G.mpEnR2.empty() || !G.mpEnR3.empty() || refreshMpRoute) {
			if (!G.mpEnR0.empty()) {
				for (auto& mp : G.mpEnR0) {
					if (G.AllTypeMp.find(mp.mpNom) != G.AllTypeMp.end()) {
						G2D::drawRectWithTexture(mp.IdTex, mp.Pos, mp.Size);
					}
				}
			}
			if (!G.mpEnR1.empty()) {
				for (auto& mp : G.mpEnR1) {
					if (G.AllTypeMp.find(mp.mpNom) != G.AllTypeMp.end()) {
						G2D::drawRectWithTexture(mp.IdTex, mp.Pos, mp.Size);
					}
				}
			}
			if (!G.mpEnR2.empty()) {
				for (auto& mp : G.mpEnR2) {
					if (G.AllTypeMp.find(mp.mpNom) != G.AllTypeMp.end()) {
						G2D::drawRectWithTexture(mp.IdTex, mp.Pos, mp.Size);
					}
				}
			}
			if (!G.mpEnR3.empty()) {
				for (auto& mp : G.mpEnR3) {
					if (G.AllTypeMp.find(mp.mpNom) != G.AllTypeMp.end()) {
						G2D::drawRectWithTexture(mp.IdTex, mp.Pos, mp.Size);
					}
				}
			}
			refreshMpRoute = false;
			
		}
		
		//affichage de la produit fianle qui sort
		if (refreshPfRoute || !G.pfEnR0.empty() || !G.pfEnR1.empty() || !G.pfEnR2.empty() || !G.pfEnR3.empty()) {
			
			// Route 0
			for (auto& pf : G.pfEnR0) {
				if (pf.IdTex != 0) {
					G2D::drawRectWithTexture(pf.IdTex, pf.Pos, pf.Size);
				}
			}

			// Route 1
			for (auto& pf : G.pfEnR1) {
				if (pf.IdTex != 0) {
					G2D::drawRectWithTexture(pf.IdTex, pf.Pos, pf.Size);
				}
			}

			// Route 2
			for (auto& pf : G.pfEnR2) {
				if (pf.IdTex != 0) {
					G2D::drawRectWithTexture(pf.IdTex, pf.Pos, pf.Size);
				}
			}

			// Route 3
			for (auto& pf : G.pfEnR3) {
				if (pf.IdTex != 0) {
					G2D::drawRectWithTexture(pf.IdTex, pf.Pos, pf.Size);
				}
			}
			refreshPfRoute = false;
		}


		if (G2D::isOnPause()) {
			G2D::drawStringFontRoman(posWord, "PAUSE", 30.0f, 4.0f, Color::White); // Texte en grand
		}

		if (G.Distributeur(G.Heros.Pos.x / G.Lpix, G.Heros.Pos.y / G.Lpix) && G.Heros.nbrObjHas==0)
			G2D::drawStringFontMono(posSentence, "Appuyez sur 'C' pour collecter mp", 13.0f, 1.0f, Color::White);
		else if(G.Heros.isNearRoute &&(!G.pfEnR0.empty() || !G.pfEnR1.empty() || !G.pfEnR2.empty() || !G.pfEnR3.empty()))
			G2D::drawStringFontMono(posSentence, "Appuyez sur 'C' pour collecter pf", 13.0f, 1.0f, Color::White);
		else if(G.Heros.isNearRoute && (G.Heros.hasMP0 || G.Heros.hasMP1 || G.Heros.hasMP2) && (G.pfEnR0.empty() && G.pfEnR1.empty() && G.pfEnR2.empty() && G.pfEnR3.empty()))
			G2D::drawStringFontMono(posSentence, "Appuyez sur 'D' pour deposer mp", 13.0f, 1.0f, Color::White);
		else if (G.Stockage(G.Heros.Pos.x / G.Lpix, G.Heros.Pos.y / G.Lpix) && G.Heros.nbrObjHas !=0 && G.Heros.hasPF)
			G2D::drawStringFontMono(posSentence, "Appuyez sur 'D' pour deposer pf", 13.0f, 1.0f, Color::White);
		else if (msgDepotPfFrist&& G.Heros.hasPF) {
			G2D::drawStringFontMono(posSentence, "Deposez pf avant de collecter mp", 13.0f, 1.0f, Color::White);
		}
		else if (msgPfTombe) {
			G2D::drawStringFontMono(posSentence, "Attention, vous avez tombe un pf !", 13.0f, 1.0f, Color::White);
		}

		G.Heros.hasMP0 ? G2D::drawRectWithTexture(G.AllTypeMp.at("Mp0").IdTex, V2(500, 5), V2(30, 30)) :
			G.Heros.hasMP1 ? G2D::drawRectWithTexture(G.AllTypeMp.at("Mp1").IdTex, V2(500, 5), V2(30, 30)) :
			G.Heros.hasMP2 ? G2D::drawRectWithTexture(G.AllTypeMp.at("Mp2").IdTex, V2(500, 5), V2(30, 30)) :
			G.Heros.hasPF ? G2D::drawRectWithTexture(G.ProduitsFinalsAFaire.at(G.Heros.pfNom).IdTex, V2(500, 5), V2(30, 30)) : NULL;
	
		G2D::drawStringFontMono(V2(540, 15), to_string(G.Heros.nbrObjHas), 13.0f, 1.0f, Color::White);

		G2D::drawStringFontMono(V2(10, 10), "Salaire: " + to_string(G.Heros.salaire), 13.0f, 1.0f, Color::White);
	}


	G2D::Show();
}


void gestion_ecran_jeu(GameData& G)
{
	bool leftGo = G2D::isKeyPressed(Key::LEFT);
	bool rightGo = G2D::isKeyPressed(Key::RIGHT);
	bool upGo = G2D::isKeyPressed(Key::UP);
	bool downGo = G2D::isKeyPressed(Key::DOWN);

	bool isMove = leftGo || rightGo || upGo || downGo;

	bool leftIsPossible = (!(G.hasBlocage((G.Heros.Pos.x - 2) / G.Lpix, G.Heros.Pos.y / G.Lpix))) && (!G.hasBlocage((G.Heros.Pos.x - 2) / G.Lpix, (G.Heros.Pos.y + G.Heros.Size.y) / G.Lpix));
	bool rightIsPossible = (!G.hasBlocage((G.Heros.Pos.x + G.Heros.Size.x + 2) / G.Lpix, G.Heros.Pos.y / G.Lpix)) && (!G.hasBlocage((G.Heros.Pos.x + G.Heros.Size.x + 2) / G.Lpix, (G.Heros.Pos.y + G.Heros.Size.y) / G.Lpix));
	bool upIsPossible = (!G.hasBlocage(G.Heros.Pos.x / G.Lpix, (G.Heros.Pos.y + G.Heros.Size.y + 2) / G.Lpix)) && (!G.hasBlocage((G.Heros.Pos.x + G.Heros.Size.x) / G.Lpix, (G.Heros.Pos.y + G.Heros.Size.y + 2) / G.Lpix));
	bool downIsPossible = (!G.hasBlocage(G.Heros.Pos.x / G.Lpix, (G.Heros.Pos.y - 2) / G.Lpix)) && (!G.hasBlocage((G.Heros.Pos.x + G.Heros.Size.x) / G.Lpix, (G.Heros.Pos.y - 2) / G.Lpix));

	if (leftGo && leftIsPossible)  G.Heros.Pos.x-=2;
	if (rightGo && rightIsPossible) G.Heros.Pos.x+=2;
	if (upGo && upIsPossible)    G.Heros.Pos.y+=2;
	if (downGo && downIsPossible)  G.Heros.Pos.y-=2;

	if ((G.frameCounter >= G.frameSwitch) && isMove) {
		G.Heros.IdTex = (G.Heros.IdTex == G.Heros.IdTexIdle) ? G.Heros.IdTexWalk : G.Heros.IdTexIdle;
		G.frameCounter = 0; // Réinitialiser le compteur
	}
	G.frameCounter += 1;

	bool HerosIsNearRoute0 = (G.EnRoute0((G.Heros.Pos.x - G.Heros.Size.x) / G.Lpix, G.Heros.Pos.y / G.Lpix)) && (G.EnRoute0((G.Heros.Pos.x - G.Heros.Size.x) / G.Lpix, (G.Heros.Pos.y + G.Heros.Size.y) / G.Lpix));
	bool HerosIsNearRoute1 = (G.EnRoute1((G.Heros.Pos.x - G.Heros.Size.x) / G.Lpix, G.Heros.Pos.y / G.Lpix)) && (G.EnRoute1((G.Heros.Pos.x - G.Heros.Size.x) / G.Lpix, (G.Heros.Pos.y + G.Heros.Size.y) / G.Lpix));
	bool HerosIsNearRoute2 = (G.EnRoute2((G.Heros.Pos.x - G.Heros.Size.x) / G.Lpix, G.Heros.Pos.y / G.Lpix)) && (G.EnRoute2((G.Heros.Pos.x - G.Heros.Size.x) / G.Lpix, (G.Heros.Pos.y + G.Heros.Size.y) / G.Lpix));
	bool HerosIsNearRoute3 = (G.EnRoute3((G.Heros.Pos.x - G.Heros.Size.x) / G.Lpix, G.Heros.Pos.y / G.Lpix)) && (G.EnRoute3((G.Heros.Pos.x - G.Heros.Size.x) / G.Lpix, (G.Heros.Pos.y + G.Heros.Size.y) / G.Lpix));
	G.Heros.isNearRoute = HerosIsNearRoute0 || HerosIsNearRoute1 || HerosIsNearRoute2 || HerosIsNearRoute3;
	G.Heros.routeNearIndex = HerosIsNearRoute0 ? 0 : HerosIsNearRoute1 ? 1 : HerosIsNearRoute2 ? 2 : HerosIsNearRoute3 ? 3 : NULL;

	//pour generer pf sselon les mp collecte
	G.pfGenerate();

	//pour collecter mp ds zone de distributeur
	if (G.Distributeur(G.Heros.Pos.x / G.Lpix, G.Heros.Pos.y / G.Lpix) && G2D::keyHasBeenHit(Key::C)){
		if (G.Heros.hasPF) msgDepotPfFrist = true;
		else{
			msgDepotPfFrist = false;
			int vAlreadyHasNum = G.Heros.alreadyHasMpNum;
			G.Heros.hasMP0 = G.MP0(G.Heros.Pos.x / G.Lpix, G.Heros.Pos.y / G.Lpix);
			G.Heros.hasMP1 = G.MP1(G.Heros.Pos.x / G.Lpix, G.Heros.Pos.y / G.Lpix);
			G.Heros.hasMP2 = G.MP2(G.Heros.Pos.x / G.Lpix, G.Heros.Pos.y / G.Lpix);
			G.Heros.hasPF = false;
			G.Heros.alreadyHasMpNum = G.Heros.hasMP0 ? 0 : G.Heros.hasMP1 ? 1 : G.Heros.hasMP2 ? 2 : G.Heros.hasPF ? 3 : NULL;
			if (G.Heros.alreadyHasMpNum != vAlreadyHasNum) {
				G.Heros.nbrObjHas = 1;
			}
			else {
				G.Heros.nbrObjHas += 1;
			}
		}
	}
	
	//pour collecter pf a cote de route
	if (G.Heros.isNearRoute && G2D::keyHasBeenHit(Key::C)) {
		int vAlreadyHasNum = G.Heros.alreadyHasMpNum;
		int indexRoute = HerosIsNearRoute0 ? 0 : HerosIsNearRoute1 ? 1 : HerosIsNearRoute2 ? 2 : 3;
		vector<_ProduitF>* routePF = nullptr;
		routePF = (indexRoute == 0) ? &G.pfEnR0 : (indexRoute == 1) ? &G.pfEnR1 :
			(indexRoute == 2) ? &G.pfEnR2 : &G.pfEnR3;

		if (routePF && !routePF->empty()) {
			string nomPf = routePF->back().pfNom;
			if (!nomPf.empty()) {
				G.Heros.hasPF = true;
				G.Heros.hasMP0 = G.Heros.hasMP1 = G.Heros.hasMP2 = false;
				G.Heros.pfNom = nomPf;
				routePF->pop_back();
				G.Heros.alreadyHasMpNum = 3;
				if (G.Heros.alreadyHasMpNum != vAlreadyHasNum) {
					G.Heros.nbrObjHas = 1;
				}
				else {
					G.Heros.nbrObjHas += 1;
				}
			}
		}
		refreshPfRoute = true;
	}

	//pour deposer mp a cote de route
	if (G.Heros.isNearRoute && (G.Heros.hasMP0 || G.Heros.hasMP1 || G.Heros.hasMP2) && G2D::isKeyPressed(Key::D) && !G.Heros.hasPF) {
		// Déterminer la route concernée
		int indexR = (HerosIsNearRoute0) ? 0 : (HerosIsNearRoute1) ? 1 : (HerosIsNearRoute2) ? 2 : 3;

		if (indexR >= 0 && indexR < 4) { // Vérification de l'index
			string nomMp = (G.Heros.hasMP0) ? "Mp0" : (G.Heros.hasMP1) ? "Mp1" : "Mp2";

			if (!nomMp.empty() && G.cmdSurEcran.count(indexR) > 0) {
				string commandeActuelle = G.cmdSurEcran.at(indexR);
				size_t espacePos = commandeActuelle.find(' ');
				string mpDemande = commandeActuelle.substr(0, espacePos);
				int quantiteDemandee = stoi(commandeActuelle.substr(espacePos + 1));

				if (mpDemande == nomMp && G.Heros.nbrObjHas >= quantiteDemandee) {
					// Mise à jour des MP
					G.allMpCollecte[nomMp] += quantiteDemandee;
					G.allMpBsn[nomMp] -= quantiteDemandee;
					if (G.allMpBsn[nomMp] <= 0) {
						G.allMpBsn.erase(nomMp);
					}

					cout << "=== APRES COLLECTION ===" << endl;
					printMap(G.allMpCollecte, "allMpCollecte");

					// Ajout des MP sur la route correspondante
					for (int i = 0; i < quantiteDemandee; i++) {
						if (G.AllTypeMp.count(nomMp) > 0) {
							_MatiereP mp = G.AllTypeMp.at(nomMp);
							mp.Pos = (indexR == 0) ? finR0 :
								(indexR == 1) ? finR1 :
								(indexR == 2) ? finR2 : finR3;

							indexR == 0 ? G.mpEnR0.push_back(mp) : indexR == 1 ? G.mpEnR1.push_back(mp) : indexR == 2 ? G.mpEnR2.push_back(mp) : G.mpEnR3.push_back(mp);
						}
					}

					// Mise à jour du jeu
					G.Heros.salaire += G.Heros.primePerMP * quantiteDemandee;
					G.Heros.nbrObjHas -= quantiteDemandee;
					if (G.Heros.nbrObjHas == 0) G.Heros.hasMP0 = G.Heros.hasMP1 = G.Heros.hasMP2 = G.Heros.hasPF = false;

					// Génération d'une nouvelle commande
					G.cmdGenerate(indexR);
					refreshCmd = true;
					refreshMpRoute = true;
				}
			}
		}
	}

	//pour deposer pf ds zone de stockage
	if (G.Stockage(G.Heros.Pos.x / G.Lpix, G.Heros.Pos.y / G.Lpix) && G2D::isKeyPressed(Key::D) && G.Heros.hasPF){
		G.Heros.salaire += G.Heros.primePerPf * G.Heros.nbrObjHas;
		G.Heros.pfAlreadyDepose += G.Heros.nbrObjHas;
		G.Heros.hasPF = false;
		G.Heros.nbrObjHas = 0;
		G.Heros.pfNom = "";
	}

	//pour faire bouger les mp en route
	if (!G.mpEnR0.empty()) {
		auto it = G.mpEnR0.begin();
		while (it != G.mpEnR0.end()) {
			if (it->Pos.x < 0 || it->Pos.y < 0 || !G.EnRoute(it->Pos.x / G.Lpix, it->Pos.y / G.Lpix)) {
				it = G.mpEnR0.erase(it);
			}
			else {
				it->move();
				++it;
			}
		}
	}
	if (!G.mpEnR1.empty()) {
		auto it = G.mpEnR1.begin();
		while (it != G.mpEnR1.end()) {
			if (it->Pos.x < 0 || it->Pos.y < 0 || !G.EnRoute(it->Pos.x / G.Lpix, it->Pos.y / G.Lpix)) {
				it = G.mpEnR1.erase(it);
			}
			else {
				it->move();
				++it;
			}
		}
	}
	if (!G.mpEnR2.empty()) {
		auto it = G.mpEnR2.begin();
		while (it != G.mpEnR2.end()) {
			if (it->Pos.x < 0 || it->Pos.y < 0 || !G.EnRoute(it->Pos.x / G.Lpix, it->Pos.y / G.Lpix)) {
				it = G.mpEnR2.erase(it);
			}
			else {
				it->move();
				++it;
			}
		}
	}
	if (!G.mpEnR3.empty()) {
		auto it = G.mpEnR3.begin();
		while (it != G.mpEnR3.end()) {
			if (it->Pos.x < 0 || it->Pos.y < 0 || !G.EnRoute(it->Pos.x / G.Lpix, it->Pos.y / G.Lpix)) {
				it = G.mpEnR3.erase(it);
			}
			else {
				it->move();
				++it;
			}
		}
	}

	//pour faire bouger les pf qui sort
	if (!G.pfEnR0.empty()) {
		auto it = G.pfEnR0.begin();
		while (it != G.pfEnR0.end()) {
			it->move();
			if (it->Pos.x < 0 || it->Pos.y < 0 || !G.EnRoute(it->Pos.x / G.Lpix, it->Pos.y / G.Lpix)) {
				if (!it->estCollecte) {
					G.Heros.salaire -= it->value;
					it->estTombe = true;
					for (auto& mpForOnePf : G.ProduitsFinalsComposants) {
						if (mpForOnePf.first == it->pfNom) {
							for (auto& mpComposant : mpForOnePf.second) {
								G.allMpBsn[mpComposant.first] += mpComposant.second;
							}
						}
					}
					msgPfTombe = true;
					timerMsgPfTombe = 2.0f;
				}
				else msgPfTombe = false;
				it = G.pfEnR0.erase(it);
			}
			else {
				++it;
			}
		}
	}
	if (!G.pfEnR1.empty()) {
		auto it = G.pfEnR1.begin();
		while (it != G.pfEnR1.end()) {
			it->move();
			if (it->Pos.x < 0 || it->Pos.y < 0 || !G.EnRoute(it->Pos.x / G.Lpix, it->Pos.y / G.Lpix)) {
				if (!it->estCollecte) {
					G.Heros.salaire -= it->value;
					it->estTombe = true;
					for (auto& mpForOnePf : G.ProduitsFinalsComposants) {
						if (mpForOnePf.first == it->pfNom) {
							for (auto& mpComposant : mpForOnePf.second) {
								G.allMpBsn[mpComposant.first] += mpComposant.second;
							}
						}
					}
					msgPfTombe = true;
					timerMsgPfTombe = 2.0f;
				}
				else msgPfTombe = false;
				it = G.pfEnR1.erase(it);
			}
			else {
				++it;
			}
		}
	}
	if (!G.pfEnR2.empty()) {
		auto it = G.pfEnR2.begin();
		while (it != G.pfEnR2.end()) {
			it->move();
			if (it->Pos.x < 0 || it->Pos.y < 0 || !G.EnRoute(it->Pos.x / G.Lpix, it->Pos.y / G.Lpix)) {
				if (!it->estCollecte) {
					G.Heros.salaire -= it->value;
					it->estTombe = true;
					for (auto& mpForOnePf : G.ProduitsFinalsComposants) {
						if (mpForOnePf.first == it->pfNom) {
							for (auto& mpComposant : mpForOnePf.second) {
								G.allMpBsn[mpComposant.first] += mpComposant.second;
							}
						}
					}
					msgPfTombe = true;
					timerMsgPfTombe = 2.0f;
				}
				else msgPfTombe = false;
				it = G.pfEnR2.erase(it);
			}
			else {
				++it;
			}
		}
	}
	if (!G.pfEnR3.empty()) {
		auto it = G.pfEnR3.begin();
		while (it != G.pfEnR3.end()) {
			it->move();
			if (it->Pos.x < 0 || it->Pos.y < 0 || !G.EnRoute(it->Pos.x / G.Lpix, it->Pos.y / G.Lpix)) {
				if (!it->estCollecte) {
					G.Heros.salaire -= it->value;
					it->estTombe = true;
					for (auto& mpForOnePf : G.ProduitsFinalsComposants) {
						if (mpForOnePf.first == it->pfNom) {
							for (auto& mpComposant : mpForOnePf.second) {
								G.allMpBsn[mpComposant.first] += mpComposant.second;
							}
						}
					}
					msgPfTombe = true;
					timerMsgPfTombe = 2.0f;
				}
				else msgPfTombe = false;
				it = G.pfEnR3.erase(it);
			}
			else {
				++it;
			}
		}
	}

	if (msgPfTombe) {
		timerMsgPfTombe -= 0.01f;
		if (timerMsgPfTombe <= 0.0f) {
			msgPfTombe = false;
		}
	}

	if (G.Heros.salaire >= VALWIN || G.Heros.pfAlreadyDepose >= NBRPF) G.Heros.isWin = true;
		
	if (G2D::isOnPause()) ecran = ECRAN::ECRAN_PAUSE;
	else if (G.Heros.isWin || G.Heros.salaire <= 0) ecran = ECRAN::ECRAN_GAME_OVER;
}

void gestion_ecran_pause()
{
	if (G2D::isKeyPressed(Key::P)) ecran = ECRAN::ECRAN_JEU;
}

void gestion_GameOver(GameData& G)
{
	static bool firstTime = true;
	if (firstTime && (G.Heros.isWin || G.Heros.salaire <= 0)) {
		G.startTime = G2D::elapsedTimeFromStartSeconds();
		firstTime = false;
	}

	G.timeLeft = 3 - (int)(G2D::elapsedTimeFromStartSeconds() - G.startTime);

	if (G.timeLeft <= 0) {
		G.init();
		ecran = ECRAN::ECRAN_ACCEUIL;
		firstTime = true; // Reset pour la prochaine fois
	}
}

void gestion_ecran_accueil(GameData& G)
{
	if (G2D::isKeyPressed(Key::ENTER)) {
		ecran = ECRAN::ECRAN_JEU;
	}
}


void Logic(GameData& G)
{
	switch (ecran)
	{
	case ECRAN::ECRAN_ACCEUIL:
		gestion_ecran_accueil(G);
		break;
	case ECRAN::ECRAN_JEU:
		gestion_ecran_jeu(G);
		break;
	case ECRAN::ECRAN_PAUSE:
		gestion_ecran_pause();
		break;
	case ECRAN::ECRAN_GAME_OVER:
		gestion_GameOver(G);
		break;
	}

}



void AssetsInit(GameData& G)
{
	// Size passé en ref et texture en param
	G.Heros.IdTexIdle = G2D::initTextureFromString(G.Heros.Size, G.Heros.texture);
	G.Heros.IdTexWalk = G2D::initTextureFromString(G.Heros.Size, G.Heros.textureWalk);
	G.Heros.IdTex = G.Heros.IdTexIdle;

	G.Heros.Size = G.Heros.Size * 2; // on peut zoomer la taille du sprite


	G.assetInit();

}

int main(int argc, char* argv[])
{
	srand(time(0));
	// Get a different random number each time the program runs

	GameData G;

	G2D::initWindow(V2(G.Lpix * 15, G.Lpix * 15), V2(0, 200), string("WORK SIMU"));

	AssetsInit(G);

	int nbCalltoLogicPerSec = 60;

	G2D::Run(Logic, Render, G, nbCalltoLogicPerSec, true);

}

