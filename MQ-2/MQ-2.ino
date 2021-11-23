
#include <MQ2.h>
int Analog_Input = A0;
int lpg, co, smoke;

MQ2 mq2(Analog_Input);

void setup(){
  Serial.begin(9600);
  mq2.begin();
}
void loop(){
  float* values= mq2.read(true); //set it false if you don't want to print the values in the Serial
lpg = mq2.readLPG();
  co = mq2.readCO();
  smoke = mq2.readSmoke();
 }
