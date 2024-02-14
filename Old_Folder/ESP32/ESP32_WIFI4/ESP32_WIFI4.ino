#include <WiFi.h>
 
const char* ssid = "LDH";
const char* password = "ehdgml43";

const int sw = 23;



WiFiServer server(80);  // 포트번호

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println(WiFi.localIP());
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
//  Serial.println(info.disconnected.reason);
  WiFi.begin(ssid, password);
}

void WiFionEvent(){
  WiFi.onEvent(WiFiStationConnected, SYSTEM_EVENT_STA_CONNECTED);
  WiFi.onEvent(WiFiGotIP, SYSTEM_EVENT_STA_GOT_IP);
  WiFi.onEvent(WiFiStationDisconnected, SYSTEM_EVENT_STA_DISCONNECTED);
}

void setup(){
  Serial.begin(115200);
  WiFi.disconnect(true);
  delay(1000);
  WiFionEvent();
  WiFi.begin(ssid, password);

  pinMode(sw, INPUT_PULLUP);
}

void loop(){
  
  if(digitalRead(sw)==0){
    Serial.println("ON");
    delay(500); 
  }
}
