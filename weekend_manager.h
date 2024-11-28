#ifndef WEEKEND_MANAGER_H
#define WEEKEND_MANAGER_H

#include <stdbool.h> // For the bool type

// Constants representing different session types
typedef enum {
    PRACTICE = 0,
    QUALIFICATION,
    SPRINT,
    RACE,
    MAX_SESSIONS
} SessionType;

// Session names for display
extern const char *session_names[MAX_SESSIONS];

// Struct to store the state of the weekend
typedef struct {
    bool session_completed[MAX_SESSIONS]; // Array to track completed sessions
    bool is_sprint_weekend;               // Flag indicating if it's a sprint weekend
} WeekendState;

// Function declarations
bool can_proceed_to_session(int current_session, const bool session_completed[]);
void resume_weekend_from_file(const char *filename, WeekendState *state);
void coordinate_sessions(WeekendState *state);

#endif // WEEKEND_MANAGER_H
