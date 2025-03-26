all : main main.o ticketToRide.o codingGameServer.o

codingGameServer.o : ../tickettorideapi/codingGameServer.c
	gcc -c ../tickettorideapi/codingGameServer.c

ticketToRide.o : ../tickettorideapi/ticketToRide.c
	gcc -c ../tickettorideapi/ticketToRide.c

main.o : main.c ../tickettorideapi/ticketToRide.c
	gcc -c main.c 

main : main.o  ticketToRide.o codingGameServer.o 
	gcc -Wall -o main main.o codingGameServer.o ticketToRide.o

clean :
	rm *.o main

git :
	git -C ../tickettorideapi pull
