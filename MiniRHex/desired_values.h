#ifndef desired_values
#define desired_values

#include "leg_info.h"

struct Vals {
  float global_theta;
  float global_velocity;
};

// Compute controller setpoints for a given leg at a specified clock time
Vals get_desired_vals(int t, Leg l);

#endif
