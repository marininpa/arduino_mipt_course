const int R_PIN = A1;
const int LED_PINS[] = {2, 3, 4, 5};
const int N = 4;
float smooth = 0;

void setup() {
Serial.begin(9600);
for (int i = 0; i < N; i++) pinMode(LED_PINS[i], OUTPUT);
}

void loop() {
int newVal = analogRead(R_PIN);
smooth += (newVal - smooth) * 0.1; // фильтр от шума фоторезистора

int level = map((int)smooth, 300, 800, 0, N);
level = constrain(level, 0, N);

for (int i = 0; i < N; i++)
digitalWrite(LED_PINS[i], i < level ? HIGH : LOW);

Serial.print(newVal);
Serial.print(" ");
Serial.println(smooth);

delay(20);
}