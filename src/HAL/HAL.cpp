#include "HAL/HAL.h"
#include "Configs/Version.h"

void HAL::Init()
{
    // Serial.begin(115200);
    HAL::Serial_Init();
    Serial.println(VERSION_FIRMWARE_NAME);
    Serial.println("Version: " VERSION_SOFTWARE);
    Serial.println("Author: " VERSION_AUTHOR_NAME);

    HAL::Relay_Init();
    HAL::Network_Init("MiWIFI_6FBC", "admin123123");
    HAL::Network_PrintWiFiInfo();
    HAL::Network_startNtp();
    HAL::Moisture_Init();
    HAL::AirSensor_Init();
    HAL::I2C_Init(true);
    HAL::ILL_Init();
    HAL::WebServer_Init();
    HAL::WeatherAPI_initializeWeatherData();
    HAL::WeatherAPI_printWeatherData();
    HAL::WeatherAPI_printIndicesData();
}

void HAL::Update()
{
    //__IntervalExecute(HAL::Moisture_Test(), 100);
    //__IntervalExecute(HAL::AirSensor_Test(), 100);
    __IntervalExecute(HAL::WeatherAPI_updateWeatherData(), 1000 * 60 * 5); // 5min

    //__IntervalExecute(HAL::WeatherAPI_printWeatherData(), 5000);
    //__IntervalExecute(HAL::WeatherAPI_printIndicesData(), 5000);
    //__IntervalExecute(HAL::NTP_digitalClockDisplay(), 5000);
    Relay_Update();
    Handle_Serial();
}
