/*parte per inviare dati al raspberry tramite MQTT*/
#include <ArduinoMqttClient.h>
#include <WiFiNINA.h>
#include "WiFi_config.h"
char ssid[] = SECRET_SSID;     // your network SSID (name)
char pass[] = SECRET_PASS;    // your network password

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient); // client MQTT

const char broker[] = "192.168.1.5";
int        port     = 1883;
const char topic[]  = "fire_warning";
//const char topic2[]  = "real_unique_topic_2";
//const char topic3[]  = "real_unique_topic_3";

//set interval for sending messages (milliseconds)
const long interval = 8000;
unsigned long previousMillis = 0;

int count = 0;
/*fine parte per inviare dati a raspberry tramite MQTT*/

bool Fire_Warning= false; // variabile per capire se ho rilevato un pericolo

#include <MQ2.h>
#include <Arduino.h>
#include <Wire.h>
#include "Adafruit_SHT31.h"


//variabili per lettura analogica MQ-2:
int Analog_Input = A0; // pin A0 dell'MQ2
int lpg, co, smoke;
MQ2 mq2(Analog_Input);

//variabili per lettura digitale MQ-2:
int digitalPinMQ2 = 4; //MQ-2 digital interface
int digitalValMQ2;


//variabili per SHT31D:
//bool enableHeater = false;
//uint8_t loopCnt = 0;
Adafruit_SHT31 sht31 = Adafruit_SHT31();

//variabili per lettura digitale KY-026:
int digitalPin = 2; // KY-026 digital interface
int digitalVal=0;




void setup() {
  Serial.begin (9600); // avvio del monitor seriale
  


  /*tentativo di connessione alla rete wifi e al broker mqtt*/ 
  Serial.print("Attempting to connect to WPA SSID: ");
  Serial.println(ssid);
  while (WiFi.begin(ssid, pass) != WL_CONNECTED){
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
  //digitalWrite(11, HIGH); //parto con led verde acceso

  mqttClient.poll(); // manda mess di keep alive per evitare di essere disconnessi dal broker (Raspberry)
  
//  mqttClient.subscribe(fire_warning);

  /*lettura dati digitali del KY-026*/
  digitalVal = digitalRead(digitalPin);
  Serial.println("lettura digitale KY-026: ");
  Serial.println(digitalVal);
  delay(1000);

  /*lettura dati digitali dell'MQ-2*/
  digitalValMQ2 = digitalRead(digitalPinMQ2);
  Serial.println("lettura digitale MQ-2: ");
  Serial.println(digitalValMQ2);
  delay(1000);

  /*pensare se vogliamo implementare un controllo del funzionamento  corretto KY-026 e MQ-2 verificando se questi suonano in corrispondenza di un aumento di temperatura, se questa non aumenta potrebbe esserci un malfunzionamento di uno dei due*/
  if (digitalVal == HIGH || digitalValMQ2 == LOW) {// pensare eventualmente ad un AND e non un OR
    Serial.println("E' stato rilevato un pericolo dai sensori");

    Fire_Warning=true;

    /*eseguo le letture della composizione in ppm dell'MQ-2*/
    float* values = mq2.read(true); //set it false if you don't want to print the values in the Serial
    lpg = mq2.readLPG();
    co = mq2.readCO();
    smoke = mq2.readSmoke();

    Serial.print("Sending message to topic: ");
    Serial.println(topic);
    Serial.println("hello");

    mqttClient.beginMessage(topic);
    mqttClient.println(Fire_Warning);
    mqttClient.println(smoke);
    mqttClient.println(lpg);
    mqttClient.println(co);
    
    mqttClient.endMessage();
    
    /*acquisizione valori tempoeratura/umidità*/
    float t = sht31.readTemperature();
    Serial.print("Temp *C = "); Serial.print(t); Serial.print("\t\t");
    float h = sht31.readHumidity();
    Serial.print("Hum. % = "); Serial.println(h);

    sound();
    cambioLed();
  }
  else {// ky low, mq-2 alto
   Serial.println("Nessun pericolo è stato rilevato dai sensori");
   digitalWrite(11, HIGH); //parto con led verde acceso
  }
}



//funzioni del codice:

/* funzione per scambiare i led da verde a rosso e farlo lampeggiare*/
void cambioLed() {
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









//funzioni non usate nel loop, le conservo per sicurezza:

/*funzione per fare il setup necessario per il sensore SHT-31D*/
//void setupSHT31D() {
//  Serial.println("SHT31 test");
//  if (!sht31.begin(0x44)) {   // Set to 0x45 for alternate i2c addr
//    Serial.println("Couldn't find SHT31");
//    while (1) delay(1);
//  }
//
//  /*Serial.print("Heater Enabled State: ");
//    if (sht31.isHeaterEnabled())
//    Serial.println("ENABLED");
//    else
//    Serial.println("DISABLED");
//    }*/
//
//}

/*funzione per calcolo temperatura/umidità*/
//float SHT31D() {
//  float t = sht31.readTemperature();
//  float h = sht31.readHumidity();
//
//  if (! isnan(t)) {  // check if 'is not a number'
//    Serial.print("Temp *C = "); Serial.print(t); Serial.print("\t\t");
//  } else {
//    Serial.println("Failed to read temperature");
//  }
//
//  if (! isnan(h)) {  // check if 'is not a number'
//    Serial.print("Hum. % = "); Serial.println(h);
//  } else {
//    Serial.println("Failed to read humidity");
//  }
//
//  delay(1000);
//
//  // Toggle heater enabled state every 30 seconds
//  // An ~3.0 degC temperature increase can be noted when heater is enabled
//  if (++loopCnt == 30) {
//    enableHeater = !enableHeater;
//    sht31.heater(enableHeater);
//    Serial.print("Heater Enabled State: ");
//    if (sht31.isHeaterEnabled())
//      Serial.println("ENABLED");
//    else
//      Serial.println("DISABLED");
//
//    loopCnt = 0;
//  }
//}
