#ifndef param_storage
#define param_storage

#include <Arduino.h>

struct Params {
  float period = 2;         // Amount of time to move all of the legs (seconds)
  float duty = 42;          // Amount of time each leg is on the ground (percent)
  float sweep = 40;         // Amount each leg rotates while on the ground (degrees)
  float down = 20;          // Average angle of each leg while on the ground (degrees)
  char gait[21] = "tripod"; // Name of the gait pattern (20 characters max)
  float kp_walk = 0.05;     // Stiffness of the motors while walking
  float kd_walk = 1.0;      // Damping of the motors while walking
  float kp_stand = 0.008;   // Stiffness of the motors while standing
  float kd_stand = 1.0;     // Damping of the motors while standing

  // New parameters must be added here and in the parseParams and subParams functions
};

// Update parameters from a string with format "param1=value1&param2=value2&..."
void parseParams(const String & s, int index = 0);

// Replace instances of {{parameter}} in a string with current parameter values
void subParams(String & s);

// Reset parameters to hardcoded default values
void resetParams(int index = 0);

// Load parameters from flash memory
void loadParams(int index = 0);

// Save parameters to flash memory (warning: memory resets when uploading a sketch)
void saveParams(int index = 0);

extern Params params;

#endif
