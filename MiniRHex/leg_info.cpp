#include "leg_info.h"
#include "gait_parameters.h"
#include <fstream>

const int STAND = 0;
const int WALK = 1;
const int LEFT = 2;
const int REVERSE = 3;
const int RIGHT = 4;

void update_gait(int leg_index, int gait, int startMillis){
  const Gait & new_gait = walk_gait;
  legs[leg_index].gait = gait;
  legs[leg_index].sweep = new_gait.sweep[leg_index];
  legs[leg_index].down = new_gait.down[leg_index];
  legs[leg_index].period = new_gait.period[leg_index];
  legs[leg_index].duty = new_gait.duty[leg_index];
  legs[leg_index].phase = new_gait.phase[leg_index];
  legs[leg_index].pronk = new_gait.pronk;
  legs[leg_index].kp = new_gait.kp;
  legs[leg_index].kd = new_gait.kd;
  update_leg_trajectory(legs[leg_index], startMillis);
}

void update_leg_trajectory(Leg& l, int startTime){
  float ground_speed;
  float recovery_speed;
  int t_s = round(l.period * l.duty);

  if (l.pronk){
    ground_speed = l.sweep / t_s;
    recovery_speed = -l.sweep / (l.period - t_s);

    l.thetas[0] = l.down;
    l.ts[0] = 0;

    l.thetas[1] = l.down - l.sweep/2;
    l.ts[1] = l.ts[0] + (l.thetas[1] - l.thetas[0]) / recovery_speed;

    l.thetas[2] = l.down;
    l.ts[2] = l.ts[1] + (l.thetas[2] - l.thetas[1]) / ground_speed;

    l.thetas[3] = l.down + l.sweep/2;
    l.ts[3] = l.ts[2] + (l.thetas[3] - l.thetas[2]) / ground_speed;

    l.thetas[4] = l.down;
    l.ts[4] = l.period;

  }
  else{ //all walking gaits (and standing technically)
    
    ground_speed = l.sweep / t_s;
    recovery_speed = (360 - l.sweep) / (l.period - t_s);

    l.thetas[0] = l.down - 180;
    l.ts[0] = 0;

    l.thetas[1] = l.down - l.sweep/2;
    l.ts[1] = l.ts[0] + (l.thetas[1] - l.thetas[0]) / recovery_speed;

    l.thetas[2] = l.down;
    l.ts[2] = l.ts[1] + (l.thetas[2] - l.thetas[1]) / ground_speed;

    l.thetas[3] = l.down + l.sweep/2;
    l.ts[3] = l.ts[2] + (l.thetas[3] - l.thetas[2]) / ground_speed;

    l.thetas[4] = l.down + 180;
    l.ts[4] = l.period;

  }
  l.startMillis = startTime;
  l.ground_speed = ground_speed;
  l.recovery_speed = recovery_speed;

}

// leg = {id, desired_theta, {stand, walk, left, reverse, right}, gait, zero, right_side}
// Leg 0 is a spacer in the array to make each leg's index equal its number
Leg fake_leg0 = {0, 0, {1, 1,  1,  1,  1}, 0, 0, false};
Leg leg1 =      {1, 0, {1, 1, -1, -1,  1}, 0, 0, false};
Leg leg2 =      {2, 0, {1, 1, -1, -1,  1}, 0, 0, false};
Leg leg3 =      {3, 0, {1, 1, -1, -1,  1}, 0, 0, false};
Leg leg4 =      {4, 0, {1, 1,  1, -1, -1}, 0, 0, true};
Leg leg5 =      {5, 0, {1, 1,  1, -1, -1}, 0, 0, true};
Leg leg6 =      {6, 0, {1, 1,  1, -1, -1}, 0, 0, true};
Leg legs[] = {fake_leg0, leg1, leg2, leg3, leg4, leg5, leg6};
