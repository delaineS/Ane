//use esp v 2.5.2

#include <ESP8266WiFi.h>
#include "DHT.h"
#include <Wire.h>
#include <BMx280I2C.h>
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID   "TMPLACafk6Ag"
#include <SPI.h>
#include <BlynkSimpleEsp8266.h>
#include "DHT.h"
#include <TimeLib.h>
#include <WidgetRTC.h>


char auth[] = "";
char ssid[] = "Wi-Fi House 2,4G_EXT";
char pass[] = "";


long momentoprima = 0;
#define intervallo 900000

#define DHTPIN 2
#define DHTTYPE DHT22

DHT dht(DHTPIN, DHTTYPE);
BMx280I2C bmx280(0x76);
BlynkTimer timer;
WidgetRTC rtc;

const byte   interruptPin = 14; // Or other pins that support an interrupt
unsigned int Debounce_Timer, Current_Event_Time, Last_Event_Time, Event_Counter;
float        vento = 0.0; 
float        rugiada;
const double r = 2.733018;

 
//Variabili
float temperatura = 0.0;
float umidita = 0.0;
float pressure = 0.0;

//Prototipi funzioni
void getDati();
void VisualizzaSeriale();
float Pressione();
float Puntor();

void clockDisplay()
{
  // You can call hour(), minute(), ... at any time
  // Please see Time library examples for details

  String currentTime = String(hour()) + ":" + minute() + ":" + second();
  String currentDate = String(day()) + " " + month() + " " + year();
  Serial.print("Current time: ");
  Serial.print(currentTime);
  Serial.print(" ");
  Serial.print(currentDate);
  Serial.println();

  Blynk.virtualWrite(V12, currentTime);
  Blynk.virtualWrite(V13, currentDate); 
  
}

BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}



void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
 pinMode(interruptPin, INPUT_PULLUP);
 dht.begin();
 Wire.begin();
 bmx280.begin();
 bmx280.resetToDefaults();

 delay(5000);
   
  
 bmx280.writeOversamplingPressure(BMx280I2C::OSRS_P_x16);
 bmx280.writeOversamplingTemperature(BMx280I2C::OSRS_T_x16);
 

  noInterrupts();                                // Disable interrupts during set-up
  attachInterrupt(digitalPinToInterrupt(interruptPin), WSpeed_ISR, FALLING); //Respond to a LOW on the interrupt pin and goto WSpeed_ISR
  timer0_isr_init();                             // Initialise Timer-0
  timer0_attachInterrupt(Timer_ISR);             // Goto the Timer_ISR function when an interrupt occurs
  timer0_write(ESP.getCycleCount() + 80000000L); // Pre-load Timer-0 with a time value of 1-second
  interrupts();
  setSyncInterval(10 * 60);   
  timer.setInterval(1000L, clockDisplay);     
}

void loop() {
  Timer_ISR();
  Pressione();
  getDati();
  Puntor();
  delay(3000);
  Blynk.run();
  timer.run();
  VisualizzaSeriale();
delay(1000);
 ESP.deepSleep(9e8);
 delay(1000);
 }



void getDati(){
  temperatura = bmx280.getTemperature();
  umidita = dht.readHumidity();
  pressure = bmx280.getPressure()/100;
    }
void VisualizzaSeriale(){
  Serial.println("I dati esterni sono :");
  Serial.print("Temperatura: ");        Serial.print(temperatura);     Serial.println(" Ã‚Â°C");
  Serial.print("Umidita': ");           Serial.print(umidita);         Serial.println(" %");
  Serial.print("Velocita' vento: ");    Serial.print(vento);           Serial.println(" km/h"); 
  Serial.print("Pressione Atmosferica "); Serial.print(pressure); Serial.println(" hPa");
  Serial.print("Punto di rugiada: "); Serial.print(rugiada); Serial.println(" C");
  Blynk.virtualWrite(V7, temperatura);
  Blynk.virtualWrite(V8, umidita);
  Blynk.virtualWrite(V9, vento);
  Blynk.virtualWrite(V10, pressure);
  Blynk.virtualWrite(V11, rugiada);

}

float Pressione(){
 bmx280.measure();
 do {
 delay(100);
 }  while(!bmx280.hasValue());

 }

float Puntor() {
  double pow(double x, double y), log(double x), Es, E, Dp;
  Es = 6.11 * pow(10, (7.5 * temperatura)/(237.7 + temperatura));
  E = ( umidita * Es ) / 100;
  Dp = (-430.22 + 237.7 * log(E))/(-log(E) + 19.08);
  rugiada = Dp;

}

 ICACHE_RAM_ATTR void WSpeed_ISR (void) {
  if (!(millis() - Debounce_Timer) < 5) {
    Debounce_Timer = millis();                                        // Set debouncer to prevent false triggering
    Event_Counter++;
  }
}

void Timer_ISR (void) {                                                       // Timer reached zero, now re-load it to repeat
  timer0_write(ESP.getCycleCount() + 80000000L);    // Reset the timer, do this first for timing accuracy
    vento = (Event_Counter*r);
  //Serial.println(vento);
  Event_Counter =0;
}
