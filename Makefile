programma: main.o rana.o
	gcc -o programma main.o rana.o -lncurses

rana.o: rana.c rana.h strutture.h
	gcc -c rana.c

main.o: main.c rana.h strutture.h
	gcc -c main.c

clean:
	rm -f *.o programma
