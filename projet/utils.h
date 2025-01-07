#ifndef UTILS_H
#define UTILS_H

typedef struct {
    char nom[50];
    float temps_meilleur_tour;
    float dernier_temps_tour;
} Pilote;

typedef struct {
    Pilote pilotes[20];
} MemoirePartagee;

float generer_temps_tour(float base_temps, int difficulte);
void tri_pilotes(Pilote pilotes[], int nb_pilotes);

const int NB_KM = 250;

//Calcule le nombre de tour d'une course
int calculNbTour(int tailleCircuit);

//Calcule le nombre de tour de la course du dimanche
int calculNbTourSprint(int tailleCircuit);

#endif
