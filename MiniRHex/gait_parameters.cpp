#include "gait_parameters.h"
#include "leg_info.h"
#include "param_storage.h"

void updateGaitParams(Gait & gait) {
  for (int i = 1; i < 7; ++i) {
    gait.sweep[i] = params.sweep;
    gait.down[i] = params.down;
    gait.period[i] = params.period * 1000;  // Seconds to milliseconds
    gait.duty[i] = params.duty / 100;       // Percent to fraction
  }
  gait.pronk = false;
  gait.kp = params.kp_walk;
  gait.kd = params.kd_walk;
  
  String pattern = String(params.gait);
  if (pattern == "bipod") {
    float phase[] = {0, 0, 0, 0.5, 0.5, 0.5};
    memcpy(gait.phase + 1, phase, sizeof(phase));
  } else if (pattern == "tripod") {
    float phase[] = {0, 0.5, 0, 0.5, 0, 0.5};
    memcpy(gait.phase + 1, phase, sizeof(phase));
  } else if (pattern == "tetrapod") {
    float phase[] = {0, 1.0/3, 2.0/3, 1.0/3, 2.0/3, 0};
    memcpy(gait.phase + 1, phase, sizeof(phase));
  } else if (pattern == "wave") {
    float phase[] = {0, 1.0/6, 2.0/6, 3.0/6, 4.0/6, 5.0/6};
    memcpy(gait.phase + 1, phase, sizeof(phase));
  } else if (pattern == "ripple") {
    float phase[] = {0, 2.0/6, 4.0/6, 5.0/6, 3.0/6, 1.0/6};
    memcpy(gait.phase + 1, phase, sizeof(phase));
  } else if (pattern == "metachronal") {
    float phase[] = {2.0/3, 1.0/3, 0, 2.0/3, 1.0/3, 0};
    memcpy(gait.phase + 1, phase, sizeof(phase));
  } else if (pattern == "bound") {
    float phase[] = {0, 0, 0.5, 0, 0, 0.5};
    memcpy(gait.phase + 1, phase, sizeof(phase));
    gait.down[2] = 180;
    gait.down[5] = 180;
    gait.duty[2] = 1;
    gait.duty[5] = 1;
    gait.sweep[2] = 1;
    gait.sweep[5] = 1;
    gait.pronk = true;
  } else if (pattern == "pronk") {
    float phase[] = {0, 0, 0, 0, 0, 0};
    memcpy(gait.phase + 1, phase, sizeof(phase));
    gait.pronk = true;
  } else {
    float phase[] = {0, 0, 0, 0, 0, 0};
    memcpy(gait.phase + 1, phase, sizeof(phase));    
  }
}

Gait walk_gait;
