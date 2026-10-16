// Печатаем содержимое корня SD карты в Serial (9600)

#include <SPI.h>
#include <SD.h>

#define SD_CS 10 // PIN CS SD карты

void setup() {
  Serial.begin(9600);
  while (!Serial) { }
  Serial.println("SD test start");

  if (!SD.begin(SD_CS)) {
    Serial.println("SD begin FAIL");
    return;
  }
  Serial.println("SD begin OK");

  File root = SD.open("/");
  if (!root) {
    Serial.println("root open FAIL");
    return;
  }

  int n = 0;
  while (true) {
    File f = root.openNextFile();
    if (!f) break;
    Serial.print(n);
    Serial.print(": ");
    Serial.print(f.name());
    if (!f.isDirectory()) {
      Serial.print("   size=");
      Serial.print(f.size());
    }
    Serial.println();
    f.close();
    n++;
  }
  root.close();
  Serial.print("total entries: ");
  Serial.println(n);
}

void loop() { }
