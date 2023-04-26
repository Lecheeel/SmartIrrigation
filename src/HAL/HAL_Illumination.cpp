#include "HAL/HAL.h"
#include "BH1750.h"
#include <Wire.h>

BH1750 lightMeter;

void HAL::ILL_Init(){
    lightMeter.configure(BH1750::CONTINUOUS_HIGH_RES_MODE);
    lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE,BH1750_ADDR);
    Serial.println(F("BH1750 Test begin"));
}

float HAL::ILL_GetValue(){
    if(lightMeter.measurementReady(true))return (float)lightMeter.readLightLevel();
    return 0;
    //Serial.println((float)lightMeter.readLightLevel());
    //return (float)lightMeter.readLightLevel();
    //Serial.print("Light: ");
    //Serial.print(lux);
    //Serial.println(" lx");
}