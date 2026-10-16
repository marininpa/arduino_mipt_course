// BME280 (I2C) -> OLED 128x64 (I2C)
// Метеостанция, выводит показания табчика BME280 на OLED экран через I2C
//
// Пины (UNO):
//   BME280: 
//           SCL=A5, SDA=A4, адрес 0x76
//   OLED:   +SCL=A5, SDA=A4, адрес 0x3C
//   Оба устройства на общей шине I2C параллельно
// Библиотеки:
#include <Wire.h> // шина
#include <math.h>
#include <U8g2lib.h> // дисплей
#include <Adafruit_Sensor.h> 
#include <Adafruit_BME280.h> // датчик

// Страничный режим: рисует по 8 строк
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Адрес BME280
#define BME_ADDR   0x76

Adafruit_BME280 bme; // определяем объект bme с типом Adafruit_BME280 из библиотеки

unsigned long lastRead = 0;
const unsigned long PERIOD_MS = 500; // период обновления, мс

float t = 0.0F, p = 0.0F, h = 0.0F;

// Форматируем float вручную: на AVR в snprintf нет поддержки %f.
void fmtTemp(char *buf, size_t n, float v) {
  int whole = (int)v;                                  // усечение к нулю
  int frac = (int)(fabsf(v - (float)whole) * 10.0F + 0.5F);
  if (frac >= 10) frac = 9;
  if (v < 0.0F && whole == 0)
    snprintf(buf, n, "T: -0.%d C", frac);
  else
    snprintf(buf, n, "T: %d.%d C", whole, frac);
}

void drawScreen() {
  u8g2.setFont(u8g2_font_6x12_tf);
  char line[22];

  u8g2.drawStr(2, 12, "BME280");

  fmtTemp(line, sizeof line, t);
  u8g2.drawStr(2, 28, line);

  snprintf(line, sizeof line, "P: %d hPa", (int)(p / 100.0F + 0.5F));
  u8g2.drawStr(2, 42, line);

  snprintf(line, sizeof line, "H: %d %%", (int)(h + 0.5F));
  u8g2.drawStr(2, 56, line);
}

void redraw() {
  u8g2.firstPage();
  do {
    drawScreen();
  } while (u8g2.nextPage());
}

void setup() {
  Serial.begin(9600);

  u8g2.begin();

// если датчик не ответил выводим на OLED экран, далее останавливаем программу
  if (!bme.begin(BME_ADDR)) {
    u8g2.firstPage();
    do {
      u8g2.setFont(u8g2_font_6x12_tf);
      u8g2.drawStr(4, 12, "BME FAIL");
    } while (u8g2.nextPage());
    for (;;);
  }


  // измерение только по запросу, датчик не греет сам себя
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1,   // температура
                  Adafruit_BME280::SAMPLING_X1,   // давление
                  Adafruit_BME280::SAMPLING_X1,   // влажность
                  Adafruit_BME280::FILTER_OFF);

  redraw();
}

void loop() {
  if (millis() - lastRead < PERIOD_MS) return; 
  lastRead = millis();

  bme.takeForcedMeasurement();

  t = bme.readTemperature();
  p = bme.readPressure();
  h = bme.readHumidity();

  Serial.print(F("T=")); Serial.print(t, 2);
  Serial.print(F(" P=")); Serial.print(p / 100.0F, 2);
  Serial.print(F(" H=")); Serial.println(h, 1);

  redraw();
}
