#include<dht.h> 
dht DHT;
#define DHT11_PIN 2
#define VCC 3
#define GND 4
float hum; //Stores humidity value 
float temp; //Stores temperature value 

void setup(){ 
  Serial.begin(9600);
  pinMode(VCC, OUTPUT);
  pinMode(GND, OUTPUT);
  digitalWrite(VCC, HIGH);
  digitalWrite(GND, LOW);
}
void loop() {
  DHT.read11(DHT11_PIN);
  hum = DHT.humidity;
  temp = DHT.temperature;
  Serial.print("Humidity: ");
  Serial.print(hum);
  Serial.print(" %, Temp: ");
  Serial.print(temp);
  Serial.println(" Celsius");
  delay(2000); //Delay 2 sec. }
}

//습도가 부정확함
