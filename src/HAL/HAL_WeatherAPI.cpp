#include "HAL/HAL.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <zlib.h>

constexpr size_t MAX_PAYLOAD_SIZE = 4096;
constexpr size_t MAX_BUFFER_SIZE = 1024;

struct WeatherData
{
    String name;      // 城市名称
    String id;        // 城市ID
    String pm2p5;     // PM2.5浓度
    String category;  // 空气质量级别
    String text;      // 天气描述
    String windDir;   // 风向
    String windScale; // 风力等级
    String windSpeed; // 风速
    String humidity;  // 相对湿度
    String vis;       // 能见度
    String precip;    // 降水量
    String temp;      // 温度
    String cloud;     // 云量
    String pressure;  // 气压
    String icon;      // 天气图标
};

struct WeatherIndex
{
    // int type;        // 天气指数类型
    String name; // 天气指数名称
    // int level;       // 天气指数级别
    String category; // 天气指数类别
    String text;     // 天气指数描述
};

struct Astronomy
{
    String sunrise;
    String sunset;
    String moonrise;
    String moonset;
    String moon_name;    // 月相名称
    String illumination; // 月亮照明度，百分比数值
};

struct WeatherUser
{
    String city; // 城市名称
    String key;  // API密钥
    String datetimeNow;
    int httpsCode;                   // HTTP状态码
    WeatherData currentWeather;      // 当前天气数据
    WeatherIndex weatherIndices[16]; // 天气指数数据
    Astronomy astronomy;             // 日出日落 月升月落和月相
};

WeatherUser userData;

void getResponse(const String &url, String &response)
{
    if (WiFi.status() != WL_CONNECTED || WiFi.getMode() == WIFI_MODE_AP)
    {
        HAL::Network_printWiFiError();
        return;
    }

    HTTPClient http;
    http.begin(url);
    http.setUserAgent(F("Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/111.0.0.0 Safari/537.36 Edg/111.0.1661.54"));
    http.setTimeout(5000);

    int httpCode = http.GET();

    if (httpCode == HTTP_CODE_OK)
    {
        response = http.getString();
    }
    else
    {
        Serial.printf("[HTTP] GET... Failed, Error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}

bool decompressGzipData(const uint8_t *gzipData, size_t gzipSize, String &decompressedData)
{
    z_stream stream;
    stream.zalloc = Z_NULL;
    stream.zfree = Z_NULL;
    stream.opaque = Z_NULL;
    stream.avail_in = 0;
    stream.next_in = Z_NULL;

    if (inflateInit2(&stream, 16 + MAX_WBITS) != Z_OK)
    {
        return false;
    }

    stream.avail_in = gzipSize;
    stream.next_in = const_cast<Bytef *>(gzipData);

    char outBuffer[MAX_BUFFER_SIZE];
    int status;

    do
    {
        stream.avail_out = sizeof(outBuffer);
        stream.next_out = reinterpret_cast<Bytef *>(outBuffer);

        status = inflate(&stream, Z_NO_FLUSH);
        if (status < 0)
        {
            inflateEnd(&stream);
            return false;
        }

        decompressedData += String(outBuffer, sizeof(outBuffer) - stream.avail_out);
    } while (stream.avail_out == 0);

    inflateEnd(&stream);
    return true;
}

void HAL::WeatherAPI_getCityInfo()
{
    String apiUrl = "https://geoapi.qweather.com/v2/city/lookup?location=" + userData.city + "&gzip=y&key=" + userData.key;
    Serial.println(apiUrl);

    String compressedPayload;
    getResponse(apiUrl, compressedPayload);

    if (compressedPayload.isEmpty())
        return;

    String payload;
    if (!decompressGzipData(reinterpret_cast<const uint8_t *>(compressedPayload.c_str()), compressedPayload.length(), payload))
    {
        Serial.println(F("Failed to decompress payload"));
        return;
    }

    Serial.println(payload);

    DynamicJsonDocument jsonBuffer(payload.length() * 2);
    DeserializationError error = deserializeJson(jsonBuffer, payload);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }
    // deserializeJson() failed: NoMemory
    Serial.println("OK");

    int code = jsonBuffer["code"].as<int>();

    if (code != 404)
    {
        userData.currentWeather.name = jsonBuffer["location"][0]["name"].as<String>();
        userData.currentWeather.id = jsonBuffer["location"][0]["id"].as<String>();
    }
    else
    {
        Serial.println("城市打错啦，请加上'qu'（区）或去掉'qu'（区）试试");
    }
}

void HAL::WeatherAPI_getAirData()
{
    String apiUrl = "https://devapi.qweather.com/v7/air/now?location=" + userData.currentWeather.id + "&gzip=y&key=" + userData.key;
    Serial.println(apiUrl);
    String compressedPayload;
    getResponse(apiUrl, compressedPayload);
    if (compressedPayload.equals(""))
        return;

    String payload;
    if (!decompressGzipData(reinterpret_cast<const uint8_t *>(compressedPayload.c_str()), compressedPayload.length(), payload))
    {
        Serial.println(F("Failed to decompress payload"));
        return;
    }

    Serial.println(payload);
    DynamicJsonDocument jsonBuffer(payload.length() * 2);
    DeserializationError error = deserializeJson(jsonBuffer, payload);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }
    Serial.println("OK");
    userData.currentWeather.pm2p5 = jsonBuffer["now"]["pm2p5"].as<String>();
    userData.currentWeather.category = jsonBuffer["now"]["category"].as<String>();
}

void HAL::WeatherAPI_getWeatherData()
{
    String apiUrl = "https://devapi.qweather.com/v7/weather/now?location=" + userData.currentWeather.id + "&gzip=y&key=" + userData.key;
    // https://devapi.qweather.com/v7/weather/10d/3d/7d 逐天
    // https://devapi.qweather.com/v7/weather/24/72/168 逐小时
    Serial.println(apiUrl);
    String compressedPayload;
    getResponse(apiUrl, compressedPayload);
    if (compressedPayload.equals(""))
        return;

    String payload;
    if (!decompressGzipData(reinterpret_cast<const uint8_t *>(compressedPayload.c_str()), compressedPayload.length(), payload))
    {
        Serial.println(F("Failed to decompress payload"));
        return;
    }

    Serial.println(payload);

    DynamicJsonDocument jsonBuffer(payload.length() * 2);
    DeserializationError error = deserializeJson(jsonBuffer, payload);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }
    Serial.println("OK");
    userData.currentWeather.text = jsonBuffer["now"]["text"].as<String>();
    userData.currentWeather.windDir = jsonBuffer["now"]["windDir"].as<String>();
    userData.currentWeather.windScale = jsonBuffer["now"]["windScale"].as<String>();
    userData.currentWeather.windSpeed = jsonBuffer["now"]["windSpeed"].as<String>();
    userData.currentWeather.humidity = jsonBuffer["now"]["humidity"].as<String>();
    userData.currentWeather.vis = jsonBuffer["now"]["vis"].as<String>();
    userData.currentWeather.precip = jsonBuffer["now"]["precip"].as<String>();
    userData.currentWeather.temp = jsonBuffer["now"]["temp"].as<String>();
    userData.currentWeather.cloud = jsonBuffer["now"]["cloud"].as<String>();
    userData.currentWeather.pressure = jsonBuffer["now"]["pressure"].as<String>();
    userData.currentWeather.icon = jsonBuffer["now"]["icon"].as<String>();
}

void HAL::WeatherAPI_getWeatherIndices()
{
    String apiUrl = "https://devapi.qweather.com/v7/indices/1d?location=" + userData.currentWeather.id + "&type=0&key=" + userData.key;
    Serial.println(apiUrl);
    String compressedPayload;
    getResponse(apiUrl, compressedPayload);
    if (compressedPayload.equals(""))
        return;

    String payload;
    if (!decompressGzipData(reinterpret_cast<const uint8_t *>(compressedPayload.c_str()), compressedPayload.length(), payload))
    {
        Serial.println(F("Failed to decompress payload"));
        return;
    }

    Serial.println(payload);

    DynamicJsonDocument jsonBuffer(payload.length() * 2);
    DeserializationError error = deserializeJson(jsonBuffer, payload);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }
    Serial.println("OK");
    for (int i = 0; i < 16; i++)
    {
        // userData.weatherIndices[i].type = jsonBuffer["daily"][i]["type"].as<int>();
        userData.weatherIndices[i].name = jsonBuffer["daily"][i]["name"].as<String>();
        // userData.weatherIndices[i].level = jsonBuffer["daily"][i]["level"].as<int>();
        userData.weatherIndices[i].category = jsonBuffer["daily"][i]["category"].as<String>();
        userData.weatherIndices[i].text = jsonBuffer["daily"][i]["text"].as<String>();
    }
}

void HAL::WeatherAPI_getSunData()
{
    String apiUrl = "https://devapi.qweather.com/v7/astronomy/sun?date=" + userData.datetimeNow + "&location=" + userData.currentWeather.id + "&key=" + userData.key;

    Serial.println(apiUrl);
    String compressedPayload;
    getResponse(apiUrl, compressedPayload);
    if (compressedPayload.equals(""))
        return;

    String payload;
    if (!decompressGzipData(reinterpret_cast<const uint8_t *>(compressedPayload.c_str()), compressedPayload.length(), payload))
    {
        Serial.println(F("Failed to decompress payload"));
        return;
    }

    Serial.println(payload);

    DynamicJsonDocument jsonBuffer(payload.length() * 2);
    DeserializationError error = deserializeJson(jsonBuffer, payload);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }
    Serial.println("OK");
    userData.astronomy.sunrise = jsonBuffer["sunrise"].as<String>();
    userData.astronomy.sunset = jsonBuffer["sunset"].as<String>();
}
void HAL::WeatherAPI_getMoonData()
{
    String apiUrl = "https://devapi.qweather.com/v7/astronomy/moon?date=" + userData.datetimeNow + "&location=" + userData.currentWeather.id + "&key=" + userData.key;

    Serial.println(apiUrl);
    String compressedPayload;
    getResponse(apiUrl, compressedPayload);
    if (compressedPayload.equals(""))
        return;

    String payload;
    if (!decompressGzipData(reinterpret_cast<const uint8_t *>(compressedPayload.c_str()), compressedPayload.length(), payload))
    {
        Serial.println(F("Failed to decompress payload"));
        return;
    }

    Serial.println(payload);

    DynamicJsonDocument jsonBuffer(payload.length() * 2);
    DeserializationError error = deserializeJson(jsonBuffer, payload);

    if (error)
    {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.c_str());
        return;
    }
    Serial.println("OK");
    userData.astronomy.moonrise = jsonBuffer["moonrise"].as<String>();
    userData.astronomy.moonset = jsonBuffer["moonrise"].as<String>();
    userData.astronomy.illumination = jsonBuffer["moonPhase"][0]["illumination"].as<String>();
    userData.astronomy.moon_name = jsonBuffer["moonPhase"][0]["name"].as<String>();
}
void HAL::WeatherAPI_printWeatherData()
{
    Serial.println(F("----当前天气----"));
    Serial.print(F("城市: "));
    Serial.println(userData.currentWeather.name);
    Serial.print(F("天气状况: "));
    Serial.println(userData.currentWeather.text);
    Serial.print(F("温度: "));
    Serial.println(userData.currentWeather.temp + "°C");
    Serial.print(F("相对湿度: "));
    Serial.println(userData.currentWeather.humidity + "%");
    Serial.print(F("风向: "));
    Serial.println(userData.currentWeather.windDir);
    Serial.print(F("风力等级: "));
    Serial.println(userData.currentWeather.windScale);
    Serial.print(F("风速: "));
    Serial.println(userData.currentWeather.windSpeed + "km/h");
    Serial.print(F("降水量: "));
    Serial.println(userData.currentWeather.precip + "mm");
    Serial.print(F("能见度: "));
    Serial.println(userData.currentWeather.vis + "km");
    Serial.print(F("云量: "));
    Serial.println(userData.currentWeather.cloud + "%");
    Serial.print(F("气压: "));
    Serial.println(userData.currentWeather.pressure + "hPa");
    Serial.print(F("天气图标: "));
    Serial.println(userData.currentWeather.icon);
    Serial.println(F("----天文----"));
    Serial.print(F("日出: "));
    Serial.println(userData.astronomy.sunrise);
    Serial.print(F("日落: "));
    Serial.println(userData.astronomy.sunset);
    Serial.print(F("月升: "));
    Serial.println(userData.astronomy.moonrise);
    Serial.print(F("月落: "));
    Serial.println(userData.astronomy.moonset);
    Serial.print(F("月相: "));
    Serial.println(userData.astronomy.moon_name);
    Serial.print(F("月亮照明度: "));
    Serial.print(userData.astronomy.illumination);
    Serial.println(F("%"));
}

void HAL::WeatherAPI_printIndicesData()
{
    Serial.println(F("----天气指数----"));

    for (size_t i = 0; i < sizeof(userData.weatherIndices) / sizeof(userData.weatherIndices[0]); i++)
    {
        Serial.print(userData.weatherIndices[i].name + ": ");
        Serial.println(userData.weatherIndices[i].text);
    }
}

void HAL::WeatherAPI_initializeWeatherData()
{
    userData.city = CITYNAME; // 替换为你的实际城市名称
    userData.key = APIKEY;
    int year1 = year();
    int month1 = month();
    int day1 = day();

    // 将日期格式化为字符串
    char dateString[9]; // 8个数字字符 + 1个终止符
    sprintf(dateString, "%04d%02d%02d", year1, month1, day1);
    userData.datetimeNow = dateString;
    // Serial.println("assssssssss" + userData.datetimeNow);
    WeatherAPI_getCityInfo();
    WeatherAPI_getAirData();
    WeatherAPI_getWeatherData();
    WeatherAPI_getWeatherIndices();
    WeatherAPI_getMoonData();
    WeatherAPI_getSunData();
}

void HAL::WeatherAPI_updateWeatherData()
{
    Serial.println("更新气象数据中...");
    WeatherAPI_getAirData();
    WeatherAPI_getWeatherData();
    WeatherAPI_getWeatherIndices();

    int year1 = year();
    int month1 = month();
    int day1 = day();

    // 将日期格式化为字符串
    char dateString[9]; // 8个数字字符 + 1个终止符
    sprintf(dateString, "%04d%02d%02d", year1, month1, day1);
    userData.datetimeNow = dateString;
    WeatherAPI_getMoonData();
    WeatherAPI_getSunData();
}

String HAL::WeatherAPI_getName()
{
    return userData.currentWeather.name;
}

String HAL::WeatherAPI_getID()
{
    return userData.currentWeather.id;
}

String HAL::WeatherAPI_getPM2p5()
{
    return userData.currentWeather.pm2p5;
}

String HAL::WeatherAPI_getCategory()
{
    return userData.currentWeather.category;
}

String HAL::WeatherAPI_getText()
{
    return userData.currentWeather.text;
}

String HAL::WeatherAPI_getWindDir()
{
    return userData.currentWeather.windDir;
}

String HAL::WeatherAPI_getWindScale()
{
    return userData.currentWeather.windScale;
}

String HAL::WeatherAPI_getWindSpeed()
{
    return userData.currentWeather.windSpeed;
}

String HAL::WeatherAPI_getHumidity()
{
    return userData.currentWeather.humidity;
}

String HAL::WeatherAPI_getVisibility()
{
    return userData.currentWeather.vis;
}

String HAL::WeatherAPI_getPrecipitation()
{
    return userData.currentWeather.precip;
}

String HAL::WeatherAPI_getTemperature()
{
    return userData.currentWeather.temp;
}

String HAL::WeatherAPI_getCloud()
{
    return userData.currentWeather.cloud;
}

String HAL::WeatherAPI_getPressure()
{
    return userData.currentWeather.pressure;
}

String HAL::WeatherAPI_getIcon()
{
    return userData.currentWeather.icon;
}

String HAL::WeatherAPI_getIllumination()
{
    return userData.astronomy.illumination;
}
String HAL::WeatherAPI_getMoonName()
{
    return userData.astronomy.moon_name;
}

String HAL::WeatherAPI_getMoonrise()
{
    return userData.astronomy.moonrise;
}

String HAL::WeatherAPI_getMoonset()
{
    return userData.astronomy.moonset;
}

String HAL::WeatherAPI_getSunrise()
{
    return userData.astronomy.sunrise;
}

String HAL::WeatherAPI_getSunset()
{
    return userData.astronomy.sunset;
}