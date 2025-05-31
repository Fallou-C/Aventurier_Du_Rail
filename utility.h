#ifndef UTILITY_H
#define UTILITY_H

void AfficherObjectif(MoveResult* mresult);
void AfficherEtatPlateau(BoardState* EtatPlateau);
void AfficherAction(MoveData* mData, MoveResult* mresult);

void JouerSolo(int continuer,MoveResult mresult ,BoardState EtatPlateau);

int* RouteGrise(int* InventaireCouleur, int a, int b, int tailleMatrice,Route route);
int* RoutePrenable(int* InventaireCouleur, Route** matriceRoute, int tailleMatrice, int min, int* wagon);

void ClaimeBarre(int option,MoveResult mresult ,BoardState EtatPlateau,MoveData data, int* InventaireCouleur);
void ClaimRoute(int option,int i, int j, Route** matriceRoute, int n,MoveResult mresult ,BoardState EtatPlateau,MoveData data, int* InventaireCouleur, int* wagon);

int distanceMini(int* D, bool* visite, int n);
int* Dijkstra(int src, Route** matrice, int n);

int* ChoixObjectif(int* InventaireObjective, MoveResult mresult ); // renvoie un tableau de 3 booléen répresentant les objectifs choisie
int* CheminCourt(int a,int b, Route** matrice, int n); // renvoie le prochain chemin à prendre pour aller de b à partir de a, renvoie -1 -1 si déjà complet

bool* CourtObjectif(int nbr_objectif, Route** matrice, int taille, int* InventaireObjective,MoveResult mresult, bool mode );

#endif