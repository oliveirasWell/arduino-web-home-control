#include <DHT.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EEPROM.h> // incluir a biblioteca
int addrFa = 0;    // endereço de escrita na memória
int addrLi = 1;

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network:
byte mac[] = {
        0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED
};
IPAddress ip(192, 168, 1, 177);

// Initialize the Ethernet server library
// with the IP address and port you want to use
// (port 80 is default for HTTP):
EthernetServer server(1212);
EthernetClient client;

#define DHTPIN 2
#define DHTTYPE DHT11   // DHT 11
#define BUTTON 3
DHT dht(DHTPIN, DHTTYPE);

char stringIN[500] = "";
int iterator;
bool light;
bool fan;
float temperatura = 0; //Variavel onde vai ser guardada a Temperatura
int valorLido = 0; // Variavel onde se ira guardar o valor lido pelo LM53  
int pinoLM35 = 1; // Pino Analogico onde vai ser ligado a pata do meio do LM35;
#define FAN 7
#define LIGHT 6
int counter=1800000;

bool auxB;

int reading;           // the current reading from the input pin
int previous = LOW;    // the previous reading from the input pin

// the follow variables are long's because the time, measured in miliseconds,
// will quickly become a bigger number than can be stored in an int.
long time = 0;         // the last time the output pin was toggled
long debounce = 500;   // the debounce time, increase if the output flickers


void setup() {
    // Open serial communications and wait for port to open:
    Serial.begin(9600);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for Leonardo only
    }

    dht.begin();

    pinMode(BUTTON, INPUT);
    pinMode(FAN, OUTPUT);
    pinMode(LIGHT, OUTPUT);

    // start the Ethernet connection and the server:
    Ethernet.begin(mac, ip);
    server.begin();
    Serial.print("server is at ");
    Serial.println(Ethernet.localIP());

    iterator = 0;
    temperatura = 0;


    auxB = EEPROM.read(addrFa);
    Serial.print("Memoria FA: ");
    Serial.println(auxB);

    auxB = EEPROM.read(addrLi);
    Serial.print("Memoria LI: ");
    Serial.println(auxB);


    fan = auxB = EEPROM.read(addrFa);
    digitalWrite(FAN, auxB );

    light = auxB = EEPROM.read(addrLi);

    digitalWrite(LIGHT, auxB);

}

void loop() {
    bool writeFlag = false;
    // listen for incoming clients
    client = server.available();

    reading = digitalRead(BUTTON);

    if (client) {
        Serial.println("new client");
        // an http request ends with a blank line
        boolean currentLineIsBlank = true;
        while (client.connected()) {
            if (client.available()) {
                char c = client.read();
                stringIN[iterator] = c;
                // if you've gotten to the end of the line (received a newline
                // character) and the line is blank, the http request has ended,
                // so you can send a reply
                if (c == '\n' && currentLineIsBlank) {
                    writeFlag = true;
                    break;
                }
                if (c == '\n') {
                    // you're starting a new line
                    currentLineIsBlank = true;
                }
                else if (c != '\r') {
                    // you've gotten a character on the current line
                    currentLineIsBlank = false;
                }
                iterator++;
            }

        }
        Serial.write("\n-\n");
        Serial.write(stringIN);
        Serial.write("-\n");

        int j = 0;
        char inAux[10] = "";
        do{
            j++;
            inAux[j] = stringIN[j];
        }while(stringIN[j] != ' ');

        Serial.write("#");
        Serial.write(inAux);
        Serial.write("\n");
        Serial.write(j+48);
        Serial.write("#");

        if(j == 3){
            j++;
            if(stringIN[j] == '/'){
                j++;
                if(stringIN[j] == 'a' && stringIN[j+1] == ' '){
                    light = !light;
                    digitalWrite(LIGHT, light);
                    EEPROM.write(addrLi, light);
                }
                else if(stringIN[j] == 'b' && stringIN[j+1] == ' '){
                    fan = !fan;
                    digitalWrite(FAN, fan);
                    EEPROM.write(addrFa, fan);
                } else if( !(stringIN[j] == 'c' && stringIN[j+1] == ' ')){
                    client.stop();
                }
            }
        }
        if(writeFlag){
            // send a standard http response header
            client.println("HTTP/1.1 200 OK");
            client.println("Content-Type: text/html");
            client.println("Connection: close");  // the connection will be closed after completion of the response
            //client.println("Refresh: 5");  // refresh the page automatically every 5 sec
            client.println();
            //client.println("<!DOCTYPE HTML>");
            //client.println("<html>");
            //client.println("<font face=\"Helvetica\" size=\"20\">");
            client.println(light);
            //client.println("<br />");
            client.println(fan);
            //client.println("<br />");
            client.println(dht.readTemperature());
            //client.println("<br />");
            client.println(dht.readHumidity());
            //client.println("<br />");
            //client.println("</html>");
            //client.println("</font>");

            auxB = EEPROM.read(addrFa);
            Serial.println();
            Serial.print("Enviada as informacoes");
            Serial.print("Memoria FA: ");
            Serial.println(auxB);

            auxB = EEPROM.read(addrLi);
            Serial.print("Memoria LI: ");
            Serial.println(auxB);
            iterator = 0;
            // give the web browser time to receive the data
            delay(1);
            // close the connection:
            client.stop();
            Serial.println("client disconnected");
        }
        /*
          Serial.print("Humidity: ");
          Serial.print(dht.readHumidity());
          Serial.print(" %\t");
          Serial.print("Temperature: ");
          Serial.print(dht.readTemperature());
          Serial.println(" *C");
        */

/*  if(counter==1800000){
    counter=0;
    Serial.println("Enviado ao ThingSpeak");
    updateThingSpeak();
  }else{
    delay(1);
   counter++;
  } */
    }
    if (reading == HIGH && previous == LOW && millis() - time > debounce) {
        light = !light;
        digitalWrite(LIGHT, light);
        EEPROM.write(addrLi, light);
        time = millis();
    }

}

float CheckTemp(){
    valorLido = analogRead(pinoLM35); //Coloca na Variavel valorLido o que está a ser lido pelo sensor.
    temperatura = (valorLido * 0.00488); ///
    temperatura = temperatura * 100; /// Conversão do valor obtido atravez da porta analogica para um valor “usavel”
    Serial.print("Temperatura actual: "); // Imprime no Serial Monitor “Temperatura Actual:”
    Serial.println(temperatura); // Imprime no Serial monitor o valor da variavel temperatura
    return temperatura; // Retorna A temperatura
}

void updateThingSpeak() {
    if (client.connect("api.thingspeak.com", 80))
    {
        int a = dht.readTemperature();
        int b = dht.readHumidity();
        String aux = "field1="+String(a)+'&'+"field2="+String(b);

        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: Z3SJPV6NFJAHO1J6\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(aux.length());
        client.print("\n\n");
        client.print(aux);
        client.stop();

        //to do
        //Serial
        // Connected
    }
    /*
      to do
      else
      Not Connected

    */
}