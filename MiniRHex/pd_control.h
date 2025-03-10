#ifndef pd_control
#define pd_control

// Implementation of PD control that attempts to take the short path around the circle
float pd_controller(float theta_act, float theta_des, float v_act, float v_des, float kp, float kd);

#endif
