#include "pd_control.h"
#include <stdio.h>
#include <math.h>

float pd_controller(float theta_act, float theta_des, float v_act, float v_des, float kp, float kd){
 	float diff = fmod(theta_des - theta_act, 360);
 	float shortest_distance = 180 - fabs(fabs(diff) - 180);
 	float dtheta = fmodf(diff + 360, 360) < 180 ? shortest_distance : -shortest_distance;
 	float dv = (v_des - v_act);
 	return kp * dtheta + kd * dv;
}
