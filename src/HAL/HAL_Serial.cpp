#include "HAL/HAL.h"
#include "Arduino.h"

void HAL::Serial_Init()
{
    Serial.begin(SERIAL_BAUD);
}

void setWifiConfigField(WiFiConfig& wifiConfig, const char* input, const char* prefix, char* field, int fieldSize, const char* successMsg) {
  int prefixLen = strlen(prefix);
  memset(&wifiConfig, 0, sizeof(wifiConfig)); // 将WiFiConfig对象清零
  if (strncmp(input, prefix, prefixLen) == 0) {
    int valueLen = strlen(input) - prefixLen;
    if (valueLen < fieldSize) {
      strncpy(field, input + prefixLen, valueLen);
      field[valueLen] = '\0';
      Serial.println(successMsg);
      HAL::Network_Save_Config(wifiConfig);
    } else {
      Serial.println("Input too long");
    }
  }
}

void setIPAddress(const String& input, IPAddress& address) {
  int equalsIndex = input.indexOf('=');
  if (equalsIndex == -1) {
    Serial.println("Invalid command");
    return;
  }
  String ipString = input.substring(equalsIndex + 1);
  if (!address.fromString(ipString)) {
    Serial.println("Invalid IP address");
    return;
  }
  Serial.print("Set IP address to: ");
  Serial.println(address.toString());
}

void HAL::Handle_Serial() {
    static char input[128];
    static int index = 0;

    while (Serial.available() > 0) {
        char c = Serial.read();
        WiFiConfig wifiConfig;
        if (c == '\n') {
            input[index] = '\0';
            setWifiConfigField(wifiConfig, input, "setwifissid=", wifiConfig.ssid, sizeof(wifiConfig.ssid) - 1, "WiFi ssid set");
            setWifiConfigField(wifiConfig, input, "setwifipasswd=", wifiConfig.password, sizeof(wifiConfig.password) - 1, "WiFi password set");
            if (strcmp(input, "rst") == 0) {
                ESP.restart();
            }else if (strncmp(input, "setwifigateway=", 15) == 0) {
                memset(&wifiConfig, 0, sizeof(wifiConfig));
                Serial.println(input);
                setIPAddress(String(input), wifiConfig.gateway);
                HAL::Network_Save_Config(wifiConfig);
            } else if (strncmp(input, "setwifiip=", 10) == 0) {
                memset(&wifiConfig, 0, sizeof(wifiConfig));
                Serial.println(input);
                setIPAddress(String(input), wifiConfig.ip);
                HAL::Network_Save_Config(wifiConfig);
            }else if (strcmp(input, "wifiinfo") == 0) {
                HAL::Network_PrintWiFiInfo();
            }
            index = 0;
        } else {
            input[index] = c;
            index++;
            if (index >= sizeof(input)) {
                index = 0;
            }
        }
    }
}