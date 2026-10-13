int buttonState = 0;

const int LED_PIN = 13;
const int BUTTON_PIN = 2;

void setup() {
pinMode(BUTTON_PIN, INPUT_PULLUP);
pinMode(LED_PIN, OUTPUT);
}

void loop() {
buttonState = digitalRead(BUTTON_PIN);
if (buttonState == LOW) { // кнопка нажата
digitalWrite(LED_PIN, HIGH);
} else {
digitalWrite(LED_PIN, LOW);
}
}