#include <stdio.h>
#include "types.h"

void afficher_resultats_en_temps_reel(Pilote pilotes[], int tour, const char *session) {
    printf("\033[H\033[J");
    printf("--- Session : %s | Tour : %d ---\n", session, tour);
    printf("Classement actuel :\n");
    for (int i = 0; i < 20; i++) {
        printf("%d. Pilote: %s | Meilleur temps: %.3f sec | Dernier tour: %.3f sec\n",
               i + 1, pilotes[i].nom, pilotes[i].temps_meilleur_tour, pilotes[i].dernier_temps_tour);
    }
}

void afficher_resultats(Pilote pilotes[], int nb_pilotes, const char *phase) {
    printf("\033[H\033[J");
    printf("--- Résultats : %s ---\n", phase);
    for (int i = 0; i < nb_pilotes; i++) {
        int minutes = (int)(pilotes[i].temps_meilleur_tour / 60);
        float seconds = pilotes[i].temps_meilleur_tour - (minutes * 60);
        printf("%d. Pilote: %s | Meilleur temps: %d:%.3f\n",
               i + 1, pilotes[i].nom, minutes, seconds);
    }
}

// Function to convert time in seconds to minutes:seconds:milliseconds
void format_temps(float seconds, char *formatted_time) {
    int minutes = (int)(seconds / MINUTES_IN_SEC);
    seconds = fmod(seconds, MINUTES_IN_SEC);
    int milliseconds = (int)((seconds - (int)seconds) * 1000);
    sprintf(formatted_time, "%02d:%05.2f", minutes, seconds + milliseconds / 1000.0);
}
