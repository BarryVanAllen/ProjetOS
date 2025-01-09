#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <fcntl.h>
#include <string.h>
#include <affichage.h>
#include <file_manager.h>
#include <types.h>

#define NB_PILOTES 20
#define NB_TOURS_ESSAIS 20
#define NB_TOURS_QUALIF 15
#define NB_TOURS_COURSE 50
#define BASE_TEMPS 70.0

typedef struct {
    Pilote pilotes[NB_PILOTES];
    int nbrLect;             // Nombre de lecteurs en cours
    sem_t mutex;             // Protection des écrivains
    sem_t mutLect;           // Protection du compteur de lecteurs
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
    printf("\033[H\033[J");
    printf("--- Session : %s | Tour : %d ---\n", session, tour);
    printf("Classement actuel :\n");
    for (int i = 0; i < NB_PILOTES; i++) {
        printf("%d. Pilote: %s | Meilleur temps: %.3f sec | Dernier tour: %.3f sec\n",
               i + 1, pilotes[i].nom, pilotes[i].temps_meilleur_tour, pilotes[i].dernier_temps_tour);
    }
}

void executer_phase_qualification(MemoirePartagee *mp, int nb_pilotes, const char *phase) {
    for (int tour = 1; tour <= NB_TOURS_QUALIF; tour++) {
        pid_t pid;
        for (int i = 0; i < nb_pilotes; i++) {
            pid = fork();
            if (pid == 0) {
                srand(getpid() + time(NULL));
                float temps_tour = generer_temps_tour(BASE_TEMPS, 5);

                sem_wait(&mp->mutex); // Section critique pour les écrivains
                mp->pilotes[i].dernier_temps_tour = temps_tour;
                if (mp->pilotes[i].temps_meilleur_tour == 0.0 || temps_tour < mp->pilotes[i].temps_meilleur_tour) {
                    mp->pilotes[i].temps_meilleur_tour = temps_tour;
                }
                sem_post(&mp->mutex); // Fin de la section critique
                exit(0);
            }
        }

        for (int i = 0; i < nb_pilotes; i++) {
            wait(NULL);
        }

        // Section critique pour les lecteurs
        sem_wait(&mp->mutLect);
        mp->nbrLect++;
        if (mp->nbrLect == 1) {
            sem_wait(&mp->mutex); // Premier lecteur bloque les écrivains
        }
        sem_post(&mp->mutLect);

        // Lecture
        tri_pilotes(mp->pilotes, nb_pilotes);
        afficher_resultats(mp->pilotes, nb_pilotes, phase);

        // Fin de la section critique pour les lecteurs
        sem_wait(&mp->mutLect);
        mp->nbrLect--;
        if (mp->nbrLect == 0) {
            sem_post(&mp->mutex); // Dernier lecteur débloque les écrivains
        }
        sem_post(&mp->mutLect);

        usleep(1000000); // Pause de 1 seconde entre les tours
    }
}

void qualification(MemoirePartagee *mp) {
    // Q1
    executer_phase_qualification(mp, NB_PILOTES, "Q1");
    printf("Éliminés après Q1 :\n");
    for (int i = 15; i < NB_PILOTES; i++) {
        printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);
    }

    // Q2
    executer_phase_qualification(mp, 15, "Q2");
    printf("Éliminés après Q2 :\n");
    for (int i = 10; i < 15; i++) {
        printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);
    }

    // Q3
    executer_phase_qualification(mp, 10, "Q3");
    printf("Résultats finaux de Q3 (Top 10) :\n");
    for (int i = 0; i < 10; i++) {
        printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);
    }
}

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

    sem_init(&mp->mutex, 1, 1);   // Initialisation du mutex pour protéger les écrivains
    sem_init(&mp->mutLect, 1, 1); // Initialisation de la protection des lecteurs
    mp->nbrLect = 0;              // Aucun lecteur actif au départ

    // Initialisation des pilotes
    for (int i = 0; i < NB_PILOTES; i++) {
        snprintf(mp->pilotes[i].nom, sizeof(mp->pilotes[i].nom), "Pilote %d", i + 1);
        mp->pilotes[i].temps_meilleur_tour = 0.0;
        mp->pilotes[i].dernier_temps_tour = 0.0;
    }

    const char *sessions[] = {"Essais Libres", "Qualifications", "Course"};
    int nb_tours[] = {NB_TOURS_ESSAIS, NB_TOURS_QUALIF, NB_TOURS_COURSE};

    for (int s = 0; s < 3; s++) {
        const char *session = sessions[s];
        int tours = nb_tours[s];

        for (int tour = 1; tour <= tours; tour++) {
            pid_t pid;
            for (int i = 0; i < NB_PILOTES; i++) {
                pid = fork();
                if (pid == 0) {
                    srand(getpid() + time(NULL));
                    float temps_tour = generer_temps_tour(BASE_TEMPS, 5);

                    sem_wait(&mp->mutex); // Section critique pour les écrivains
                    mp->pilotes[i].dernier_temps_tour = temps_tour;
                    if (mp->pilotes[i].temps_meilleur_tour == 0.0 || temps_tour < mp->pilotes[i].temps_meilleur_tour) {
                        mp->pilotes[i].temps_meilleur_tour = temps_tour;
                    }
                    sem_post(&mp->mutex); // Fin de la section critique
                    exit(0);
                }
            }

            // Processus parent : lit les données des fils
            for (int i = 0; i < NB_PILOTES; i++) {
                wait(NULL);
            }

            // Section critique pour les lecteurs
            sem_wait(&mp->mutLect);
            mp->nbrLect++;
            if (mp->nbrLect == 1) {
                sem_wait(&mp->mutex); // Premier lecteur bloque les écrivains
            }
            sem_post(&mp->mutLect);

            // Lecture
            tri_pilotes(mp->pilotes);
            afficher_resultats_en_temps_reel(mp->pilotes, tour, session);

            // Fin de la section critique pour les lecteurs
            sem_wait(&mp->mutLect);
            mp->nbrLect--;
            if (mp->nbrLect == 0) {
                sem_post(&mp->mutex); // Dernier lecteur débloque les écrivains
            }
            sem_post(&mp->mutLect);

            usleep(1000000); // Pause de 1 seconde entre les tours
        }
    }

    sem_destroy(&mp->mutex);
    sem_destroy(&mp->mutLect);

    if (shmdt(mp) == -1) {
        perror("Erreur de détachement de mémoire partagée");
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Erreur de destruction de mémoire partagée");
    }

    return 0;
}
