// HW8. Управление светодиодами через регистры + энкодер.
//
// Условие (лекция 8): управлять несколькими светодиодами с помощью регистров.
// Гирлянда из 4 светодиодов на PORTB, энкодер задаёт число горящих:
//   вращение в одну сторону  - зажигаем по одному (счёт вверх);
//   вращение в другую сторону - гасим по одному (счёт вниз);
//   нажатие кнопки энкодера   - гасим все сразу.
//
// Пины (UNO):
//   Светодиоды: D8, D9, D10, D11 -> PORTB, биты PB0..PB3
//   Энкодер:    CLK = D4, DT = D5, KEY/SW = D2 (INT0, активный низкий)
//
// Светодиоды подключены через резистор 220 Ом, катод в GND.

const uint8_t ENC_CLK = 4;  // энкодер S1
const uint8_t ENC_DT  = 5;  // энкодер S2
const uint8_t ENC_SW  = 2;  // кнопка энкодера -> INT0

const uint8_t LED_MASK = 0x0F;    // биты PB0..PB3 - наши светодиоды
const int LED_COUNT_MAX = 4;      // сколько светодиодов всего

volatile bool btnPressed = false; // флаг из прерывания

int litCount = 0;                 // сколько светодиодов сейчас горит (0..4)
int lastClk;                      // прошлое состояние CLK
unsigned long lastStepMs = 0;     // защита энкодера от дребезга
const unsigned long ENC_DEBOUNCE_MS = 5;

// Обработчик прерывания от кнопки: только ставим флаг.
void keyISR() {
  btnPressed = true;
}

// Зажечь ровно n младших светодиодов, остальные погасить.
void setLeds(int n) {
  uint8_t mask;
  if (n <= 0)                  mask = 0x00;
  else if (n >= LED_COUNT_MAX) mask = LED_MASK;
  else                         mask = (uint8_t)((1 << n) - 1);

  // сохраняем старшие биты PORTB (D12, D13), меняем только младшие 4
  PORTB = (uint8_t)((PORTB & ~LED_MASK) | mask);
}

// Опрос вращения энкодера в loop (без прерывания).
void readEncoder() {
  int clk = digitalRead(ENC_CLK);
  if (clk != lastClk) {
    unsigned long now = millis();
    if (now - lastStepMs >= ENC_DEBOUNCE_MS) {
      if (digitalRead(ENC_DT) != clk) {      // направление вращения
        if (litCount < LED_COUNT_MAX) litCount++;
      } else {
        if (litCount > 0) litCount--;
      }
      setLeds(litCount);
      lastStepMs = now;
    }
    lastClk = clk; // состояние CLK обновляем при любой смене уровня
  }
}

void setup() {
  DDRB  |= LED_MASK;    // PB0..PB3 - выходы
  PORTB &= ~LED_MASK;   // все светодиоды погашены

  pinMode(ENC_CLK, INPUT_PULLUP);
  pinMode(ENC_DT,  INPUT_PULLUP);
  pinMode(ENC_SW,  INPUT_PULLUP);

  lastClk = digitalRead(ENC_CLK);
  attachInterrupt(digitalPinToInterrupt(ENC_SW), keyISR, FALLING);
}

void loop() {
  // нажатие кнопки -> погасить всё
  if (btnPressed) {
    btnPressed = false;
    litCount = 0;
    PORTB &= ~LED_MASK;
  }

  readEncoder();
}
