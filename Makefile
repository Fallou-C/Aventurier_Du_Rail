all :

main.exe : main.c
	gcc -o main.exe main.c

clean :
	rm *.o main.exe
