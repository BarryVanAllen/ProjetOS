#include "weekend_manager.h"
#include <stdio.h>
#include <stdbool.h>

#define MAX_SESSIONS 4

// Session names for display
const char *session_names[MAX_SESSIONS] = {"Practice", "Qualification", "Sprint", "Race"};

// Function to check if all previous sessions are complete before starting the next one
bool can_proceed_to_session(int current_session, const bool session_completed[]) {
    for (int i = 0; i < current_session; ++i) {
        if (!session_completed[i]) {
            return false; // A previous session is incomplete
        }
    }
    return true; // All previous sessions are complete
}

// Function to resume weekend state from a file
void resume_weekend_from_file(const char *filename, WeekendState *state) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        fprintf(stderr, "Error: Unable to open file %s\n", filename);
        return;
    }

    fscanf(file, "%d", &(state->is_sprint_weekend));
    for (int i = 0; i < MAX_SESSIONS; ++i) {
        int completed;
        fscanf(file, "%d", &completed);
        state->session_completed[i] = (completed != 0); // Convert int to bool
    }

    fclose(file);
}

// Function to coordinate the sessions in the correct order
void coordinate_sessions(WeekendState *state) {
    int max_sessions = state->is_sprint_weekend ? 3 : 4; // Sprint weekends skip the race session

    for (int i = 0; i < max_sessions; ++i) {
        if (can_proceed_to_session(i, state->session_completed)) {
            printf("Proceeding with %s session.\n", session_names[i]);
            state->session_completed[i] = true;
        } else {
            printf("Cannot proceed to %s session. Previous sessions incomplete.\n", session_names[i]);
            break;
        }
    }
}
