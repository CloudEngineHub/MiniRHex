#include "desired_values.h"
#include <math.h>
#include <fstream>

Vals get_desired_vals_internal(int t, Leg l){
  // Assume t has been adjusted for phasing

  int sign = l.sign[l.gait];
  float theta;
  float velocity;

  if (t < l.ts[1]){
    theta = l.thetas[0] + t * l.recovery_speed;
    velocity = l.recovery_speed;
  }
  else if (t < l.ts[2]){
    theta = l.thetas[1] + (t - l.ts[1]) * l.ground_speed;
    velocity = l.ground_speed;
  }
  else if (t < l.ts[3]){
    theta = l.thetas[2] + (t - l.ts[2]) * l.ground_speed;
    velocity = l.ground_speed;
  }
  else{
    theta = l.thetas[3] + (t - l.ts[3]) * l.recovery_speed;
    velocity = l.recovery_speed;
  }

  theta = fmod(theta + 180.0, 360.0) - 180.0; // wrap thetas
  theta = theta * sign;
  velocity = velocity * sign;
  Vals result = {theta, velocity};
  return result;
}

Vals get_desired_vals(int t, Leg l) {
  // Handles phasing and start time
  int elapsed_time = t - l.startMillis;
  t = fmodf(elapsed_time - l.phase * l.period, l.period);
  return get_desired_vals_internal(t, l);
}
