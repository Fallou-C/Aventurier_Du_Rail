all : main main.o ticketToRide.o clientAPI.o

clientAPI.o : ../tickettorideapi/clientAPI.c
	gcc -c ../tickettorideapi/clientAPI.c

ticketToRide.o : ../tickettorideapi/ticketToRide.c
	gcc -c ../tickettorideapi/ticketToRide.c

main.o : main.c ../tickettorideapi/ticketToRide.c
	gcc -c main.c 

main : main.o  ticketToRide.o clientAPI.o 
	gcc -Wall -o main main.o clientAPI.o ticketToRide.o

clean :
	rm *.o main

git :
	git -C ../tickettorideapi pull
