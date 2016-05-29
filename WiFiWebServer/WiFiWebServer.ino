#include <Arduino.h>

#include <ESP8266WiFi.h>
#include <EEPROM.h>
#include <DHT.h>

#define FAN 4
#define LIGHT 5
#define DHTPIN 0
#define DHTTYPE DHT11
#define BUTTON 2

bool fan;
bool light;
bool aux;
int reading;
int addr_fan = 0;
int addr_light = 1;
long tempo = 0;
long debounce = 500;

const char *ssid = "xxx";
const char *password = "xxxxxx";

IPAddress ip(192, 168, 1, 177);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(1212);
DHT dht(DHTPIN, DHTTYPE);

void setup() {
    Serial.begin(115200);
    WiFi.begin(ssid, password);
    WiFi.config(ip, gateway, subnet);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    server.begin();
    /**********************************************************************/
    fan = aux = EEPROM.read(addr_fan);
    digitalWrite(FAN, aux);
    light = aux = EEPROM.read(addr_light);
    digitalWrite(LIGHT, aux);
    /**********************************************************************/
    dht.begin();
    pinMode(BUTTON, INPUT);
    pinMode(FAN, OUTPUT);
    pinMode(LIGHT, OUTPUT);
    /**********************************************************************/
}

void loop() {

    reading = digitalRead(BUTTON);
  
    WiFiClient client = server.available();
    if (!client) {
        return;
    }

    Serial.println("new client");
    while (!client.available()) {
        delay(1);
    }

    String req = client.readStringUntil('\r');
    Serial.println(req);
    client.flush();

    if (req.indexOf("/a") != -1) {
        fan = !fan;
        digitalWrite(FAN, fan);
        EEPROM.write(addr_fan, fan);
    } else if (req.indexOf("/b") != -1) {
        light = !light;
        digitalWrite(LIGHT, light);
        EEPROM.write(addr_light, light);
    } else if (req.indexOf("/c") == -1) {
        Serial.println("invalid request");
        client.stop();
        return;
    }

    client.flush();
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("Connection: close");
    client.println();
    client.println(light);
    client.println(fan);
    client.println(dht.readTemperature());
    client.println(dht.readHumidity());
    
    delay(1);
    Serial.println("Client disonnected");
}
