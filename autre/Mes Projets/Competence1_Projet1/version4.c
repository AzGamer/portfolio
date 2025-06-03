#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <stdbool.h>
#include <time.h>

///////////////////////////////
// Définition des constantes //
///////////////////////////////

// Taille du plateau
#define LARGEUR 81       // Largeur du plateau de jeu
#define HAUTEUR 41       // Hauteur du plateau de jeu

// Composition du serpent
const char HEAD='O';
const char BODY='X';
const char FIN='a';

// direction des touches
const char HAUT='z';
const char BAS='s';
const char GAUCHE='q';
const char DROITE='d';

// constante à propos du cadre du jeu 80*40
const int TAILLE_PAVE=5;
const int NB_PAVER=4;
const char BORD='#';
const char VIDE=' ';

// constantes à propos des pommes
const int NB_POMME_FIN=10;
int NB_POMME=5;
const char POMME='6';

// constante à propos du serepnt
const int TAILLE_INITIAL=10;
int TAILLE=TAILLE_INITIAL + NB_POMME_FIN;        // Taille du serpent
int VITESSE=100000; //0.1 seconde
const int xInitial=40;
const int yInitial=20;


////////////////////////////////////////////////
// Initialisation des procédures et fonctions //
////////////////////////////////////////////////

// Affiche un caractère c à la position (x,y), tout 3 passer en paramètres
void afficher(int x,int y,char c);

// Efface le caractère à la position (x,y)
void effacer(int x,int y);

// Dessine le serpent à partir des positions (lesX[], lesY[])
void dessinerSerpent(int lesX[], int lesY[]);

// Déplace le serpent d'une case dans la direction donnée
void progresser(int lesX[], int lesY[], char direction, bool * collision, char cadre[LARGEUR][HAUTEUR], bool * mange, int taille);



//Initialisation du tableau avec un '#' qui correspond aux bordure et ' ' Pour l'intérieur
void initPlateau(char cadre[LARGEUR][HAUTEUR], int lesX[], int lesY[],int taille);

//Dessine le plateau du plateau
void dessinerPlateau(char cadre[LARGEUR][HAUTEUR]);

//Dessine dans le cadre un paver de 5*5 
void paver(char cadre[LARGEUR][HAUTEUR],int x,int y);

//Vérifie si la position (x,y) est valide pour placer un pavers
bool positionValidePourPaver(int x, int y, int lesX[], int lesY[]);

//AJoute une pomme au jeux
void ajouterPomme(char cadre[LARGEUR][HAUTEUR], int lesX[],int lesY[], int taille);

// Active l'affichage de caractère saisi dans le terminal
void enableEcho();

// Désactive l'affichage de caractère saisi dans le terminal
void disableEcho();

//Permet de positionner le curseur à une position x et y
void gotoXY(int x, int y);

// Retourne 1 si une touche est pressée et 0 si non
int kbhit();


////////////////////////////////////////////////
// Définition des procédures et des fonctions //
////////////////////////////////////////////////

int main() {
   // Définition des constantes
   int lesX[TAILLE];
   int lesY[TAILLE];
   int taille = TAILLE_INITIAL;
   int score=0;
   bool collision = false;
   bool mange = false;
   char cadre[LARGEUR][HAUTEUR];
   char touche=' ';
   char direction = DROITE;
   //int nbPommeManger = 0;


   //Initialisation des positions du serpent
   
   //On place la tête
   lesX[0] = xInitial;
   lesY[0] = yInitial;
   //On place les TAILLE-1 parties du corps derrière la tête
   for (int i = 0; i < taille; i++) {
      lesX[i] = xInitial - i;
      lesY[i] = yInitial;
   }


   //On vide le contenu du terminal
   //Désactive l'affichage systématique de caractère(s) dans le terminal
   //Dessine le serpent à la position initiale (ici x et y = 20)
   system("clear");
   disableEcho();
   dessinerSerpent(lesX, lesY);

   srand(time(NULL));

   initPlateau(cadre,lesX, lesY,taille);
   dessinerPlateau(cadre);

   //Tant que la touche saisie n'est pas celle de fin définie en constante
   //on fait avancer le serpent de 1 emplacement dans le terminal dans la direction passée en paramètre

   while (touche != FIN) {
      //vérifie si y'a une collision si oui fin du jeu
      progresser(lesX, lesY, direction, &collision, cadre, &mange, taille);
      if(collision){
         //system("clear");
         enableEcho();
         gotoXY(0,HAUTEUR+1);
         printf("Vous avez perdu!\n");
         return EXIT_FAILURE;
      }


      // si une pomme est manger on monte le score la vitesse et la taille du serpent
      // si le score atteint le nombre définit en constante alors le jeu est fini
      //on ajoute un pomme si le score n'a pas atteint le MAX
      if(mange){
         score++;
         VITESSE=VITESSE*0.9;
         mange=false;
         taille++;
         if(score>=NB_POMME_FIN){
            enableEcho();
            gotoXY(0,HAUTEUR+1);
            printf("Vous avez gagné!\n");
            return EXIT_SUCCESS;
         }
         ajouterPomme(cadre,lesX,lesY,taille);

      }
      
      
      
      usleep(VITESSE); //Met un temps d'attente entre plusieurs exécutions
      if (kbhit()) {
         touche = getchar(); //si une touche est pressée on la récupère dans la variable touche et on modifie la variable direction en conséquence
         if(touche==HAUT && direction!=BAS){
            direction=HAUT;
         } else if(touche==BAS && direction!=HAUT){
            direction=BAS;
         } else if(touche==GAUCHE && direction!=DROITE){
            direction=GAUCHE;
         } else if(touche==DROITE && direction!=GAUCHE){
            direction=DROITE;
         }
      }
   }


   //On active à nouveau l'affichage de caractère dans le terminal
   enableEcho();
   printf("\n");
   gotoXY(0,HAUTEUR+1);
   return EXIT_SUCCESS;
}




void afficher(int x,int y,char c){
   //On se déplace à la position x et y et affichons le caractère passé en paramètre
   gotoXY(x,y);
   printf("%c",c);
   gotoXY(1,1);
}

void effacer(int x,int y){
   // On se déplace à la position x et y et on affiche un espace pour remplacer le caractère présent
   gotoXY(x,y);
   printf(" ");
   gotoXY(1,1);
}

void dessinerSerpent(int lesX[], int lesY[]){
   // Dessine la tête du serpent
   //on affiche (taille du serpent -1) fois une partie du corps
   for (int indice=1;indice<TAILLE;indice++){
      afficher(lesX[indice],lesY[indice],BODY);
   }
   afficher(lesX[0],lesY[0],HEAD);
}

void progresser(int lesX[], int lesY[], char direction, bool * collision, char cadre[LARGEUR][HAUTEUR], bool * mange, int taille){
   effacer(lesX[taille-1], lesY[taille-1]); // Efface le dernier segment

   // Faire suivre chaque segment le segment précédent
   for (int i = taille - 1; i > 0; i--) {
      lesX[i] = lesX[i - 1];
      lesY[i] = lesY[i - 1];
   }

   // Déplacer la tête dans la direction choisie
   if (direction == HAUT) {
      lesY[0] -= 1; // Déplacer vers le haut
   } else if (direction == BAS) {
      lesY[0] += 1; // Déplacer vers le bas
   } else if (direction == GAUCHE) {
      lesX[0] -= 1; // Déplacer vers la gauche
   } else if (direction == DROITE) {
      lesX[0] += 1; // Déplacer vers la droite
   }
   
   //si le serpent sort il réaparait de l'autre côté

   if(lesX[0]<0 && lesY[0]>0){ // sortie gauche entrée droite
      lesX[0] = LARGEUR;
      lesY[0] = HAUTEUR/2;
   }else if(lesX[0]>0 && lesY[0]<0){ //sortie haut entrée bas
      lesX[0]=LARGEUR/2;
      lesY[0]=HAUTEUR;
   }else if(lesX[0]<LARGEUR && lesY[0]>HAUTEUR){ //sortie bas entrée haut
      lesX[0]=LARGEUR/2;
      lesY[0]=0;
   }else if(lesX[0]>=LARGEUR && lesY[0]<=HAUTEUR){ //sortie droite entrée gauche
      lesX[0] = 0;
      lesY[0] = HAUTEUR/2;

   }

   ///////////////////////////////////////
   // Gestion des collisions du serpent //
   ///////////////////////////////////////
   
   
   //Collision du serpent sur lui même
   for (int i = 1; i < taille; i++) {
      if (lesX[0] == lesX[i] && lesY[0] == lesY[i]){
         *collision = true;
      }
   }

   //Collision de la tête du serpent avec un bordXXXXXXXXXO
   if (cadre[lesX[0]][lesY[0]] == BORD) {
      *collision = true;
   }

   //COllision entre le serpent et une pomme

   if (cadre[lesX[0]][lesY[0]] == POMME){
      *mange = true;
      cadre[lesX[0]][lesY[0]]=VIDE;
   }
   
   dessinerSerpent(lesX, lesY); // Affiche le serpent mis à jour


}





// Initialisation d'un tableau de taille largeur et hauteur en constante et dont les bord sont des #
void initPlateau(char cadre[LARGEUR][HAUTEUR], int lesX[], int lesY[],int taille){
   int nbPaver=0;

   
   // Remplir les bordures avec des '#'
   for (int i=0;i<LARGEUR;i++) {
      cadre[i][1]=BORD;                 // Bordure supérieure
      cadre[i][HAUTEUR-1]=BORD;       // Bordure inférieure
   }
   for (int j=0;j<HAUTEUR;j++) {
      cadre[1][j]=BORD;                 // Bordure gauche
      cadre[LARGEUR-1][j]=BORD;       // Bordure droite
   }

   //Troue définie dans le milieu de chacune des bordures avec un espace VIDE
   cadre[LARGEUR/2][1]=VIDE;
   cadre[LARGEUR/2][HAUTEUR-1]=VIDE;
   cadre[1][HAUTEUR/2]=VIDE;
   cadre[LARGEUR-1][HAUTEUR/2]=VIDE;


   // Remplir l'intérieur avec des espaces
   for (int i=2;i<LARGEUR-1;i++) {
      for (int j=2;j<HAUTEUR-1;j++) {
         cadre[i][j]=VIDE;
      }
   }

   while (nbPaver < NB_PAVER) {
      int x = rand() % (LARGEUR - TAILLE_PAVE - 4) + 3; 
      int y = rand() % (HAUTEUR - TAILLE_PAVE - 4) + 3; 

      // Vérifie que le pavé ne se superpose pas au serpent
      if (positionValidePourPaver(x, y, lesX, lesY)) {
         paver(cadre, x, y);
         nbPaver++;
      }
   }
   for(int i=0; i<NB_POMME;i++){
      ajouterPomme(cadre,lesX,lesY,taille);
   }
}
// Affiche le plateau de jeu dans le terminal en parcourant chaque case.
// Pour chaque position (x, y), affiche le caractère correspondant du cadre.
void dessinerPlateau(char cadre[LARGEUR][HAUTEUR]) {
   for (int x = 0; x < LARGEUR; x++) { 
      for (int y = 0; y < HAUTEUR; y++) {
         afficher(x, y, cadre[x][y]); // Affiche le caractère du cadre à la position (x, y)
      }
      printf("\n"); // Nouvelle ligne après chaque ligne de largeur complète
   }
}
// Dessine un pavé de 5x5 dans le cadre à partir de la position (x, y).
// Utilise BORD pour remplir chaque case du pavé.
void paver(char cadre[LARGEUR][HAUTEUR], int x, int y) {
   for (int i = 0; i < TAILLE_PAVE; i++) {         // Parcours les lignes du pavé
      for (int j = 0; j < TAILLE_PAVE; j++) {      // Parcours les colonnes du pavé
         cadre[x + i][y + j] = BORD;               // Place le caractère de bordure dans chaque case
      }
   }
}

bool positionValidePourPaver(int x, int y, int lesX[], int lesY[]) {
   // Parcours chaque segment du serpent pour vérifier s'il se trouve dans la zone du pavé 5x5
   for (int i = 0; i < TAILLE; i++) { 
      // Vérifie si les coordonnées du serpent `(lesX[i], lesY[i])`
      // se trouvent dans la zone définie par `(x, y)` comme coin supérieur gauche
      if (lesX[i] >= x && lesX[i] < x + TAILLE_PAVE && lesY[i] >= y && lesY[i] < y + TAILLE_PAVE) {
         return false; // Conflit détecté, le pavé se superpose au serpent
      }
   }
   return true; // Aucun conflit détecté
}

void ajouterPomme(char cadre[LARGEUR][HAUTEUR], int lesX[], int lesY[], int taille) {
   int xPomme = 0; // Coordonnée X de la pomme
   int yPomme = 0; // Coordonnée Y de la pomme
   bool posValide = false; // Indicateur de validité de la position

   // Répéter jusqu'à trouver une position valide pour la pomme
   while (cadre[xPomme][yPomme] != VIDE || posValide == true) {
      posValide = false; // Réinitialiser la validité de la position

      // Générer des coordonnées aléatoires dans les limites autorisées
      xPomme = rand() % (LARGEUR - 3) + 2;
      yPomme = rand() % (HAUTEUR - 3) + 2;

      // Vérifier si la position générée correspond à une des positions interdites
      for (int i = 0; i < taille; i++) {
         if (lesX[i] == xPomme && lesY[i] == yPomme) {
            posValide = true; // La position n'est pas valide
         }
      }
   }

   // Placer la pomme dans la grille
   cadre[xPomme][yPomme] = POMME;

   // Afficher la pomme sur la grille
   afficher(xPomme, yPomme, POMME);
}
   




//Permet de positionner le curseur à une position x et y
void gotoXY(int x, int y) {
   printf("\033[%d;%df", y, x);
}
// La fonction teste si une touche est frappée au clavier (sans bloquer l'exécution !=scanf()) et renvoie 1 si oui et 0 si non
int kbhit(){
   // La fonction retourne :
   // 1 si un caractère est présent
   // 0 si aucun caractère n'est présent
   
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
      unCaractere=1;
   }
   return unCaractere;
}
// Désactive l'affichage de caractère saisi dans le terminal
void disableEcho() {
   struct termios tty;

   // Obtenir les attributs du terminal
   if (tcgetattr(STDIN_FILENO, &tty) == -1) {
      perror("tcgetattr");
      exit(EXIT_FAILURE);
   }

   // Désactiver le flag ECHO
   tty.c_lflag &= ~ECHO;

   // Appliquer les nouvelles configurations
   if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
      perror("tcsetattr");
      exit(EXIT_FAILURE);
   }
}
// Active l'affichage de caractère saisi dans le terminal
void enableEcho() {

   struct termios tty;

   // Obtenir les attributs du terminal
   if (tcgetattr(STDIN_FILENO, &tty) == -1) {
      perror("tcgetattr");
      exit(EXIT_FAILURE);
   }

   // Réactiver le flag ECHO
   tty.c_lflag |= ECHO;

   // Appliquer les nouvelles configurations
   if (tcsetattr(STDIN_FILENO, TCSANOW, &tty) == -1) {
      perror("tcsetattr");
      exit(EXIT_FAILURE);
   }
}
