#ifdef ESP32
  #include <WiFi.h>
  #include <Wire.h>
  #include <U8g2lib.h>
  #include <AsyncTCP.h>
#else
  #include <ESP8266WiFi.h>
  #include <ESPAsyncTCP.h>
#endif
#include <ESPAsyncWebServer.h>

// Replace with your network credentials
const char* ssid = "KT_GiGA_2G_Wave2_1205";
const char* password = "8ec4hkx000";

const char* PARAM_INPUT_1 = "state";

const int output = 4;
const int buttonPin = 3;
const int relay = 20;
const int volt_input = 1;

#include "neopixel.h"
#include "MyLittleFS.h"

// Variables will change:
int ledState = LOW;          // the current state of the output pin
int buttonState;             // the current reading from the input pin
int lastButtonState = LOW;   // the previous reading from the input pin
bool onFlag = false; 
bool offFlag = false; 
// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 100;    // the debounce time; increase if the output flickers

unsigned long lastOnOffTime = 0;
unsigned long OnOffInterval = 2000;

// Create AsyncWebServer object on port 80
AsyncWebServer server(80);
AsyncEventSource events("/events");
MyNeopixel* myNeopixel = new MyNeopixel();
MyLittleFS* myLittleFS = new MyLittleFS();


String outputState();
String processor(const String& var);
void turnOnRelay();
void turnOffRelay();


void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

  pinMode(output, OUTPUT); 
  pinMode(relay, OUTPUT);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(volt_input, INPUT_PULLUP);

  //digitalWrite(output, LOW);
  for (int i = 0; i < 10; i++)
  {
    if(analogRead(volt_input) > 4000)
    {
      digitalWrite(output, HIGH);
      myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 0, 255), 50, 50);
    }
    else
    {
      digitalWrite(output, LOW);
      myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 0), 50, 50);
    }
  }
  
  
  digitalWrite(relay, LOW);
  
  myLittleFS->InitLitteFS();
  
  IPAddress ip (172, 30, 1, 41);  // M5stamp -> 40, LCD 0.42-> 41
  IPAddress gateway (172, 30, 1, 254);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.mode(WIFI_STA);
  WiFi.config(ip, gateway, subnet);
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(F("."));
  }
  Serial.println();
  Serial.print(F("WiFi Connected -> IP : "));
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(LittleFS, "/index.html", "text/html", false, processor);
  });

  // Send a GET request to <ESP_IP>/update?state=<inputMessage>
  server.on("/update", HTTP_GET, [] (AsyncWebServerRequest *request) {
    String inputMessage;
    String inputParam;
    // GET input1 value on <ESP_IP>/update?state=<inputMessage>
    if (request->hasParam(PARAM_INPUT_1)) {
      inputMessage = request->getParam(PARAM_INPUT_1)->value();
      inputParam = PARAM_INPUT_1;
      digitalWrite(output, inputMessage.toInt()); // 밑 digitalRead 와 연관있음

      if(inputMessage.toInt())
      {
        onFlag = true;
      }
      else
      {
        offFlag = true;
      }

      
      //myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 50 * inputMessage.toInt(), 50);
      ledState = !ledState;
    }
    else {
      inputMessage = "No message sent";
      inputParam = "none";
    }
    Serial.printf("INPUT MESSAGE : %s\n", inputMessage);
    request->send(200, "text/plain", "OK");
  });

  // Send a GET request to <ESP_IP>/state
  server.on("/state", HTTP_GET, [] (AsyncWebServerRequest *request) {
    request->send(200, "text/plain", String(digitalRead(output)).c_str());
  });
  // Start server
  events.onConnect([](AsyncEventSourceClient *client){
    if(client->lastId()){
      Serial.printf("Client reconnected! Last message ID that it gat is: %u\n", client->lastId());
    }
    //send event with message "hello!", id current millis
    // and set reconnect delay to 1 second
    client->send("hello!",NULL,millis(),10000);
  });
  //HTTP Basic authentication
  server.addHandler(&events);
  server.serveStatic("/", LittleFS, "/");
  server.begin();
  Serial.printf("Server Started\n");
}
  
void loop() {

  int reading = digitalRead(buttonPin);
  
  if (reading != lastButtonState) {
    // reset the debouncing timer
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == HIGH) {
        ledState = !ledState;
      }
    }
  }

  lastButtonState = reading;

  // WiFi Reconnect
  if(onFlag)
  {
    turnOnRelay();
    onFlag = false;
  }
  else if(offFlag)
  {
    turnOffRelay();
    offFlag = false;
  }

  if ((millis() - lastOnOffTime) > OnOffInterval)
  {
    events.send("ping",NULL,millis());

    if(analogRead(volt_input) > 4000)
    {
      digitalWrite(output, HIGH);
      // On 이벤트 잘 작동 X -> 처음 킬 때는 잘 작동함
      events.send(String("On").c_str(), "current_on", millis());
      myNeopixel->pickOneLED(0, myNeopixel->strip->Color(255, 0, 255), 50, 50);
            
    }
    else
    {
      digitalWrite(output, LOW);
      Serial.println("EVENT");
      events.send(String("Off").c_str(), "current_off", millis());
    
      myNeopixel->pickOneLED(0, myNeopixel->strip->Color(200, 50, 0), 50, 50);
      delay(500);
    }
    lastOnOffTime = millis();
  }
  

}


String outputState(){
  if(digitalRead(output))
  {
    return "checked";
  }
  else 
  {
    return "";
  }
  return "";
}

// Replaces placeholder with button section in your web page
String processor(const String& var){
  //Serial.println(var);
  if(var == "BUTTONPLACEHOLDER"){
    String buttons ="";
    String outputStateValue = outputState();
    buttons+= "<h4>Output - GPIO 2 - State <span id=\"outputState\"></span></h4><label class=\"switch\"><input type=\"checkbox\" onchange=\"toggleCheckbox(this)\" id=\"output\" " + outputStateValue + "><span class=\"slider\"></span></label>";
    return buttons;
    //return outputStateValue;
  }
  // else if(var == "CURRENT")
  // {
  //   String outputStateValue;
  //   if ( analogRead(volt_input) > 4000)
  //   {
  //     outputStateValue = "On";
  //   }
  //   else
  //   {
  //     outputStateValue = "Off";
  //   }
    
  //   return outputStateValue;
  // }
  return String();
}

void turnOnRelay()
{
  digitalWrite(relay, LOW);
  delay(50);
  digitalWrite(relay, HIGH);
  delay(50);
  digitalWrite(relay, LOW);
  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(0, 255, 0), 50, 50);
}

void turnOffRelay()
{
  digitalWrite(relay, HIGH);
  while(analogRead(volt_input) > 4000)
  {
    delay(100);
  }
  digitalWrite(relay, LOW);
  myNeopixel->pickOneLED(0, myNeopixel->strip->Color(50, 50, 50), 5, 50);
}