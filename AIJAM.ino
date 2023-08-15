
#include "ThingSpeak.h"   // thư viện sử dụng thingspeak cloud
#include <WiFi.h>         // kết nối internet cho mạch esp32
#include <LiquidCrystal_I2C.h> // LCD
#include "DHT.h"
#include "MQ135.h"
#define PIN_MQ135 34

LiquidCrystal_I2C lcd(0x27, 16, 2);
MQ135 mq135_sensor = MQ135(PIN_MQ135);


#define SECRET_SSID "" //change ssid and pass base on the location
#define SECRET_PASS ""
#define SECRET_CH_ID 2209559
#define SECRET_WRITE_APIKEY "J3EWORUBRSJN1UCT"


char ssid[] = SECRET_SSID;   // your network SSID (name) 
char pass[] = SECRET_PASS;   // your network password
int keyIndex = 0;            // your network key Index number (needed only for WEP)
WiFiClient  client;

unsigned long myChannelNumber = SECRET_CH_ID;
const char * myWriteAPIKey = SECRET_WRITE_APIKEY;

const int buttonPin2 = 33;
const int buttonPin1 = 25;
const int ledPin = 32;
const int dhtPin = 18;
DHT dht(dhtPin, DHT11);
int inputPin = 5;       
int pirState = LOW;     
int val = 0;
const int DHTTYPE = DHT11;


void setup()
{
  pinMode(inputPin, INPUT);
  Serial.begin(9600);
  WiFi.mode(WIFI_STA);   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  pinMode(buttonPin1, INPUT);
  pinMode(buttonPin2, INPUT);
  pinMode(inputPin, INPUT);
  pinMode(ledPin, OUTPUT);
  lcd.init();  
  lcd.backlight();
  dht.begin();  
}
 
void loop()
{
   if(WiFi.status() != WL_CONNECTED)
   {
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(SECRET_SSID);
    while(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass); 
      Serial.print(".");
      delay(5000);     
    } 
    Serial.println("\nConnected.");
  }   
  ThingSpeak.begin(client);  // Initialize ThingSpeak
  int buttonStatus1 = digitalRead(buttonPin1);
  int buttonStatus2 = digitalRead(buttonPin2);
  float t = dht.readTemperature();
  float h = dht.readHumidity();
  float gas_ppm_f = mq135_sensor.getCorrectedPPM(t, h);
  val = digitalRead(inputPin);    // đọc giá trị đầu vào.
   Serial.println(val);
   
    digitalWrite(ledPin, HIGH);
    lcd.setCursor(0,0);
    lcd.print("Tem:");
    lcd.setCursor(4,0);
    lcd.print(t,0);
    lcd.print("*C");
    lcd.setCursor(9,0);
    lcd.print("Hum:");
    lcd.setCursor(13,0);
    lcd.print(h,0);
    lcd.print("%");
    Serial.println(t);
    Serial.println(h);
    lcd.setCursor(0,1);
    lcd.print("Gas:            ");
    lcd.setCursor(5,1);
    lcd.print(gas_ppm_f,0);
    lcd.setCursor(12,1);
    lcd.print("ppm");
    Serial.println(gas_ppm_f);
    int gas_alert = 0;
    if(isnan(gas_ppm_f)){
      gas_alert = 1;
    }
  
  Serial.println(val);
//  int x1 = ThingSpeak.writeField(myChannelNumber, 1, t, myWriteAPIKey);
//  int x2 = ThingSpeak.writeField(myChannelNumber, 2, h, myWriteAPIKey);
//  int x3 = ThingSpeak.writeField(myChannelNumber, 3, gas_ppm_f, myWriteAPIKey);
//  int x4 = ThingSpeak.writeField(myChannelNumber, 4, val, myWriteAPIKey);
  
  ThingSpeak.setField(1, t);
  ThingSpeak.setField(2, h);
  ThingSpeak.setField(3, gas_ppm_f);
  ThingSpeak.setField(4, val);
  ThingSpeak.setField(6, gas_alert);
  
  
  // write to the ThingSpeak channel
  int x = ThingSpeak.writeFields(myChannelNumber, myWriteAPIKey);
  if(x == 200){
    Serial.println("Channel update successful.");
  }
  else{
    Serial.println("Problem updating channel. HTTP error code " + String(x));
  }
  }
 
