#include <stdio.h>
#include <math.h>
#include "types.h"


void afficher_resultats(Pilote pilotes[], int nb_pilotes, const char *phase) {
    printf("\033[H\033[J"); // Nettoie l'écran (optionnel)
    printf("--- Résultats : %s ---\n", phase);
    printf("\n┌───────────┬────────────┬────────────┬────────────┬────────────┬────────────┬────────────┬──────────────────┬───────────┐\n");
    printf("| Position  | Num        | S1         | S2         | S3         | Lap        | Lap Time   | Best Lap Time    | Ecart     |\n");
    printf("├───────────┼────────────┼────────────┼────────────┼────────────┼────────────┼────────────┼──────────────────┼───────────┤\n");

    for (int i = 0; i < nb_pilotes; i++) {
        // Calcul des minutes et secondes pour le dernier temps et le meilleur tour
        int minutes = (int)(pilotes[i].dernier_temps_tour / 60);
        float seconds = pilotes[i].dernier_temps_tour - (minutes * 60);
        
        int best_minutes = (int)(pilotes[i].temps_meilleur_tour / 60);
        float best_seconds = pilotes[i].temps_meilleur_tour - (best_minutes * 60);

        // Affichage des données alignées avec des largeurs fixes
        printf("│ %-10d│ %-11d│ %-11.3f│ %-11.3f│ %-11.3f│ %d:%07.3f  │ %-11.3f│ %d:%07.3f       │ %-10.3f │\n",
               i + 1,                     // Position
               pilotes[i].num,            // Numéro du pilote
               pilotes[i].secteur_1,      // Secteur 1
               pilotes[i].secteur_2,      // Secteur 2
               pilotes[i].secteur_3,      // Secteur 3
               minutes, seconds,          // Dernier temps au format MM:SS.mmm
               pilotes[i].dernier_temps_tour, // Temps total du dernier tour
               best_minutes, best_seconds, // Meilleur temps au format MM:SS.mmm
               pilotes[i].temps_meilleur_tour); // Écart (si calculé)
    }

    printf("└───────────┴────────────┴────────────┴────────────┴────────────┴────────────┴────────────┴──────────────────┴───────────┘\n");
}


// Function to convert time in seconds to minutes:seconds:milliseconds
void format_temps(float seconds, char *formatted_time) {
    int minutes = (int)(seconds / 60);
    seconds = fmod(seconds, 60);
    int milliseconds = (int)((seconds - (int)seconds) * 1000);
    sprintf(formatted_time, "%02d:%05.2f", minutes, seconds + milliseconds / 1000.0);
}
