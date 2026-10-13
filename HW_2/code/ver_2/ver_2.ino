const int PINS[] = {2, 3, 4, 5};
const int N = 4;
const int SENSOR_PIN = A0;

void setup(){
for (int i = 0; i < N; i++) pinMode(PINS[i], OUTPUT);
}

void loop(){
int val = analogRead(SENSOR_PIN);
// ступень от 0 до N: сколько светодиодов горит
int level = map(val, 0, 1023, 0, N);
level = constrain(level, 0, N);

for (int i = 0; i < N; i++){
digitalWrite(PINS[i], i < level ? HIGH : LOW);
}
delay(20);
}