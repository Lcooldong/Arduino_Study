#include <WiFi.h>
 
const char* ssid = "LDH";
const char* password = "ehdgml43";

void WiFiStationConnected(WiFiEvent_t event, WiFiEventInfo_t info){
}

void WiFiGotIP(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println(WiFi.localIP());
}

void WiFiStationDisconnected(WiFiEvent_t event, WiFiEventInfo_t info){
  Serial.println(info.disconnected.reason);
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

  /* Remove WiFi event
  Serial.print("WiFi Event ID: ");
  Serial.println(eventID);
  WiFi.removeEvent(eventID);*/
  WiFi.begin(ssid, password);
}

void loop(){
  
}
