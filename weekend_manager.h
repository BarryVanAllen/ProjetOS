#ifndef WEEKEND_MANAGER_H
#define WEEKEND_MANAGER_H

#include <string>

// Constants representing different session types
enum SessionType {
    PRACTICE = 0,
    QUALIFICATION,
    SPRINT,
    RACE,
    MAX_SESSIONS
};

// Session names for display
extern const std::string session_names[MAX_SESSIONS];

// Struct to store the state of the weekend
struct WeekendState {
    bool session_completed[MAX_SESSIONS] = {false}; // Array to track completed sessions
    bool is_sprint_weekend = false; // Flag indicating if it's a sprint weekend
};

// Function declarations
bool can_proceed_to_session(int current_session, const bool session_completed[]);
void resume_weekend_from_file(const char *filename, WeekendState &state);
void coordinate_sessions(WeekendState &state);

#endif // WEEKEND_MANAGER_H
