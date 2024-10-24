#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include "config.h"

int secteur(int *voiture);

int main()
{
    int voiture = 4;
    
    secteur(voiture);
}

int secteur(int *voiture)
{
    srand(time (0));
    int i;
    double total = 0.0;
    int minutes = 0;
    double secondes = 0.0;

for(i=0; i<3; i++)
{
    double temps = 30.0 + (rand() / (double)RAND_MAX) * (50.0 - 40.0);
    printf("Secteur %d : %.3f secondes\n", i+1 , temps);

    total += temps;

}

    minutes = total/60;
    secondes = fmod(total, 60);
    //Afficher du temps total pour un tour (lap)
    printf("Voiture %d : temps du tour (lap) : %.0d:%.3f secondes\n", voiture, minutes, secondes);
}

int practice()
{
    

}
