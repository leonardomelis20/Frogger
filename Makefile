programma: main.o rana.o coccodrilli.o disegni.o collisioni.o proiettili.o granate.o gestione_partita.o 
	gcc -o programma main.o rana.o coccodrilli.o disegni.o collisioni.o proiettili.o granate.o gestione_partita.o -lncursesw

main.o: main.c rana.h coccodrilli.h strutture.h disegni.h  collisioni.h proiettili.h granate.h gestione_partita.h 
	gcc -c main.c -o main.o -lncursesw

collisioni.o: collisioni.c collisioni.h strutture.h proiettili.h granate.h
	gcc -c collisioni.c -o collisioni.o -lncursesw

gestione_partita.o: gestione_partita.c gestione_partita.h strutture.h utility.h
	gcc -c gestione_partita.c -o gestione_partita.o -lncursesw

rana.o: rana.c rana.h strutture.h disegni.h
	gcc -c rana.c -o rana.o -lncursesw

coccodrilli.o: coccodrilli.c coccodrilli.h strutture.h 
	gcc -c coccodrilli.c -o coccodrilli.o -lncursesw

disegni.o: disegni.c disegni.h strutture.h 
	gcc -c disegni.c -o disegni.o -lncursesw

proiettili.o: proiettili.c proiettili.h strutture.h
	gcc -c proiettili.c -o proiettili.o -lncursesw

granate.o: granate.c granate.h strutture.h 
	gcc -c granate.c -o granate.o -lncursesw

utility.o: utility.c utility.h strutture.h
	gcc -c utility.c -o utility.o -lncursesw

clean:
	rm -f *.o programma