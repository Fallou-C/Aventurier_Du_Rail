#include <stdlib.h>
#include <stdio.h>
#include "../tickettorideapi/ticketToRide.h"
#include "../tickettorideapi/clientAPI.h"
#include "matrice.h"
#include "bot.h"
#include "utility.h"

extern int DEBUG_LEVEL;

//extern int* objectif_actuelle;


//faire une structure route-> 0 si prise par nous +inf sinon    + dirac +  matrice de pointeur
//stocker carte de couleur dans un tableau de int + stocker objective dans liste


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

#bug : 
-pproblème si ville plus ascessible

*/



/*************** Nouveauu API configuration : ***********************
 * 
 * adress serveur en ligne : http://82.29.170.160:8889/
sendGameSetting( char* , GameData );

char* = "TRAINING NICE_BOT"
	"TRAINING PLAY_RANDOM"
	"TRAINING DO_NOTHING"
seed debug : 
	probleme de message 10592888
	pioche sans raison 10479304  8104058: 
	core dump : 5199207 check -> pb quand tous étaient fini (chercher d'autre seed du même style enn sah)
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
	//seed=9892492
	BoardState EtatPlateau;
	DEBUG_LEVEL = MESSAGE;

	connectToCGS("82.29.170.160", 15001, "Natachaa");
	sendGameSettings("TRAINING PLAY_RANDOM seed=10592888",&Gdata);
	
	MoveResult mresult;
	MoveData data;

	/*	
	for (int i =0; i< Gdata.nbTracks; i++)
	{
		printf("%d %d %d %d %d\n",Gdata.trackData[i*5],Gdata.trackData[1 + 5*i],Gdata.trackData[2 + i*5],Gdata.trackData[3 + 5*i],Gdata.trackData[4 + 5*i]);
	}
	*/
	//printf("%d",Gdata.nbCities);
	
	printf("%d\n",Gdata.gameSeed);
	
	int n = Gdata.nbCities;
	Route** matrice = AllouerMatrice(n);
	MatriceAdjacence( matrice, n, Gdata.nbTracks, Gdata.trackData);
	//AfficherMatrice(matrice, n);
	//DetruireMatrice(matrice, n);

	int QuiJoue = Gdata.starter;
	int wagon=45;
	int min=4;
	int objectif_actuelle = 0;
	// on initialise les cartes qu'on pioche : 
	for(int i=0; i<4 ;i++)
	{
		InventaireCouleur[Gdata.cards[i]] += 1;
	}
	
	while(continuer)
	{
		printBoard();
		printf("%d joue\n",QuiJoue);
		
		if (QuiJoue == 1) // l'adversaire joue
		{
			printf("adversaire joue \n");
			getMove(&data, &mresult);
			if ( (data.action == 2) || ((data.action == 3) && (data.drawCard != 9)) ||  data.action == 4 )
			{
				getMove(&data, &mresult);
			}
			if (data.action == 1)
			{
				int a = data.claimRoute.from;
				int b = data.claimRoute.to;
				matrice[a][b].Nbr_Wagon = 2000;
				matrice[b][a].Nbr_Wagon = 2000;
				printf("il prend from %d to %d \n",a,b);
			}
			QuiJoue = 0;
		}
		else
		{
			ClaimCourtChemin(firstturn , mresult , EtatPlateau,  InventaireCouleur, InventaireObjective,  matrice, n, data, &wagon,min,objectif_actuelle);
			//ClaimeurFou(firstturn, mresult ,EtatPlateau,InventaireCouleur,matrice,n,data,min,&wagon);
			//for(int i =0;i<10;i++){printf("nombre de carte couleur dispo : couleur %d nombre %d \n",i,InventaireCouleur[i]);}
			//for(int i =0;i<10;i++){printf("objecitfs : de %d à %d \n",InventaireObjective[2*i],InventaireObjective[i*2 +1]);}
			if(min>wagon){min=wagon;} 
			// penser à compter les cartes
			//printf("nbr wagon : %d\n",wagon);
			QuiJoue = 1;
			firstturn = 0 ;
		}
		//printf("qui commence : %d \n",Gdata.starter);
		//JouerSolo(continuer,mresult,EtatPlateau);
	}
	printf("c'est fini\n");
	DetruireMatrice(matrice, n);
	return 1;
}

/*
debug seed 5199207:  check
Les villes :
Kansas City�ur 17 Houston 14 score: 5 
Montréal;`�ur 29 Atlanta 25 score: 9 
Winnipeg ;`�ur 10 Houston 14 score: 12 





✘Objective Sault St. Marie (20) → Nashville (26) : -8 points
	✘Objective Toronto (28) → Miami (35) : -10 points



	int TirerPiocher=1;
	int ComptePioche=0;
	int ChoisirCarte = 1;
//	int draw = 4;
//	int choose = 5;
	MoveData moveData1;
	moveData1.action=4;
	MoveData moveData2 = {5,{0,1,1}};
	MoveData moveData3;
	MoveData moveData4;
	moveData4.action = 2;
if (Gdata.starter!=1)
		{
			printf("ça envoie \n");
			if(TirerPiocher)
			{
				connect = sendMove(&moveData1, &mresult);
				printf("je tire! %d \n",connect);
				printf("le move est : %d \n",mresult.state);
				AfficherObjectif(mresult);
				TirerPiocher--;
				//free(&mresult);
			}
			else
			{
				connect = sendMove(&moveData2, &mresult);
				printf("connect : %d \n",connect);
				printf("on a bien pioché : %d \n",mresult.state);
				//TirerPiocher++;
				if (ComptePioche<5 && ChoisirCarte)
				{
					sendMove(&moveData4, &mresult);
					ChoisirCarte --;
					ComptePioche++;
				}
				else
				{
					ComptePioche--;
					ChoisirCarte++;
					Gdata.starter--;
				//free(&mresult);
				}
			}
		}
		else
		{
			printf("gyatt\n");
			getMove(&moveData3, &mresult);
			printf("que fait l'adversaire: %d \n",moveData3.action);
			if (moveData3.action==4)
			{
				//on fait rien
			}
			else
			{
				Gdata.starter++;
			}
		}*/
