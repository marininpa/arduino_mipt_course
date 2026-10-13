const int PINS[] = {2, 3, 4, 5};
const int N = 4;
float smooth = 0;
const int SENSOR_PIN = A0;

void setup(){
for (int i = 0; i < N; i++) pinMode(PINS[i], OUTPUT);
Serial.begin(9600);
}

void loop(){
int newVal = analogRead(SENSOR_PIN);
smooth += (newVal - smooth) * 0.1; // бегущее среднее, α = 0.1

int level = map((int)smooth, 0, 1023, 0, N);
level = constrain(level, 0, N);

for (int i = 0; i < N; i++)
digitalWrite(PINS[i], i < level ? HIGH : LOW);

Serial.print(newVal);
Serial.print(" ");
Serial.println(smooth);

delay(20);
}