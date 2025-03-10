#include "leg_info.h"
#include "desired_values.h"
#include "pd_control.h"
#include "gait_parameters.h"
#include "param_storage.h"

#include <DynamixelShield.h>
#include <WiFiNINA.h>

DynamixelShield dxl;
using namespace ControlTableItem;

// Sync read/write structs
ParamForSyncWriteInst_t sync_write_param;
ParamForSyncReadInst_t sync_read_param;
RecvInfoFromStatusInst_t read_result;

// Rewritable globals
float desired_vel;
float desired_theta;
float actual_vel;
float actual_theta;
float kp;
float kd;
float control_signal;

// Legs setup
const int legs_active = 6;

// Motor values
float present_velocity[legs_active + 1];
float present_position[legs_active + 1];
float goal_velocity[legs_active + 1];

// Status
bool flash;         // LED on or off
bool shutdown;      // Disable motor movement
int voltage;        // Battery voltage
int voltage_check;  // Motor voltage
bool wifi;          // WiFi connection active

void setup() {
  Serial.begin(57600);                             // set the Serial Monitor to match this baud rate
  dxl.begin(57600);                                // 57600 is the default baud rate for XL330 motors
  dxl.setPortProtocolVersion(2.0);                 // XL330 motors use protocol 2
  
  pinMode(LED_BUILTIN, OUTPUT);                    // setup LED
  loadParams();                                    // load parameters from flash memory
  updateGaitParams(walk_gait);                     // update walk gait with loaded parameters
  int t_start = millis();
  for (int i = 1; i <= legs_active; i++) {         // legs stored at their index
    dxl.torqueOff(legs[i].id);
    dxl.setOperatingMode(legs[i].id, OP_VELOCITY); // change servo to wheel mode
    dxl.writeControlTableItem(RETURN_DELAY_TIME, legs[i].id, 0);
    dxl.writeControlTableItem(VELOCITY_P_GAIN, legs[i].id, 40);
    dxl.torqueOn(legs[i].id);
    update_gait(i, STAND, t_start);                // set initial gait parameters for each leg
  }

  sync_write_param.id_count = legs_active;         // set sync read/write motor IDs
  sync_read_param.id_count = legs_active;
  for (int i=1; i<=legs_active; i++) {
    sync_write_param.xel[i-1].id = legs[i].id;
    sync_read_param.xel[i-1].id = legs[i].id;
  }

  begin_wifi();
}

void jump_ready() {
  int t_start = millis();
  for (int i = 1; i <= legs_active; i++) {
    legs[i].desired_theta = 90;
    update_gait(i, STAND, t_start);
  }
  Serial.println("JUMP READY");
}

void jump() {

  int t_start = millis();
  while (millis() - t_start < 900) {
    Serial.println(t_start - millis());
    if (millis() - t_start > 0) {
      dxl.setGoalVelocity(legs[1].id, 100, UNIT_PERCENT);
      dxl.setGoalVelocity(legs[4].id, -100, UNIT_PERCENT);
    }
    if (millis() - t_start > 100) {
      dxl.setGoalVelocity(legs[2].id, 100, UNIT_PERCENT);
      dxl.setGoalVelocity(legs[5].id, -100, UNIT_PERCENT);
    }
    if (millis() - t_start > 190) {
      dxl.setGoalVelocity(legs[3].id, 100, UNIT_PERCENT);
      dxl.setGoalVelocity(legs[6].id, -100, UNIT_PERCENT);
    }
  }
  for (int i = 1; i <= legs_active; i++) {
    legs[i].desired_theta = 0;
    update_gait(i, STAND, t_start);
  }
}

void reset() {
  for (int i=1; i<=legs_active; i++) {
    dxl.reboot(legs[i].id);
  }
  delay(200);
  for (int i=1; i<=legs_active; i++) {
    dxl.setOperatingMode(legs[i].id, OP_VELOCITY);
    dxl.torqueOn(legs[i].id);
  }
}

int count = 0;
int t = 0;
void loop() {
  // loop counter
  count++;

  // Every 10 loop iterations, find max voltage supplied to each leg and compare with nominal
  if (count % 10 == 0) {
    voltage = 0;
    for (int i = 1; i <= legs_active; i++) {
      voltage_check = dxl.readControlTableItem(PRESENT_INPUT_VOLTAGE, legs[i].id);
      if (voltage_check > voltage) voltage = voltage_check;
    }
    Serial.print("Voltage: ");
    Serial.print(voltage/10.0);
    Serial.print(", Frequency: ");
    Serial.print(10000/(millis()-t));
    Serial.println("Hz");
    t = millis();

    bool update = false;
    if (voltage > 45) {
      if (flash) update = true;
      flash = false;            // off (safe voltage)
      shutdown = false;
    }
    else if (voltage > 40) {
      if (!flash) update = true;
      flash = true;             // on (low voltage)
      shutdown = false;
    }
    else {
      if (!flash) update = true;
      flash = true;             // shutdown motors (very low voltage)
      shutdown = true;
    }

    if (update) {
      for (int i = 1; i <= legs_active; i++) {
        if (flash) {
          dxl.ledOn(legs[i].id);
        } else {
          dxl.ledOff(legs[i].id);
        }
      }
    }
  }

  // teleop control
  if (Serial.available() || wifi) {
    char a;
    if (Serial.available()) {
      a = (char)(Serial.read());
    } else {
      a = update_wifi();
    }
    int gait = -1;
    switch (a) {
      case 'q': // stand
        gait = STAND;
        break;
      case 'w': // forwards
        gait = WALK;
        break;
      case 'a': // left
        gait = LEFT;
        break;
      case 's': // reverse
        gait = REVERSE;
        break;
      case 'd': // right
        gait = RIGHT;
        break;
      case 'x': // prepare jump
        jump_ready();
        break;
      case 'j': // jump
        jump();
        break;
      case 'z': // reboot motors
        reset();
        break;
    }

    if (gait != -1) {
      int t_start = millis();
      for (int i = 1; i <= legs_active; i++) {
        update_gait(i, gait, t_start);
      }
    }
  }

  // Compute leg velocities
  sync_read_position();
  sync_read_velocity();
  for (int i = 1; i <= legs_active; i++) {
    actual_theta = present_position[i] - legs[i].zero;  // degrees
    actual_vel = present_velocity[i];                   // degrees/millisecond
    if (legs[i].gait == STAND) { // standing or sitting
      desired_theta = legs[i].desired_theta;
      desired_vel = 0;
      kp = params.kp_stand;
      kd = params.kd_stand;
    } else { // walking, turning
      Vals v = get_desired_vals(millis(), legs[i]);
      desired_theta = v.global_theta;
      desired_vel = v.global_velocity;
      kp = legs[i].kp;
      kd = legs[i].kd;
    }
    if (legs[i].right_side) {
      desired_theta *= -1;
      desired_vel *= -1;
    }
    control_signal = pd_controller(actual_theta, desired_theta, actual_vel, desired_vel, kp, kd);
    if (shutdown) {
      goal_velocity[i] = 0;
    } else {
      goal_velocity[i] = actual_vel + control_signal;
    }
  }
  sync_write_velocity();
}

void sync_read_velocity() {
  sync_read_param.addr = 128; // Present Velocity of DYNAMIXEL-X series
  sync_read_param.length = 4;
  dxl.syncRead(sync_read_param, read_result);
  int raw;
  for (int i=1; i<=legs_active; i++) {
    memcpy(&raw, read_result.xel[i-1].data, read_result.xel[i-1].length);
    present_velocity[i] = raw * .06 * 0.229;
  }
}

void sync_read_position() {
  sync_read_param.addr = 132; // Present Position of DYNAMIXEL-X series
  sync_read_param.length = 4;
  dxl.syncRead(sync_read_param, read_result);
  int raw;
  for (int i=1; i<=legs_active; i++) {
    memcpy(&raw, read_result.xel[i-1].data, read_result.xel[i-1].length);
    present_position[i] = raw * 0.088;
  }
}

void sync_write_velocity() {
  sync_write_param.addr = 104; // Goal Velocity of DYNAMIXEL-X series
  sync_write_param.length = 4;
  for (int i=1; i<=legs_active; i++) {
    int raw = goal_velocity[i] / .06 / 0.229;
    memcpy(sync_write_param.xel[i-1].data, &raw, 4);
  }
  dxl.syncWrite(sync_write_param);
}
