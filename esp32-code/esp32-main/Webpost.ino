#include <WiFi.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include "esp_adc_cal.h"
#include "time.h"

// Define sensor pins
#define LM35_Sensor1 34
#define LDR_Sensor2 35
#define LM35_Enable 26  // Digital pin to control LM35 power
#define LDR_Enable 27   // Digital pin to control LDR power

#define Buzzer 27
#define SOUND_SPEED 0.034
#define CM_TO_INCH 0.393701

// LED variables
int led1Pin = 17; // Red
int led2Pin = 16; // Green

// LM35 Sensor variables
int LM35_Raw = 0;
float LM35_TempC = 0.0;
float Voltage = 0.0;

// LDR Sensor variables
int LDR_Raw = 0;

// Ultra variables
long duration;
float distanceCm;
const int trigPin = 14;
const int echoPin = 26;

// Date & Time variables
String Date;
String Time;

// Wifi credentials
const char* ssid = "Gelembo";
const char* password = "t4sjmpbrg";

// Server URL
String server = "http://192.168.208.67:8000/post";

// Timer variables
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

// NTP server configuration
const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 7 * 3600; // Offset for WIB (UTC+7)
const int daylightOffset_sec = 0; // No daylight saving time for WIB

void setup() {
    Serial.begin(115200);
    pinMode(LM35_Sensor1, INPUT);
    pinMode(LDR_Sensor2, INPUT);
    pinMode(LM35_Enable, OUTPUT);
    pinMode(LDR_Enable, OUTPUT);
    pinMode(led1Pin, OUTPUT); // Set pin untuk LED1 sebagai output
    pinMode(led2Pin, OUTPUT); // Set pin untuk LED2 sebagai output
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    pinMode(Buzzer, OUTPUT); 

    // Initialize sensors power control pins
    digitalWrite(LM35_Enable, LOW);
    digitalWrite(LDR_Enable, LOW);

    // Connect to Wifi
    WiFi.begin(ssid, password);
    Serial.println("Connecting");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());
    
    // Initialize NTP
    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    Serial.println("Timer set to 5 seconds (timerDelay variable), it will take 5 seconds before publishing the first reading.");
}

void loop() {
    digitalWrite(Buzzer, LOW);   // Buzzer OFF
    
    // Read LM35 Sensor
    digitalWrite(LM35_Enable, HIGH); // Enable LM35
    delay(100); // Wait for sensor to stabilize
    LM35_Raw = analogRead(LM35_Sensor1);
    Voltage = readADC_Cal(LM35_Raw);
    LM35_TempC = Voltage / 10;
    digitalWrite(LM35_Enable, LOW); // Disable LM35

    // Read LDR Sensor
    digitalWrite(LDR_Enable, HIGH); // Enable LDR
    delay(100); // Wait for sensor to stabilize
    LDR_Raw = analogRead(LDR_Sensor2);
    digitalWrite(LDR_Enable, LOW); // Disable LDR

    // Ultrasonic Sensor
    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    
    duration = pulseIn(echoPin, HIGH);
    distanceCm = duration * SOUND_SPEED / 2;

    //  Get Date & Time value
    Time = printCurrentTime();
    Date = printCurrentDate();

    if (distanceCm < 15) {
      digitalWrite(led1Pin, HIGH); // Red LED ON
      digitalWrite(led2Pin, LOW);  // Green LED OFF
      digitalWrite(Buzzer, HIGH);  // Buzzer ON
    } else {
      digitalWrite(led1Pin, LOW);  // Red LED OFF
      digitalWrite(led2Pin, HIGH); // Green LED ON
      digitalWrite(Buzzer, LOW);   // Buzzer OFF
    }

    if ((millis() - lastTime) > timerDelay) {
        if (WiFi.status() == WL_CONNECTED) {
            WiFiClient client;
            HTTPClient http;

            http.begin(client, server);

            http.addHeader("Content-Type", "application/json");
            char buf[256];
            snprintf(buf, sizeof(buf), "{\"api_key\":\"%s\",\"sensor\":\"%s\",\"temperature\":\"%.2f\",\"light\":\"%d\",\"date\":\"%s\",\"time\":\"%s\",\"distance\":\"%.2f\"}",
                "supersecure",
                "LM35",
                LM35_TempC,
                LDR_Raw,
                Date.c_str(),
                Time.c_str(),
                distanceCm);

            Serial.println(buf);
            int httpResponseCode = http.POST(buf);

            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);

            http.end();
        } else {
            Serial.println("WiFi Disconnected");
        }
        lastTime = millis();
    }
}

String printCurrentDate() {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char dateString[20];

    // Format date as "YYYY-MM-DD"
    strftime(dateString, sizeof(dateString), "%Y-%m-%d", t);
    String date = String(dateString);
    return date;
}

String printCurrentTime() {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char timeString[20];

    // Format time as "HH:MM:SS"
    strftime(timeString, sizeof(timeString), "%H:%M:%S", t);
    String time = String(timeString);
    return time;
}

uint32_t readADC_Cal(int ADC_Raw) {
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    return (esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}
