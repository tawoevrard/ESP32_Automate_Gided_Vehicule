#include <Arduino.h>
// Pins und Variablen für die Motoren
const int ENA = 23; // PMW Pin für den linken Motor
const int IN1 =22 ;// motor Left 
const int IN2 =21 ;// motor Left
const int ENB = 4; // PMW Pin für den rechten Motor
const int IN3 =19 ; // motor Right    
const int IN4 =18 ;// motor Right
// Pins und Variablen für die Sensoren
const int trig = 14; // Pin für den Trigger des Ultraschallsensors
const int echo = 27; // Pin für das Echo des Ultraschallsensors
const int DO1 =35 ;// Pins für rechte linien-Sensor
const int DO2 =34 ;// Pins für mittlere Sensor
const int DO3 =32 ; // Pins für Linke Sensor
// ---ESP 32 PWM Frequenz und Auflösung
const int freq = 3000; // Frequenz für PWM
const int resolution = 8; // Auflösung für PWM (0 bis 255)
const int channelA = 0; // Kanal für linken Motor
const int channelB = 1; // Kanal für rechten Motor

// Geschwindigkeit der Motoren (0 bis 255)
const int speed_forward = 150; //  Geschwindigkeit für vorwärts
const int speed_curve = 100; //  Geschwindigkeit für Kurvenfahrt
const int speed_stop = 0; //  Geschwindigkeit für Stop

// Definition des Zustands des Roboters FSM 
enum state {STOP, 
    FORWARD, 
    LEFT, 
    RIGHT};
 state currentState = STOP; // Initialisierung des Zustands auf STOP
 void setup(){
    Serial.begin(115200);  // damit ich den Ausgäge im monitor lesen kann 
  // Initialisierung der Motoren
  pinMode(IN1, OUTPUT); 
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT); 
  pinMode(IN4, OUTPUT);
  digitalWrite(IN1, LOW); 
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);

  // Initialisierung der Sensoren
  pinMode(trig, OUTPUT);
  pinMode(echo, INPUT);
  pinMode(DO1, INPUT);
  pinMode(DO2, INPUT);
  pinMode(DO3, INPUT);
    stop(); // Roboter im STOP-Zustand starten
    // Initialisierung der PWM für die Motoren
  
  ledcAttach(ENA,freq, resolution); // linken Motor an Kanal 0 anschließen
  ledcAttach(ENB,freq, resolution); // rechten Motor an Kanal 1
 }
 void loop(){
 int rechtesensor = digitalRead(DO1);
 int mittlereSensor = digitalRead(DO2);
 int linkesSensor = digitalRead(DO3);
 int distanz = messen(); // Distanz messen
 // live werte im seriellen monitor anzeigen
 Serial.print("L: "); Serial.print(linkesSensor);
  Serial.print(" | M: "); Serial.print(mittlereSensor);
  Serial.print(" | R: "); Serial.print(rechtesensor);
  Serial.print(" | Dist: "); Serial.println(distanz);
 // FSM Logik
if (distanz < 20) { // Wenn die Distanz kleiner als 20 cm ist
    currentState = STOP; // Zustand auf STOP setzen
  } else if ((linkesSensor == LOW && mittlereSensor == HIGH && rechtesensor == LOW)||(linkesSensor == HIGH && mittlereSensor == HIGH)||(mittlereSensor == HIGH && rechtesensor == HIGH)) {
    currentState = FORWARD; // Zustand auf FORWARD setzen
  } else if (linkesSensor == LOW && mittlereSensor == LOW && rechtesensor == HIGH) {
    currentState = RIGHT; // Zustand auf RIGHT setzen
  } else if (linkesSensor == HIGH && mittlereSensor == LOW && rechtesensor == LOW) {
    currentState = LEFT; // Zustand auf LEFT setzen
  } else {
    currentState = STOP; // Zustand auf STOP setzen
  } 
  // Aktionen basierend auf dem aktuellen Zustand
  switch (currentState) {
    case STOP:
      stop();
      break;
    case FORWARD:
      vorwaerts();
      break;
    case LEFT:
      Links();
      break;
    case RIGHT:
      Rechts();
      break;
  }
  delay(10); // Kurze Verzögerung, um die Schleife zu stabilisieren
 }
 // Funktionen für die Bewegungen des Roboters
void stop(){
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, LOW);
    // Geschwindigkeit der Motoren begrenzen 
    ledcWrite(ENA, speed_stop); // Geschwindigkeit für linken Motor auf 0 setzen
    ledcWrite(ENB, speed_stop); // Geschwindigkeit für rechten Motor auf 0 setzen
}
void vorwaerts(){
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    // Geschwindigkeit der Motoren begrenzen
    ledcWrite(ENA, speed_forward); // Geschwindigkeit für linken Motor setzen
    ledcWrite(ENB, speed_forward); // Geschwindigkeit für rechten Motor setzen
}


void Links(){
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH);
    digitalWrite(IN4, LOW);
    // Geschwindigkeit der Motoren begrenzen
    ledcWrite(ENA, speed_curve); // Geschwindigkeit für linken Motor setzen
    ledcWrite(ENB, speed_forward); // Geschwindigkeit für rechten Motor setzen
}
void Rechts(){
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW);
    digitalWrite(IN4, HIGH);
    // Geschwindigkeit der Motoren begrenzen
    ledcWrite(ENA, speed_forward); // Geschwindigkeit für linken Motor setzen
    ledcWrite(ENB, speed_curve); // Geschwindigkeit für rechten Motor setzen
}
// Funktion zum Messen der Distanz
int  messen(){ // Funktion zum Messen der Distanz
    digitalWrite(trig, LOW); 
    delayMicroseconds(2);
    digitalWrite(trig, HIGH);
    delayMicroseconds(10);
    digitalWrite(trig, LOW);
    long dauer = pulseIn(echo, HIGH); // Dauer speichern, bis das Echo zurückkommt
    int distanz = dauer * 0.034 / 2; // Distanz in cm berechnen (Schallgeschwindigkeit: 343 m/s)
    return distanz; // Distanz zurückgeben
}