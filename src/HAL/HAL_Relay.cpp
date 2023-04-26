#include "HAL/HAL.h"

bool Relay_State,Relay_LongOpen=1;
uint ftime;

void HAL::Relay_Init(){
    pinMode(CONFIG_RELAY_PIN,OUTPUT);
    digitalWrite(CONFIG_RELAY_PIN,0);
    Relay_State=0;
    ftime=0;
}
void HAL::Relay_Open(){
    digitalWrite(CONFIG_RELAY_PIN,1);
    Relay_State=Relay_LongOpen=1;
}

void HAL::Relay_Close(){
    digitalWrite(CONFIG_RELAY_PIN,0);
    Relay_State=0;
}

bool HAL::Relay_GetState(){
    return Relay_State;
}

void HAL::Relay_Open_ML(uint ml){
    ftime=millis()+ ml*100;
    HAL::Relay_Open();
}

void HAL::Relay_Update(){
    if(millis()>ftime&&(~Relay_LongOpen))HAL::Relay_Close();
}