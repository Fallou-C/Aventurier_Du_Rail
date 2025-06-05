#include "matrice.h"
#include "../tickettorideapi/ticketToRide.h"
#include "../tickettorideapi/clientAPI.h"

#ifndef BOT_H
#define BOT_H

void ClaimeurFou(int firstturn ,MoveResult mresult ,BoardState EtatPlateau, int* InventaireCouleur, Route** matrice,int taille, MoveData data,int min, int* wagon); // il claim si possible
void ClaimCourtChemin(int firstturn ,MoveResult mresult ,BoardState EtatPlateau, int* InventaireCouleur,int* InventaireObjective, Route** matrice,int taille, MoveData data, int* wagon,int min,int* objectif_actuelle,int adv_wagon);


#endif