programma: main.o rana.o coccodrilli.o disegni.o
	gcc -o programma main.o rana.o coccodrilli.o disegni.o -lncurses

coccodrilli.o: coccodrilli.c coccodrilli.h strutture.h
	gcc -c coccodrilli.c

disegni.o: disegni.c disegni.h strutture.h 
	gcc -c disegni.c

rana.o: rana.c rana.h strutture.h
	gcc -c rana.c

main.o: main.c rana.h strutture.h disegni.h coccodrilli.h
	gcc -c main.c

clean:
	rm -f *.o programma
