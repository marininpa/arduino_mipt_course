const int LED_PIN = 13;
const int SENSOR_PIN = A0;

void setup()
{
    pinMode(LED_PIN, OUTPUT);
}

void loop(){
int val = analogRead(SENSOR_PIN);
// макс. значение примерно 1023, берём с запасом ниже
if (val > 900) digitalWrite(LED_PIN, HIGH);
else digitalWrite(LED_PIN, LOW);
}