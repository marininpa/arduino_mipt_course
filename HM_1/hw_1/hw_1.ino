int buttonState = 0;
const int LED_PIN = 13;
const int BUTTON_PIN = 2;

void setup() {
pinMode(BUTTON_PIN, INPUT); // кнопка
pinMode(LED_PIN, OUTPUT); // светодиод
}

void loop() {
buttonState = digitalRead(BUTTON_PIN);
if (buttonState == HIGH) {
digitalWrite(LED_PIN, HIGH);
} else {
digitalWrite(LED_PIN, LOW);
}
}