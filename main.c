#include <stdlib.h>
#include <stdio.h>
#include "../tickettorideapi/ticketToRide.h"

extern int DEBUG_LEVEL;

typedef struct _Route{
	int ville1;
	int ville2;
	int Couleur1;
	int Couleur2;
	int Nbr_Wagon;
} Route;

//faire une structure route-> 0 si prise par nous +inf sinon    + dirac +  matrice de pointeur
//stocker carte de couleur dans un tableau de int + stocker objective dans liste

void AfficherObjectif(MoveResult* mresult)
{
	for (int i=0;i<3;i++)
	{
		printCity(mresult->objectives[i].from);
		printCity(mresult->objectives[i].to);
		printf(" %d \n", mresult->objectives[i].score);
	}
	
}


void JouerSolo(int continuer,MoveResult mresult)
{
	int OnJoue;
	MoveData moveData;
	printf("Jouons à un jeu: \n 0: l'adversaire joue \n 1: piocher des objectives \n 2:  piochez wagons\n 3: choisi wagons\n 4: ff\n");
	scanf("%d",&OnJoue);
	switch (OnJoue)
	{
	case 0: //on laisse l'adversaire jouer
		getMove(&moveData, &mresult);
		printf("move :%d\n",mresult.state);
		free(mresult.opponentMessage);
		free(mresult.message);
		break;

	case 1: // on pioche des objectives
		moveData.action=4;
		sendMove(&moveData, &mresult);
		printf("move :%d\n",mresult.state);
		AfficherObjectif(&mresult);
		free(mresult.opponentMessage);
		free(mresult.message);
		printf("lesquels prendre? \n");
		int val;
		//int* tab = (int*)malloc(sizeof(int)*3);
		for (int i=0;i<3;i++)
		{
			scanf("%d",&val);
			moveData.chooseObjectives[i]=val;
		}
		moveData.action = 5;
		//moveData.chooseObjectives[3] = tab;
		sendMove(&moveData, &mresult);
		printf("move :%d\n",mresult.state);
		//free(tab);
		free(mresult.opponentMessage);
		free(mresult.message);
		break;

	case 2: // on piche un carte de la pioche wagon
		moveData.action = 2;
		sendMove(&moveData, &mresult);
		printf("move :%d\n",mresult.state);
		free(mresult.opponentMessage);
		free(mresult.message);
		break;
	
	case 3:
		int carte;
		moveData.action = 3; // on choisit un wagon
		printf("choisir carte");
		scanf("%d",&carte);
		moveData.drawCard = carte;
		break;

	case 4:
		printf("fin du fun \n");
		continuer=0;
		quitGame();
		break;
	
	}
}


int main(void)
{
	int continuer=1;
	GameSettings Gsetting=GameSettingsDefaults;
	Gsetting.starter=1;
	GameData Gdata=GameDataDefaults;
//	int draw = 4;
//	int choose = 5;
	MoveData moveData1;
	moveData1.action=4;
	MoveData moveData2 = {5,{0,1,1}};
	MoveData moveData3;
	MoveData moveData4;
	moveData4.action = 2;
	DEBUG_LEVEL = MESSAGE;
	int connect = connectToCGS("82.64.1.174", 15001);
	sendName("Naaaaaaaaaaaaatacha");
	sendGameSettings(Gsetting,&Gdata);
	MoveResult mresult;
	printf("on est connecté : %d\n",connect);
	int TirerPiocher=1;
	int ComptePioche=0;
	int ChoisirCarte = 1;
	while(continuer)
	{
		//printf("qui commence : %d \n",Gdata.starter);
		printBoard();
		JouerSolo(continuer,mresult);
		
		//en mettre plein pour pouvoir jouer et les mettre dans result
	}
	printf("c'est fini");
	return 1;
}

/*if (Gdata.starter!=1)
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