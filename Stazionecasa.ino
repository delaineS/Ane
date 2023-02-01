/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com  
*********/
#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID   ""
#include <BlynkSimpleEsp8266.h>
#include <TimeLib.h>
#include <WidgetRTC.h>


char auth[] = "";
char ssid[] = "";
char pass[] = "";

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);
BlynkTimer timer;
WidgetRTC rtc;

#define DHTPIN 14     // Digital pin connected to the DHT sensor

// Uncomment the type of sensor in use:
//#define DHTTYPE    DHT11     // DHT 11
#define DHTTYPE    DHT22     // DHT 22 (AM2302)
//#define DHTTYPE    DHT21     // DHT 21 (AM2301)

DHT dht(DHTPIN, DHTTYPE);

void getDati();
void VisualizzaSeriale();
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

  Blynk.virtualWrite(V14, currentTime);
  Blynk.virtualWrite(V13, currentDate); 
  
}
BLYNK_CONNECTED() {
  // Synchronize time on connection
  rtc.begin();
}

void setup() {
  Serial.begin(115200);
  Blynk.begin(auth, ssid, pass);
  dht.begin();

  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;);
  }
  delay(2000);
  display.clearDisplay();
  display.setTextColor(WHITE);
}

void loop() {
  delay(5000);

  //read temperature and humidity
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
  }
  Blynk.run();
  timer.run();
  Blynk.virtualWrite(V4, t);
  Blynk.virtualWrite(V8, h);
  // clear display
  display.clearDisplay();
  
  // display temperature
  display.setTextSize(2);
  display.setCursor(0,0);
  display.print(t);
  display.print(" ");
  display.setTextSize(1);
  display.cp437(true);
  display.write(167);
  display.setTextSize(1);
  display.print("C");
  
  // display humidity
  display.setTextSize(2);
  display.setCursor(0, 15);
  display.print(h);
  display.print(" %"); 
  
  display.display(); 

  
  delay(1000);
 ESP.deepSleep(9e8);
 delay(1000);
}
