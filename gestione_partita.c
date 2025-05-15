#include "gestione_partita.h" 


bool end_game (int vite, int count_tane, Messaggio rana, pid_t pid_frog, pid_t *pid_crocs, pid_t bullets, pid_t grenades)
{
   
}






/*bool finePartita(WINDOW *finestraGioco, Oggetto ranocchio, int vite, bool buffer, int punteggio,
                 int difficolta, Oggetto tempo, Oggetto macchina[], Oggetto camion[], Oggetto tronco[], bool *partitaInCorso, bool partitaFinita, bool audio)
{
     se anche solo una delle condizioni di fine della partita viene rispettata si entra nell'if, che poi stampa la scritta giusta in base a
     quale di queste condizioni è vera. viene poi chiesto se si vuole fare una nuova partita e in base al valore restituito si prosegue.
     i processi vengono killati indipendentemente da questo dato che vengono ricreati all'avvio della partita 
    int i;
    bool riniziaPartita;
    if (ranocchio.id == q || vite == 0 || buffer == false || partitaFinita)
    {
        (*partitaInCorso) = false;
        werase(finestraGioco);

        if (vite == 0 || partitaFinita)
            gameOver(finestraGioco, punteggio);

        else if (buffer == false)
            vittoria(finestraGioco, punteggio);

        werase(finestraGioco);
        wrefresh(finestraGioco);

        riniziaPartita = pausaeNuovaPartita(finestraGioco, 2);

        if (!riniziaPartita)
        {
            werase(finestraGioco);
            wrefresh(finestraGioco);
            schermataFinale(finestraGioco);
        }

        for (i = 0; i < NUMERO_MACCHINE; i++)
            kill(macchina[i].pid, SIGKILL);

        for (i = 0; i < NUMERO_CAMION; i++)
            kill(camion[i].pid, SIGKILL);

        for (i = 0; i < NUMERO_TRONCHI + difficolta; i++)
            kill(tronco[i].pid, SIGKILL);

        kill(tempo.pid, SIGKILL);

        kill(ranocchio.pid, SIGKILL);

        if (audio)
            system("killall ffplay");
    }

    return riniziaPartita;
}
*/
