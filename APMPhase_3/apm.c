#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <math.h>
#include <time.h>
#include <string.h>

#define GRAVITY 32.174 // ft/s^2
#define PI 3.14159
#define MAX_AIRPORTS 10
#define MAX_WAYPOINTS 20
#define MAX_ALTITUDE 40000
#define MIN_ALTITUDE 0
#define MAX_SPEED 500
#define MIN_SPEED 0
#define MAX_FUEL 10000
#define MAX_BANK_ANGLE 30
#define MIN_BANK_ANGLE 0


// Aircraft type definitions
typedef enum {
    AIRCRAFT_CESSNA,
    AIRCRAFT_BOEING737,
    AIRCRAFT_AIRBUS320
} AircraftType;

// Weather conditions
struct Weather {
    float wind_speed;      // knots
    float wind_direction;  // degrees
    float temperature;     // Celsius
    float pressure;       // hPa
    float visibility;     // nm
    int precipitation;    // 0=none, 1=rain, 2=snow
};

// Enhanced Airport structure
struct Airport {
    char name[20];
    char code[4];
    float lat;
    float lon;
    float elevation;      // ft
    float runway_length; // ft
    float runway_heading; // degrees
    int has_ils;         // 0 or 1
};

// Waypoint structure
struct Waypoint {
    char name[20];
    float lat;
    float lon;
    float altitude;      // ft
    float speed;        // knots
    int type;          // 0=airport, 1=fix, 2=vor, 3=ndb
};

// Enhanced FlightData structure
struct FlightData {
    float altitude;        // ft
    float speed;          // kt
    float fuel;           // gal
    float throttle;       // 0-1
    float heading;        // deg
    float bank_angle;     // deg
    float turn_radius;    // nm
    float distance_remaining; // nm
    float vertical_speed; // ft/min
    float ground_speed;   // kt
    float true_airspeed;  // kt
    float indicated_airspeed; // kt
    float mach_number;    // Mach
    float g_force;        // G
    float temperature;    // C
    float pressure;       // hPa
    float density_altitude; // ft
    float lat;           // latitude
    float lon;           // longitude
    float prev_altitude; // previous altitude for vertical speed calculation
    int phase;           // 0=Ground, 1=Takeoff, 2=Climb, 3=Cruise, 4=Descent, 5=Landing
    int flaps;           // 0-40 degrees
    int gear;            // 0=up, 1=down
    int autopilot;       // 0=off, 1=on
    int transponder;     // 0=off, 1=on
    AircraftType type;   // Aircraft type
};

// Aircraft performance parameters
struct AircraftPerformance {
    float max_speed;      // knots
    float max_altitude;   // ft
    float max_fuel;       // gal
    float empty_weight;   // lbs
    float max_weight;     // lbs
    float max_thrust;     // lbs
    float fuel_flow;      // gal/hr
    float stall_speed;    // knots
    float vne;           // knots
    float vno;           // knots
    float vfe;           // knots
};

// Global variables
struct Airport airports[MAX_AIRPORTS] = {
    {"Colombo (CMB)", "CMB", 6.9271, 79.8612, 7, 11000, 4, 1},
    {"Delhi (DEL)", "DEL", 28.6139, 77.2090, 777, 12500, 28, 1},
    {"Mumbai (BOM)", "BOM", 19.0887, 72.8679, 37, 11400, 9, 1},
    {"Bangalore (BLR)", "BLR", 13.1979, 77.7063, 3016, 12000, 9, 1},
    {"Chennai (MAA)", "MAA", 12.9900, 80.1634, 52, 12000, 7, 1},
    {"Kathmandu (KTM)", "KTM", 27.6966, 85.3591, 4390, 10000, 2, 0},
    {"Dhaka (DAC)", "DAC", 23.8433, 90.4082, 30, 10500, 14, 1},
    {"Singapore (SIN)", "SIN", 1.3644, 103.9915, 22, 13000, 2, 1},
    {"Kuala Lumpur (KUL)", "KUL", 2.7456, 101.7071, 69, 12400, 14, 1},
    {"Bangkok (BKK)", "BKK", 13.9125, 100.6068, 5, 12000, 19, 1}
};

struct Weather current_weather = {
    10.0,    // wind_speed
    270.0,   // wind_direction
    15.0,    // temperature
    1013.25, // pressure
    10.0,    // visibility
    0        // precipitation
};

// Function declarations
void initAircraftPerformance(AircraftType type, struct AircraftPerformance* perf);
void calculateAerodynamics(struct FlightData* plane, struct AircraftPerformance* perf);
void updateWeather(void);
void calculateWindEffect(struct FlightData* plane);
void updateNavigation(struct FlightData* plane, struct Waypoint* waypoints, int num_waypoints);
void checkFlightEnvelope(struct FlightData* plane, struct AircraftPerformance* perf);
void updateInstruments(struct FlightData* plane);
void logData(FILE* log, int time, struct FlightData plane, const char* dep, const char* dest);
void displayFlightInfo(struct FlightData* plane, int time);
void handleUserInput(struct FlightData* plane, struct AircraftPerformance* perf);
const char* getPhaseName(int phase);
float calculateDistance(float lat1, float lon1, float lat2, float lon2);
void initFlight(struct FlightData* plane, int dep_idx, int dest_idx);
void updateFlight(struct FlightData* plane);

// Initialize aircraft performance based on type
void initAircraftPerformance(AircraftType type, struct AircraftPerformance* perf) {
    switch(type) {
        case AIRCRAFT_CESSNA:
            perf->max_speed = 160;
            perf->max_altitude = 14000;
            perf->max_fuel = 56;
            perf->empty_weight = 1670;
            perf->max_weight = 2550;
            perf->max_thrust = 230;
            perf->fuel_flow = 10;
            perf->stall_speed = 47;
            perf->vne = 163;
            perf->vno = 126;
            perf->vfe = 85;
            break;
        case AIRCRAFT_BOEING737:
            perf->max_speed = 350;
            perf->max_altitude = 41000;
            perf->max_fuel = 6875;
            perf->empty_weight = 91000;
            perf->max_weight = 174200;
            perf->max_thrust = 27000;
            perf->fuel_flow = 2500;
            perf->stall_speed = 108;
            perf->vne = 350;
            perf->vno = 280;
            perf->vfe = 230;
            break;
        case AIRCRAFT_AIRBUS320:
            perf->max_speed = 350;
            perf->max_altitude = 39800;
            perf->max_fuel = 6875;
            perf->empty_weight = 93000;
            perf->max_weight = 170000;
            perf->max_thrust = 27000;
            perf->fuel_flow = 2500;
            perf->stall_speed = 108;
            perf->vne = 350;
            perf->vno = 280;
            perf->vfe = 230;
            break;
    }
}

// Calculate aerodynamic forces
void calculateAerodynamics(struct FlightData* plane, struct AircraftPerformance* perf) {
    float density_ratio = pow((288.15 - 0.0065 * plane->altitude) / 288.15, 4.256);
    float q = 0.5 * 0.002377 * density_ratio * pow(plane->true_airspeed * 1.68781, 2);
    
    // Basic lift calculation
    float cl = 1.0; // Simplified lift coefficient
    float wing_area = 174.0; // ft² (example for 737)
    float lift = q * cl * wing_area;
    
    // Basic drag calculation
    float cd = 0.02 + 0.1 * pow(cl, 2); // Simplified drag coefficient
    float drag = q * cd * wing_area;
    
    // Update aircraft state
    plane->g_force = lift / (perf->empty_weight * GRAVITY);
    plane->density_altitude = plane->altitude + (1013.25 - plane->pressure) * 30;
}

// Update weather conditions
void updateWeather(void) {
    // Simple random weather changes
    current_weather.wind_speed += (rand() % 3 - 1) * 0.5;
    current_weather.wind_direction += (rand() % 3 - 1) * 5.0;
    current_weather.temperature -= 0.0065 * 100; // Temperature lapse rate
    current_weather.pressure = 1013.25 * pow((288.15 - 0.0065 * 100) / 288.15, 5.256);
}

// Calculate wind effects
void calculateWindEffect(struct FlightData* plane) {
    float wind_x = current_weather.wind_speed * sin(current_weather.wind_direction * PI / 180.0);
    float wind_y = current_weather.wind_speed * cos(current_weather.wind_direction * PI / 180.0);
    
    float aircraft_x = plane->speed * sin(plane->heading * PI / 180.0);
    float aircraft_y = plane->speed * cos(plane->heading * PI / 180.0);
    
    float ground_x = aircraft_x + wind_x;
    float ground_y = aircraft_y + wind_y;
    
    plane->ground_speed = sqrt(ground_x * ground_x + ground_y * ground_y);
    plane->true_airspeed = plane->speed;
    plane->indicated_airspeed = plane->true_airspeed * sqrt(current_weather.pressure / 1013.25);
    plane->mach_number = plane->true_airspeed / 661.0; // Speed of sound at sea level
}

// Update navigation
void updateNavigation(struct FlightData* plane, struct Waypoint* waypoints, int num_waypoints) {
    // Basic navigation update
    float lat_rad = plane->lat * PI / 180.0;
    float lon_rad = plane->lon * PI / 180.0;
    
    // Update position based on ground speed and heading
    float distance_nm = plane->ground_speed / 3600.0;
    float heading_rad = plane->heading * PI / 180.0;
    
    plane->lat += (distance_nm * cos(heading_rad)) / 60.0;
    plane->lon += (distance_nm * sin(heading_rad)) / (60.0 * cos(lat_rad));
}

// Check flight envelope
void checkFlightEnvelope(struct FlightData* plane, struct AircraftPerformance* perf) {
    // Speed limits
    if (plane->speed > perf->vne) {
        printf("WARNING: Exceeding VNE!\n");
        plane->speed = perf->vne;
    }
    if (plane->speed < perf->stall_speed) {
        printf("WARNING: Below stall speed!\n");
        plane->speed = perf->stall_speed;
    }
    
    // Altitude limits
    if (plane->altitude > perf->max_altitude) {
        printf("WARNING: Exceeding maximum altitude!\n");
        plane->altitude = perf->max_altitude;
    }
    if (plane->altitude < MIN_ALTITUDE) {
        printf("WARNING: Below minimum altitude!\n");
        plane->altitude = MIN_ALTITUDE;
    }
    
    // G-force limits
    if (plane->g_force > 2.5) {
        printf("WARNING: High G-force!\n");
    }
}

// Update flight instruments
void updateInstruments(struct FlightData* plane) {
    // Update various flight instruments
    plane->vertical_speed = (plane->altitude - plane->prev_altitude) * 60.0;
    plane->prev_altitude = plane->altitude;
}

// Display flight information
void displayFlightInfo(struct FlightData* plane, int time) {
    system("cls"); // Clear screen
    printf("\n=== Aircraft Performance Monitor ===\n");
    printf("Time: %d s\n", time);
    printf("Phase: %s\n", getPhaseName(plane->phase));
    printf("\nFlight Parameters:\n");
    printf("Altitude: %.0f ft\n", plane->altitude);
    printf("Speed: %.0f kt (GS: %.0f kt)\n", plane->speed, plane->ground_speed);
    printf("Heading: %.0f°\n", plane->heading);
    printf("Bank Angle: %.0f°\n", plane->bank_angle);
    printf("Vertical Speed: %.0f ft/min\n", plane->vertical_speed);
    printf("Fuel: %.1f gal\n", plane->fuel);
    printf("Distance Remaining: %.0f nm\n", plane->distance_remaining);
    printf("\nAircraft State:\n");
    printf("Flaps: %d°\n", plane->flaps);
    printf("Gear: %s\n", plane->gear ? "DOWN" : "UP");
    printf("Autopilot: %s\n", plane->autopilot ? "ON" : "OFF");
    printf("Transponder: %s\n", plane->transponder ? "ON" : "OFF");
    printf("\nWeather:\n");
    printf("Wind: %.0f kt from %.0f°\n", current_weather.wind_speed, current_weather.wind_direction);
    printf("Temperature: %.1f°C\n", current_weather.temperature);
    printf("Pressure: %.1f hPa\n", current_weather.pressure);
    printf("\nControls:\n");
    printf("T - Throttle\n");
    printf("B - Bank Angle\n");
    printf("F - Flaps\n");
    printf("G - Gear\n");
    printf("A - Autopilot\n");
    printf("X - Transponder\n");
    printf("Q - Quit\n");
}

// Handle user input
void handleUserInput(struct FlightData* plane, struct AircraftPerformance* perf) {
    if (_kbhit()) {
        char input = _getch();
        switch (input) {
            case 't':
                printf("\nEnter throttle (0-1): ");
                scanf("%f", &plane->throttle);
                if (plane->throttle < 0 || plane->throttle > 1) plane->throttle = 0.8;
                break;
            case 'b':
                printf("\nEnter bank angle (0-30 deg): ");
                scanf("%f", &plane->bank_angle);
                if (plane->bank_angle < MIN_BANK_ANGLE || plane->bank_angle > MAX_BANK_ANGLE) 
                    plane->bank_angle = 15;
                break;
            case 'f':
                printf("\nEnter flaps (0-40 deg): ");
                scanf("%d", &plane->flaps);
                if (plane->flaps < 0 || plane->flaps > 40) plane->flaps = 0;
                break;
            case 'g':
                plane->gear = !plane->gear;
                break;
            case 'a':
                plane->autopilot = !plane->autopilot;
                break;
            case 'x':
                plane->transponder = !plane->transponder;
                break;
            case 'q':
                return;
        }
    }
}

const char* getPhaseName(int phase) {
    switch (phase) {
        case 0: return "Ground";
        case 1: return "Takeoff";
        case 2: return "Climb";
        case 3: return "Cruise";
        case 4: return "Descent";
        case 5: return "Landing";
        default: return "Unknown";
    }
}

float calculateDistance(float lat1, float lon1, float lat2, float lon2) {
    float dlat = (lat2 - lat1) * PI / 180.0;
    float dlon = (lon2 - lon1) * PI / 180.0;
    lat1 *= PI / 180.0; lat2 *= PI / 180.0;
    float a = sin(dlat / 2) * sin(dlat / 2) + cos(lat1) * cos(lat2) * sin(dlon / 2) * sin(dlon / 2);
    float c = 2 * atan2(sqrt(a), sqrt(1 - a));
    return 3440.0 * c;
}

void initFlight(struct FlightData* plane, int dep_idx, int dest_idx) {
    float distance = calculateDistance(airports[dep_idx].lat, airports[dep_idx].lon,
                                     airports[dest_idx].lat, airports[dest_idx].lon);
    plane->distance_remaining = distance;
    plane->altitude = airports[dep_idx].elevation;
    plane->prev_altitude = plane->altitude;
    plane->speed = 0;
    plane->fuel = distance * 3.0 + 500; // 3 gal/nm + reserve
    plane->throttle = 0.8;
    plane->heading = airports[dep_idx].runway_heading;
    plane->bank_angle = 0;
    plane->turn_radius = 0;
    plane->phase = 0; // Ground
    plane->lat = airports[dep_idx].lat;
    plane->lon = airports[dep_idx].lon;
    plane->flaps = 0;
    plane->gear = 1; // Start with gear down
    plane->autopilot = 0;
    plane->transponder = 1;
    plane->temperature = current_weather.temperature;
    plane->pressure = current_weather.pressure;
}

void updateFlight(struct FlightData* plane) {
    switch (plane->phase) {
        case 0: // Ground - wait for takeoff
            if (plane->speed > 0) {
                plane->speed -= 1.0; // Decelerate on ground
            }
            break;
            
        case 1: // Takeoff
            plane->speed += plane->throttle * 10.0;
            plane->fuel -= plane->throttle * 0.3;
            if (plane->speed > 120) {
                plane->phase = 2; // Lift off to climb
                plane->altitude += 50;
                plane->gear = 0; // Retract gear
            }
            break;
            
        case 2: // Climb
            plane->speed += plane->throttle * 2.0 - 0.5;
            plane->altitude += plane->speed * 0.5; // 500 ft/s climb rate
            plane->fuel -= plane->throttle * 0.2;
            plane->distance_remaining -= plane->speed / 3600.0;
            if (plane->altitude >= 30000) {
                plane->phase = 3; // Cruise
                plane->flaps = 0; // Retract flaps
            }
            break;
            
        case 3: // Cruise
            plane->speed += (plane->throttle * 400 - plane->speed) * 0.05;
            plane->altitude += (30000 - plane->altitude) * 0.1;
            plane->fuel -= plane->throttle * 0.15;
            plane->distance_remaining -= plane->speed / 3600.0;
            if (plane->distance_remaining < 100) {
                plane->phase = 4; // Descent
                plane->flaps = 10; // Deploy initial flaps
            }
            break;
            
        case 4: // Descent
            plane->speed += (plane->throttle * 300 - plane->speed) * 0.05;
            plane->altitude -= plane->speed * 0.4; // 400 ft/s descent
            plane->fuel -= plane->throttle * 0.1;
            plane->distance_remaining -= plane->speed / 3600.0;
            if (plane->altitude <= 5000) {
                plane->phase = 5; // Approach
                plane->flaps = 20; // Deploy more flaps
                plane->gear = 1; // Extend gear
            }
            break;
            
        case 5: // Landing
            plane->speed -= 5.0;
            plane->altitude -= plane->speed * 0.2;
            plane->fuel -= plane->throttle * 0.05;
            plane->distance_remaining -= plane->speed / 3600.0;
            if (plane->altitude <= 0) {
                plane->altitude = 0; // Landed
                plane->flaps = 0; // Retract flaps
            }
            break;
    }
}

void logData(FILE* log, int time, struct FlightData plane, const char* dep, const char* dest) {
    fprintf(log, "[%d s] Phase: %s, Alt: %.0f ft, Speed: %.0f kt (GS: %.0f kt), "
                "Heading: %.0f°, Bank: %.0f°, VS: %.0f ft/min, "
                "Fuel: %.1f gal, Dist: %.0f nm, "
                "Flaps: %d°, Gear: %s, AP: %s, XPDR: %s "
                "[%s -> %s]\n",
            time, getPhaseName(plane.phase),
            plane.altitude, plane.speed, plane.ground_speed,
            plane.heading, plane.bank_angle, plane.vertical_speed,
            plane.fuel, plane.distance_remaining,
            plane.flaps, plane.gear ? "DOWN" : "UP",
            plane.autopilot ? "ON" : "OFF",
            plane.transponder ? "ON" : "OFF",
            dep, dest);
}

// Main function
int main() {
    struct FlightData plane = {0};
    struct AircraftPerformance perf;
    int flight_time = 0, dep_idx, dest_idx;
    FILE* log = fopen("flight_log.txt", "w");
    if (!log) {
        printf("ERROR: Could not open flight_log.txt!\n");
        return 1;
    }

    // Initialize random seed
    srand(time(NULL));

    // Select aircraft type
    printf("Select Aircraft Type:\n");
    printf("0: Cessna 172\n");
    printf("1: Boeing 737\n");
    printf("2: Airbus A320\n");
    scanf("%d", &plane.type);
    initAircraftPerformance(plane.type, &perf);

    // Airport Selection
    printf("\nSelect Departure Airport (0-%d):\n", MAX_AIRPORTS - 1);
    for (int i = 0; i < MAX_AIRPORTS; i++) 
        printf("%d: %s (%s)\n", i, airports[i].name, airports[i].code);
    scanf("%d", &dep_idx);
    
    printf("\nSelect Destination Airport (0-%d, != %d):\n", MAX_AIRPORTS - 1, dep_idx);
    do {
        scanf("%d", &dest_idx);
    } while (dest_idx == dep_idx);

    // Initialize flight
    initFlight(&plane, dep_idx, dest_idx);
    printf("\nFlight Plan: %s to %s\n", airports[dep_idx].name, airports[dest_idx].name);
    printf("Distance: %.0f nm | Fuel: %.0f gal\n", plane.distance_remaining, plane.fuel);
    printf("Ready on runway. Request takeoff? (y/n): ");

    // Main flight loop
    while (plane.fuel > 0 && (plane.phase < 5 || plane.altitude > 0)) {
        handleUserInput(&plane, &perf);
        
        updateFlight(&plane);
        calculateAerodynamics(&plane, &perf);
        calculateWindEffect(&plane);
        updateNavigation(&plane, NULL, 0);
        checkFlightEnvelope(&plane, &perf);
        updateInstruments(&plane);
        updateWeather();
        
        displayFlightInfo(&plane, flight_time);
        logData(log, flight_time, plane, airports[dep_idx].name, airports[dest_idx].name);

        Sleep(1000);
        flight_time++;
    }

    printf("\nFlight Ended: %s\n", plane.altitude <= 0 ? "Landed" : "Fuel Out");
    fprintf(log, "[END] Alt: %.0f ft, Speed: %.0f kt, Fuel: %.1f gal, Dist Remain: %.0f nm\n",
            plane.altitude, plane.speed, plane.fuel, plane.distance_remaining);
    fclose(log);
    return 0;
}