#include <ESP8266WiFi.h>
#include <PubSubClient.h>
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

//const char* ssid = "iptime245";
//const char* password = "pw101485";

const char* ssid = "LDH";
const char* password = "ehdgml43";
const char* mqtt_server = "broker.mqtt-dashboard.com";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE  (50)
char msg[MSG_BUFFER_SIZE];
int value = 0;




void setup() {
#if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
  clock_prescale_set(clock_div_1);
#endif
  
  pinMode(BUILTIN_LED, OUTPUT);     // Initialize the BUILTIN_LED pin as an output

  Serial.begin(115200);
  lcd.init();
  lcd.backlight();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  
  strip.begin();           // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();            // Turn OFF all pixels ASAP
  strip.setBrightness(50); // Set BRIGHTNESS to about 1/5 (max = 255)
  colorWipe(strip.Color(0, 0, 0), 0);
}



void loop() {
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
}



void setup_wifi() {

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  lcd.setCursor(0,0); //첫번째 line
  lcd.print("NodeMCU Neopixel");
  lcd.setCursor(0,1); //두번째 line
  lcd.print(WiFi.localIP().toString());
}

void callback(char* topic, byte* payload, unsigned int length) {

  String mypayload;
  for (int i = 0; i < length; i++) {
    mypayload += (char)payload[i];
  }

  String mytopic = topic;

  Serial.print("NodeMCU TOPIC=");
  Serial.println(mytopic);
  Serial.print("NodeMCU PAYLOAD=");
  Serial.println(mypayload);

  if(mytopic == "nodemcu/rgb"){
    int r = hex2byte(mypayload.substring(1,3)); //bc
    int g = hex2byte(mypayload.substring(3,5)); //24
    int b = hex2byte(mypayload.substring(5,7)); //24

    colorWipe(strip.Color(r, g, b), 50);
    client.publish("nodemcu/notice", "color chaged");

    if(mypayload == "Rainbow") rainbow(10);
    if(mypayload == "nothing") colorWipe(strip.Color(0, 0, 0), 50);
  }
  
//  if(mytopic == "nodemcu/rainbow1"){
//    int r = hex2byte(mypayload.substring(1,3)); //bc
//    int g = hex2byte(mypayload.substring(3,5)); //24
//    int b = hex2byte(mypayload.substring(5,7)); //24
//    
//    if(mypayload == "1")rainbow(10);
//    else colorWipe(strip.Color(r, g, b), 50);
//    client.publish("nodemcu/notice", "style changed");
//  }
  
//  if(mytopic == "nodemcu/rainbow2"){
//    theaterChaseRainbow(50);
//    client.publish("nodemcu/notice", "style changed");
//  }
 

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
  }else if(input[0] >= 'A' && input[0] <= 'F'){
    num1 = input[0]-'A'+10;
  }

  if(input[1] >= '0' && input[1] <= '9'){
    //숫자인 경우
    num2 = input[1] - '0';
  }else if(input[1] >= 'A' && input[1] <= 'F'){
    num2 = input[1]-'A'+10;
  }

  return num1*16+num2;
}

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic", "NodeMCU Conneted");
      // ... and resubscribe
      //내가 구독하고자하는 topic을 등록하는 부분
      client.subscribe("nodemcu/rgb");
//      client.subscribe("nodemcu/rainbow1"); 
//      client.subscribe("nodemcu/rainbow2"); //너무 느림
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
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
