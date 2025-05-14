programma: main.o rana.o coccodrilli.o disegni.o collisioni.o proiettili.o granate.o
	gcc -o programma main.o rana.o coccodrilli.o disegni.o collisioni.o proiettili.o granate.o -lncurses

main.o: main.c rana.h coccodrilli.h strutture.h disegni.h  collisioni.h proiettili.h granate.h
	gcc -c main.c -o main.o -lncurses

collisioni.o: collisioni.c collisioni.h strutture.h proiettili.h granate.h
	gcc -c collisioni.c -o collisioni.o -lncurses

rana.o: rana.c rana.h strutture.h disegni.h
	gcc -c rana.c -o rana.o -lncurses

coccodrilli.o: coccodrilli.c coccodrilli.h strutture.h 
	gcc -c coccodrilli.c -o coccodrilli.o -lncurses

disegni.o: disegni.c disegni.h strutture.h 
	gcc -c disegni.c -o disegni.o -lncurses

proiettili.o: proiettili.c proiettili.h strutture.h
	gcc -c proiettili.c -o proiettili.o -lncurses

granate.o: granate.c granate.h strutture.h 
	gcc -c granate.c -o granate.o -lncurses


clean:
	rm -f *.o programma