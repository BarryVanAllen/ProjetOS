#include <stdlib.h>
#include "utils.h"

float generer_temps_tour(float base_temps, int difficulte) {
    float variation = (rand() % 500) / 1000.0;
    float coefficient_difficulte = 1.0 + (difficulte * 0.01);
    return base_temps * coefficient_difficulte + variation;
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

int calculNbTour(int tailleCircuit){
  int res = NB_KM / tailleCircuit;
  return res;
}

int calculNbTourSprint(int tailleCircuit){
  int rest = 100/ tailleCircuit;
  return rest;
}
