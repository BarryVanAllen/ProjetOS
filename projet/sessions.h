#ifndef SESSIONS_H
#define SESSIONS_H

#include "types.h"

// Déclare les fonctions utilisées dans sessions.c
void traiter_session(const char *session, MemoirePartagee *mp);
void afficher_eliminés(MemoirePartagee *mp, int start, int end);
void afficher_top_10(MemoirePartagee *mp, int top);
void executer_toutes_les_sessions(MemoirePartagee *mp);
void executer_tour(MemoirePartagee *mp, int nb_pilotes, char *phase, int nb_tours);
void free_practice(MemoirePartagee *mp, int repeat);
void qualification(MemoirePartagee *mp);
void course(MemoirePartagee *mp);
void afficher_menu();
#endif
