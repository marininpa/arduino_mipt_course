#include <Wire.h>

#define MPU 0x68 // адрес, нашли в первой программе
#define PWR_MGMT_1 0x6B // управление питанием
#define ACCEL_XOUT_H 0x3B // адрес старшего регистра оси Х для акселерометра, остальные через смещение
#define GYRO_XOUT_H 0x43 // адрес старшего регистра оси Х для гироскопа, остальные через смещение
#define TMP_OUT_H 0x41 // температура

// акселерометр
// 0x3B — ACCEL_XOUT_H (ось X, старший байт)
// 0x3C — ACCEL_XOUT_L (ось X, младший байт)
// 0x3D — ACCEL_YOUT_H (ось Y, старший байт)
// 0x3E — ACCEL_YOUT_L (ось Y, младший байт)
// 0x3F — ACCEL_ZOUT_H (ось Z, старший байт)
// 0x40 — ACCEL_ZOUT_L (ось Z, младший байт)

// гироскоп
// 0x43 - GYRO_XOUT_H (ось X, старший байт)
// GYRO_XOUT_H + 2 (ось Y, старший байт)
// GYRO_XOUT_H + 4 (ось Z, старший байт)



void setup(){
Wire.begin();
Serial.begin(9600);
Serial.println("Init MPU-6050");
Wire.beginTransmission(MPU);
Wire.write(PWR_MGMT_1);
Wire.write(0); // выводим MPU из спящего режима
Wire.endTransmission();
Serial.println("Init done");
delay(100);
}

int16_t readRaw(int reg){
Wire.beginTransmission(MPU);
Wire.write(reg);
Wire.endTransmission(false); // не бросаем шину
Wire.requestFrom(MPU, 2); // запрашиваем два байта
int16_t h = Wire.read(); // читаем старший байт
int16_t l = Wire.read(); // читаем младший байт
return (h << 8) | l; // склеиваем в 16-битное значение
}

void loop(){
Serial.print("Acc: "); // акселерометр
Serial.print(readRaw(ACCEL_XOUT_H)); // X
Serial.print(" ");
Serial.print(readRaw(ACCEL_XOUT_H + 2)); // Y
Serial.print(" ");
Serial.print(readRaw(ACCEL_XOUT_H + 4)); // Z
Serial.print(" Gyr: "); // гироскоп
Serial.print(readRaw(GYRO_XOUT_H));
Serial.print(" ");
Serial.print(readRaw(GYRO_XOUT_H + 2));
Serial.print(" ");
Serial.print(readRaw(GYRO_XOUT_H + 4));
Serial.print(" Tmp: ");  // температура
Serial.println((readRaw(TMP_OUT_H)/340.0)+36.53);

delay(200);
}