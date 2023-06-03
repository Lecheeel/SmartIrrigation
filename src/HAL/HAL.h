#ifndef __HAL_H
#define __HAL_H

#include <stdint.h>
#include <Arduino.h>
#include <TimeLib.h>
#include "CommonMacro.h"
#include "Configs/Config.h"

namespace HAL
{
    void Init();
    void Update();
    /* Serial */
    void Serial_Init();
    void Handle_Serial();
    /* I2C */
    void I2C_Init(bool startScan);

    /* Relay */
    void Relay_Init();
    void Relay_Open();
    void Relay_Close();
    void Relay_Open_ML(uint ml);
    void Relay_Update();
    bool Relay_GetState();

    /* Soil Moisture Sensor */
    void Moisture_Init();
    float Moisture_GetValue();
    void Moisture_Test();

    /* Illumination Sensor */
    void ILL_Init();
    float ILL_GetValue();

    /* Air temperature and humidity sensor */
    void AirSensor_Init();
    float AirSensor_GetTemperature();
    float AirSensor_GetHumidity();
    void AirSensor_Test();

    /* Network */
    void Network_Init(String ssid, String password);
    void Network_Save_Config(WiFiConfig &wifiConfig);
    unsigned int Network_GetBilibiliFans(String uid);
    void Network_PrintWiFiInfo();
    void Network_printWiFiError();
    void Network_startNtp();
    void NTP_digitalClockDisplay();

    /* WebServer */
    void WebServer_Init();

    /* weather data related */
    void WeatherAPI_getCityInfo();
    void WeatherAPI_getAirData();
    void WeatherAPI_getWeatherData();
    void WeatherAPI_getWeatherIndices();
    void WeatherAPI_getWeatherIndices();
    void WeatherAPI_initializeWeatherData();
    void WeatherAPI_updateWeatherData();
    void WeatherAPI_printWeatherData();
    void WeatherAPI_printIndicesData();
    void WeatherAPI_getMoonData();
    void WeatherAPI_getSunData();

    String WeatherAPI_getName();          // 返回城市名称
    String WeatherAPI_getID();            // 返回城市ID
    String WeatherAPI_getPM2p5();         // 返回PM2.5浓度
    String WeatherAPI_getCategory();      // 返回空气质量级别
    String WeatherAPI_getText();          // 返回天气描述
    String WeatherAPI_getWindDir();       // 返回风向
    String WeatherAPI_getWindScale();     // 返回风力等级
    String WeatherAPI_getWindSpeed();     // 返回风速
    String WeatherAPI_getHumidity();      // 返回相对湿度
    String WeatherAPI_getVisibility();    // 返回能见度
    String WeatherAPI_getPrecipitation(); // 返回降水量
    String WeatherAPI_getTemperature();   // 返回温度
    String WeatherAPI_getCloud();         // 返回云量
    String WeatherAPI_getPressure();      // 返回气压
    String WeatherAPI_getIcon();          // 返回天气图标
    String WeatherAPI_getIllumination();  // 返回月亮的照明度
    String WeatherAPI_getMoonName();      // 返回月亮名称
    String WeatherAPI_getMoonrise();      // 返回月升时间
    String WeatherAPI_getMoonset();       // 返回月落时间
    String WeatherAPI_getSunrise();       // 返回日出时间
    String WeatherAPI_getSunset();        // 返回日落时间
}

#endif
