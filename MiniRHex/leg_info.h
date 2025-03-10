#ifndef leg_info
#define leg_info

#include <math.h>

struct Leg {
  int id;               // Motor index
  float desired_theta;  // Leg angle while standing (degrees)
  int sign[5];          // Direction of rotation in each gait (-1 or 1)
  int gait;             // Current gait
  float zero;           // Motor offset angle (degrees)
  bool right_side;      // Negate output of right-side motors

  // Gait parameters
  int startMillis;
  float sweep;
  float down;
  int period;
  float duty;
  float phase;
  bool pronk;
  float kp;
  float kd;

  // Leg trajectory
  float recovery_speed; // Speed during swing phase (degrees/millisecond)
  float ground_speed;   // Speed during stance phase (degrees/millisecond)
  float thetas[5];      // Angles at each keyframe (degrees)
  float ts[5];          // Times at each keyframe (milliseconds)
};

// Update gait parameters
void update_gait(int leg_index, int gait, int startMillis);

// Compute keyframes at each stage of the gait
void update_leg_trajectory(Leg& l, int startTime);

extern Leg legs[7];
extern int num_gaits;
extern const int STAND;
extern const int WALK;
extern const int LEFT;
extern const int REVERSE;
extern const int RIGHT;

#endif
