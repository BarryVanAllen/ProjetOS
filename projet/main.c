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
#include "affichage.h"
#include "file_manager.h"
#include "types.h"

#define NB_TOURS_ESSAIS 20
#define NB_TOURS_QUALIF 15
#define NB_TOURS_COURSE 50
#define VARIATION_MIN -3000 // Minimum time variation in milliseconds
#define VARIATION_MAX 3000  // Maximum time variation in milliseconds
#define BASE_TEMPS_SEC 30.000

float generer_temps_secteur() {
    return BASE_TEMPS_SEC + ((rand() % (VARIATION_MAX - VARIATION_MIN + 1)) + VARIATION_MIN) / 1000.0;
}

void generer_temps_pilote(Pilote *pilote) {
    pilote->secteur_1 = generer_temps_secteur();
    pilote->secteur_2 = generer_temps_secteur();
    pilote->secteur_3 = generer_temps_secteur();
    pilote->dernier_temps_tour = pilote->secteur_1 + pilote->secteur_2 + pilote->secteur_3;
}

void tri_pilotes(Pilote pilotes[], int nb_pilotes) {
    for (int i = 0; i < nb_pilotes - 1; i++) {
        for (int j = 0; j < nb_pilotes - i - 1; j++) {
            if (pilotes[j].temps_meilleur_tour > pilotes[j + 1].temps_meilleur_tour) {
                Pilote temp = pilotes[j];
                pilotes[j] = pilotes[j + 1];
                pilotes[j + 1] = temp;
            }
        }
    }
}

void gestion_semaphore(MemoirePartagee *mp, int is_writer) {
    if (is_writer) {
        sem_wait(&mp->mutex); // Protection des écrivains
    } else {
        sem_wait(&mp->mutLect);
        mp->nbrLect++;
        if (mp->nbrLect == 1) sem_wait(&mp->mutex); // Premier lecteur bloque les écrivains
        sem_post(&mp->mutLect);
    }
}

void fin_gestion_semaphore(MemoirePartagee *mp, int is_writer) {
    if (is_writer) {
        sem_post(&mp->mutex); // Libération des écrivains
    } else {
        sem_wait(&mp->mutLect);
        mp->nbrLect--;
        if (mp->nbrLect == 0) sem_post(&mp->mutex); // Dernier lecteur débloque les écrivains
        sem_post(&mp->mutLect);
    }
}

void executer_tour(MemoirePartagee *mp, int nb_pilotes, const char *phase, int nb_tours) {
    pid_t pid;
    for (int tour = 1; tour <= nb_tours; tour++) {
        for (int i = 0; i < nb_pilotes; i++) {
            pid = fork();
            if (pid == 0) {
                srand(getpid() + time(NULL));
                gestion_semaphore(mp, 1); // Section critique pour les écrivains
                generer_temps_pilote(&mp->pilotes[i]);
                if (mp->pilotes[i].temps_meilleur_tour == 0.0 || mp->pilotes[i].dernier_temps_tour < mp->pilotes[i].temps_meilleur_tour) {
                    mp->pilotes[i].temps_meilleur_tour = mp->pilotes[i].dernier_temps_tour;
                }
                fin_gestion_semaphore(mp, 1);
                exit(0);
            }
        }
        for (int i = 0; i < nb_pilotes; i++) wait(NULL); // Attente des processus enfants
        gestion_semaphore(mp, 0); // Section critique pour les lecteurs
        tri_pilotes(mp->pilotes, nb_pilotes);
        afficher_resultats(mp->pilotes, nb_pilotes, phase);
        fin_gestion_semaphore(mp, 0); // Fin de la section critique
        usleep(1000000); // Pause de 1 seconde
    }
}

void free_practice(MemoirePartagee *mp, int repeat) {
    for (int i = 0; i < repeat; i++) {
        char session_name[14]; // "FP" + up to 3 digits + null terminator
        snprintf(session_name, sizeof(session_name), "FP%d", i + 1);
        printf("Début des essais libres (%s)\n", session_name);
        executer_tour(mp, NB_PILOTES, session_name, NB_TOURS_ESSAIS);
    }
}


void qualification(MemoirePartagee *mp) {
    const int pilotes_q1 = NB_PILOTES, pilotes_q2 = 15, pilotes_q3 = 10;
    printf("Début de la qualification\n");

    // Phase Q1
    executer_tour(mp, pilotes_q1, "Q1", NB_TOURS_QUALIF);
    printf("Éliminés après Q1 :\n");
    for (int i = pilotes_q2; i < NB_PILOTES; i++) printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);
    
    // Phase Q2
    executer_tour(mp, pilotes_q2, "Q2", NB_TOURS_QUALIF);
    printf("Éliminés après Q2 :\n");
    for (int i = pilotes_q3; i < pilotes_q2; i++) printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);
    
    // Phase Q3
    executer_tour(mp, pilotes_q3, "Q3", NB_TOURS_QUALIF);
    printf("Résultats finaux de Q3 (Top 10) :\n");
    for (int i = 0; i < pilotes_q3; i++) printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);
}

void course(MemoirePartagee *mp) {
    printf("Début de la course\n");
    executer_tour(mp, NB_PILOTES, "Course", NB_TOURS_COURSE);
}

void ecrire_resultats_csv(const char *filename, Pilote pilotes[], int nb_pilotes, const char *session) {
    char **data = malloc(nb_pilotes * sizeof(char *));
    for (int i = 0; i < nb_pilotes; i++) {
        char formatted_time[50];
        format_temps(pilotes[i].temps_meilleur_tour, formatted_time);
        data[i] = malloc(100 * sizeof(char)); // Exemple de taille
        snprintf(data[i], 100, "%s,%s,%f", pilotes[i].nom, session, pilotes[i].temps_meilleur_tour);
    }
    write_to_csv(filename, data, nb_pilotes, 0);
    for (int i = 0; i < nb_pilotes; i++) free((void *)data[i]);
    free(data);
}

int main() {
    // Initialisation de la mémoire partagée et des sémaphores
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
    sem_init(&mp->mutLect, 1, 1);  // Initialisation de la protection des lecteurs
    mp->nbrLect = 0;               // Aucun lecteur actif au départ

    // Initialisation des pilotes
    int count = 0;
    Pilote *pilotes = mp->pilotes;
    if (parse_csv_to_pilotes("pilotes.csv", &pilotes, &count) == 0) {
        for (int i = 0; i < count; i++) {
            // lock the mutex after printing
            gestion_semaphore(mp, 1); // Section critique pour les écrivains
            //fait ce que tu veux

            fin_gestion_semaphore(mp, 1);
            // Print the data from shared memory
            printf("Nom: %s, Num: %d, Temps Meilleur Tour: %.2f, Dernier Temps Tour: %.2f\n",
                   pilotes[i].nom, pilotes[i].num, pilotes[i].temps_meilleur_tour, pilotes[i].dernier_temps_tour);

            // Unlock the mutex after printing

        }
    }

    // Appel indépendant des différentes sessions
    free_practice(mp,3);  // FP1 , FP2, FP3
    qualification(mp);    // Qualification
    course(mp);           // Course

    // Nettoyage
    shmdt(mp);
    shmctl(shmid, IPC_RMID, NULL); // Suppression de la mémoire partagée
    sem_destroy(&mp->mutex);
    sem_destroy(&mp->mutLect);

    return 0;
}
