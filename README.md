# Frogger Resurrection 🐸

Implementazione in C del classico arcade **Frogger**, sviluppata come progetto finale del corso di **Sistemi Operativi** (A.A. 2024-2025) presso l'Università degli Studi di Cagliari.

Il progetto è realizzato in **due versioni distinte**, che dimostrano l'uso di diversi paradigmi di programmazione concorrente su Linux.

---

## Descrizione

Il giocatore guida una rana attraverso un fiume pieno di coccodrilli per raggiungere le 5 tane nella parte superiore dello schermo. Il gioco è a tempo: ogni manche si conclude quando la rana muore, il tempo scade, o la rana raggiunge una tana. Il giocatore vince quando tutte le tane sono chiuse.

**Elementi di gioco:**
- 🐸 Rana controllata dal giocatore (tasti direzionali + barra spaziatrice per granate)
- 🐊 Coccodrilli che scorrono orizzontalmente su 8+ corsie a velocità variabile
- 💣 Granate (rana) e proiettili (coccodrilli) con gestione delle collisioni
- 🕳️ 5 tane da raggiungere nella parte superiore
- ⏱️ Timer per manche, punteggio e vite residue

---

## Versioni

### `versione_processi` — Processi + Pipe
Ogni oggetto dinamico (rana, singoli coccodrilli, proiettili) è gestito da un **processo separato**. La comunicazione avviene tramite una **singola pipe condivisa** secondo il pattern produttore-consumatore: ogni processo produttore invia le proprie coordinate a un processo consumatore dedicato alla gestione della grafica e delle collisioni.

### `versione_thread` — Thread + Shared Memory
Ogni oggetto dinamico è gestito da un **thread separato**. La comunicazione avviene tramite un **buffer produttore-consumatore in memoria condivisa**: limitato, circolare e protetto da mutex e semafori (libreria pthread) per evitare race condition e accessi concorrenti non sicuri.

---

## Architettura

```
N produttori (rana, coccodrilli, proiettili)
        │
        │  coordinate via pipe (processi)
        │  oppure buffer circolare condiviso (thread)
        ▼
1 consumatore (gestore grafica + collisioni)
        │
        ├── disegna gli oggetti a schermo (ncurses)
        ├── rileva collisioni
        └── gestisce fine manche / fine partita
```

---

## Requisiti

- Sistema operativo: **Linux** (Ubuntu 22.04 64-bit raccomandato)
- Compilatore: `gcc`
- Librerie: `ncurses`, `pthread`

```bash
sudo apt install libncurses5-dev libncursesw5-dev
```

---

## Compilazione ed esecuzione

```bash
# Versione processi
cd versione_processi
make
./frogger

# Versione thread
cd versione_thread
make
./frogger
```

---

## Controlli

| Tasto | Azione |
|-------|--------|
| `↑` `↓` `←` `→` | Muovi la rana |
| `Spazio` | Lancia due granate (sinistra e destra) |
| `P` | Pausa / Riprendi |
| `Q` | Esci dal gioco |

---

## Struttura del repository

```
Frogger/
├── versione_processi/   # Implementazione con processi e pipe
│   ├── frogger.c
│   ├── Makefile
│   └── ...
└── versione_thread/     # Implementazione con thread e shared memory
    ├── frogger.c
    ├── Makefile
    └── ...
```

---

## Corso

**Sistemi Operativi** — Università degli Studi di Cagliari, A.A. 2024-2025  
Docenti: Salvatore Carta, Livio Pompianu
