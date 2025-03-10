#ifndef gait_parameters
#define gait_parameters

struct Gait {
  float sweep[7]; // Sweep angle (degrees)
  float down[7];  // Down angle (degrees)
  int period[7];  // Period (milliseconds)
  float duty[7];  // Duty factor
  float phase[7]; // Phase offset
  bool pronk;     // Avoid recirculating leg
  float kp;       // Proportional gain (deg/ms^2)
  float kd;       // Derivative gain (deg/ms)
};

// Update gaits with the latest parameter values
void updateGaitParams(Gait & gait);

extern Gait walk_gait;

#endif
