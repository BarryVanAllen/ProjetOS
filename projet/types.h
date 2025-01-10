// types.h
#include <semaphore.h>
#ifndef TYPES_H
#define TYPES_H
#define NB_PILOTES 20

typedef struct {
    char nom[50];
    int num;
    float temps_meilleur_tour;
    float dernier_temps_tour;
    float temps_course_total;
    float secteur_1;
    float secteur_2;
    float secteur_3;
} Pilote;

typedef struct {
    Pilote pilotes[NB_PILOTES];
    int nbrLect;             // Nombre de lecteurs en cours
    sem_t mutex;             // Protection des écrivains
    sem_t mutLect;           // Protection du compteur de lecteurs
} MemoirePartagee;

#endif // TYPES_H

