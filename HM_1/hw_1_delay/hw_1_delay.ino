int buttonState = 0;

const int LED_PIN = 13;
const int BUTTON_PIN = 2;

void setup() {
pinMode(BUTTON_PIN, INPUT_PULLUP);
pinMode(LED_PIN, OUTPUT);
}

enum { button_read_period = 10 };

void loop() {
static uint8_t button_state = 0; // биты = последние отсчёты
static uint32_t ts = 0;

if (millis() - ts > button_read_period) {
ts = millis();
button_state <<= 1; // забываем самое старое состояние
button_state |= digitalRead(BUTTON_PIN); // пишем текущий отсчёт
}

// 0xff = 8 раз подряд отпущена, 0x00 = 8 раз подряд нажата
digitalWrite(LED_PIN, (button_state == 0x00) ? HIGH : LOW);
}