#include <stdio.h>
#include <string.h>
#include "sessions.h"
#include "affichage.h"
#include "types.h"
#include "file_manager.h"
#define NB_TOURS_ESSAIS 20
#define NB_TOURS_QUALIF 15
#define NB_TOURS_COURSE 50



// Fonction d'exécution des sessions
void traiter_session(const char *session, MemoirePartagee *mp) {
    // temp call to test
    executer_tour(mp, NB_PILOTES, "FP1", NB_TOURS_ESSAIS);
    return
    if (strcmp(session, "fp1") == 0) {
        printf("Début de Free Practice 1\n");
        executer_tour(mp, NB_PILOTES, "FP1", NB_TOURS_ESSAIS);
    } else if (strcmp(session, "fp2") == 0) {
        printf("Début de Free Practice 2\n");
        executer_tour(mp, NB_PILOTES, "FP2", NB_TOURS_ESSAIS);
    } else if (strcmp(session, "fp3") == 0) {
        printf("Début de Free Practice 3\n");
        executer_tour(mp, NB_PILOTES, "FP3", NB_TOURS_ESSAIS);
    } else if (strcmp(session, "q1") == 0) {
        printf("Début de Qualification Phase 1 (Q1)\n");
        executer_tour(mp, NB_PILOTES, "Q1", NB_TOURS_QUALIF);
    } else if (strcmp(session, "q2") == 0) {
        printf("Début de Qualification Phase 2 (Q2)\n");
        executer_tour(mp, 15, "Q2", NB_TOURS_QUALIF);
    } else if (strcmp(session, "q3") == 0) {
        printf("Début de Qualification Phase 3 (Q3)\n");
        executer_tour(mp, 10, "Q3", NB_TOURS_QUALIF);
    } else if (strcmp(session, "qualif") == 0) {
        printf("Début de la Qualification complète\n");
        qualification(mp);
    } else if (strcmp(session, "race") == 0) {
        printf("Début de la Course\n");
        course(mp);
    } else if (strcmp(session, "all") == 0) {
        printf("Début du programme complet : Essais libres, Qualifications, Course\n");
        executer_toutes_les_sessions(mp);
    } else {
        printf("Session inconnue : %s\n", session);
        printf("Sessions disponibles : fp1, fp2, fp3, q1, q2, q3, qualif, race, all\n");
    }
}

// Fonction pour afficher les éliminés
void afficher_eliminés(MemoirePartagee *mp, int start, int end) {
    printf("Éliminés après la session :\n");
    for (int i = start; i < end; i++) {
        printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);
    }
}

// Fonction pour afficher le top 10
void afficher_top_10(MemoirePartagee *mp, int top) {
    printf("Résultats finaux (Top 10) :\n");
    for (int i = 0; i < top; i++) {
        printf("%d. Pilote: %s\n", i + 1, mp->pilotes[i].nom);
    }
}

// Fonction pour exécuter toutes les sessions
void executer_toutes_les_sessions(MemoirePartagee *mp) {
    printf("\n--- Free Practice 1 ---\n");
    free_practice(mp, 1);
    printf("\n--- Free Practice 2 ---\n");
    free_practice(mp, 1);
    printf("\n--- Free Practice 3 ---\n");
    free_practice(mp, 1);

    printf("\n--- Qualification Phase 1 (Q1) ---\n");
    executer_tour(mp, NB_PILOTES, "Q1", NB_TOURS_QUALIF);
    afficher_eliminés(mp, 15, NB_PILOTES);

    printf("\n--- Qualification Phase 2 (Q2) ---\n");
    executer_tour(mp, 15, "Q2", NB_TOURS_QUALIF);
    afficher_eliminés(mp, 10, 15);

    printf("\n--- Qualification Phase 3 (Q3) ---\n");
    executer_tour(mp, 10, "Q3", NB_TOURS_QUALIF);
    afficher_top_10(mp, 10);

    printf("\n--- Course ---\n");
    course(mp);
}
