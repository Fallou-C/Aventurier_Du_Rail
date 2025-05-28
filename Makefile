all : main main.o ticketToRide.o clientAPI.o

clientAPI.o : ../tickettorideapi/clientAPI.c
	gcc -c ../tickettorideapi/clientAPI.c

ticketToRide.o : ../tickettorideapi/ticketToRide.c
	gcc -c ../tickettorideapi/ticketToRide.c

matrice.o : matrice.c ../tickettorideapi/ticketToRide.c ../tickettorideapi/clientAPI.c
	gcc -c matrice.c

utility.o : utility.c  matrice.c ../tickettorideapi/ticketToRide.c ../tickettorideapi/clientAPI.c
	gcc -c utility.c 

bot.o : bot.c utility.c  matrice.c ../tickettorideapi/ticketToRide.c ../tickettorideapi/clientAPI.c
	gcc -c bot.c

main.o : main.c ../tickettorideapi/ticketToRide.c matrice.c utility.c bot.c
	gcc -c main.c 

main : main.o  ticketToRide.o clientAPI.o matrice.o utility.o bot.o
	gcc -Wall -o main main.o clientAPI.o ticketToRide.o matrice.o utility.o bot.o

clean :
	rm *.o main

git :
	git -C ../tickettorideapi pull
