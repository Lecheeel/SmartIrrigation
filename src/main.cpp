#include <Arduino.h>
#include "HAL/HAL.h"

void setup() {
  HAL::Init();
}

void loop() {
  HAL::Update();
  // digitalWrite(CONFIG_RELAY_PIN,0);
  // sleep(1);
  // digitalWrite(CONFIG_RELAY_PIN,1);
  // sleep(1);
}