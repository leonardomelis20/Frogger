programma: main.o rana.o coccodrilli.o disegni.o collisioni.o proiettili.o
	gcc -o programma main.o rana.o coccodrilli.o disegni.o collisioni.o proiettili.o -lncurses

main.o: main.c rana.h strutture.h disegni.h coccodrilli.h collisioni.h proiettili.h
	gcc -c main.c -lncurses

coccodrilli.o: coccodrilli.c coccodrilli.h strutture.h proiettili.h
	gcc -c coccodrilli.c -lncurses

collisioni.o:  collisioni.c collisioni.h coccodrilli.h rana.h strutture.h 
	gcc -c collisioni.c -lncurses

proiettili.o: collisioni.c collisioni.h strutture.h
	gcc -c proiettili.c -lncurses

disegni.o: disegni.c disegni.h strutture.h 
	gcc -c disegni.c -lncurses

rana.o: rana.c rana.h strutture.h
	gcc -c rana.c -lncurses

clean:
	rm -f *.o programma