#include <WiFi.h>


const char* ssid     = "LDH";
const char* password = "ehdgml43";
WiFiServer server(80);

static bool hasWifi = false;


unsigned long interval = 5000;
unsigned long previousMillis = 0;

///////////////////////////////////////////////////////

static void InitWifi()
{
  Serial.println("Connecting...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED){
    delay(1000);
    Serial.print(".");  
  }
  hasWifi = true;
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

static void Re_Wifi(){
  unsigned long currentMillis = millis();
  
  if ((WiFi.status() != WL_CONNECTED) && (currentMillis - previousMillis >= interval)){
    Serial.print(millis());
    Serial.println(" : Reconnecting to WiFi...");
    WiFi.disconnect();
    WiFi.reconnect();   
    previousMillis = currentMillis;
  }
}


//////////////////////////////////////////////////////

void setup() {
  Serial.begin(115200);
  delay(1000);
  hasWifi = false;
  InitWifi();
  if (!hasWifi){
    Serial.println("Not Connected");
    return;
  }

}

void loop() {
  WiFiClient client = server.available();
  Re_Wifi();

}
