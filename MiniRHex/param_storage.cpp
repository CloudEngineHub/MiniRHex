#include "param_storage.h"

#include <FlashStorage_SAMD.h>

Params params;

const int SIGNATURE = 0xBEEFDEED;
const uint16_t ADDRESS = 0;

void parseParams(const String & s, int index) {
  bool save = false;
  bool load = false;
  bool reset = false;
  int startIndex = 0;
  int splitIndex;
  int endIndex;
  while (splitIndex != -1) {
    splitIndex = s.indexOf('=', startIndex);
    if (splitIndex == -1) break;
    endIndex = s.indexOf('&', startIndex);
    if (endIndex == -1) endIndex = s.length();
    String key = s.substring(startIndex, splitIndex);
    String val = s.substring(splitIndex + 1, endIndex);
    if (key == "period") {
      params.period = val.toFloat();
    } else if (key == "duty") {
      params.duty = val.toFloat();
    } else if (key == "sweep") {
      params.sweep = val.toFloat();
    } else if (key == "down") {
      params.down = val.toFloat();
    } else if (key == "gait") {
      val.toCharArray(params.gait, 21);
    } else if (key == "save") {
      save = (val == "true");
    } else if (key == "load") {
      load = (val == "true");
    } else if (key == "reset") {
      reset = (val == "true");
    }
    startIndex = endIndex + 1;
  }
  if (reset) {
    resetParams();
  }
  if (save) {
    saveParams(index);
  }
  if (load) {
    loadParams(index);
  }
}

void subParams(String & s) {
  s.replace("(PERIOD)", String(params.period));
  s.replace("(DUTY)", String(params.duty));
  s.replace("(SWEEP)", String(params.sweep));
  s.replace("(DOWN)", String(params.down));
  s.replace("(GAIT)", "\"" + String(params.gait) + "\"");
}

void resetParams(int index) {
  params = Params();
}

void loadParams(int index) {
  int offset = index * (sizeof(params) + sizeof(SIGNATURE));
  int signature;
  EEPROM.get(offset + ADDRESS, signature);
  if (signature == SIGNATURE) {
    EEPROM.get(offset + ADDRESS + sizeof(SIGNATURE), params);
  } else {
    resetParams(index);
  }
}

void saveParams(int index) {
  int offset = index * (sizeof(params) + sizeof(SIGNATURE));
  EEPROM.put(offset + ADDRESS, SIGNATURE);
  EEPROM.put(offset + ADDRESS + sizeof(SIGNATURE), params);
  if (!EEPROM.getCommitASAP()) EEPROM.commit();
}
