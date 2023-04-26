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

/* Humidity Sensor */
    void   HUM_Init();
    float  HUM_GetValue();
    void   HUM_Test();

/* Illumination Sensor */
    void ILL_Init();
    float ILL_GetValue();

/* Network */
    void Network_Init(String ssid, String password);
    void Network_Save_Config(WiFiConfig& wifiConfig);
    unsigned int Network_GetBilibiliFans(String uid);
    void Network_PrintWiFiInfo();

/* WebServer */
    void WebServer_Init();
}

#endif
