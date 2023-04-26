#ifndef __HAL_H
#define __HAL_H

#include <stdint.h>
#include <Arduino.h>
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
    void   Moisture_Init();
    float  Moisture_GetValue();
    void   Moisture_Test();

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
    void Network_Save_Config(WiFiConfig& wifiConfig);
    unsigned int Network_GetBilibiliFans(String uid);
    void Network_PrintWiFiInfo();

/* WebServer */
    void WebServer_Init();
}

#endif
