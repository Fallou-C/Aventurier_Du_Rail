#include <stdlib.h>
#include <stdio.h>
#include "../tickettorideapi/ticketToRide.h"
#include "../tickettorideapi/clientAPI.h"

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
	int inf = 2000;
	for(int i = 0;i<NbrCity; i++)
	{
		for(int j = 0; j < i; j++)
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
			printf("%d %d;",matrice[i][j].Couleur1,matrice[i][j].Couleur2);
		}
		printf("%d %d)\n",matrice[i][n-1].Couleur1,matrice[i][n-1].Couleur2);
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

void AfficherAction(MoveData* mData, MoveResult* mresult)
{
	switch (mData -> action)
	{
	case 1:
		printf("il prend la route entre %d et %d avec %d locomotives %d\n",mData -> claimRoute.from,mData -> claimRoute.to,mData -> claimRoute.nbLocomotives,mData -> claimRoute.color);
		break;

	case 2:
		printf("il pioche une carte à l'aveugle \n");
		break;

	case 3:
		printf("il choisit la carte : %d \n",mData -> drawCard);
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
		AfficherAction(&moveData,&mresult);
		free(mresult.opponentMessage);
		free(mresult.message);
		if ( ((moveData.action%2) || (moveData.drawCard!=9 )) && (moveData.action != 1) )
		{
			getMove(&moveData, &mresult);
			AfficherAction(&moveData,&mresult);
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

	case 2: // on pioche une carte de la pioche wagon face caché
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
/*
int MachineEtat(MoveResult mresult, MoveData moveData) //on s'inpire pas mal de ce qui a été fait pour jouer solo
{
	int action = moveData.action;
	// des switch 5 cas : on doit jouer une fois, deux fois, l'adversaire joue une fois, deux fois, c'est ff
	switch(action)
	{
	case 1:
		return action;
		break;
	case 2:
		getmove(&mresult,&moveData);
		return moveData.action;
                break;
        case 3:
                break;
        case 4:
                break;
        case 5:
                break;
	}
}*/

int* RouteGrise(int* InventaireCouleur, int a, int b, int tailleMatrice,Route route)
{
	int* tab = (int *)malloc(5 * sizeof(int));
	int j = 0;
	for(int i=0;i<10;i++)
	{
		if (InventaireCouleur[i] > InventaireCouleur[j]) {j = i;}
	} // j indice du max
	//printf("couleur max : %d \n",j);
	//printf(" on peut claime : %d\n",InventaireCouleur[j] >= route.Nbr_Wagon);
	if (InventaireCouleur[j] >= route.Nbr_Wagon){
		tab[0] = a;
		tab[1] = b;
		tab[2] = j;
		tab[3] = route.Nbr_Wagon;
		tab[4] = 1;
		InventaireCouleur[j] -= route.Nbr_Wagon;
		return tab;
	}
	tab[0] = 0;
	tab[1] = 0;
	tab[2] = 0;
	tab[3] = 0;
	tab[4] = 0; // pas de route à prendre :(
	return tab;
}

int* RoutePrenable(int* InventaireCouleur, Route** matriceRoute, int tailleMatrice)
{
	int* tab = (int *)malloc(5 * sizeof(int));
	Route route;
	for (int i=0; i < tailleMatrice ;i++)
	{
		for (int  j=0; j < i ; j++)
		{
			route = matriceRoute[i][j];
			if ((route.Couleur1 == 9 || route.Couleur2 == 9) && route.Nbr_Wagon > 0){
				tab = RouteGrise(InventaireCouleur,i,j,tailleMatrice,route);
				if (tab[4]){ // on renvoie que si on peut claim
					matriceRoute[i][j].Nbr_Wagon = 0;
					matriceRoute[j][i].Nbr_Wagon = 0;
					return tab;}
			}
			if ( ((InventaireCouleur[route.Couleur1]) >= route.Nbr_Wagon ) && (route.Nbr_Wagon != 0))//+ InventaireCouleur[9] on ignore les joker
			{
				tab[0] = i;
				tab[1] = j;
				tab[2] = route.Couleur1;
				tab[3] = route.Nbr_Wagon;
				tab[4] = 1; // on indique que l'on peut prendre la route
				matriceRoute[i][j].Nbr_Wagon = 0;
				matriceRoute[j][i].Nbr_Wagon = 0;
				return tab;
			}
			else if ( ((InventaireCouleur[route.Couleur2]) >= route.Nbr_Wagon )  && (route.Nbr_Wagon != 0))
			{
				tab[0] = i;
				tab[1] = j;
				tab[2] = route.Couleur2;
				tab[3] = route.Nbr_Wagon;
				tab[4] = 1;
				matriceRoute[i][j].Nbr_Wagon = 0;
				matriceRoute[j][i].Nbr_Wagon = 0;
				return tab;
			}
		}
	}
	tab[0] = 0;
	tab[1] = 0;
	tab[2] = 0;
	tab[3] = 0;
	tab[4] = 0; // pas de route à prendre :(
	return tab;
}

void ClaimeurFou(int firstturn ,MoveResult mresult ,BoardState EtatPlateau, int* InventaireCouleur, Route** matrice,int taille, MoveData data) // il claim si possible
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
		int* routeClaimable = RoutePrenable(InventaireCouleur,matrice,taille); //tableau de 4 entier

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
			data.claimRoute.nbLocomotives = 0; // nombre de joker qu'on joue
			printf("from %d to %d color %d nbloc %d \n",routeClaimable[0],routeClaimable[1],routeClaimable[2],routeClaimable[3]);
			sendMove(&data, &mresult);
		}
		else // sinon on pioche
		{
			printf("on pioche \n");
			data.action = 2;
			sendMove(&data, &mresult);
			sendMove(&data, &mresult);
		}
	}
}


/*************** Nouveauu API configuration : ***********************
 * 
 * adress serveur en ligne : http://82.29.170.160:8889/
sendGameSetting( char* , GameData );

char* = "TRAINING NICE_BOT"
	"TRAINING RANDOM_PLAYER"
	"TRAINING DO_NOTHING"
seed debut : f1b531fb0b78
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
	//Gdata.gameSeed = 0xf1b531fb0b78;
	BoardState EtatPlateau;
	DEBUG_LEVEL = MESSAGE;

	connectToCGS("82.29.170.160", 15001, "Natacha");
	sendGameSettings("TRAINING NICE_BOT seed=9892492",&Gdata);
	
	MoveResult mresult;
	MoveData data;
	//printf("on est connecté : %d\n",connect);

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
			ClaimeurFou(firstturn, mresult ,EtatPlateau,InventaireCouleur,matrice,n,data);
			//for(int i =0;i<10;i++){printf("nombre de carte couleur dispo : couleur %d nombre %d \n",i,InventaireCouleur[i]);}
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
