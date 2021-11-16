// NodeMCU pin
// D1,2,5,6,7 제한없이 사용가능
#include "DHT.h"

#define DHTPIN 15   // DHT11이 연결된 핀
//#define VCC D6    //외부 전원 쓰는 것이 좋음 잘 연결 안됨
//#define GND D7

#define DHTTYPE DHT11   // DHT 11, DHT시리즈중 11을 선택합니다.

DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);
//  pinMode(VCC, OUTPUT);
//  pinMode(GND, OUTPUT);
//  digitalWrite(VCC, HIGH);
//  digitalWrite(GND, LOW);
  Serial.println("DHTxx test!");

  dht.begin();
}

void loop() {
  delay(2000);

  float h = dht.readHumidity();// 습도를 측정합니다.
  float t = dht.readTemperature();// 온도를 측정합니다.
  float f = dht.readTemperature(true);// 화씨 온도를 측정합니다.

  // 값 읽기에 오류가 있으면 오류를 출력합니다.
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // 보정된 화씨 값을 가져옵니다.
  float hif = dht.computeHeatIndex(f, h);
  // 보정된 섭씨 값을 가져옵니다.
  float hic = dht.computeHeatIndex(t, h, false);

  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" | ");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C |");
  Serial.print(f);
  Serial.print(" *F |");
  Serial.print("Heat index: "); //열지수, 체감온도
  Serial.print(hic);
  Serial.print(" *C |");
  Serial.print(hif);
  Serial.println(" *F");
}
