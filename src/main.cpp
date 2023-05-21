#include <Arduino.h>
#include <PS4BT.h>
#include <usbhub.h>

#ifdef dobogusinclude
#include <spi4teensy3.h>
#endif
#include <SPI.h>

USB Usb;
BTD Btd(&Usb);

PS4BT PS4(&Btd, PAIR);

int moter[4] = {};
/*            
 moter[0] = 前右
 moter[1] = 前左
 moter[2] = 後右
 moter[3] = 後左
*/


int vx, vy, omega;
char direction_pin[8] = {30, 32, 34, 36, 31, 33, 35, 37};
char power_pin[4] = {3, 5, 6, 9};
int moter_power[4] = {};
void moter_powervar();
void moter_direction();
void moter_out();

void setup()
{
  for (int i = 0; i < 4; i++)
  {
    pinMode(power_pin[i], OUTPUT);
  }

  Serial.begin(115200);

#if !defined(__MIPSEL__)                                //
  while (!Serial)                                       //
    ;                                                   //
#endif                                                  //
  if (Usb.Init() == -1)                                 //
  {                                                     //　ps4のなんかすごいやつ
    Serial.print(F("\r\nOSC did not start"));           //
    while (1);                                          //
  }                                                     //
  Serial.print(F("\r\nPS4 Bluetooth Library Started")); //
}

void loop()
{
  Usb.Task();
  if (PS4.connected())
  {
    if (PS4.getAnalogHat(LeftHatX) > 0 || PS4.getAnalogHat(LeftHatY) > 0 || PS4.getAnalogHat(RightHatX) > 0){
        vx = PS4.getAnalogHat(LeftHatX) - 127;
        vy = PS4.getAnalogHat(LeftHatY) - 127 ;
        omega = (PS4.getAnalogHat(RightHatX) - 127)* 100 / 127;
    } 
  }
  moter_powervar();
  moter_direction();
  moter_out();

  if (PS4.getButtonClick(OPTIONS))
  {
    Serial.print(F("\r\nPS"));
    PS4.disconnect();
  }

  for(int i = 0; i < 4; i++){
    Serial.print(moter_power[i]);
    Serial.print(" ");
  }
  Serial.println("");
}
  

void moter_powervar(){
  moter[0] = -vx - vy - omega;
  moter[1] = -vx + vy - omega;
  moter[2] =  vx - vy - omega;
  moter[3] =  vx + vy - omega;
  
  for(int i =0; i < 4; i++){
  moter_power[i] = moter[i];
  }

  for(int i = 0; i < 4; i++){
    if(abs(moter_power[i]) < 30){
      moter_power[i] = 0;
    }
  }
}

void moter_direction(){
  for(int i = 0; i < 4; i++){
    if(moter_power[i] > 30){
      digitalWrite(direction_pin[i], 1);
      digitalWrite(direction_pin[i + 4], 0);
    }

    else if(moter_power[i] < -30){
      digitalWrite(direction_pin[i], 0);
      digitalWrite(direction_pin[i + 4], 1);
    }

    else{
      digitalWrite(direction_pin[i], 0);
      digitalWrite(direction_pin[i + 4], 0);
    }
  }
}

void moter_out(){
   for(int i = 0; i < 4; i++){
    analogWrite(power_pin[i], abs(moter_power[i]));
   }
}