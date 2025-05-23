#include <stdlib.h>
#include <stdio.h>
#include "../tickettorideapi/ticketToRide.h"
#include "../tickettorideapi/clientAPI.h"
#include "matrice.h"
#include "bot.h"
#include "utility.h"

int objectif_actuelle = 0;

void ClaimeurFou(int firstturn ,MoveResult mresult ,BoardState EtatPlateau, int* InventaireCouleur, Route** matrice,int taille, MoveData data,int min, int* wagon) // il claim si possible
{
	if(firstturn) //premier tour on prend des objectives
	{
		printf("on joue premier tour \n");
		data.action = 4;
		sendMove(&data, &mresult);
		data.action =  5;
		data.chooseObjectives[0] = 1;
		data.chooseObjectives[1] = 0;
		data.chooseObjectives[2] = 1;
		sendMove(&data,&mresult);
	}
	else
	{
		 //on prend si on peut
		int* routeClaimable = RoutePrenable(InventaireCouleur,matrice,taille,min,wagon); //tableau de 4 entier

/*
		for (int i=0 ; i<4 ; i++)
		{
			printf(" route : %d ", routeClaimable[i]);
		}
		printf("\n");
*/

		if ( routeClaimable[4] )
		{
			printf("on claim \n");
			data.action = 1;
			data.claimRoute.from = routeClaimable[0];
			data.claimRoute.to = routeClaimable[1];
			data.claimRoute.color = routeClaimable[2];
			data.claimRoute.nbLocomotives = routeClaimable[5]; // nombre de joker qu'on joue
			printf("from %d to %d color %d nbloc %d \n",routeClaimable[0],routeClaimable[1],routeClaimable[2],routeClaimable[3]);
			sendMove(&data, &mresult);
		}
		else // sinon on pioche
		{
			printf("on pioche \n");
			data.action = 2;
			sendMove(&data, &mresult);
			InventaireCouleur[mresult.card] += 1;
			sendMove(&data, &mresult);
			InventaireCouleur[mresult.card] += 1;
		}
	}
}


void ClaimCourtChemin(int firstturn ,MoveResult mresult ,BoardState EtatPlateau, int* InventaireCouleur,int* InventaireObjective, Route** matrice,int taille, MoveData data, int* wagon,int min)
{
	if(firstturn) //premier tour on prend des objectives
	{
		printf("on joue premier tour \n");
		data.action = 4;
		sendMove(&data, &mresult);
		data.action =  5;
		data.chooseObjectives[0] = 1;
		data.chooseObjectives[1] = 0;
		data.chooseObjectives[2] = 1;
		AfficherObjectif(&mresult);
		InventaireObjective[0] = mresult.objectives[0].from;
		InventaireObjective[1] = mresult.objectives[0].to;
		InventaireObjective[2] = mresult.objectives[2].from;
		InventaireObjective[3] = mresult.objectives[2].to;
		InventaireObjective[4] = -1;
		// mettre -1 sur le dernier objectif comme arrêt

		sendMove(&data,&mresult);
	}
	else	//attention cas fini objectif dans la liste
	{
		int a,b;
		a = InventaireObjective[objectif_actuelle * 2];
		b = InventaireObjective[objectif_actuelle * 2 + 1];
		int* prochain_claim = CheminCourt(a,b,matrice,taille);
		if ( prochain_claim[0] == -1)
		{	if (a!=-1){
			objectif_actuelle +=1;}
			a = InventaireObjective[objectif_actuelle * 2];
			b = InventaireObjective[objectif_actuelle * 2 + 1];
			prochain_claim = CheminCourt(a,b,matrice,taille);
		}
		printf("à claim :%d %d\n",prochain_claim[0],prochain_claim[1]);
		if (a!=-1){
			ClaimRoute(0,prochain_claim[0],prochain_claim[1],matrice, taille,mresult ,EtatPlateau,data,InventaireCouleur,wagon);
			}
		else{ClaimeurFou(firstturn, mresult ,EtatPlateau,InventaireCouleur,matrice,taille,data,min,wagon);}
		free(prochain_claim);
	}	
}
