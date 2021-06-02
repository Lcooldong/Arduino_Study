#define led 13

void setup(){

    Serial.begin(9600);
    pinMode(led, OUTPUT);
}

void loop(){

    digitalWrite(led, HIGH);
    Serial.println("HIGH");
    delay(200);
    digitalWrite(led, LOW);
    Serial.println("LOW");
    delay(200);

}
