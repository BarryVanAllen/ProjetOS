#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>
#include <pthread.h>
#include "simulation.h"
#include "affichage.h"
#include "utils.h"

void simulate_session(MemoirePartagee *mp, const char *session, int nb_tours, int nb_pilotes) {
    for (int tour = 1; tour <= nb_tours; tour++) {
        pid_t pid;
        for (int i = 0; i < nb_pilotes; i++) {
            pid = fork();
            if (pid == 0) {
                srand(getpid() + time(NULL)); // Graine unique
                float temps_tour = generer_temps_tour(70.0, 5);

                // Section critique : mise à jour de la mémoire partagée
                pthread_mutex_lock(&mp->mutex);
                mp->pilotes[i].dernier_temps_tour = temps_tour;

                // Met à jour le meilleur temps si nécessaire
                if (mp->pilotes[i].temps_meilleur_tour == 0.0 || temps_tour < mp->pilotes[i].temps_meilleur_tour) {
                    mp->pilotes[i].temps_meilleur_tour = temps_tour;
                }
                pthread_mutex_unlock(&mp->mutex); // Fin de la section critique

                exit(0); // Terminer le processus enfant
            }
        }

        for (int i = 0; i < nb_pilotes; i++) {
            wait(NULL); // Attendre la fin de chaque processus enfant
        }

        // Section critique : tri et affichage
        pthread_mutex_lock(&mp->mutex);
        tri_pilotes(mp->pilotes, nb_pilotes);
        afficher_resultats_en_temps_reel(mp->pilotes, tour, session);
        pthread_mutex_unlock(&mp->mutex); // Fin de la section critique

        usleep(500000); // Pause pour simuler un délai réaliste
    }
}
