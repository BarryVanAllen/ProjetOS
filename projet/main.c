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
    sem_t semaphore_fils;
    sem_t semaphore_pere;
} MemoirePartagee;

float generer_temps_tour(float base_temps, int difficulte) {
    float variation = (rand() % 500) / 1000.0;
    float coefficient_difficulte = 1.0 + (difficulte * 0.01);
    return base_temps * coefficient_difficulte + variation;
}

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

void afficher_resultats_en_temps_reel(Pilote pilotes[], int tour, const char *session) {
    printf("\033[H\033[J");
    printf("--- Session : %s | Tour : %d ---\n", session, tour);
    printf("Classement actuel :\n");
    for (int i = 0; i < NB_PILOTES; i++) {
        printf("%d. Pilote: %s | Meilleur temps: %.3f sec | Dernier tour: %.3f sec\n",
               i + 1, pilotes[i].nom, pilotes[i].temps_meilleur_tour, pilotes[i].dernier_temps_tour);
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

    sem_init(&mp->semaphore_fils, 1, 0);
    sem_init(&mp->semaphore_pere, 1, NB_PILOTES);

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

                    sem_wait(&mp->semaphore_pere);
                    mp->pilotes[i].dernier_temps_tour = temps_tour;
                    if (mp->pilotes[i].temps_meilleur_tour == 0.0 || temps_tour < mp->pilotes[i].temps_meilleur_tour) {
                        mp->pilotes[i].temps_meilleur_tour = temps_tour;
                    }
                    sem_post(&mp->semaphore_fils);
                    exit(0);
                }
            }

            for (int i = 0; i < NB_PILOTES; i++) {
                sem_wait(&mp->semaphore_fils);
            }

            tri_pilotes(mp->pilotes);
            afficher_resultats_en_temps_reel(mp->pilotes, tour, session);

            usleep(1000000); // Pause de 1 seconde entre les tours

            for (int i = 0; i < NB_PILOTES; i++) {
                sem_post(&mp->semaphore_pere);
            }
        }
    }

    sem_destroy(&mp->semaphore_fils);
    sem_destroy(&mp->semaphore_pere);

    if (shmdt(mp) == -1) {
        perror("Erreur de détachement de mémoire partagée");
    }
    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("Erreur de destruction de mémoire partagée");
    }

    return 0;
}
