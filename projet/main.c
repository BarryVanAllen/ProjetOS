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

int main(int argc, char *argv[]) {
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
    sem_init(&mp->mutLect, 1, 1); // Initialisation de la protection des lecteurs
    mp->nbrLect = 0;              // Aucun lecteur actif au départ

    // Initialisation des pilotes
    int count = 0;
    Pilote *pilotes = mp->pilotes;
    if (parse_csv_to_pilotes("pilotes.csv", &pilotes, &count) == 0) {
        gestion_semaphore(mp, 1); // Section critique pour les écrivains
        for (int i = 0; i < count; i++) {
            // Copier les données des pilotes dans la mémoire partagée
            strcpy(mp->pilotes[i].nom, pilotes[i].nom);
            mp->pilotes[i].num = pilotes[i].num;
            mp->pilotes[i].temps_meilleur_tour = pilotes[i].temps_meilleur_tour;
            mp->pilotes[i].dernier_temps_tour = pilotes[i].dernier_temps_tour;
            mp->pilotes[i].temps_course_total = 0.0;
            mp->pilotes[i].secteur_1 = 0.0;
            mp->pilotes[i].secteur_2 = 0.0;
            mp->pilotes[i].secteur_3 = 0.0;
        }
        fin_gestion_semaphore(mp, 1);
    }

    // Menu de sélection basé sur les arguments de ligne de commande
    if (argc != 2) {
        printf("Usage: %s [session]\n", argv[0]);
        printf("Sessions disponibles :\n");
        printf("  fp1   : Free Practice 1\n");
        printf("  fp2   : Free Practice 2\n");
        printf("  fp3   : Free Practice 3\n");
        printf("  q1    : Qualification Phase 1 (Q1)\n");
        printf("  q2    : Qualification Phase 2 (Q2)\n");
        printf("  q3    : Qualification Phase 3 (Q3)\n");
        printf("  qualif: Qualification complète (Q1, Q2, Q3)\n");
        printf("  race  : Course\n");
        printf("  all   : Tout exécuter (Essais libres, qualifications, course)\n");
        shmdt(mp);
        shmctl(shmid, IPC_RMID, NULL);
        sem_destroy(&mp->mutex);
        sem_destroy(&mp->mutLect);
        return 1;
    }

    // Exécution de la session spécifiée
    if (strcmp(argv[1], "fp1") == 0) {
        printf("Début de Free Practice 1\n");
        executer_tour(mp, NB_PILOTES, "FP1", NB_TOURS_ESSAIS);
    } else if (strcmp(argv[1], "fp2") == 0) {
        printf("Début de Free Practice 2\n");
        executer_tour(mp, NB_PILOTES, "FP2", NB_TOURS_ESSAIS);
    } else if (strcmp(argv[1], "fp3") == 0) {
        printf("Début de Free Practice 3\n");
        executer_tour(mp, NB_PILOTES, "FP3", NB_TOURS_ESSAIS);
    } else if (strcmp(argv[1], "q1") == 0) {
        printf("Début de Qualification Phase 1 (Q1)\n");
        executer_tour(mp, NB_PILOTES, "Q1", NB_TOURS_QUALIF);
        printf("Éliminés après Q1 :\n");
        for (int i = 15; i < NB_PILOTES; i++) printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);
    } else if (strcmp(argv[1], "q2") == 0) {
        printf("Début de Qualification Phase 2 (Q2)\n");
        executer_tour(mp, 15, "Q2", NB_TOURS_QUALIF);
        printf("Éliminés après Q2 :\n");
        for (int i = 10; i < 15; i++) printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);
    } else if (strcmp(argv[1], "q3") == 0) {
        printf("Début de Qualification Phase 3 (Q3)\n");
        executer_tour(mp, 10, "Q3", NB_TOURS_QUALIF);
        printf("Résultats finaux de Q3 (Top 10) :\n");
        for (int i = 0; i < 10; i++) printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);
    } else if (strcmp(argv[1], "qualif") == 0) {
        printf("Début de la Qualification complète\n");
        qualification(mp);
    } else if (strcmp(argv[1], "race") == 0) {
        printf("Début de la Course\n");
        course(mp);
    } else if (strcmp(argv[1], "all") == 0) {
        printf("Début du programme complet : Essais libres, Qualifications, Course\n");

        // Exécution des essais libres
        printf("\n--- Free Practice 1 ---\n");
        executer_tour(mp, NB_PILOTES, "FP1", NB_TOURS_ESSAIS);
        printf("\n--- Free Practice 2 ---\n");
        executer_tour(mp, NB_PILOTES, "FP2", NB_TOURS_ESSAIS);
        printf("\n--- Free Practice 3 ---\n");
        executer_tour(mp, NB_PILOTES, "FP3", NB_TOURS_ESSAIS);

        // Exécution des qualifications
        printf("\n--- Qualification Phase 1 (Q1) ---\n");
        executer_tour(mp, NB_PILOTES, "Q1", NB_TOURS_QUALIF);
        printf("Éliminés après Q1 :\n");
        for (int i = 15; i < NB_PILOTES; i++) printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);

        printf("\n--- Qualification Phase 2 (Q2) ---\n");
        executer_tour(mp, 15, "Q2", NB_TOURS_QUALIF);
        printf("Éliminés après Q2 :\n");
        for (int i = 10; i < 15; i++) printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);

        printf("\n--- Qualification Phase 3 (Q3) ---\n");
        executer_tour(mp, 10, "Q3", NB_TOURS_QUALIF);
        printf("Résultats finaux de Q3 (Top 10) :\n");
        for (int i = 0; i < 10; i++) printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);

        // Exécution de la course
        printf("\n--- Course ---\n");
        course(mp);
    } else {
        printf("Session inconnue : %s\n", argv[1]);
        printf("Sessions disponibles : fp1, fp2, fp3, q1, q2, q3, qualif, race, all\n");
    }

    // Nettoyage
    shmdt(mp);
    shmctl(shmid, IPC_RMID, NULL); // Suppression de la mémoire partagée
    sem_destroy(&mp->mutex);
    sem_destroy(&mp->mutLect);

    return 0;
}

