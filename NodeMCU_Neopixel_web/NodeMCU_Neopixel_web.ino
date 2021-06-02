#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
 #include <avr/power.h> // Required for 16 MHz Adafruit Trinket
#endif

#define LED_PIN   D7
#define LED_COUNT 30

LiquidCrystal_I2C lcd(0x27,16,2);//1602
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

#ifndef STASSID
#define STASSID "iptime245"
#define STAPSK  "pw101485"
#endif

const char* ssid     = STASSID;
const char* password = STAPSK;

ESP8266WebServer server(80);

const int led = LED_BUILTIN;

const String postForms = "<html>\
  <head>\
    <meta charset=\"utf-8\">\
    <title>IoT 무드등 제어!</title>\
  </head>\
  <body>\
    <h1>IoT 무드등 제어!</h1><br>\
    <form method=\"post\" enctype=\"application/x-www-form-urlencoded\" action=\"/postform/\">\
      <h1>색상을 하나 골라주세요! </h1><input type=\"color\" name=\"mycolor\">\
      <input type=\"submit\" value=\"Submit\">\
    </form>\
  </body>\
</html>";

void handleRoot() {
  digitalWrite(led, 1);
  server.send(200, "text/html", postForms); //response
  digitalWrite(led, 0);
}

void handleForm() {
  if (server.method() != HTTP_POST) {
    digitalWrite(led, 1);
    server.send(405, "text/plain", "Method Not Allowed");
    digitalWrite(led, 0);
  } else {
    digitalWrite(led, 1);
    String message = "<html><meta charset=\"utf-8\">";

    //server.arg(0)
    //#bc2424
    //01234567
    int r = hex2byte(server.arg(0).substring(1,3)); //bc
    int g = hex2byte(server.arg(0).substring(3,5)); //24
    int b =hex2byte(server.arg(0).substring(5,7)); //24

    colorWipe(strip.Color(r, g, b), 50);
    
    for (uint8_t i = 0; i < server.args()-1; i++) {
      message += "변수명 = " + server.argName(i) + ", 값 = " + server.arg(i) + "<BR>";
    }
    message += "</html>";
    server.send(200, "text/html", message);
    digitalWrite(led, 0);
  }
}

byte hex2byte(String input){
  //bc
  //input[0] //b
  //input[1] //c
  //0~9 A(10), B(11), C(12), D(13), E(14), F(15)

  int num1 = 0;
  int num2 = 0;
  if(input[0] >= '0' && input[0] <= '9'){
    //숫자인 경우
    num1 = input[0] - '0';
  }else if(input[0] >= 'a' && input[0] <= 'f'){
    num1 = input[0]-'a'+10;
  }

  if(input[1] >= '0' && input[1] <= '9'){
    //숫자인 경우
    num2 = input[1] - '0';
  }else if(input[1] >= 'a' && input[1] <= 'f'){
    num2 = input[1]-'a'+10;
  }

  return num1*16+num2;
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}

void colorWipe(uint32_t color, int wait) {
  for(int i=0; i<strip.numPixels(); i++) { // For each pixel in strip...
    strip.setPixelColor(i, color);         //  Set pixel's color (in RAM)
    strip.show();                          //  Update strip to match
    delay(wait);                           //  Pause for a moment
  }
}

void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) { 
    for(int b=0; b<3; b++) { 
      strip.clear();         
      for(int c=b; c<strip.numPixels(); c += 3) {
        strip.setPixelColor(c, color); 
      }
      strip.show(); 
      delay(wait);  
    }
  }
}

void rainbow(int wait) {
  for(long firstPixelHue = 0; firstPixelHue < 5*65536; firstPixelHue += 256) {
    for(int i=0; i<strip.numPixels(); i++) { 
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); 
    delay(wait);  
  }
}

void theaterChaseRainbow(int wait) {
  int firstPixelHue = 0;     
  for(int a=0; a<30; a++) {  
    for(int b=0; b<3; b++) { 
      strip.clear();        
      for(int c=b; c<strip.numPixels(); c += 3) {   
        int      hue   = firstPixelHue + c * 65536L / strip.numPixels();
        uint32_t color = strip.gamma32(strip.ColorHSV(hue)); 
        strip.setPixelColor(c, color); 
      }
      strip.show();               
      delay(wait);                 
      firstPixelHue += 65536 / 90; 
    }
  }
}








void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.setCursor(0,0); //첫번째 line
  lcd.print("Nockanda light");
  lcd.setCursor(0,1); //두번째 line
  lcd.print(WiFi.localIP().toString());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);

  server.on("/postform/", handleForm);

  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
  
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif

  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  colorWipe(strip.Color(0, 0, 0), 0);

  
}

void loop(void) {
  server.handleClient();
}
