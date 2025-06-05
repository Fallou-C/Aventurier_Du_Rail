#include <stdlib.h>
#include <stdio.h>
#include "../tickettorideapi/ticketToRide.h"
#include "../tickettorideapi/clientAPI.h"
#include "matrice.h"
#include "bot.h"
#include "utility.h"

extern int DEBUG_LEVEL;

//extern int* objectif_actuelle;

/*
#to do :
-pb si entouré ou chemin impossible

-algo de choix du chemin le plus rentable en terme de point pour les objectifs (choix)
- pioche les cartes pour faire les objectifs
- quand reprendre des objectifs
- dystra et choix du plus petit chemin -> prendre en compte les cartes wagons qu'on a au début et garder les plus court ou plus rentable
- dystra modifié pour choisir le chemin le plus rentable en fonction d'un delta décallage ou calculer les n chemin les plus court et choisir le plus court ou plus rentable
- espèrance pour amliorer le chemin le plus rentable
- attention cas où on a plus de 50 cartes !  (en soit jamais atteint si pas fait de connerie)

- struc parametre pour simplifier les entrées pour les bots ?

#bug : 
-pproblème si ville plus ascessible
	15722936 9616553  server fail (fail plus car le bot à changé)
	15019007 10485065 11899199 ??? -> corrigé !
	seed=15019007 on a un objectif dans un autre dés le début
	4048977 gros lag like the f???

	4220643 on pioche trop 

	seed=10988714 je sais pas pourquoi le score change à chaque fois

*/



/*************** Nouveauu API configuration : ***********************
 * 
 * adress serveur en ligne : http://82.29.170.160:8889/
sendGameSetting( char* , GameData );

char* = "TRAINING NICE_BOT"
	"TRAINING PLAY_RANDOM"
	"TRAINING DO_NOTHING"
seed debug : 15019007 11547494 fini pas un objectif à analyser (peut keblo ou autre)
//seed=9892492
seed=4220643 11004452 8394301 seed=2752013 7577041 7557724 15247008
+  messge de base dans le gamedata (message et op_message) null par défaut 
*/


int main(void)
{	
	extern int DEBUG_LEVEL;
	DEBUG_LEVEL = INTERN_DEBUG;

	int continuer=1;
	int firstturn = 1;
	int InventaireCouleur[10] = {0,0,0,0,0,0,0,0,0,0};
	int InventaireObjective[20];

	GameData Gdata;
	BoardState EtatPlateau;
	DEBUG_LEVEL = MESSAGE;

	connectToCGS("82.29.170.160", 15001, "Nataaaacha");
	sendGameSettings("TRAINING NICE_BOT seed=5199207",&Gdata); //TRAINING NICE_BOT seed=5199207    		
	
	MoveResult mresult;
	MoveData data;
	
	printf("%d\n",Gdata.gameSeed);
	
	int n = Gdata.nbCities;
	Route** matrice = AllouerMatrice(n);
	MatriceAdjacence( matrice, n, Gdata.nbTracks, Gdata.trackData);

	int QuiJoue = Gdata.starter;
	int wagon=45;
	int adv_wagon=45;
	int min=10;
	int objectif_actuelle = 0;
	int nbcard;

	// on initialise les cartes qu'on pioche : 
	for(int i=0; i<4 ;i++)
	{
		InventaireCouleur[Gdata.cards[i]] += 1;
	}
	
	while(continuer)
	{
		getBoardState(&EtatPlateau);
		nbcard = 0;
		for(int i=0;i<10;i++){nbcard += InventaireCouleur[i];}
		if (QuiJoue == 1) // l'adversaire joue
		{
			getMove(&data, &mresult);
			if ( (data.action == 2) || ((data.action == 3) && (data.drawCard != 9)) ||  data.action == 4 )
			{
				getMove(&data, &mresult);
			}
			if (data.action == 1)
			{
				int a = data.claimRoute.from;
				int b = data.claimRoute.to;
				adv_wagon -= matrice[a][b].Nbr_Wagon;
				matrice[a][b].Nbr_Wagon = 2000;
				matrice[b][a].Nbr_Wagon = 2000;
			}
			QuiJoue = 0;
			if (mresult.state == 1 || mresult.state == -1 ){continuer=0;} // on s'arrêté
		}
		
		else if (nbcard > 45) 
		{
			if (mresult.state == 1 || mresult.state == -1 ){sendMove(&data, &mresult);continuer=0;} // on s'arrêté
			ClaimeurFou(firstturn, mresult ,EtatPlateau,InventaireCouleur,matrice,n,data,min,&wagon);
			QuiJoue = 1;
		}
		// prendre des objectifs quand on en a fini où que l'on peut plus faire ce qu'on a
		else
		{
			if (mresult.state == 1 || mresult.state == -1 ){sendMove(&data, &mresult);continuer=0;} // on s'arrêté
			ClaimCourtChemin(firstturn , mresult , EtatPlateau,  InventaireCouleur, InventaireObjective,  matrice, n, data, &wagon,min,&objectif_actuelle,adv_wagon);
			if(min>wagon){min=wagon;}
			QuiJoue = 1;
			firstturn = 0 ;
		}
		
	}
	DetruireMatrice(matrice, n);
	quitGame();
	return 1;
}