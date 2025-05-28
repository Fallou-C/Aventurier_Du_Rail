#include <stdlib.h>
#include <stdio.h>
#include "../tickettorideapi/ticketToRide.h"
#include "../tickettorideapi/clientAPI.h"
#include "matrice.h"


Route** AllouerMatrice(int n) // matrice carré de pointeur de route de taille n
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


void AfficherMatrice(Route** matrice, int n)
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

