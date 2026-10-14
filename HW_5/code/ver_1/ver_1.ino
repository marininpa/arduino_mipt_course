#include <Wire.h>

void setup(){
Wire.begin();
Serial.begin(9600);
while(!Serial); // ждём готовности порта
Serial.println("I2C Scanner");
}
void loop(){
Serial.println("Scanning...");
int n = 0;
for(byte a = 8; a < 127; a++){
Wire.beginTransmission(a); // сканируешь шину, ищем адрес устройства
if(Wire.endTransmission() == 0){
Serial.print("Found at 0x");
Serial.println(a, HEX); // выводим адрес в serial порт
n++;
}
}
if(n == 0) Serial.println("No devices");
delay(5000);
}