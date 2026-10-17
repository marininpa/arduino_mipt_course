// BME280 (I2C) -> OLED 128x64 (I2C) + энкодер с кнопкой
// Метеостанция, вывод на OLED. Поворот энкодера выбирает строку T/P/H,
// нажатие кнопки энкодера (аппаратное прерывание) показывает выбранный
// параметр крупным шрифтом, повторное нажатие возвращает все три показателя.
//
// Пины (UNO):
//   BME280:  SCL=A5, SDA=A4, адрес 0x76
//   OLED:    SCL=A5, SDA=A4, адрес 0x3C  (общая шина I2C параллельно)
//   Энкодер: S1=4 (EN_A), S2=5 (EN_B), KEY=2 (кнопка, INT0, активный низкий)
//
// Библиотеки:
#include <Wire.h> // шина
#include <math.h>
#include <U8g2lib.h> // дисплей
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h> // датчик

// --- энкодер + кнопка
const int EN_A = 4;     // энкодер S1
const int EN_B = 5;     // энкодер S2
const int KEY_PIN = 2;  // кнопка энкодера -> INT0 (только 2 или 3 на UNO)

volatile bool pressed = false;  // флаг от прерывания: true - кнопка нажата
int selected = 0;               // 0=T, 1=P, 2=H
bool showSingle = false;        // false - все три, true - крупный один
int lastA;                      // прошлое состояние EN_A для определения вращения
// защита от дребезга энкодера
unsigned long lastEncMove = 0; // момент последнего принятого шага
const unsigned long ENC_DEBOUNCE_MS = 10; // задержка энкодера

// ---

// Страничный режим: рисует по 8 строк
U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// Адрес BME280
#define BME_ADDR   0x76

Adafruit_BME280 bme; // объект bme с типом Adafruit_BME280 из библиотеки

unsigned long lastRead = 0;
const unsigned long PERIOD_MS = 500; // период обновления, мс

float t = 0.0F, p = 0.0F, h = 0.0F;

// Координаты строк T/P/H в обычном режиме
const uint8_t PARAM_Y[3] = { 28, 42, 56 };

// Форматируем число для вывода десятых
void fmtNum(char *buf, size_t n, float v) {
float a = fabsf(v); // модуль
int whole = (int)a; // целая часть
int frac = (int)((a - whole) * 10.0F + 0.5F); // первая цифра дробной
if (frac == 10) { frac = 0; whole++; } // .9 округлилась до .10 -> перенос
snprintf(buf, n, "%s%d.%d", v < 0.0F ? "-" : "", whole, frac);
}

// Форматируем температуру для вывода десятых
void fmtTemp(char *buf, size_t n, float v) {
float a = fabsf(v);
int whole = (int)a;
int frac = (int)((a - whole) * 10.0F + 0.5F);
if (frac == 10) { frac = 0; whole++; }
snprintf(buf, n, "T: %s%d.%d C", v < 0.0F ? "-" : "", whole, frac);
}

void drawScreen() {
  char line[26];

  if (showSingle) {
    // Режим крупного одного параметра
    u8g2.setFont(u8g2_font_10x20_tf);
    u8g2.setCursor(4, 34);
    switch (selected) {
      case 0: fmtNum(line, sizeof line, t);            break; // T
      case 1: fmtNum(line, sizeof line, p / 100.0F);   break; // P, гПа
      case 2: fmtNum(line, sizeof line, h);            break; // H
    }
    u8g2.print(line);

    // единицы измерения - мелким шрифтом
    u8g2.setFont(u8g2_font_6x12_tf);
    const char *unit;
    switch (selected) {
      case 0: unit = "T, C";    break;
      case 1: unit = "P, hPa";  break;
      default: unit = "H, %";   break;
    }
    u8g2.drawStr(4, 52, unit);
  } else {
    // Режим трёх показателей со стрелкой у выбранной строки
    u8g2.setFont(u8g2_font_6x12_tf);

    u8g2.drawStr(2, 12, "BME280");
    u8g2.drawStr(2, PARAM_Y[selected], ">"); // стрелка выбора

    fmtTemp(line, sizeof line, t);
    u8g2.drawStr(8, PARAM_Y[0], line);

    snprintf(line, sizeof line, "P: %d hPa", (int)(p / 100.0F + 0.5F));
    u8g2.drawStr(8, PARAM_Y[1], line);

    snprintf(line, sizeof line, "H: %d %%", (int)(h + 0.5F));
    u8g2.drawStr(8, PARAM_Y[2], line);
  }
}

void redraw() {
  u8g2.firstPage();
  do {
    drawScreen();
  } while (u8g2.nextPage());
}

// обработчик прерывания от нажатия кнопки энкодера
void keyISR() {
  pressed = true; // только флаг!
}

// опрос вращения энкодера в главном цикле (без прерывания)
void readEncoder() {
int a = digitalRead(EN_A);
if (a != lastA) {
unsigned long now = millis();
if (now - lastEncMove >= ENC_DEBOUNCE_MS) { // прошло достаточно времени
if (digitalRead(EN_B) != a) selected++;
else selected--;
if (selected < 0) selected = 2;
if (selected > 2) selected = 0;
lastEncMove = now; // запомнили принятый шаг
}
lastA = a; // состояние CLK обновляем при любой смене уровня
}
}

void setup() {
// настраиваем пины для энкодера
  pinMode(EN_A, INPUT_PULLUP);
  pinMode(EN_B, INPUT_PULLUP);
  pinMode(KEY_PIN, INPUT_PULLUP);
// активиркем прерывание на KEY_PIN по спадающему фронту
  attachInterrupt(digitalPinToInterrupt(KEY_PIN), keyISR, FALLING);
// запоминаем текущее состояние энкодера
  lastA = digitalRead(EN_A);

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

  // измерение только по запросу
  bme.setSampling(Adafruit_BME280::MODE_FORCED,
                  Adafruit_BME280::SAMPLING_X1,   // температура
                  Adafruit_BME280::SAMPLING_X1,   // давление
                  Adafruit_BME280::SAMPLING_X1,   // влажность
                  Adafruit_BME280::FILTER_OFF);

  redraw();
}

void loop() {
  // 1. обработка нажатия (флаг пришёл из прерывания) - переключение режима
  if (pressed) {
    pressed = false; // сбрасываем флаг
    showSingle = !showSingle; // меняем режим отображения на противоположный
  }

  // 2. поворот энкодера - выбор строки (только пока выводим все три)
  if (!showSingle) readEncoder();

  // 3. обновление показаний с периодом
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