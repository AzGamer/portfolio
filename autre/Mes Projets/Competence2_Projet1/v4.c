/************************************************/
/*	      IMPORTATION DES BIBLIOTHÈQUES			*/
/************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
/************************************************/
/*	                CONSTANTES			        */
/************************************************/
// taille du serpent
#define TAILLE 10
// dimensions du plateau
#define LARGEUR_PLATEAU 80	
#define HAUTEUR_PLATEAU 40
// position initiale de la tête du serpent
#define X_INITIAL1 40
#define Y_INITIAL1 13

#define X_INITIAL2 40
#define Y_INITIAL2 27
// nombre de pommes à manger pour gagner
#define NB_POMMES 10
// temporisation entre deux déplacements du serpent (en microsecondes)
#define ATTENTE 50000
// caractères pour représenter le serpent
#define CORPS 'X'
#define TETE1 '1'
#define TETE2 '2'

// touches de direction ou d'arrêt du jeu
#define HAUT 'z'
#define BAS 's'
#define GAUCHE 'q'
#define DROITE 'd'
#define STOP 'a'
// caractères pour les éléments du plateau
#define BORDURE '#'
#define PAVE '#'
#define VIDE ' '
#define POMME '6'
// constantes des pavés
#define TAILLE_PAVE 5
#define NB_PAVES 6
// coordonées des issues du plateau
#define X_ISSUES_HAUT_BAS (LARGEUR_PLATEAU / 2)
#define Y_ISSUES_GAUCHE_DROITE (HAUTEUR_PLATEAU / 2)
#define Y_ISSUE_HAUT 1
#define Y_ISSUE_BAS HAUTEUR_PLATEAU
#define X_ISSUE_GAUCHE 1
#define X_ISSUE_DROITE LARGEUR_PLATEAU
// tableaux pour les coordonées des pommes
const int LES_POMMES_X[NB_POMMES] = {40,75,78,2,9,78,74,2,72,5};
const int LES_POMMES_Y[NB_POMMES] = {20,38,2,2,5,38,32,38,32,2};
//tableaux pour les coordonées des pavés
const int LES_PAVES_X[NB_PAVES] = {4,73,4,73,38,38};
const int LES_PAVES_Y[NB_PAVES] = {4,4,33,33,14,22};



// compteur de pommes mangées
int nbPommesMangees1 = 0;
int nbPommesMangees2 = 0;


// définition d'un type pour le plateau : tPlateau
// Attention, pour que les indices du tableau 2D (qui commencent à 0) coincident
// avec les coordonées à l'écran (qui commencent à 1), on ajoute 1 aux dimensions
// et on neutralise la ligne 0 et la colonne 0 du tableau 2D (elles ne sont jamais
// utilisées)
typedef char tPlateau[LARGEUR_PLATEAU + 1][HAUTEUR_PLATEAU + 1];


// Protypes des fonctions
void initPlateau(tPlateau plateau); // Initialise le plateau de jeu
void dessinerPlateau(tPlateau plateau); // Dessine le plateau de jeu
void ajouterPomme(tPlateau plateau); // Ajoute une pomme au plateau et l'affiche
void afficher(int, int, char); // Affiche un caractère à une position donnée
void effacer(int x, int y); // Efface un caractère à une position donnée (en le remplaçant par un espace)
void dessinerSerpent1(int lesX[], int lesY[]); // Dessine le serpent sur le plateau

void dessinerSerpent2(int lesX[], int lesY[]); // Dessine le serpent sur le plateau

void progresser1(int lesX[], int lesY[], int lesX2[], int lesY2[], char direction, tPlateau plateau, bool *collision, bool *pommeMangee); // Fait progresser le serpent dans la direction donnée

void progresser2(int lesX[], int lesY[], int lesX2[], int lesY2[], char direction, tPlateau plateau, bool *collision, bool *pommeMangee); // Fait progresser le serpent dans la direction donnée

void gotoxy(int x, int y); // Déplace le curseur à une position donnée
int kbhit(); // Détecte si une touche est pressée
void disable_echo(); // Désactive l'affichage des touches clavier dans le terminal
void enable_echo(); // Active l'affichage des touches clavier dans le terminal
bool estCollisionPotentielle(int x, int y, int lesX[], int lesY[], int lesX2[], int lesY2[], tPlateau plateau); // Vérifie si une collision est potentielle avec le serpent à une position donnée
void distancePlusCourte(int lesX[], int lesY[], int nbPommes, int *xPosCible, int *yPosCible); // Détermine quel chemin est le plus court entre le chemin direct et l'utilisation des issues
void determinerDirection(int lesX[], int lesY[], int lesX2[], int lesY2[], char * direction, int xPositionCible, int yPositionCible, tPlateau lePlateau); // Détermine la direction à prendre

/************************************************/
/*	        	PROGRAMME PRINCIPAL 			*/
/************************************************/

int main(){
	// variable pour le nombre de déplacements
	int nombreDeplacements1 = 0;
	int nombreDeplacements2 = 0;

	// 2 tableaux contenant les positions des éléments qui constituent le serpent1
    int lesX1[TAILLE];
	int lesY1[TAILLE];

	// 2 tableaux contenant les positions des éléments qui constituent le serpent2
    int lesX2[TAILLE];
	int lesY2[TAILLE];

	// représente la touche frappée par l'utilisateur : touche de direction ou pour l'arrêt
	char touche;

	// direction courante du serpent (HAUT, BAS, GAUCHE ou DROITE)
	char direction1 = DROITE;
	char direction2 = GAUCHE;

	// le plateau de jeu
	tPlateau lePlateau;

    // initialisation des variables booléennes de contrôle de jeu : collision, gagné et pomme mangée
	bool collision = false;
	bool gagne = false;
	bool pommeMangee1 = false;
	bool pommeMangee2 = false;
   
	// initialisation de la position du serpent : positionnement de la
	// tête en (X_INITIAL, Y_INITIAL), puis des anneaux à sa gauche
	for(int i = 0 ; i < TAILLE ; i++){
		lesX1[i] = X_INITIAL1 - i;
		lesY1[i] = Y_INITIAL1;
	}

	for(int i = 0 ; i < TAILLE ; i++){
		lesX2[i] = X_INITIAL2 + i;
		lesY2[i] = Y_INITIAL2;
	}

	// mise en place du plateau
	initPlateau(lePlateau);
	system("clear");
	dessinerPlateau(lePlateau);

	ajouterPomme(lePlateau);

	// initialisation : le serpent se dirige vers la DROITE
	dessinerSerpent1(lesX1, lesY1);
	dessinerSerpent2(lesX2, lesY2);

	disable_echo();

    // direction de départ
	touche = DROITE;

	// boucle de jeu. Arret si touche STOP, si collision avec une bordure ou si toutes les pommes sont mangées
	do{
        // Position vers laquelle doit se diriger le serpent
		int xPositionCible1 = LES_POMMES_X[nbPommesMangees1 + nbPommesMangees2]; 
		int yPositionCible1 = LES_POMMES_Y[nbPommesMangees1 + nbPommesMangees2];
        
		// Pour le deuxième serpent
		int xPositionCible2 = LES_POMMES_X[nbPommesMangees1 + nbPommesMangees2]; 
		int yPositionCible2 = LES_POMMES_Y[nbPommesMangees1 + nbPommesMangees2];

        // Calcul de la du parcours le plus court entre la tête du serpent et la pomme, et modification de la position cible en si besoin d'emprunter une issue
		// Pour les 2 serpents
		distancePlusCourte(lesX1, lesY1, nbPommesMangees1 + nbPommesMangees2, &xPositionCible1, &yPositionCible1);
		distancePlusCourte(lesX2, lesY2, nbPommesMangees1 + nbPommesMangees2, &xPositionCible2, &yPositionCible2);

		// Pour les 2 serpents, on détermine la direction à prendre
		determinerDirection(lesX1, lesY1, lesX2, lesY2, &direction1, xPositionCible1, yPositionCible1, lePlateau);
		determinerDirection(lesX2, lesY2, lesX1, lesY1, &direction2, xPositionCible2, yPositionCible2, lePlateau);
		
		// Faire progresser le serpent dans la direction choisie
        progresser1(lesX1, lesY1, lesX2, lesY2, direction1, lePlateau, &collision, &pommeMangee1);
        progresser2(lesX2, lesY2, lesX1, lesY1, direction2, lePlateau, &collision, &pommeMangee2);

		// Vérifier si les serpents entrent en collision
		if (lesX1[0] == lesX2[0] && lesY1[0] == lesY2[0]) {
    		collision = true;
		}

        // Augmenter le nombre de 1
		nombreDeplacements1++;
		nombreDeplacements2++;

        // Si on mange une pomme
		if (pommeMangee1 || pommeMangee2){
            // Alors on augmente le nombre de pommes mangées
            if (pommeMangee1){
				nbPommesMangees1++;
			}
			if (pommeMangee2){
				nbPommesMangees2++;
			}

            // On vérifie si on a mangé toutes les pommes du jeu
			gagne = (nbPommesMangees1 + nbPommesMangees2 == NB_POMMES);

            // Si on a pas mangé toutes les pommes du jeu, alors on affiche la pomme suivante
			if (!gagne){
				ajouterPomme(lePlateau);
				pommeMangee1 = false;
				pommeMangee2 = false;
			}	
		}

        // Si on a pas mangé toutes les pommes du jeu
		if (!gagne){
            // Alors si on ne rencontre pas de collision
			if (!collision){
                // On attend (permet de gérer la vitesse du serpent)
				usleep(ATTENTE);

                // On vérifie la saisie d'un caractère au clavier et on modifie la variable touche en conséquence
				if (kbhit() == 1){
					touche = getchar();
				}
			}
		}
		gotoxy(LARGEUR_PLATEAU + 1, 1);
		printf("%d", collision);
	} while (touche != STOP && !collision && !gagne);


    // Réactiver l'affichage des caractères clavier
    enable_echo();

    // Déplacer le curseur sous le plateau
	gotoxy(1, HAUTEUR_PLATEAU + 1);

    // Afficher le nombre de déplacements
	printf("Nombre de déplacements du serpent n°1 : %d\n", nombreDeplacements1);
	printf("Nombre de déplacements du serpent n°2 : %d\n", nombreDeplacements2);

    // Afficher le nombre de pommes mangées
	printf("Nombre de pommes mangées n°1 : %d\n", nbPommesMangees1);
	printf("Nombre de pommes mangées n°2 : %d\n", nbPommesMangees2);

    // Fin du programme
	return EXIT_SUCCESS;
}


/************************************************/
/*		FONCTIONS ET PROCEDURES DU JEU 			*/
/************************************************/

void determinerDirection(int lesX[], int lesY[], int lesX2[], int lesY2[], char * direction, int xPositionCible, int yPositionCible, tPlateau lePlateau){
	if (xPositionCible == lesX2[0] && yPositionCible == lesY2[0]) {
		// Si la tête du serpent 2 atteint la même position cible
		// Dévier pour éviter la collision
		if (!estCollisionPotentielle(lesX2[0] - 1, lesY2[0], lesX, lesY, lesX2, lesY2, lePlateau)) {
			*direction = GAUCHE; // Essayer d'aller à gauche
		} else if (!estCollisionPotentielle(lesX2[0] + 1, lesY2[0], lesX, lesY, lesX2, lesY2, lePlateau)) {
			*direction = DROITE; // Essayer d'aller à droite
		} else if (!estCollisionPotentielle(lesX2[0], lesY2[0] - 1, lesX, lesY, lesX2, lesY2, lePlateau)) {
			*direction = HAUT; // Essayer d'aller en haut
		} else if (!estCollisionPotentielle(lesX2[0], lesY2[0] + 1, lesX, lesY, lesX2, lesY2, lePlateau)) {
			*direction = BAS; // Essayer d'aller en bas
		}
	}

	if (*direction == HAUT || *direction == BAS){
		// Détermination automatique de la direction
		if ((*direction == HAUT && estCollisionPotentielle(lesX[0], lesY[0] - 1, lesX, lesY, lesX2, lesY2, lePlateau))){
			if (xPositionCible <= lesX[0] && !estCollisionPotentielle(lesX[0] - 1, lesY[0], lesX, lesY, lesX2, lesY2, lePlateau)){
				*direction = GAUCHE;
			}
			else{
				*direction = DROITE;
			}
		}
		else if ((*direction == BAS && estCollisionPotentielle(lesX[0], lesY[0] + 1, lesX, lesY, lesX2, lesY2, lePlateau))){
			if (xPositionCible <= lesX[0] && !estCollisionPotentielle(lesX[0] - 1, lesY[0], lesX, lesY, lesX2, lesY2, lePlateau)){
				*direction = GAUCHE;
			}
			else{
				*direction = DROITE;
			}
		}
		// Sinon si la pomme est plus à gauche que la tête du serpent on choisi de se diriger vers la gauche
		else if (xPositionCible < lesX[0]){
			*direction = GAUCHE;
		}
		// Sinon si la pomme est plus à droite que la tête du serpent on choisi de se diriger vers la droite
		else if (xPositionCible > lesX[0]){ 
			*direction = DROITE;
		}
		// Si la pomme est plus haute que la tête du serpent on choisi de se diriger vers le haut
		else if (yPositionCible < lesY[0]){
			*direction = HAUT;
		}
		// Sinon si la pomme est plus basse que la tête du serpent on choisi de se diriger vers le bas
		else if (yPositionCible > lesY[0]){
			*direction = BAS;
		}
	}
	else{
		// Détermination automatique de la direction
		if ((*direction == GAUCHE && estCollisionPotentielle(lesX[0] - 1, lesY[0], lesX, lesY, lesX2, lesY2, lePlateau))){
			if (yPositionCible <= lesY[0] && !estCollisionPotentielle(lesX[0], lesY[0] - 1, lesX, lesY, lesX2, lesY2, lePlateau)){
				*direction = HAUT;
			}
			else{
				*direction = BAS;
			}
		}
		else if ((*direction == DROITE && estCollisionPotentielle(lesX[0] + 1, lesY[0], lesX, lesY, lesX2, lesY2, lePlateau))){
			if (yPositionCible <= lesY[0] && !estCollisionPotentielle(lesX[0], lesY[0] - 1, lesX, lesY, lesX2, lesY2, lePlateau)){
				*direction = HAUT;
			}
			else{
				*direction = BAS;
			}
		}
		// Si la pomme est plus haute que la tête du serpent on choisi de se diriger vers le haut
		else if (yPositionCible < lesY[0]){
			*direction = HAUT;
		}
		// Sinon si la pomme est plus basse que la tête du serpent on choisi de se diriger vers le bas
		else if (yPositionCible > lesY[0]){
			*direction = BAS;
		}
		// Sinon si la pomme est plus à gauche que la tête du serpent on choisi de se diriger vers la gauche
		else if (xPositionCible < lesX[0]){
			*direction = GAUCHE;
		}
		// Sinon si la pomme est plus à droite que la tête du serpent on choisi de se diriger vers la droite
		else if (xPositionCible > lesX[0]){ 
			*direction = DROITE;
		}
	}



	// Si la direction qu'on choisi est la gauche, et que cette direction entraîne une collision
	if (*direction == GAUCHE && estCollisionPotentielle(lesX[0] - 1, lesY[0], lesX, lesY, lesX2, lesY2, lePlateau)){
		// Alors, si la la case au-dessus de la position de la tête du serpent est libre, on va vers le haut
		if (!estCollisionPotentielle(lesX[0], lesY[0] - 1, lesX, lesY, lesX2, lesY2, lePlateau)){
			*direction = HAUT;
		}
		// Sinon, si la la case au-dessous de la position de la tête du serpent est libre, on va vers le bas
		else if (!estCollisionPotentielle(lesX[0], lesY[0] + 1, lesX, lesY, lesX2, lesY2, lePlateau)){
			*direction = BAS;
		}
		// Sinon, si la la case à droite de la position de la tête du serpent est libre, on va vers la droite
		else if (!estCollisionPotentielle(lesX[0] + 1, lesY[0], lesX, lesY, lesX2, lesY2, lePlateau)){
			*direction = DROITE;
		}
	}
	// Sinon si la direction qu'on choisi est la droite, et que cette direction entraine une collision 
	else if (*direction == DROITE && estCollisionPotentielle(lesX[0] + 1, lesY[0], lesX, lesY, lesX2, lesY2, lePlateau)){
		// Alors, si la base au-dessus de la position de la tête du serpent est libre, on va vers le haut
		if (!estCollisionPotentielle(lesX[0], lesY[0] - 1, lesX, lesY, lesX2, lesY2, lePlateau)){
			*direction = HAUT;
		}
		// Sinon si la case en-dessous de la position de la tête du serpent est libre, on va vers le bas
		else if (!estCollisionPotentielle(lesX[0], lesY[0] + 1, lesX, lesY, lesX2, lesY2, lePlateau)){
			*direction = BAS;
		}
		// Sinon si la case à gauche de la position de la tête du serpent est libre, on va vers la gauche
		else if (!estCollisionPotentielle(lesX[0] - 1, lesY[0], lesX, lesY, lesX2, lesY2, lePlateau)){
			*direction = GAUCHE;
		}
	}
	// Si la direction qu'on choisi est le haut, et que cette direction entraîne une collision
	else if (*direction == HAUT && estCollisionPotentielle(lesX[0], lesY[0] - 1, lesX, lesY, lesX2, lesY2, lePlateau)){
		// Si la case à gauche de la position de la tête du serpent est libre, on va vers la gauche
		if (!estCollisionPotentielle(lesX[0] - 1, lesY[0], lesX, lesY, lesX2, lesY2, lePlateau)){
			*direction = GAUCHE;
		}
		// Si la case à droite de la position de la tête du serpent est libre, on va vers la droite
		else if (!estCollisionPotentielle(lesX[0] + 1, lesY[0], lesX, lesY, lesX2, lesY2, lePlateau)){
			*direction = DROITE;
		}
		// Sinon si la case en-dessous de la position de la tête du serpent est libre, on va vers le bas
		else if (!estCollisionPotentielle(lesX[0], lesY[0] + 1, lesX, lesY, lesX2, lesY2, lePlateau)){
			*direction = BAS;
		}
	}
	// Sinon si la direction qu'on choisi est le bas, et que cette direction entraine une collision 
	else if (*direction == BAS && estCollisionPotentielle(lesX[0], lesY[0] + 1, lesX, lesY, lesX2, lesY2, lePlateau)){
		// Si la case à gauche de la position de la tête du serpent est libre, on va vers la gauche
		if (!estCollisionPotentielle(lesX[0] - 1, lesY[0], lesX, lesY, lesX2, lesY2, lePlateau)){
			*direction = GAUCHE;
		}
		// Si  la case à droite de la position de la tête du serpent est libre, on va vers la droite
		else if (!estCollisionPotentielle(lesX[0] + 1, lesY[0], lesX, lesY, lesX2, lesY2, lePlateau)){
			*direction = DROITE;
		}
		// Si la case au-dessus de la position de la tête du serpent est libre, on va vers le haut
		else if (!estCollisionPotentielle(lesX[0], lesY[0] - 1, lesX, lesY, lesX2, lesY2, lePlateau)){
			*direction = HAUT;
		}
	}

	// Si la tête du serpent se situe sur l'issue du haut, alors on se dirige vers le bas
	if (lesY[0] == 1 && !estCollisionPotentielle(lesX[0], lesY[0] + 1, lesX, lesY, lesX2, lesY2, lePlateau)){
		*direction = BAS;
	}
	// Si la tête du serpent se situe sur l'issu du bas, alors on se dirige vers le haut
	else if (lesY[0] == HAUTEUR_PLATEAU && !estCollisionPotentielle(lesX[0], lesY[0] - 1, lesX, lesY, lesX2, lesY2, lePlateau)){
		*direction = HAUT;
	}
	// Si la tête du serpent se situe sur l'issue de gauche, alors on se dirige vers la droite
	else if (lesX[0] == 1 && !estCollisionPotentielle(lesX[0] + 1, lesY[0], lesX, lesY, lesX2, lesY2, lePlateau)){
		*direction = DROITE;
	}
	// Si la tête du serpent est sur l'issue de droite, alors on se dirige vers la gauche
	else if (lesX[0] == LARGEUR_PLATEAU && !estCollisionPotentielle(lesX[0] - 1, lesY[0], lesX, lesY, lesX2, lesY2, lePlateau)){
		*direction = GAUCHE;
	}
}


// Initialise le plateau de jeu
void initPlateau(tPlateau plateau){
	// initialisation du plateau avec des espaces
	for (int i = 1 ; i <= LARGEUR_PLATEAU ; i++){
		for (int j = 1 ; j <= HAUTEUR_PLATEAU ; j++){
			plateau[i][j] = VIDE;
		}
	}

	// Mise en place la bordure autour du plateau
	// première ligne
	for (int i = 1 ; i <= LARGEUR_PLATEAU ; i++){
		plateau[i][1] = BORDURE;
	}

	// lignes intermédiaires
	for (int j = 1 ; j <= HAUTEUR_PLATEAU ; j++){
		plateau[1][j] = BORDURE;
		plateau[LARGEUR_PLATEAU][j] = BORDURE;
	}

	// dernière ligne
	for (int i = 1 ; i <= LARGEUR_PLATEAU ; i++){
		plateau[i][HAUTEUR_PLATEAU] = BORDURE;
	}

    // mise en place des pavés
	for (int k = 0; k < NB_PAVES; k++){
		for (int i = 0; i < TAILLE_PAVE; i++){ // Parcours les lignes du pavé
			for (int j = 0; j < TAILLE_PAVE; j++){ // Parcours les colonnes du pavé
				plateau[LES_PAVES_X[k] + i][LES_PAVES_Y[k] + j] = BORDURE; // Place le caractère de bordure dans chaque case
			}
		}
	}

	// perçage du plateau au milieu des 4 côtés
	plateau[X_ISSUE_GAUCHE][Y_ISSUES_GAUCHE_DROITE] = VIDE;
	plateau[X_ISSUE_DROITE][Y_ISSUES_GAUCHE_DROITE] = VIDE;
	plateau[X_ISSUES_HAUT_BAS][Y_ISSUE_HAUT] = VIDE;
	plateau[X_ISSUES_HAUT_BAS][Y_ISSUE_BAS] = VIDE;
}

// Dessine le plateau de jeu
void dessinerPlateau(tPlateau plateau){
	// affiche à l'écran le contenu du tableau 2D représentant le plateau
	for (int i = 1 ; i <= LARGEUR_PLATEAU ; i++){
		for (int j = 1 ; j <= HAUTEUR_PLATEAU ; j++){
			afficher(i, j, plateau[i][j]);
		}
	}
}

// Ajoute une pomme au plateau et l'affiche
void ajouterPomme(tPlateau plateau){
	// affiche une pomme aux coordonées fournies par les tableaux LES_POMMES_X et LES_POMMES_Y
	plateau[LES_POMMES_X[nbPommesMangees1 + nbPommesMangees2]][LES_POMMES_Y[nbPommesMangees1 + nbPommesMangees2]] = POMME;
	afficher(LES_POMMES_X[nbPommesMangees1 + nbPommesMangees2], LES_POMMES_Y[nbPommesMangees1 + nbPommesMangees2], POMME);
}

// Affiche un caractère à une position donnée
void afficher(int x, int y, char car){
	gotoxy(x, y);
	printf("%c", car);
	gotoxy(1, 1);
}

// Efface un caractère à une position donnée (en le remplaçant par un espace)
void effacer(int x, int y){
	gotoxy(x, y);
	printf(" ");
	gotoxy(1, 1);
}

// Dessine le serpent sur le plateau
void dessinerSerpent1(int lesX[], int lesY[]){
	// affiche les anneaux puis la tête
	for(int i = 1 ; i < TAILLE ; i++){
		afficher(lesX[i], lesY[i], CORPS);
	}

    // affiche la tête du serpent
	afficher(lesX[0], lesY[0], TETE1);
}

void dessinerSerpent2(int lesX[], int lesY[]){
	// affiche les anneaux puis la tête
	for(int i = 1 ; i < TAILLE ; i++){
		afficher(lesX[i], lesY[i], CORPS);
	}

    // affiche la tête du serpent
	afficher(lesX[0], lesY[0], TETE2);
}

// Fait progresser le serpent dans la direction donnée
void progresser1(int lesX[], int lesY[], int lesX2[], int lesY2[], char direction, tPlateau plateau, bool * collision, bool * pommeMangee){
	// efface le dernier élément avant d'actualiser la position de tous les 
	// éléments du serpent avant de le  redessiner et détecte une
	// collision avec une pomme ou avec une bordure

    // Effacer le dernier élément du serpent
	effacer(lesX[TAILLE - 1], lesY[TAILLE - 1]);

    // déplacer les anneaux du serpent
	for(int i = TAILLE - 1 ; i > 0 ; i--){
		lesX[i] = lesX[i - 1];
		lesY[i] = lesY[i - 1];
	}

	// faire progresser la tete dans la nouvelle direction
	switch(direction){
		case HAUT : 
			lesY[0] = lesY[0] - 1;
			break;
		case BAS:
			lesY[0] = lesY[0] + 1;
			break;
		case DROITE:
			lesX[0] = lesX[0] + 1;
			break;
		case GAUCHE:
			lesX[0] = lesX[0] - 1;
			break;
	}
	
	// Le serpent rentre dans les portes
	if (lesX[0] < 1){ // Entre dans la porte de GAUCHE
        lesX[0] = LARGEUR_PLATEAU; // Sort par la porte de DROITE
    }
	if (lesX[0] > LARGEUR_PLATEAU){ // Entre dans la porte de DROITE
        lesX[0] = 1; // Sort par la porte de GAUCHE
    }
	if (lesY[0] < 1){ // Entre dans la porte du HAUT
        lesY[0] = HAUTEUR_PLATEAU; // Sort par la porte du BAS
    }
	if (lesY[0] > HAUTEUR_PLATEAU){ // Entre dans la porte du BAS
        lesY[0] = 1; // SOrt par la porte du HAUT
    }

    // remise du booléen qui indique la collision avec une pomme
	*pommeMangee = false;

    // détection d'une collision avec une autre tete du serpent
    for(int i = 1 ; i < TAILLE ; i++){
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]){
            *collision = true;
        }
		else if (lesX[0] == lesX2[i] && lesY[0] == lesY2[i]){
            *collision = true;
        }
    }
	
	// détection d'une "collision" avec une pomme
	if (plateau[lesX[0]][lesY[0]] == POMME){
		*pommeMangee = true;
		// la pomme disparait du plateau
		plateau[lesX[0]][lesY[0]] = VIDE;
	}

	// détection d'une collision avec la bordure
	else if (plateau[lesX[0]][lesY[0]] == BORDURE){
		*collision = true;
	}

	if (lesX[0] == lesX2[0] && lesY[0] == lesY2[0]) {
		// Le deuxième serpent détecte qu'il va entrer en collision avec le premier
		// Dévier pour éviter le serpent 1
		if (!estCollisionPotentielle(lesX2[0] - 1, lesY2[0], lesX, lesY, lesX2, lesY2, plateau)) {
			lesX[0]--; // Dévie vers la gauche
		} else if (!estCollisionPotentielle(lesX2[0] + 1, lesY2[0], lesX, lesY, lesX2, lesY2, plateau)) {
			lesX[0]++; // Dévie vers la droite
		} else if (!estCollisionPotentielle(lesX2[0], lesY2[0] - 1, lesX, lesY, lesX2, lesY2, plateau)) {
			lesY[0]--; // Dévie vers le haut
		} else if (!estCollisionPotentielle(lesX2[0], lesY2[0] + 1, lesX, lesY, lesX2, lesY2, plateau)) {
			lesY[0]++; // Dévie vers le bas
		}
	}

    // dessine le serpent sur le plateau à sa nouvelle position
   	dessinerSerpent1(lesX, lesY);
}


void progresser2(int lesX[], int lesY[], int lesX2[], int lesY2[], char direction, tPlateau plateau, bool * collision, bool * pommeMangee){
	// efface le dernier élément avant d'actualiser la position de tous les 
	// éléments du serpent avant de le  redessiner et détecte une
	// collision avec une pomme ou avec une bordure

    // Effacer le dernier élément du serpent
	effacer(lesX[TAILLE - 1], lesY[TAILLE - 1]);

    // déplacer les anneaux du serpent
	for(int i = TAILLE - 1 ; i > 0 ; i--){
		lesX[i] = lesX[i - 1];
		lesY[i] = lesY[i - 1];
	}

	// faire progresser la tete dans la nouvelle direction
	switch(direction){
		case HAUT : 
			lesY[0] = lesY[0] - 1;
			break;
		case BAS:
			lesY[0] = lesY[0] + 1;
			break;
		case DROITE:
			lesX[0] = lesX[0] + 1;
			break;
		case GAUCHE:
			lesX[0] = lesX[0] - 1;
			break;
	}
	
	// Le serpent rentre dans les portes
	if (lesX[0] < 1){ // Entre dans la porte de GAUCHE
        lesX[0] = LARGEUR_PLATEAU; // Sort par la porte de DROITE
    }
	if (lesX[0] > LARGEUR_PLATEAU){ // Entre dans la porte de DROITE
        lesX[0] = 1; // Sort par la porte de GAUCHE
    }
	if (lesY[0] < 1){ // Entre dans la porte du HAUT
        lesY[0] = HAUTEUR_PLATEAU; // Sort par la porte du BAS
    }
	if (lesY[0] > HAUTEUR_PLATEAU){ // Entre dans la porte du BAS
        lesY[0] = 1; // SOrt par la porte du HAUT
    }

    // remise du booléen qui indique la collision avec une pomme
	*pommeMangee = false;

    // détection d'une collision avec une autre tete du serpent
    for(int i = 1 ; i < TAILLE ; i++){
        if (lesX[0] == lesX[i] && lesY[0] == lesY[i]){
            *collision = true;
        }
		else if (lesX[0] == lesX2[i] && lesY[0] == lesY2[i]){
            *collision = true;
        }
    }
	
	// détection d'une "collision" avec une pomme
	if (plateau[lesX[0]][lesY[0]] == POMME){
		*pommeMangee = true;
		// la pomme disparait du plateau
		plateau[lesX[0]][lesY[0]] = VIDE;
	}

	// détection d'une collision avec la bordure
	else if (plateau[lesX[0]][lesY[0]] == BORDURE){
		*collision = true;
	}

	if (lesX[0] == lesX2[0] && lesY[0] == lesY2[0]) {
		// Le deuxième serpent détecte qu'il va entrer en collision avec le premier
		// Dévier pour éviter le serpent 1
		if (!estCollisionPotentielle(lesX2[0] - 1, lesY2[0], lesX, lesY, lesX2, lesY2, plateau)) {
			lesX[0]--; // Dévie vers la gauche
		} else if (!estCollisionPotentielle(lesX2[0] + 1, lesY2[0], lesX, lesY, lesX2, lesY2, plateau)) {
			lesX[0]++; // Dévie vers la droite
		} else if (!estCollisionPotentielle(lesX2[0], lesY2[0] - 1, lesX, lesY, lesX2, lesY2, plateau)) {
			lesY[0]--; // Dévie vers le haut
		} else if (!estCollisionPotentielle(lesX2[0], lesY2[0] + 1, lesX, lesY, lesX2, lesY2, plateau)) {
			lesY[0]++; // Dévie vers le bas
		}
	}

    // dessine le serpent sur le plateau à sa nouvelle position
   	dessinerSerpent2(lesX, lesY);
}

// Vérifie si une collision est potentielle avec le serpent à une position donnée
bool estCollisionPotentielle(int x, int y, int lesX[], int lesY[], int lesX2[], int lesY2[], tPlateau plateau){
	bool collision = false;
	// Si il y a une bordure au même endroit que la tête alors une collision est détecté
	if (plateau[x][y] == BORDURE){
		collision = true;
	}
	else if (x == lesX2[0] && y == lesY2[0]){
		collision = true;
	}
	// Sinon, on vérifie que le serpent n'entre pas en collision avec lui-même
    else{
		for (int i = 1; i < TAILLE; i++){
			if ((x == lesX[i] && y == lesY[i]) || (x == lesX2[i] && y == lesY2[i])){
				collision = true;
			}
    	}
	}
    return collision;
}

// Détermine quel chemin est le plus court entre le chemin direct et l'utilisation des issues
void distancePlusCourte(int lesX[], int lesY[], int nbPommes, int *xPosCible, int *yPosCible){
	// Distance entre la tête du serpent et la pomme
	int distanceDirect = abs(LES_POMMES_X[nbPommes] - lesX[0]) + abs(LES_POMMES_Y[nbPommes] - lesY[0]);
	
	// Distance entre la tête du serpent et la porte du haut + distance entre la sortie de la porte du haut (donc porte bas) et la pomme
	int distancePorteHautPuisPomme = (abs(X_ISSUES_HAUT_BAS - lesX[0]) + abs(Y_ISSUE_HAUT - lesY[0])) + (abs(LES_POMMES_X[nbPommes] - X_ISSUES_HAUT_BAS) + abs(LES_POMMES_Y[nbPommes] - Y_ISSUE_BAS)); // distance de la tête du serpent à la porte désigner + distance entre la sortie de la porte et la pomme
	
	// Distance entre la tête du serpent et la porte du bas + distance entre la sortie de la porte du bas (donc porte haut) et la pomme
	int distancePorteBasPuisPomme = (abs(X_ISSUES_HAUT_BAS - lesX[0]) + abs(Y_ISSUE_BAS - lesY[0])) + (abs(LES_POMMES_X[nbPommes] - X_ISSUES_HAUT_BAS) + abs(LES_POMMES_Y[nbPommes] - Y_ISSUE_HAUT));
	
	// Distance entre la tête du serpent et la porte de gauche + distance entre la sortie de la porte de gauche (donc porte droite) et la pomme
	int distancePorteGauchePuisPomme = (abs(X_ISSUE_GAUCHE - lesX[0]) + abs(Y_ISSUES_GAUCHE_DROITE - lesY[0])) + (abs(LES_POMMES_X[nbPommes] - X_ISSUE_DROITE) + abs(LES_POMMES_Y[nbPommes] - Y_ISSUES_GAUCHE_DROITE));
	
	// Distance entre la tête du serpent et la porte de droite + distance entre la sortie de la porte de droite (donc porte gauche) et la pomme
	int distancePorteDroitePuisPomme = (abs(X_ISSUE_DROITE - lesX[0]) + abs(Y_ISSUES_GAUCHE_DROITE - lesY[0])) + (abs(LES_POMMES_X[nbPommes] - X_ISSUE_GAUCHE) + abs(LES_POMMES_Y[nbPommes] - Y_ISSUES_GAUCHE_DROITE));


	// rechecher la distance la plus petite qui donnera les coordonnées de la porte à prendre (si plus avantageux d'en emprunter une) pour arriver jusqu'à la pomme en le moins de mouvement possible
	int mini = distanceDirect;
	if(distancePorteHautPuisPomme < mini){
		mini = distancePorteHautPuisPomme;
		*xPosCible = X_ISSUES_HAUT_BAS;
		*yPosCible = Y_ISSUE_HAUT;
	}
	if(distancePorteBasPuisPomme < mini){
		mini = distancePorteBasPuisPomme;
		*xPosCible = X_ISSUES_HAUT_BAS;
		*yPosCible = Y_ISSUE_BAS;
	}
	if(distancePorteGauchePuisPomme < mini){
		mini = distancePorteGauchePuisPomme;
		*xPosCible = X_ISSUE_GAUCHE;
		*yPosCible = Y_ISSUES_GAUCHE_DROITE;
	}
	if(distancePorteDroitePuisPomme < mini){
		mini = distancePorteDroitePuisPomme;
		*xPosCible = X_ISSUE_DROITE;
		*yPosCible = Y_ISSUES_GAUCHE_DROITE;
	}
}


/************************************************/
/*				 FONCTIONS UTILITAIRES 			*/
/************************************************/
// Déplace le curseur à une position donnée
void gotoxy(int x, int y){ 
    printf("\033[%d;%df", y, x);
}

// Détecte si une touche est pressée
int kbhit(){
	// la fonction retourne :
	// 1 si un caractere est present
	// 0 si pas de caractere présent
	int unCaractere=0;
	struct termios oldt, newt;
	int ch;
	int oldf;

	// mettre le terminal en mode non bloquant
	tcgetattr(STDIN_FILENO, &oldt);
	newt = oldt;
	newt.c_lflag &= ~(ICANON | ECHO);
	tcsetattr(STDIN_FILENO, TCSANOW, &newt);
	oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
	fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
 
	ch = getchar();

	// restaurer le mode du terminal
	tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
	fcntl(STDIN_FILENO, F_SETFL, oldf);
 
	if(ch != EOF){
		ungetc(ch, stdin);
		unCaractere = 1;
	} 
	return unCaractere;
}

// Fonction pour désactiver l'echo
void disable_echo(){
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == - 1){
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Désactiver le flag ECHO
    tty.c_lflag &= ~ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == - 1){
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}

// Fonction pour réactiver l'echo
void enable_echo(){
    struct termios tty;

    // Obtenir les attributs du terminal
    if (tcgetattr(STDIN_FILENO, &tty) == - 1){
        perror("tcgetattr");
        exit(EXIT_FAILURE);
    }

    // Réactiver le flag ECHO
    tty.c_lflag |= ECHO;

    // Appliquer les nouvelles configurations
    if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == - 1){
        perror("tcsetattr");
        exit(EXIT_FAILURE);
    }
}