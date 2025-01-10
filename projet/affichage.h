#ifndef AFFICHAGE_H
#define AFFICHAGE_H
#include "types.h"

void afficher_resultats_en_temps_reel(Pilote pilotes[], int tour, const char *session);
void afficher_resultats(Pilote pilotes[], int nb_pilotes, const char *phase);
void format_temps(float seconds, char *formatted_time);

#endif
