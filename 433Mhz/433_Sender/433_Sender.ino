#include <RH_ASK.h>
#include <SPI.h> // Not actually used but needed to compile
#include <time.h>

RH_ASK driver;

void setup()
{
    Serial.begin(115200);    // Debugging only
    if (!driver.init())
         Serial.println("init failed");

    srand(time(NULL));
}

void loop()
{
    int randnum = rand();

    int modnum = randnum % 3;
    if (modnum == 0) { // send red
        const char *msg = "red";
        if (driver.send((uint8_t *)msg, strlen(msg))) {
            driver.waitPacketSent();
            Serial.println(msg);
        }
        else {
          Serial.println("Failed to send a message...");
        }
    }
    else if (modnum == 1) { // send green
        const char *msg = "green";
        if (driver.send((uint8_t *)msg, strlen(msg))) {
            driver.waitPacketSent();
            Serial.println(msg);
        }
        else {
          Serial.println("Failed to send a message...");
        }
    }
    else { // send yellow
        const char *msg = "yellow";
        if (driver.send((uint8_t *)msg, strlen(msg))) {
            driver.waitPacketSent();
            Serial.println(msg);
        }
        else {
          Serial.println("Failed to send a message...");
        }
    }
    
    delay(1000);
}
