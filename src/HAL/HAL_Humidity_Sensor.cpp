#include "HAL/HAL.h"
#include <cmath>
/*
struct HAL_Humidity_Sensor
{
    //actual time(16 data-16time)
    float hum_by_actual[16];
    long time_hum_by_actual[16];

    //Daily analysis(1h-5data *24h)
    float hum_day_analysis[5*24];
    //time: 0h-24h

    //day ave(1h-1data)
    float hum_day_ave[24];
    //time: 0h-24h

    int time_hum_actual;

    int time_hum_day_ana;
    ulong time_lasttime_day_ana;

    int time_hum_day_ave;
    ulong time_lasttime_day_ave;
}*hum;*/
void HAL::HUM_Init(){
    pinMode(CONFIG_HUM_PIN,INPUT);
}


double calculateHumidity(int reading) {
  double a = 2; // 可以根据需要调整此值
  double humidity = 100.0 - (100.0 / pow(1.0 + (reading - 1000.0) / (4095.0 - 1000.0), a));
  if (humidity < 0.0) {
    humidity = 0.0;
  }
  return 100.00-humidity;
}
float HAL::HUM_GetValue(){
    //return (float)float((3000.00-(float)analogRead(CONFIG_HUM_PIN))/20);//1v-3v
    return calculateHumidity(analogRead(CONFIG_HUM_PIN));
}

void HAL::HUM_Test(){
     //Serial.println("ttttttttttttttttt");
     //Serial.println((analogRead(CONFIG_HUM_PIN) - 4095) * (100 - 0) / (700 - 4095) + 0);
     Serial.println(calculateHumidity(analogRead(CONFIG_HUM_PIN)));
    // int air_val=457;//Put sensor in air and check value (Serial.println(Soil_val);)
    // int water_val=196;//Put sensor in water and check value(Serial.println(Soil_val);)
    // int Soil_val=0;//set as int Soil val
    // int Soil_percent=0;//set as int soil_percent
    
    // Soil_val=analogRead(CONFIG_HUM_PIN);//read values from analog pin A0 (SoilSensor)
    // //Serial.println(Soil_val);//only for first step to calibrate sensor(air and water values)
    // Soil_percent=map(Soil_val,water_val,air_val,0,100);//map function re-maps a number from low to high to another values. For example if sennsor is in air value is 457 .Serial port will print 100% 
    // if(Soil_percent>=100){
    // Soil_percent=100;
    // }

    // Serial.println(Soil_percent);//print our percent value on Serial monitor
    // Serial.print("%");
    
}