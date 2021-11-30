/**parte per inviare dati al raspberry tramite MQTT*/
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "WiFi_config.h"
char ssid[] = SECRET_SSID;     // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient); // client MQTT

const char broker[] = "192.168.1.7";// ip broker mqtt
int        port     = 1883;
const char topic[]  = "fire_warning";
const char topic2[]  = "lectures";


//fisso intervallo per inviare dati all'MQTT broker
const long interval = 4000;
unsigned long previousMillis = 0;

int count = 0;
/*fine parte per inviare dati a raspberry tramite MQTT*/

#include <MQ2.h>
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"


//variabili per lettura analogica MQ-2:
int Analog_Input = A0; // pin A0 Arduino
int lpg = 0, co = 0, smoke = 0;
MQ2 mq2(Analog_Input);

//variabili per lettura digitale MQ-2:
int digitalPinMQ2 = 4; //MQ-2 digital interface
int digitalValMQ2;

// variabili per lettura SHT-31D:
Adafruit_SHT31 sht31 = Adafruit_SHT31();

//variabili per lettura digitale KY-026:
int digitalPin = 2; // KY-026 digital interface
int digitalVal = 0;


void setup() {
  Serial.begin (9600); // avvio del monitor seriale


  /*tentativo di connessione alla rete wifi e al broker mqtt*/
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED) {
    // failed, retry
    Serial.print(".");
    delay(5000);
  }

  Serial.println("You're connected to the network");
  Serial.println();

  Serial.print("Attempting to connect to the MQTT broker: ");
  Serial.println(broker);

  if (!mqttClient.connect(broker, port)) {
    Serial.print("MQTT connection failed! Error code = ");
    Serial.println(mqttClient.connectError());

    while (1);
  }
  Serial.println("You're connected to the MQTT broker!");
  Serial.println();
  /**/


  pinMode (Analog_Input, INPUT); //rx dati analogici dal pin dell'MQ-2
  pinMode(digitalPinMQ2, INPUT); //rx dati digitali dal pin dell'MQ-2

  pinMode(digitalPin, INPUT); //rx dati digitali dal pin del KY-026

  pinMode (8, OUTPUT); // tx comandi al pin del buzzer
  pinMode (12, OUTPUT); // tx comandi al pin del led rosso
  pinMode (11, OUTPUT);  // tx comandi al pin del led verde

  digitalWrite(11, HIGH); //accendo led verde se le connessioni sono giuste


  /*esegue il setup dello sketch per misurare temperatura e umidità*/
  Serial.println("SHT31 test");
  if (!sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
    Serial.println("Couldn't find SHT31");
    while (1) delay(1);
  }

  mq2.begin();
}




void loop() {

  mqttClient.poll(); // manda mess di keep alive per evitare di essere disconnessi dal broker (Raspberry)


  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time a message was sent
    previousMillis = currentMillis;



    /*lettura dati digitali del KY-026*/
    digitalVal = digitalRead(digitalPin);
    Serial.println("digital lecture of KY-026: ");
    Serial.println(digitalVal);
    delay(4000);

    /*lettura dati digitali dell'MQ-2*/
    digitalValMQ2 = digitalRead(digitalPinMQ2);
    Serial.println("digital lecture of MQ-2: ");
    Serial.println(digitalValMQ2);
    delay(4000);


    if (digitalVal == HIGH || digitalValMQ2 == LOW) {
      Serial.println("E' stato rilevato un pericolo dai sensori");

      /*eseguo le letture della composizione in ppm dell'MQ-2*/
      float* values = mq2.read(true); 
      lpg = mq2.readLPG();
      co = mq2.readCO();
      smoke = mq2.readSmoke();


      Serial.print("Sending message to topic: ");
      Serial.println(topic);

      mqttClient.beginMessage(topic);
      mqttClient.println("PERICOLO RILEVATO DA SENSORE 1");
      mqttClient.endMessage();


      sound();
      changeLed();




      /*acquisizione valori tempoeratura/umidità*/
      float t = sht31.readTemperature();
      Serial.print("Temp *C = "); Serial.print(t); Serial.print("\t\t");
      float h = sht31.readHumidity();
      Serial.print("Hum. % = "); Serial.println(h);

    }


    else {// ky low, mq-2 alto

      Serial.print("Sending message to topic2: ");
      Serial.println(topic2);

      mqttClient.beginMessage(topic2);
      mqttClient.println("NESSUN PERICOLO RILEVATO DA SENSORE 1");
      mqttClient.endMessage();

      digitalWrite(11, HIGH); //parto con led verde acceso
    }

  }


}






//funzioni del codice:

/* funzione per scambiare i led da verde a rosso e farlo lampeggiare*/
void changeLed() {
  digitalWrite (11, LOW); // spengo led verde
  digitalWrite (12, HIGH); // accendo led rosso

  delay (1000);

  digitalWrite (12, LOW); // spengo led rosso
  delay (1000);
}
/*funzione per far suonare il buzzer a intermittenza*/
void sound() {
  digitalWrite (8, HIGH);
  delay (1000);
  digitalWrite (8, LOW);
  delay (1000);
}
