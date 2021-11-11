

void setup() {
pinMode (8, OUTPUT); // direzione del pin buzzer
//pinMode (12, OUTPUT); // direzione del pin led
Serial.begin (9600); // avvio del monitor seriale
}
 
void loop() {
int fiamma = analogRead(A0); // rilevo il valore da A0
Serial.println(fiamma); // stampo su monitor seriale
 
if (fiamma > 350) { // se il valore supera 350...
 
alarm(); // faccio lampeggiare il led e faccio suonare il buzzer
sound();
}
else {
 
// altrimenti non fa nulla
}
}
 
void alarm(){
digitalWrite (12, HIGH);
delay (500);
digitalWrite (12, LOW);
delay (500);
}
 
void sound(){
digitalWrite (8, HIGH);
delay (1000);
digitalWrite (8, LOW);
delay (1000);
}
