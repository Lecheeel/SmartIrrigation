#include "HAL/HAL.h"
#include <WiFi.h>
#include <EEPROM.h>
#include <HTTPClient.h>

void HAL::Network_Init(String ssid, String password)
{
	WiFiConfig config;
	memset(&config, 0, sizeof(config)); // 将WiFiConfig对象清零
	EEPROM.begin(sizeof(config));
	EEPROM.get(0, config);
	EEPROM.end();

	// 连接WiFi网络或创建热点
	if (config.mode == 0)
	{
		// 创建热点
		WiFi.softAP(config.ssid, config.password);
	}
	else if (!ssid.isEmpty() && !password.isEmpty())
	{
		strncpy(config.ssid, ssid.c_str(), sizeof(config.ssid));
		strncpy(config.password, password.c_str(), sizeof(config.password));
		WiFi.begin(config.ssid, config.password);
		IPAddress subnet(255, 255, 255, 0);
		IPAddress local_ip(192, 168, 1, 77);
		IPAddress getway(192, 168, 1, 1);
		if (!WiFi.config(local_ip, getway, subnet))
		{
			Serial.println("STA Failed to configure");
		}

		// if (WiFi.waitForConnectResult() != WL_CONNECTED) {
		// 	Serial.println("WiFi Failed to connect");
		// 	return;
		//}
	}
	else
	{
		WiFi.begin(config.ssid, config.password);
		IPAddress subnet(255, 255, 255, 0);
		if (!WiFi.config(config.ip, config.gateway, subnet))
		{
			Serial.println("STA Failed to configure");
		}
	}
	Serial.println("");
	Serial.print("Connecting: ");
	Serial.print(config.ssid);
	Serial.print(" @");
	Serial.println(config.password);

	if (WiFi.waitForConnectResult() != WL_CONNECTED)
	{
		Serial.println("WiFi Failed to connect");
		return;
	}
	Serial.println("");
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	config.ip = WiFi.localIP();
	config.gateway = WiFi.gatewayIP();
	Network_Save_Config(config);
}

void HAL::Network_Save_Config(WiFiConfig &wifiConfig)
{
	EEPROM.begin(sizeof(WiFiConfig));
	WiFiConfig storedConfig;
	EEPROM.get(0, storedConfig);

	// Retrieve missing variables from storedConfig
	if (strlen(wifiConfig.ssid) > 0)
	{
		memcpy(storedConfig.ssid, wifiConfig.ssid, sizeof(wifiConfig.ssid));
	}
	if (strlen(wifiConfig.password) > 0)
	{
		memcpy(storedConfig.password, wifiConfig.password, sizeof(wifiConfig.password));
	}
	if (wifiConfig.ip != INADDR_NONE)
	{
		storedConfig.ip = wifiConfig.ip;
	}
	if (wifiConfig.gateway != INADDR_NONE)
	{
		storedConfig.gateway = wifiConfig.gateway;
	}
	EEPROM.put(0, storedConfig);
	EEPROM.commit();

	EEPROM.end();
}

unsigned int HAL::Network_GetBilibiliFans(String uid)
{
	String fansCount = "";
	HTTPClient http;
	http.begin("http://api.bilibili.com/x/relation/stat?vmid=" + uid);

	// start connection and send HTTP headerFFF
	int httpCode = http.GET();

	// httpCode will be negative on error
	if (httpCode == HTTP_CODE_OK)
	{
		String payload = http.getString();
		int pos = (payload.indexOf("follower"));
		fansCount = payload.substring(pos + 10, payload.length() - 2);
	}
	else
	{
		Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
	}
	http.end();

	return atol(fansCount.c_str());
}

void HAL::Network_PrintWiFiInfo()
{
	Serial.println("--------WiFi info--------");
	Serial.print("SSID: ");
	Serial.println(WiFi.SSID());
	Serial.print("BSSID: ");
	Serial.println(WiFi.BSSIDstr());
	Serial.print("Signal strength: ");
	Serial.print(WiFi.RSSI());
	Serial.println(" dBm");
	Serial.print("IP address: ");
	Serial.println(WiFi.localIP());
	Serial.print("Subnet mask: ");
	Serial.println(WiFi.subnetMask());
	Serial.print("Gateway: ");
	Serial.println(WiFi.gatewayIP());
	Serial.println("-------------------------");
}