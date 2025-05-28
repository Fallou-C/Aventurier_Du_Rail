#ifndef MATRICE_H
#define MATRICE_H

typedef struct _Route{
	int Couleur1;
	int Couleur2;
	int Nbr_Wagon;
} Route;

Route** AllouerMatrice(int n); // matrice carré de pointeur de route de taille n
void DetruireMatrice(Route** matrice,int n); // on a une matrice de pointeur de route
void MatriceAdjacence(Route** matrice,int NbrCity,int nbTrack, int* trackData); // création de la matrice d'adjacence
void AfficherMatrice(Route** matrice, int n);

#endif