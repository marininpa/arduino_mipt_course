const int R_PIN = A0;
const int LED_PINS[] = {3, 5, 6, 9};
const int N = 4;
float smooth = 0;

void setup() {
Serial.begin(9600);
for (int i = 0; i < N; i++) pinMode(LED_PINS[i], OUTPUT);
}

void loop() {
int newVal = analogRead(R_PIN);
smooth += (newVal - smooth) * 0.1; // фильтр от шума

int level = map((int)smooth, 0, 1023, 0, 255);
level = constrain(level, 0, 255);

for (int i = 0; i < N; i++)
analogWrite(LED_PINS[i],level);

Serial.print(smooth);
Serial.print(" ");
Serial.println(level);

delay(20);
}