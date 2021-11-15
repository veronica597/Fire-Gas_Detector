int sogliaGas = 400;
int sogliaFiamma =350;

void setup() {
  
pinMode (8, OUTPUT); // direzione del pin buzzer
//pinMode (12, OUTPUT); // direzione del pin led rosso
//pinMode (11, OUTPUT);  // direzione del pin led verde
//pinMode ( A5, INPUT); // leggo dati in ingresso dal sensore di gas MQ-2
Serial.begin (9600); // avvio del monitor seriale
}
 
void loop() {
int fiamma = analogRead(A0); // rilevo il valore da A0
int gas = analogRead (A5); // rilevo il valore da A5

Serial.println(fiamma); // stampo su monitor seriale

Serial.print("valore del MQ-2: ");
Serial.println(gas);

if (gas > sogliaGas){
    cambioLed();
    sound();
  }
 
if (fiamma > sogliaFiamma){
 
    cambioLed(); 
    sound();
    
  }
else { 
    LedVerde();
    noTone(8); // non fa suonare il buzzer
  }
}




void LedVerde(){ // spengo led rosso e accendo led verde
  digitalWrite(12,LOW);
  digitalWrite(11,HIGH);
  }
 
void cambioLed(){ // funzione per scambiare i led da verde a rosso, e lo fa lampeggiare i led
digitalWrite (11, LOW); // spengo led verde
digitalWrite (12, HIGH); // accendo led rosso

delay (500);

digitalWrite (12, LOW); // spengo led rosso
delay (500);
}
 
void sound(){ // funzione per far suonare il buzzer
digitalWrite (8, HIGH);
delay (1000);
digitalWrite (8, LOW);
delay (1000);
}
