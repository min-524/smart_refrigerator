//#ifdef ESP32
#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>

const char* ssid     = "HACKATHON 5";
const char* password = "HACK-567";

const char* serverName = "http://leejiseok.iptime.org/input.php";

// Keep this API Key value to be compatible with the PHP code provided in the project page. 
// If you change the apiKeyValue value, the PHP file /post-esp-data.php also needs to have the same key 


float getVPP();
const int sensorIn = 34;
int mVperAmp = 66; // use 185 for 5A Module and 100 for 20A Module

double Voltage = 0;
double VRMS = 0;
double AmpsRMS = 0;

int pin1=27;
int pin2=26;
int pin3=14;
int repin=4;

//Adafruit_BME280 bme;  // I2C
//Adafruit_BME280 bme(BME_CS);  // hardware SPI
//Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK);  // software SPI

void setup() {
  pinMode(pin1, OUTPUT);
  pinMode(pin2, OUTPUT);
  pinMode(pin3, OUTPUT);
  Serial.begin(115200);
  
  WiFi.begin(ssid, password);
  Serial.println("Connecting");
  while(WiFi.status() != WL_CONNECTED) { 
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  // (you can also pass in a Wire library object like &Wire2)
//  bool status = bme.begin(0x76);
 // if (!status) {
   // Serial.println("Could not find a valid BME280 sensor, check wiring or change I2C address!");
   // while (1);
  //}
}

void loop() {
  Voltage = getVPP();
  VRMS = (Voltage/2.0) *0.707;//전압측정
  AmpsRMS = (VRMS * 1000)/mVperAmp;//전류측정((AmpsRMS*220.0)/100.0)
  int a= digitalRead(repin);
  Serial.println(String(a));
  Serial.println(String(AmpsRMS));
  if(a==0){
    digitalWrite(pin1,LOW);
    digitalWrite(pin2,HIGH);
  }
  else{
    digitalWrite(pin1,LOW);
    digitalWrite(pin2,LOW);
  }
  //Check WiFi connection status
  if(AmpsRMS<4){
    AmpsRMS=0;
  }
  if(WiFi.status()== WL_CONNECTED){
    WiFiClient client;
    HTTPClient http;
    
    // Your Domain name with URL path or IP address with path
    http.begin(client, serverName);
    
    // Specify content-type header
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Prepare your HTTP POST request data
    String httpRequestData = "open=" + String(a) + "&power=" + String(AmpsRMS)+"";
    Serial.print("httpRequestData: ");
    Serial.println(httpRequestData);
    
    int httpResponseCode = http.POST(httpRequestData);
     
        
    if (httpResponseCode>0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    }
    else {
      Serial.print("Error code: ");
      Serial.println(httpResponseCode);
    }
    // Free resources
    http.end();
  }
  else {
    Serial.println("WiFi Disconnected");
  }
  //Send an HTTP POST request every 30 seconds
  delay(3000);  
}
float getVPP()//전력 측정 함수
{
  float result;
  
  int readValue;             //value read from the sensor
  int maxValue = 0;          // store max value here
  int minValue = 4096;          // store min value here
  
   uint32_t start_time = millis();
   while((millis()-start_time) < 1000) // 1초동안 값을 모아서 AC 전류의 최고점, 최저점을 찾아 평균치를 구함
   {
       readValue = analogRead(sensorIn);
       // see if you have a new maxValue
       if (readValue > maxValue) 
       {
           /*record the maximum sensor value*/
           maxValue = readValue;
       }
       if (readValue < minValue) 
       {
           /*record the maximum sensor value*/
           minValue = readValue;
       }
   }
   
   // Subtract min from max
   result = ((maxValue - minValue) * 5.0)/1024.0;        // 5V 분해능, 아날로그핀의 분해능 1024
      
   return result;
 }
