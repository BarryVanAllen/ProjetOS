#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h>
#include "affichage.h"

#define NB_PILOTES 20
#define NB_TOURS_ESSAIS 20
#define NB_TOURS_QUALIF 15
#define NB_TOURS_COURSE 50
#define BASE_TEMPS 70.0

typedef struct {
    char nom[50];
    float temps_meilleur_tour;
    float dernier_temps_tour;
} Pilote;

typedef struct {
    Pilote pilotes[NB_PILOTES];
} MemoirePartagee;

// Fonction pour générer un temps de tour aléatoire
float generer_temps_tour(float base_temps, int difficulte) {
    float variation = (rand() % 500) / 1000.0;
    float coefficient_difficulte = 1.0 + (difficulte * 0.01);
    return base_temps * coefficient_difficulte + variation;
}

// Fonction de tri des pilotes par meilleur temps
void tri_pilotes(Pilote pilotes[]) {
    for (int i = 0; i < NB_PILOTES - 1; i++) {
        for (int j = 0; j < NB_PILOTES - i - 1; j++) {
            if (pilotes[j].temps_meilleur_tour > pilotes[j + 1].temps_meilleur_tour) {
                Pilote temp = pilotes[j];
                pilotes[j] = pilotes[j + 1];
                pilotes[j + 1] = temp;
            }
        }
    }
}

// Fonction pour afficher les résultats en temps réel
void afficher_resultats_en_temps_reel(Pilote pilotes[], int tour, const char *session) {
    printf("\033[H\033[J"); // Efface l'écran (codes ANSI)
    printf("--- Session : %s | Tour : %d ---\n", session, tour);
    printf("Classement actuel :\n");
    for (int i = 0; i < NB_PILOTES; i++) {
        printf("%d. Pilote: %s | Meilleur temps: %.3f sec | Dernier tour: %.3f sec\n",
               i + 1, pilotes[i].nom, pilotes[i].temps_meilleur_tour, pilotes[i].dernier_temps_tour);
    }
}

// Fonction principale
int main() {
    key_t key = ftok("f1_simulation", 65);
    int shmid = shmget(key, sizeof(MemoirePartagee), 0666 | IPC_CREAT);
    if (shmid == -1) {
        perror("Erreur de création de mémoire partagée");
        exit(1);
    }

    MemoirePartagee *mp = (MemoirePartagee *)shmat(shmid, NULL, 0);
    if (mp == (void *)-1) {
        perror("Erreur de rattachement de mémoire partagée");
        exit(1);
    }

    // Initialisation des pilotes
    for (int i = 0; i < NB_PILOTES; i++) {
        snprintf(mp->pilotes[i].nom, sizeof(mp->pilotes[i].nom), "Pilote %d", i + 1);
        mp->pilotes[i].temps_meilleur_tour = 0.0;
        mp->pilotes[i].dernier_temps_tour = 0.0;
    }

    const char *sessions[] = {"Essais Libres", "Qualifications", "Course"};
    int nb_tours[] = {NB_TOURS_ESSAIS, NB_TOURS_QUALIF, NB_TOURS_COURSE};

    // Boucle sur les sessions
    for (int s = 0; s < 3; s++) {
        const char *session = sessions[s];
        int tours = nb_tours[s];

        for (int tour = 1; tour <= tours; tour++) {
            pid_t pid;
            for (int i = 0; i < NB_PILOTES; i++) {
                pid = fork();
                if (pid == 0) {
                    // Processus enfant : génère un temps pour ce pilote
                    srand(getpid() + time(NULL)); // Graine unique pour chaque processus
                    float temps_tour = generer_temps_tour(BASE_TEMPS, 5);
                    mp->pilotes[i].dernier_temps_tour = temps_tour;

                    // Met à jour le meilleur temps si nécessaire
                    if (mp->pilotes[i].temps_meilleur_tour == 0.0 || temps_tour < mp->pilotes[i].temps_meilleur_tour) {
                        mp->pilotes[i].temps_meilleur_tour = temps_tour;
                    }
                    exit(0);
                }
            }

            // Processus parent : attend que tous les enfants terminent
            for (int i = 0; i < NB_PILOTES; i++) {
                wait(NULL);
            }

            // Tri des pilotes et affichage des résultats
            tri_pilotes(mp->pilotes);
            afficher_resultats_en_temps_reel(mp->pilotes, tour, session);

            // Attente pour simuler un intervalle entre les tours
            usleep(500000); // 0.5 seconde
        }
    }

    // Détachement et destruction de la mémoire partagée
    if (shmdt(mp) == -1) {
        perror("Erreur de détachement de mémoire partagée");
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Erreur de destruction de mémoire partagée");
    }

    return 0;
}

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
