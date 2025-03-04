programma: main.o rana.o coccodrilli.o disegni.o collisioni.o 
	gcc -o programma main.o rana.o coccodrilli.o disegni.o collisioni.o -lncurses

collisioni.o:  collisioni.c collisioni.h coccodrilli.h rana.h strutture.h 
	gcc -c collisioni.c 

coccodrilli.o: coccodrilli.c coccodrilli.h strutture.h
	gcc -c coccodrilli.c

disegni.o: disegni.c disegni.h strutture.h 
	gcc -c disegni.c

rana.o: rana.c rana.h strutture.h
	gcc -c rana.c

main.o: main.c rana.h strutture.h disegni.h coccodrilli.h collisioni.h
	gcc -c main.c

clean:
	rm -f *.o programma
