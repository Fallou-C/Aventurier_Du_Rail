#include <stdlib.h>
#include <stdio.h>
#include "../tickettorideapi/ticketToRide.h"
#include "../tickettorideapi/clientAPI.h"
#include "matrice.h"
#include "utility.h"

void AfficherObjectif(MoveResult* mresult)
{
	printf("Les villes :\n");
	for (int i=0;i<3;i++)
	{
		printCity(mresult->objectives[i].from);
		printf(" %d ",mresult->objectives[i].from);
		printCity(mresult->objectives[i].to);
		printf(" %d ",mresult->objectives[i].to);
		printf("score: %d \n", mresult->objectives[i].score);
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
		tab[3] = 0;
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


int* RoutePrenable(int* InventaireCouleur, Route** matriceRoute, int tailleMatrice, int min, int* wagon)
{
	int* tab = (int *)malloc(6 * sizeof(int));
	Route route;
	for (int i=0; i < tailleMatrice ;i++)
	{
		for (int  j=0; j < i ; j++)
		{
			route = matriceRoute[i][j];
			if ((route.Couleur1 == 9 || route.Couleur2 == 9) && route.Nbr_Wagon > min && route.Nbr_Wagon <= *wagon){
				tab = RouteGrise(InventaireCouleur,i,j,tailleMatrice,route);
				if (tab[4]){ // on renvoie que si on peut claim
					*wagon -= matriceRoute[i][j].Nbr_Wagon;
					matriceRoute[i][j].Nbr_Wagon = 0;
					matriceRoute[j][i].Nbr_Wagon = 0;
					return tab;}
			}
			if (route.Couleur1 != 9 && ((InventaireCouleur[route.Couleur1] +  InventaireCouleur[9]) >= route.Nbr_Wagon ) && (route.Nbr_Wagon > min) && ( route.Nbr_Wagon <= *wagon))//+ InventaireCouleur[9] on ignore les joker
			{
				tab[0] = i;
				tab[1] = j;
				tab[2] = route.Couleur1;
				tab[3] = route.Nbr_Wagon;
				tab[4] = 1; // on indique que l'on peut prendre la route

				if (InventaireCouleur[route.Couleur1] >= route.Nbr_Wagon){
					InventaireCouleur[route.Couleur1] -= route.Nbr_Wagon;
					tab[5] = 0;
				}
				else {
					tab [5] = route.Nbr_Wagon - InventaireCouleur[route.Couleur1];
					InventaireCouleur[9] -= route.Nbr_Wagon - InventaireCouleur[route.Couleur1];
					InventaireCouleur[route.Couleur1] = 0;
				}
				*wagon -= matriceRoute[i][j].Nbr_Wagon;

				matriceRoute[i][j].Nbr_Wagon = 0;
				matriceRoute[j][i].Nbr_Wagon = 0;
				return tab;
			}
			else if (route.Couleur2 != 9 &&  ((InventaireCouleur[route.Couleur2]) >= route.Nbr_Wagon )  && (route.Nbr_Wagon > min) && (route.Nbr_Wagon <= *wagon))
			{
				tab[0] = i;
				tab[1] = j;
				tab[2] = route.Couleur2;
				tab[3] = route.Nbr_Wagon;
				tab[4] = 1;
				
				if (InventaireCouleur[route.Couleur2] >= route.Nbr_Wagon){
					InventaireCouleur[route.Couleur2]-= route.Nbr_Wagon;
					tab[5] = 0;
				}
				else {
					tab [5] = route.Nbr_Wagon - InventaireCouleur[route.Couleur2];
					InventaireCouleur[9] -= route.Nbr_Wagon - InventaireCouleur[route.Couleur2];
					InventaireCouleur[route.Couleur2] = 0;
				}
				*wagon -= matriceRoute[i][j].Nbr_Wagon;

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
	tab[5] = 0;
	return tab;
}

void ClaimeBarre(int option,MoveResult mresult ,BoardState EtatPlateau,MoveData data, int* InventaireCouleur,int couleur)
{
	switch (option)
		{
		case 0: // pioche à l'aveugle deux fois
			printf("on pioche \n");
			data.action = 2;
			sendMove(&data, &mresult);
			InventaireCouleur[mresult.card] += 1;
			sendMove(&data, &mresult);
			InventaireCouleur[mresult.card] += 1;
			break;
		
		case 1: // on pioche à l'aveugle une fois
			printf("on pioche \n");
			data.action = 2;
			sendMove(&data, &mresult);
			InventaireCouleur[mresult.card] += 1;
			break;

		case 2: //prendre des objectifs
			// to do 
			break;

		case 3:
			// faire fonction pioche par rapport au chemin tous ça 
			printf("on pioche ! \n");
			bool OnAPioche = false;
			for(int i=0;i<5;i++){
				if (EtatPlateau.card[i] == couleur && !OnAPioche)
				{
					data.action = 3; 
					data.drawCard = couleur;
					sendMove(&data, &mresult);
					free(mresult.opponentMessage);
					free(mresult.message);
					printf("on a prit %d hihihi \n",couleur);
					OnAPioche = true;
					getBoardState(&EtatPlateau);
				}
			}
			if (!OnAPioche) {ClaimeBarre(1,mresult , EtatPlateau, data,  InventaireCouleur, 9);}
			if (couleur != 9 || !OnAPioche)
			{
				OnAPioche = false;
				for(int i=0;i<5;i++){
				if (EtatPlateau.card[i] == couleur && !OnAPioche)
				{
					data.action = 3; 
					data.drawCard = couleur;
					sendMove(&data, &mresult);
					free(mresult.opponentMessage);
					free(mresult.message);
					printf("on a prit %d hihihi \n",couleur);
					getBoardState(&EtatPlateau);
					OnAPioche = true;
				}
			}
			}
			if (!OnAPioche) {ClaimeBarre(1,mresult , EtatPlateau, data,  InventaireCouleur, 9);}
			break;
		}
}

void ClaimRoute(int option,int i, int j, Route** matriceRoute, int n,MoveResult mresult ,BoardState EtatPlateau,MoveData data, int* InventaireCouleur, int* wagon)
{
	int* tab = (int *)malloc(6 * sizeof(int));
	Route route = matriceRoute[i][j];
    printf("route : %d %d %d\n",route.Couleur1,route.Couleur2,route.Nbr_Wagon);
	if ((route.Couleur1 == 9 || route.Couleur2 == 9) && (route.Nbr_Wagon > 0) && (route.Nbr_Wagon <= *wagon) ){
		tab = RouteGrise(InventaireCouleur,i,j,n,route);
		if (tab[4]){ // on renvoie que si on peut claim
			*wagon -= matriceRoute[i][j].Nbr_Wagon;
			matriceRoute[i][j].Nbr_Wagon = 0;
			matriceRoute[j][i].Nbr_Wagon = 0;
			data.action = 1;
			data.claimRoute.from = tab[0];
			data.claimRoute.to = tab[1];
			data.claimRoute.color = tab[2];
			data.claimRoute.nbLocomotives = tab[5]; // nombre de joker qu'on joue
			printf("from %d to %d color %d nbloc %d \n",tab[0],tab[1],tab[2],tab[3]);
			sendMove(&data, &mresult);
		}
		else{
			ClaimeBarre(option,mresult , EtatPlateau, data,  InventaireCouleur, 9);
		}
	}
	else if (((InventaireCouleur[route.Couleur1] + InventaireCouleur[9]) >= route.Nbr_Wagon ) && (route.Nbr_Wagon != 0) && (route.Nbr_Wagon <= *wagon))
	{
		tab[0] = i;
		tab[1] = j;
		tab[2] = route.Couleur1;
		tab[3] = route.Nbr_Wagon;
		tab[4] = 1; // on indique que l'on peut prendre la route

		if (InventaireCouleur[route.Couleur1] >= route.Nbr_Wagon){
			InventaireCouleur[route.Couleur1] -= route.Nbr_Wagon;
			tab[5] = 0;
		}
		else {
			tab [5] = route.Nbr_Wagon - InventaireCouleur[route.Couleur1];
			InventaireCouleur[9] -= route.Nbr_Wagon - InventaireCouleur[route.Couleur1];
			InventaireCouleur[route.Couleur1] = 0;
		}

		*wagon -= matriceRoute[i][j].Nbr_Wagon;
		matriceRoute[i][j].Nbr_Wagon = 0;
		matriceRoute[j][i].Nbr_Wagon = 0;

		data.action = 1;
		data.claimRoute.from = tab[0];
		data.claimRoute.to = tab[1];
		data.claimRoute.color = tab[2];
		data.claimRoute.nbLocomotives = tab[5]; // nombre de joker qu'on joue

		printf("from %d to %d color %d nbloc %d \n",tab[0],tab[1],tab[2],tab[3]);
		sendMove(&data, &mresult);
	}
	else if (((InventaireCouleur[route.Couleur2]) >= route.Nbr_Wagon )  && (route.Nbr_Wagon != 0) && (route.Nbr_Wagon <= *wagon))
	{
		tab[0] = i;
		tab[1] = j;
		tab[2] = route.Couleur2;
		tab[3] = route.Nbr_Wagon;
		tab[4] = 1;
		
		if (InventaireCouleur[route.Couleur2] >= route.Nbr_Wagon){
			InventaireCouleur[route.Couleur2]-= route.Nbr_Wagon;
			tab[5] = 0;
		}
		else {
			tab [5] = route.Nbr_Wagon - InventaireCouleur[route.Couleur2];
			InventaireCouleur[9] -= route.Nbr_Wagon - InventaireCouleur[route.Couleur2];
			InventaireCouleur[route.Couleur2] = 0;
		}

		*wagon -= matriceRoute[i][j].Nbr_Wagon;

		matriceRoute[i][j].Nbr_Wagon = 0;
		matriceRoute[j][i].Nbr_Wagon = 0;

		data.action = 1;
		data.claimRoute.from = tab[0];
		data.claimRoute.to = tab[1];
		data.claimRoute.color = tab[2];

		data.claimRoute.nbLocomotives = tab[5]; // nombre de joker qu'on joue
		printf("from %d to %d color %d nbloc %d \n",tab[0],tab[1],tab[2],tab[3]);
		sendMove(&data, &mresult);
	}
	else
	{
		if (InventaireCouleur[route.Couleur1] > InventaireCouleur[route.Couleur2]){
			ClaimeBarre(option,mresult , EtatPlateau, data,  InventaireCouleur,route.Couleur1);}
		else{
			ClaimeBarre(option,mresult , EtatPlateau, data,  InventaireCouleur,route.Couleur2);
		}
	}
	free(tab);
}


int distanceMini(int* D, bool* visite, int n)
{
	int indice_min;
	int min = 2000;
	for (int i=0; i < (n-1); i++)
	{
		if (visite[i] == false && D[i]<min)
		{
			min = D[i];
			indice_min = i;
		}
	}
	return indice_min;
}

int* Dijkstra(int src, Route** matrice, int n) // n = taille de la matrice
{
	bool* visite = (bool *)malloc(n * sizeof(bool)); // ville visité
	int* D = (int *)malloc(n * sizeof(int)); // distance 
	int* prec = (int *)malloc(n * sizeof(int)); // précédent
	int* D_prec = (int *)malloc(2*n * sizeof(int));// fusion des deux 
	int u; // ville acutellement considéré dans la boucle
	for(int i=0 ; i < n ; i++){D[i] = 1999;}
	D[src] = 0;

	for(int i=0; i < n; i++)
	{
		u = distanceMini(D, visite, n);
		visite[u] = true;
		for(int j = 0; j < n; j++)
		{
			if ((visite[j] == false) && (matrice[u][j].Nbr_Wagon < 2000) && (D[u] + matrice[u][j].Nbr_Wagon < D[j]))
			{
				D[j] = D[u] + matrice[u][j].Nbr_Wagon;
				prec[j] = u;
			}
		}
	}
	for(int i=0; i<2*n;i++)
	{
		if (i<n){D_prec[i]=D[i];}
		else {D_prec[i]=prec[i-n];}
	}
	return D_prec;
}

int* CheminCourt(int a,int b, Route** matrice, int n) // renvoie le prochain chemin à prendre pour aller de b à partir de a, renvoie -1 -1 si déjà complet
{
	
	int* D = (int *)malloc(n * sizeof(int));
	int* prec = (int *)malloc(n * sizeof(int));
	int* chemin = (int *)malloc(2 * sizeof(int));
	int* D_prec = Dijkstra(a,matrice,n);

	chemin[0]=a;
	chemin[1]=b;

	for(int i=0; i<2*n;i++)
	{
		if (i<n){D[i]=D_prec[i];}
		else {prec[i-n]=D_prec[i];}
	}

	//printf("ville de départ: %d ",a);for(int  i = 0; i < n ; i++){printf(" distance par rapport à %d = %d \n", i,D[i]);}printf("\n");
	//printf("tab prec de %d ",a);for(int  i = 0; i < n ; i++){printf(" prec de %d : %d \n", i,prec[i]);}printf("\n");
	//int last=b;printf("chemin à prendre : ");for(int i = 0; i<n;i++){printf(" %d ",prec[last]);last=prec[last];}printf("\n");

	int der_ville = b;
	while( (der_ville != a) && (matrice[prec[der_ville]][der_ville].Nbr_Wagon != 0) ) // si on arrive sur a ou une route déjà claim
	{
		chemin[1] = der_ville;
		der_ville = prec[der_ville];
		chemin[0] = der_ville;
		//printf(" deux dernière ville %d %d\n",chemin[0],chemin[1]);
	}

	if (matrice[prec[b]][b].Nbr_Wagon == 0) 
	{
		chemin[0]=-1;
		chemin[1]=-1;
		return chemin;
	}

	return chemin;
}

bool* CourtObjectif(int nbr_objectif, Route** matrice, int taille, int* InventaireObjective,MoveResult mresult, bool mode ) //améliorable en prenant ceux gratuit par exemple
{
	// on prend les objectifs les plus simples à faire
	int minmax;
	if (mode){minmax = 1;} // on prend les plus petits
	else{minmax = -1;} // les plus longs
	bool* obj = (bool*)malloc(sizeof(bool)*3);
	obj[0] = false;
	obj[1] = false;
	obj[2] = false;
	int* distance = (int*)malloc(sizeof(int)*3);

	int objective[6] = {mresult.objectives[0].from,mresult.objectives[0].to,mresult.objectives[1].from,mresult.objectives[1].to,mresult.objectives[2].from,mresult.objectives[2].to};

	for(int i=0;i<3;i++)
	{
		distance[i] = Dijkstra(objective[2*i],matrice, taille)[objective[2*i + 1]];
	}

	int ind_obj = 0;
	int ind_min = 0;
	for(int i = 0; i < nbr_objectif; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			if ((distance[j]*minmax < distance[ind_min]*minmax) && (!obj[j])) {ind_min = j;}
		}
		obj[ind_min] = true;
		InventaireObjective[ind_obj*2] = mresult.objectives[ind_min].from;
		InventaireObjective[ind_obj*2 + 1] = mresult.objectives[ind_min].to;
		ind_obj += 1;
		if (obj[0]){ind_min = 1;}
		else {ind_min = 0;}
	}
	InventaireObjective[ind_obj*2] = -1;
	return obj;
}

// fonction pioche couleur en fonction de l'objectif désiré : locomotive ou couleur du prochain chemin à prendre sinon pioche au pif
void PiocheChemin(BoardState EtatPlateau,int couleur)
{
	//
}