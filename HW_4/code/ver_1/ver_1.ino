// Управлени семисегментным индикатором FYS-3611 через сдвиговый регистр 74HC595
// Индикатор с общим катодом
// Доступен вывод значений и управление яркостью через PWM на входе OE 74HC595

const int SER_PIN = 8; // SER 74HC595
const int CLK_PIN = 9; // SRCLK 74HC595
const int LATCH_PIN = 10; // RCLK 74HC595
const int OE_PIN = 11; // OE 74HC595 & PWM
const int R_PIN = A0; // Фоторезистор

// Коды цифр, общий катод (активный сегмент = 1).
//  бит0->Qa a, бит1->Qb b, бит2->Qc c, бит3->Qd d,
//  бит4->Qe e, бит5->Qf f, бит6->Qg g, бит7->Qh (точка).
const byte DIGITS[10] = {
  0x3F, // 0: a b c d e f
  0x06, // 1: b c
  0x5B, // 2: a b g e d
  0x4F, // 3: a b g c d
  0x66, // 4: f g b c
  0x6D, // 5: a f g c d
  0x7D, // 6: a f g e c d
  0x07, // 7: a b c
  0x7F, // 8: все сегменты
  0x6F  // 9: a b c d f g
};

// Пороги АЦП, подбраются под текущую освещённость и динамический диапазон делителя фоторезистор/резистор
const int DARK  = 300;   // темно
const int LIGHT = 800;   // ярко

// Фильтр от шума
float smooth = 0;

void sendDigit(int level) {
  digitalWrite(LATCH_PIN, LOW);
  shiftOut(SER_PIN, CLK_PIN, MSBFIRST, DIGITS[level]);
  digitalWrite(LATCH_PIN, HIGH);
}

void setup() {
  pinMode(SER_PIN, OUTPUT);
  pinMode(CLK_PIN, OUTPUT);
  pinMode(LATCH_PIN, OUTPUT);
  pinMode(OE_PIN, OUTPUT);
  analogWrite(OE_PIN,191); // яркость 1/4, чем больше значение тем меньше яркость, т.к. вход инверсный
  Serial.begin(9600);          // дебаг в порт
}


void loop() {
  int  newVal = analogRead(R_PIN);
  smooth += (newVal - smooth) * 0.1; // фильтр от шума

  // Читаем значение на входе АЦП и маштабируем значение для вывода на индикатор, 1- минимум, 9 - максимум
  int level = map(int(smooth), DARK, LIGHT, 1, 9);
  level = constrain(level, 1, 9);

  sendDigit(level);

  // Вывод в порт дебаг
  Serial.print(smooth);
  Serial.print(" ");
  Serial.println(level);

  delay(50);
}