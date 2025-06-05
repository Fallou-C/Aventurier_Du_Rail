#include <stdlib.h>
#include <stdio.h>
#include "../tickettorideapi/ticketToRide.h"
#include "../tickettorideapi/clientAPI.h"
#include "matrice.h"
#include "bot.h"
#include "utility.h"

void ClaimeurFou(int firstturn ,MoveResult mresult ,BoardState EtatPlateau, int* InventaireCouleur, Route** matrice,int taille, MoveData data,int min, int* wagon) // il claim si possible
{
	if(firstturn) //premier tour on prend des objectives
	{
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
		if ( routeClaimable[4] )
		{
			data.action = 1;
			data.claimRoute.from = routeClaimable[0];
			data.claimRoute.to = routeClaimable[1];
			data.claimRoute.color = routeClaimable[2];
			data.claimRoute.nbLocomotives = routeClaimable[5]; // nombre de joker qu'on joue
			sendMove(&data, &mresult);
		}
		else // sinon on pioche
		{
			data.action = 2;
			sendMove(&data, &mresult);
			InventaireCouleur[mresult.card] += 1;
			sendMove(&data, &mresult);
			InventaireCouleur[mresult.card] += 1;
		}
	}
}


void ClaimCourtChemin(int firstturn ,MoveResult mresult ,BoardState EtatPlateau, int* InventaireCouleur,int* InventaireObjective, Route** matrice,int taille, MoveData data, int* wagon,int min,int* objectif_actuelle,int adv_wagon)
{
	if(firstturn) //premier tour on prend des objectives
	{
		printf("on joue premier tour \n");
		data.action = 4;
		sendMove(&data, &mresult);
		data.action =  5;
		bool* obj = CourtObjectif(2,  matrice,  taille,  InventaireObjective, mresult, true ,*objectif_actuelle);
		
		data.chooseObjectives[0] = obj[0];
		data.chooseObjectives[1] = obj[1];
		data.chooseObjectives[2] = obj[2];
		sendMove(&data,&mresult);
	}
	else	//attention cas fini objectif dans la liste + cas objectifs inclue l'un dans l'autre
	{
		int a,b;
		a = InventaireObjective[*objectif_actuelle * 2];
		b = InventaireObjective[*objectif_actuelle * 2 + 1];
		int* prochain_claim = CheminCourt(a,b,matrice,taille);
		if ( prochain_claim[0] == -1 && a!=-1)
		{
			*objectif_actuelle +=1;
			a = InventaireObjective[*objectif_actuelle * 2];
			b = InventaireObjective[*objectif_actuelle * 2 + 1];
			if (a != -1){
				prochain_claim = CheminCourt(a,b,matrice,taille);}
		}
		if (prochain_claim[0] != -1){ //on regarde si l'objetifs à été fait
			ClaimRoute(3,prochain_claim[0],prochain_claim[1],matrice, taille,mresult ,EtatPlateau,data,InventaireCouleur,wagon);
			}
		else if ((adv_wagon > 20) && (*wagon > 10) && (a ==-1)){ // penser à regazrder les wagons de l'adversaire
			AjoutObjectif(taille, matrice,InventaireObjective, mresult, data,*wagon,objectif_actuelle);
		}
		else{
			ClaimeurFou(firstturn, mresult ,EtatPlateau,InventaireCouleur,matrice,taille,data,min,wagon);}
		free(prochain_claim);
	}	
}
