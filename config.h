#ifndef CONFIG_H_INCLUDED
#define CONFIG_H_INCLUDED
#include <stdbool.h>

typedef struct circuits circuits;
struct circuits
{
    char nom[100];
    bool sprint;
};

#endif // CONFIG_H_INCLUDED
