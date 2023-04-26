#include "HAL/HAL.h"
#include "DHT.h"

DHT dht(DHTPIN, DHTTYPE);

void HAL::AirSensor_Init(){
    Serial.println(F("DHT Initializing..."));
    dht.begin();
}

float HAL::AirSensor_GetTemperature(){
    return dht.readTemperature();
}

float HAL::AirSensor_GetHumidity(){
    return dht.readHumidity();
}