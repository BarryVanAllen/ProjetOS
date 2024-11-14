#include "weekend_manager.h"
#include <iostream>
#include <fstream>
using namespace std;

// Session names for display
const string session_names[MAX_SESSIONS] = {"Practice", "Qualification", "Sprint", "Race"};

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
void resume_weekend_from_file(const char *filename, WeekendState &state) {
    ifstream file(filename);
    if (!file.is_open()) {
        cerr << "Error: Unable to open file " << filename << endl;
        return;
    }

    file >> state.is_sprint_weekend;
    for (int i = 0; i < MAX_SESSIONS; ++i) {
        file >> state.session_completed[i];
    }
    file.close();
}

// Function to coordinate the sessions in the correct order
void coordinate_sessions(WeekendState &state) {
    int max_sessions = state.is_sprint_weekend ? 3 : 4; // Sprint weekends skip the race session

    for (int i = 0; i < max_sessions; ++i) {
        if (can_proceed_to_session(i, state.session_completed)) {
            cout << "Proceeding with " << session_names[i] << " session." << endl;
            state.session_completed[i] = true;
        } else {
            cout << "Cannot proceed to " << session_names[i] << " session. Previous sessions incomplete." << endl;
            break;
        }
    }
}
