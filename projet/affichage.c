#include <stdio.h>
#include <math.h>
#include "types.h"

// Fonction pour trouver le pilote le plus rapide dans chaque secteur
void afficher_meilleurs_secteurs(Pilote pilotes[], int nb_pilotes) {
    Pilote meilleur_secteur_1 = pilotes[0];
    Pilote meilleur_secteur_2 = pilotes[0];
    Pilote meilleur_secteur_3 = pilotes[0];

    for (int i = 1; i < nb_pilotes; i++) {
        if (pilotes[i].secteur_1 < meilleur_secteur_1.secteur_1) {
            meilleur_secteur_1 = pilotes[i];
        }
        if (pilotes[i].secteur_2 < meilleur_secteur_2.secteur_2) {
            meilleur_secteur_2 = pilotes[i];
        }
        if (pilotes[i].secteur_3 < meilleur_secteur_3.secteur_3) {
            meilleur_secteur_3 = pilotes[i];
        }
    }

    printf("\n--- Meilleurs Secteurs ---\n");
    printf("┌─────────────┬────────────┬────────────┐\n");
    printf("| Secteur     | Pilote     | Temps      |\n");
    printf("├─────────────┼────────────┼────────────┤\n");
    // Affichage des meilleurs secteurs
    printf("| Secteur 1   | %-10d | %-11.3f|\n", meilleur_secteur_1.num, meilleur_secteur_1.secteur_1);
    printf("| Secteur 2   | %-10d | %-11.3f|\n", meilleur_secteur_2.num, meilleur_secteur_2.secteur_2);
    printf("| Secteur 3   | %-10d | %-11.3f|\n", meilleur_secteur_3.num, meilleur_secteur_3.secteur_3);
    printf("└─────────────┴────────────┴────────────┘\n");
}

void afficher_resultats(Pilote pilotes[], int nb_pilotes, const char *phase) {
    printf("\033[H\033[J"); // Nettoie l'écran (optionnel)
    printf("--- Résultats : %s ---\n", phase);
    printf("\n┌───────────┬────────────┬────────────┬────────────┬────────────┬────────────┬────────────┬──────────────────┬───────────┐\n");
    printf("| Position  | Num        | S1         | S2         | S3         | Lap Time   | Lap        | Best Lap Time    | Ecart     |\n");
    printf("├───────────┼────────────┼────────────┼────────────┼────────────┼────────────┼────────────┼──────────────────┼───────────┤\n");

    for (int i = 0; i < nb_pilotes; i++) {
        // Calcul des minutes et secondes pour le dernier temps et le meilleur tour
        int minutes = (int)(pilotes[i].dernier_temps_tour / 60);
        float seconds = pilotes[i].dernier_temps_tour - (minutes * 60);
        
        int best_minutes = (int)(pilotes[i].temps_meilleur_tour / 60);
        float best_seconds = pilotes[i].temps_meilleur_tour - (best_minutes * 60);

        // Affichage des données alignées avec des largeurs fixes
        printf("│ %-10d│ %-11d│ %-11.3f│ %-11.3f│ %-11.3f│  %d:%06.3f  │ %-11d│   %d:%06.3f       │ %-9.3f │\n",
               i + 1,                     // Position
               pilotes[i].num,            // Numéro du pilote
               pilotes[i].secteur_1,      // Temps secteur 1
               pilotes[i].secteur_2,      // Temps secteur 2
               pilotes[i].secteur_3,      // Temps secteur 3
               minutes, seconds,          // Dernier temps tour (minutes et secondes)
               pilotes[i].num_tour,       // Nombre total de tours
               best_minutes, best_seconds,// Meilleur temps tour (minutes et secondes)
               pilotes[i].temps_meilleur_tour); // Temps total du meilleur tour
    }

    printf("└───────────┴────────────┴────────────┴────────────┴────────────┴────────────┴────────────┴──────────────────┴───────────┘\n");

    // Affichage des meilleurs temps dans chaque secteur
    afficher_meilleurs_secteurs(pilotes, nb_pilotes);
}


// Function to convert time in seconds to minutes:seconds:milliseconds
void format_temps(double temps_en_secondes, char *resultat) {
    int minutes = (int)(temps_en_secondes / 60); // Extraire les minutes
    int secondes = (int)(temps_en_secondes) % 60; // Extraire les secondes
    int millisecondes = (int)((temps_en_secondes - (int)temps_en_secondes) * 1000); // Extraire les millisecondes

    // Formater le temps
    sprintf(resultat, "%d:%02d:%03d", minutes, secondes, millisecondes);
}

void menu(char *program_name) {
    printf("Usage: %s [session]\n", program_name);
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
}
