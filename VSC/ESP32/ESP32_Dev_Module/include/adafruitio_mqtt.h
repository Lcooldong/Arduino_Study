// io.adafruit.com root CA
#ifndef __AIO_MQTT_H__
#define __AIO_MQTT_H__

#define MQTT_BROKER   "io.adafruit.com"
#define MQTT_PORT     1883
#define MQTT_USERNAME "CoolDong"
#define AIO_KEY       "aio_mBft13w1ftSP1DT8xqQ7UsJh11PF"

#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

class MQTT
{

public:
    void MQTT_connect(Adafruit_MQTT_Client _client);
    void MQTT_reconnect(Adafruit_MQTT_Client _client);

private:
    
};




#endif