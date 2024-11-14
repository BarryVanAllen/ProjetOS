#include <iostream>
using namespace std;

// Maximum number of drivers in the standings
const int MAX_DRIVERS = 20;

// Points system for a regular race
int race_points[] = {25, 18, 15, 12, 10, 8, 6, 4, 2, 1}; // Points for positions 1 to 10
// Points system for a sprint race
int sprint_points[] = {8, 7, 6, 5, 4, 3, 2, 1}; // Points for positions 1 to 8

// Array to store the driver standings (assumed to be global for simplicity)
int driver_standings[MAX_DRIVERS] = {0};

// Function to update driver standings based on finishing positions
void update_standings(int car_positions[], bool is_sprint) {
    int num_drivers = is_sprint ? 8 : 10; // Number of positions awarded points

    // Loop through the positions and award points
    for (int i = 0; i < num_drivers; ++i) {
        int car_id = car_positions[i];
        if (car_id >= 0 && car_id < MAX_DRIVERS) {
            if (is_sprint) {
                driver_standings[car_id] += sprint_points[i];
            } else {
                driver_standings[car_id] += race_points[i];
            }
        }
    }
}

// Function to award the fastest lap bonus point
void award_fastest_lap_bonus(int car_id) {
    // Check if the car is in the top 10 (i.e., if it has already been awarded points)
    if (car_id >= 0 && car_id < 10) {
        driver_standings[car_id] += 1; // Award the bonus point
    }
}

int main() {
    // Example usage
    int race_positions[MAX_DRIVERS] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9}; // Car positions in a regular race
    update_standings(race_positions, false); // Update standings for a regular race

    // Award fastest lap bonus to car ID 2
    award_fastest_lap_bonus(2);

    // Display updated standings
    cout << "Driver Standings:" << endl;
    for (int i = 0; i < MAX_DRIVERS; ++i) {
        cout << "Car " << i << ": " << driver_standings[i] << " points" << endl;
    }

    return 0;
}
