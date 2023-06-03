#ifndef __CONFIG_H
#define __CONFIG_H

/*=========================
   Hardware Configuration
 *=========================*/

#define NULL_PIN PD0

/* Serial */
#define SERIAL_BAUD 115200

/* Buzzer */
#define CONFIG_BUZZ_PIN 25
#define CONFIG_BUZZ_CHANNEL 2
#define CONFIG_SOUND_ENABLE_DEFAULT true

/* I2C */
#define CONFIG_MCU_SDA_PIN 4
#define CONFIG_MCU_SCL_PIN 5

/* Relay */
#define CONFIG_RELAY_PIN 27

/* Illumination sensor */
#define BH1750_ADDR 0x23

/* Soil Moisture Sensor */
// ADC2 is in use by WiFi
#define CONFIG_HUM_PIN 36

/* Air temperature and humidity sensor */
#define DHTPIN 14     // DHT11连接到Arduino的引脚
#define DHTTYPE DHT11 // 传感器类型

/* WiFi  */
struct WiFiConfig
{
  char ssid[32];
  char password[64];
  int mode;
  IPAddress ip;
  IPAddress gateway;
};

/* NTP */
#define TIMEZONE 8UL
#define TIME_SECS_PER_HOUR 3600UL
#define NTP_SEND_LOCALPORT 8888

/* weather data related */
#define CITYNAME "xiamen"
#define APIKEY "55119a054033493abca3a104a1f05d3d"

#endif