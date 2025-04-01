#include <stdlib.h>
#include <stdio.h>
#include "../tickettorideapi/ticketToRide.h"

extern int DEBUG_LEVEL;

typedef struct _Route{
	int Couleur1;
	int Couleur2;
	int Nbr_Wagon;
} Route;

//faire une structure route-> 0 si prise par nous +inf sinon    + dirac +  matrice de pointeur
//stocker carte de couleur dans un tableau de int + stocker objective dans liste

Route **AllouerMatrice(int n) // matrice carré de pointeur de route de taille n
{
	Route** matrice=(Route**)malloc(n*sizeof(Route*));
	for(int i=0;i<n;i++)
	{
		matrice[i]=(Route *)malloc(n*sizeof(Route));
	}
	return matrice;
}

void DetruireMatrice(Route** matrice,int n) // on a une matrice de pointeur de route
{
	for(int i=0;i<n;i++)
	{
		free(matrice[i]);
	}
	free(matrice);
}

void MatriceAdjacence(Route** matrice,int NbrCity,int nbTrack, int* trackData) // création de la matrice d'adjacence
{
	int inf = -1;
	for(int i = 0;i<NbrCity; i++)
	{
		for(int j = 0; j<i; j++)
		{
			for(int k = 0; k < nbTrack*5; k+=5)
			{
				
				if ((j == trackData[k]) && (i == trackData[k + 1]))
				{
					//printf("i: %d j: %d track :%d \n",i,j,trackData[k+2]);
					matrice[i][j].Nbr_Wagon = trackData[k + 2];
					matrice[i][j].Couleur1 = trackData[k + 3];
					matrice[i][j].Couleur2 = trackData[k + 4];
				}
				matrice[j][i] = matrice[i][j];
			}
			
		}
	}
	for(int i = 0;i<NbrCity; i++)
	{
		for(int j = 0; j<i; j++)
		{
			for(int k = 0; k < nbTrack*5; k+=5)
			{
				
				if (matrice[i][j].Nbr_Wagon == 0)
				{
					//printf("i: %d j: %d track :%d \n",i,j,trackData[k+2]);
					matrice[i][j].Nbr_Wagon = inf;
				}
				matrice[j][i] = matrice[i][j];
			}
			
		}
	}
}


void AfficherMatrice(Route** matrice, int n) //question 1
{
	for(int i=0;i<n;i++)
	{
		printf("(");
		for(int j=0;j<(n-1);j++)
		{
			printf("%d;",matrice[i][j].Nbr_Wagon);
		}
		printf("%d)\n",matrice[i][n-1].Nbr_Wagon);
	}
}

void AfficherObjectif(MoveResult* mresult)
{
	printf("Les villes :\n");
	for (int i=0;i<3;i++)
	{
		printCity(mresult->objectives[i].from);
		printf(" ");
		printCity(mresult->objectives[i].to);
		printf(" %d \n", mresult->objectives[i].score);
	}
}

void AfficherEtatPlateau(BoardState* EtatPlateau)
{
	printf(" les locomotives :");
	for(int i=0;i<5;i++)
	{
		printf("%d ",EtatPlateau->card[i]);
	}
	printf("\n");
}

void AfficherAction(MoveData* mData)
{
	switch (mData -> action)
	{
	case 1:
		printf("il prend la route \n");
		break;

	case 2:
		printf("il pioche une carte à l'aveugle \n");
		break;

	case 3:
		printf("il choisit une carte \n");
		break;

	case 4:
		printf("il pioche des objectifs \n");
		break;

	case 5:
		printf("il choisit des objectifs \n");
		break;
	}
}

void JouerSolo(int continuer,MoveResult mresult ,BoardState EtatPlateau)
{
	int OnJoue;
	MoveData moveData;
	getBoardState(&EtatPlateau);
	AfficherEtatPlateau(&EtatPlateau);
	printf("Jouons à un jeu: \n 0: l'adversaire joue \n 1: piocher des objectives \n 2: piochez wagons\n 3: choisi wagons\n 4: claim route\n 5: ff\n");
	scanf("%d",&OnJoue);
	switch (OnJoue)
	{
	case 0: //on laisse l'adversaire jouer
		getMove(&moveData, &mresult);
		AfficherAction(&moveData);
		free(mresult.opponentMessage);
		free(mresult.message);
		if ( ((moveData.action%2) || (moveData.drawCard!=9 )) && (moveData.action != 1) )
		{
			getMove(&moveData, &mresult);
			AfficherAction(&moveData);
			free(mresult.opponentMessage);
			free(mresult.message);
		}
		//faire if quand le joueur adversaire doit rejouer
		break;

	case 1: // on pioche des objectives
		moveData.action = 4;
		sendMove(&moveData, &mresult);
		printf("move :%d\n",mresult.state);
		AfficherObjectif(&mresult);
		free(mresult.opponentMessage);
		free(mresult.message);
		printf("lesquels prendre? \n");
		int val;
		for (int i=0;i<3;i++)
		{
			scanf("%d",&val);
			moveData.chooseObjectives[i]=val;
		}
		moveData.action = 5;
		sendMove(&moveData, &mresult);
		printf("move :%d\n",mresult.state);
		free(mresult.opponentMessage);
		free(mresult.message);
		break;

	case 2: // on pioche un carte de la pioche wagon face caché
		moveData.action = 2;
		sendMove(&moveData, &mresult);
		printf("move :%d\n",mresult.state);
		printf("carte pioché :%d\n",mresult.card);
		free(mresult.opponentMessage);
		free(mresult.message);
		printf("vous pouvez rejouer 2 ou 3");
		break;
	
	case 3: // on choisit un wagon face visible attention si on prend un locomotive on doit passer notre tour
		int carte;
		moveData.action = 3; 
		printf("choisir carte\n");
		scanf("%d",&carte);
		moveData.drawCard = carte;
		sendMove(&moveData, &mresult);
		printf("move :%d\n",mresult.state);
		free(mresult.opponentMessage);
		free(mresult.message);
		if (moveData.drawCard == 9) {printf("vous pouvez rejouer 2 ou 3");}
		break;

	case 4: // claim une route
		printf("On prend une route \n");
		moveData.action = 1;
		printf("ville1 puis ville2 puis couleur puis nbrlocomotive \n");
		int value;
		scanf("%d",&value);
		moveData.claimRoute.from = value;
		scanf("%d",&value);
		moveData.claimRoute.to = value;
		scanf("%d",&value);
		moveData.claimRoute.color = value;
		scanf("%d",&value);
		moveData.claimRoute.nbLocomotives = value;
		sendMove(&moveData, &mresult);
		printf("move :%d\n",mresult.state);
		free(mresult.opponentMessage);
		free(mresult.message);
		break;

	case 5: // on abandonne le jeu
		printf("fin du fun \n");
		continuer = 0;
		quitGame();
		break;
	
	}
}


int main(void)
{
	extern int DEBUG_LEVEL;
	DEBUG_LEVEL = INTERN_DEBUG;

	int continuer=1;
	int InventaireCouleur[10];
	int InventaireObjective[20];

	GameSettings Gsetting=GameSettingsDefaults;
	Gsetting.starter=1;
	GameData Gdata = GameDataDefaults;
	BoardState EtatPlateau;
	DEBUG_LEVEL = MESSAGE;

	int connect = connectToCGS("82.64.1.174", 15001);
	sendName("Nataaaachaaa");
	sendGameSettings(Gsetting,&Gdata);
	MoveResult mresult;
	printf("on est connecté : %d\n",connect);

	/*
	for (int i =0; i< Gdata.nbTracks; i++)
	{
		printf("%d %d %d %d %d\n",Gdata.trackData[i*5],Gdata.trackData[1 + 5*i],Gdata.trackData[2 + i*5],Gdata.trackData[3 + 5*i],Gdata.trackData[4 + 5*i]);
	}*/
	//printf("%d",Gdata.nbCities);
	
	
	int n = Gdata.nbCities;
	Route** matrice = AllouerMatrice(n);
	MatriceAdjacence( matrice, n, Gdata.nbTracks, Gdata.trackData);
	AfficherMatrice(matrice, n);
	DetruireMatrice(matrice, n);

	while(continuer)
	{
		printBoard();
		printf("qui commence : %d \n",Gdata.starter);
		JouerSolo(continuer,mresult,EtatPlateau);
		
	}
	printf("c'est fini");
	return 1;
}

/*
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
