#include "HAL/HAL.h"
#include "Configs/Version.h"

void HAL::Init()
{
    //Serial.begin(115200);
    HAL::Serial_Init();
    Serial.println(VERSION_FIRMWARE_NAME);
    Serial.println("Version: " VERSION_SOFTWARE);
    Serial.println("Author: " VERSION_AUTHOR_NAME);

    HAL::Relay_Init();
    HAL::Network_Init("", "");
    HAL::Network_PrintWiFiInfo();
    HAL::Moisture_Init();
    HAL::AirSensor_Init();
    HAL::I2C_Init(true);
    HAL::ILL_Init();
    HAL::WebServer_Init();
}

void HAL::Update()
{
    //__IntervalExecute(HAL::Moisture_Test(), 100);
    Relay_Update();
    Handle_Serial();
}
