#include "HAL/HAL.h"
#include <WiFi.h>
#include <EEPROM.h>
#include <HTTPClient.h>
#include <WiFiUdp.h>

WiFiUDP Udp;
const int NTP_PACKET_SIZE = 48;
byte packetBuffer[NTP_PACKET_SIZE];
const char *ntpServerName = "ntp.ntsc.ac.cn";

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
		WiFi.begin(config.ssid, config.password); /*
		 IPAddress subnet(255, 255, 255, 0);
		 IPAddress local_ip(192, 168, 1, 77);
		 IPAddress getway(192, 168, 1, 1);
		 if (!WiFi.config(local_ip, getway, subnet))
		 {
			 Serial.println("STA Failed to configure");
		 }*/
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

	// 设置静态 DNS
	// IPAddress dns(223, 5, 5, 5);
	//  WiFi.config(config.ip, config.gateway, dns);
	// WiFi.config(IPAddress(0, 0, 0, 0), IPAddress(0, 0, 0, 0), dns);
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

void HAL::Network_printWiFiError()
{
	switch (WiFi.status())
	{
	case WL_NO_SHIELD:
		Serial.println("WiFi模块未找到");
		break;
	case WL_IDLE_STATUS:
		Serial.println("空闲状态");
		break;
	case WL_NO_SSID_AVAIL:
		Serial.println("未找到指定的WiFi网络");
		break;
	case WL_SCAN_COMPLETED:
		Serial.println("WiFi扫描已完成");
		break;
	case WL_CONNECTED:
		// 连接成功，不是错误情况
		break;
	case WL_CONNECT_FAILED:
		Serial.println("连接失败");
		break;
	case WL_CONNECTION_LOST:
		Serial.println("连接丢失");
		break;
	case WL_DISCONNECTED:
		Serial.println("已断开连接");
		break;
	default:
		Serial.println("未知错误");
		break;
	}
}

// 向给定地址的时间服务器发送NTP请求
void sendNTPpacket(IPAddress &address)
{
	// 将缓冲区中的所有字节设置为0
	memset(packetBuffer, 0, NTP_PACKET_SIZE);
	// 初始化NTP请求数据包的值
	packetBuffer[0] = 0b11100011; // LI, Version, Mode
	packetBuffer[1] = 0;		  // Stratum, or type of clock
	packetBuffer[2] = 6;		  // Polling Interval
	packetBuffer[3] = 0xEC;		  // Peer Clock Precision
	// 8字节的零值用于Root Delay和Root Dispersion
	packetBuffer[12] = 49;
	packetBuffer[13] = 0x4E;
	packetBuffer[14] = 49;
	packetBuffer[15] = 52;
	// 所有NTP字段已经设置好值，现在可以发送请求时间戳的数据包
	Udp.beginPacket(address, 123); // NTP请求使用端口123
	Udp.write(packetBuffer, NTP_PACKET_SIZE);
	Udp.endPacket();
}

// 获取NTP时间
time_t getNtpTime()
{
	IPAddress ntpServerIP; // NTP服务器的IP地址

	while (Udp.parsePacket() > 0)
		; // 丢弃之前接收到的任何数据包
	// 发送NTP请求
	Serial.println("发送NTP请求");
	// 从池中获取一个随机服务器
	WiFi.hostByName(ntpServerName, ntpServerIP);
	Serial.print(ntpServerName);
	Serial.print(": ");
	Serial.println(ntpServerIP);
	sendNTPpacket(ntpServerIP); // 发送NTP数据包
	uint32_t beginWait = millis();
	while (millis() - beginWait < 1500)
	{
		int size = Udp.parsePacket();
		if (size >= NTP_PACKET_SIZE)
		{
			// 接收NTP响应
			Serial.println("接收NTP响应");
			Udp.read(packetBuffer, NTP_PACKET_SIZE); // 读取数据包到缓冲区
			unsigned long secsSince1900;
			// 将从位置40开始的四个字节转换为长整型
			secsSince1900 = (unsigned long)packetBuffer[40] << 24;
			secsSince1900 |= (unsigned long)packetBuffer[41] << 16;
			secsSince1900 |= (unsigned long)packetBuffer[42] << 8;
			secsSince1900 |= (unsigned long)packetBuffer[43];
			return secsSince1900 - 2208988800UL + TIMEZONE * TIME_SECS_PER_HOUR; // 返回时间
		}
	}
	// 没有收到NTP响应
	Serial.println("没有收到NTP响应 :-(");
	return 0; // 如果无法获取时间，则返回0
}

// 同步NTP时间
void syncNtpTime()
{
	setSyncProvider(getNtpTime);
	setSyncInterval(300);
}

void HAL::Network_startNtp()
{
	Serial.println();
	Serial.println("----同步NTP时间----");
	Serial.println("启动UDP");
	Udp.begin(NTP_SEND_LOCALPORT);
	Serial.print("本地端口: ");
	Serial.println(Udp.remotePort());
	Serial.println("等待同步");
	syncNtpTime();
}

void printDigits(int digits)
{
	// utility for digital clock display: prints preceding colon and leading 0
	Serial.print(":");
	if (digits < 10)
		Serial.print('0');
	Serial.print(digits);
}

void HAL::NTP_digitalClockDisplay()
{
	// digital clock display of the time
	Serial.print("Time: ");
	Serial.print(hour());
	printDigits(minute());
	printDigits(second());
	Serial.print(" ");
	Serial.print(day());
	Serial.print(".");
	Serial.print(month());
	Serial.print(".");
	Serial.print(year());
	Serial.println();
}