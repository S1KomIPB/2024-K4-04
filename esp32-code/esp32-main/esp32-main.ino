#include <WiFi.h>
#include <HTTPClient.h>
#include "esp_adc_cal.h" 

#define LM35_Sensor1 34 
int LM35_Raw_Sensor1 = 0; 
float LM35_TempC_Sensor1 = 0.0; 
float LM35_TempF_Sensor1 = 0.0; 
float Voltage = 0.0; 

const char* ssid = "hotspot";
const char* password = "password";


//Your Domain name with URL path or IP address with path
String server = "http://192.168.5.143:8000/temperature/";

// time in miliseconds
unsigned long lastTime = 0;

// delay in 5000 ms (5 seconds)
unsigned long timerDelay = 5000;


// connect with Wi-Fi
void setup() {
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
 
  Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

// main code
void loop() {

  // read raw data from LM35
  LM35_Raw_Sensor1 = analogRead(LM35_Sensor1);   
  Voltage = readADC_Cal(LM35_Raw_Sensor1);

  // convert data to proper temperature value (C --> Celcius; F --> Fahrenheit)
  LM35_TempC_Sensor1 = Voltage / 10;
  LM35_TempF_Sensor1 = (LM35_TempC_Sensor1 * 1.8) + 32;

  // send an HTTP POST request every 5 seconds
  String serverName = server + String(LM35_TempC_Sensor1);
  if ((millis() - lastTime) > timerDelay) {

    // check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
     
      WiFiClient client;
      HTTPClient http;
    
      // Your Domain name with URL path or IP address with path
      http.begin(client, serverName);
      
      // Specify content-type header & send data
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      String httpRequestData = "";
      int httpResponseCode = http.POST(httpRequestData);
      // http.addHeader("Content-Type", "application/json");
      // int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");
      // http.addHeader("Content-Type", "text/plain");
      // int httpResponseCode = http.POST("Hello, World!");
      
      Serial.print("HTTP Response code: "); Serial.println(httpResponseCode);
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
}

// kalibrasi nilai voltage dari format raw (nantinya voltage diubah ke nilai suhu dll)
uint32_t readADC_Cal(int ADC_Raw) 
{ 
  esp_adc_cal_characteristics_t adc_chars; 
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars); 
  return(esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars)); 
}