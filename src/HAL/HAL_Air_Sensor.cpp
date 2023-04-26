#include "HAL/HAL.h"
#include "DHT.h"

DHT dht(DHTPIN, DHTTYPE);

void HAL::AirSensor_Init()
{
    Serial.println(F("DHT Initializing..."));
    dht.begin();
}

float HAL::AirSensor_GetTemperature()
{
    return dht.readTemperature();
}

float HAL::AirSensor_GetHumidity()
{
    return dht.readHumidity();
}

void HAL::AirSensor_Test()
{
    float humidity = dht.readHumidity();       // 读取湿度
    float temperature = dht.readTemperature(); // 读取温度
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.print("%  Temperature: ");
    Serial.print(temperature);
    Serial.println("°C");
}