#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <HTTPClient.h>
#include <Arduino.h>
#include "esp_adc_cal.h"
#include "time.h"
#include <MFRC522.h>
#include <SPI.h>
#define SS_PIN 5
#define RST_PIN 27
#define buzzerPin 27

const long gmtOffset_sec = 7 * 3600; 
const int daylightOffset_sec = 0; 
const int led1Pin = 17; 
const int led2Pin = 16; 
const int LM35_Sensor1 = 34;
const int LDR_Sensor2 = 35;
const int LM35_Enable = 25;
const int LDR_Enable = 26;
const int trigPin = 14;
const int echoPin = 26;
const float SOUND_SPEED = 0.034;
const float CM_TO_INCH = 0.393701;
const float D3 = 5250;
const float C3 = 4950;
const float B2 = 4562; 
const float A2 = 4058; 
const float G2 = 3654; 
const float F2 = 3255; 
const float E2 = 3048; 
const float D2 = 2741; 
const float C2 = 2441; 
const float B = 1969;
const float N = 0;  
const char *ssid = "Gelembo";
const char *password = "t4sjmpbrg";
const char* ntpServer = "pool.ntp.org";


// Melody of "Susu Murni"
int melodyA[] = {G2,E2,C2,C2,C2,N,C2,C2,B,C2,D2,N,A2,F2,D2,D2,D2,N,D2,D2,C2,D2,E2,N,C2,E2,G2,G2,G2,N,A2,G2,A2,B2,C2,N,C2,C2,A2,G2,N,F2,E2,D2,C2};
int noteDurationsA[] = {4,4,2,2,2,4,4,4,3.5,4,4,4,4,4,2,2,2,4,4,4,4,4,3.5,4,3,4,2.5,2.5,2,3,4,4,4,4,3,2,3,3,4,3,2,3,2,2,1.5};

// Melody of "Mary Had a Little Lamb"
int melodyB[] = {B2, A2, G2, A2, B2, B2, B2, A2, A2, A2, B2, B2, B2, B2, A2, G2, A2, B2, B2, B2, A2, A2, B2, A2, G2};
int noteDurationsB[] = {3, 4, 4, 4, 4, 4, 2, 4, 4, 2, 4, 4, 2, 4, 4, 4, 4, 4, 4, 2, 4, 4, 4, 4, 2};

float distanceCm;
float LM35_TempC = 0.0;
int LDR_Raw = 0;
long duration;
unsigned long lastTime = 0;
unsigned long timerDelay = 5000;

String Date;
String Time;
String getCardName(byte *uid);
String getNuidHex(byte *uid);

WebServer server(80);
MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;

byte authorizedCards[][4] = {
  {0x53, 0xA6, 0x37, 0xA7},
  {83, 166, 55, 167}
};

const int authorizedCardsCount = sizeof(authorizedCards) / sizeof(authorizedCards[0]);

bool cardStatus = false;
byte lastCard[4] = {0};


void playMelody(int melody[], int noteDurations[], int size) {
  for (int thisNote = 0; thisNote < size; thisNote++) {
    int noteDuration = 1000 / noteDurations[thisNote];
    tone(buzzerPin, melody[thisNote], noteDuration);
    delay(noteDuration * 1.1);  
  }
  noTone(buzzerPin);  
}

void playMelodyA() {
  playMelody(melodyA, noteDurationsA, sizeof(melodyA) / sizeof(int));
}

void playMelodyB() {
  playMelody(melodyB, noteDurationsB, sizeof(melodyB) / sizeof(int));
}

bool isAuthorized(byte *uid) {
    for (int i = 0; i < authorizedCardsCount; i++) {
        if (memcmp(uid, authorizedCards[i], 4) == 0) {
            return true;
        }
    }
    return false;
}

void rfidhandler() {
    if (!rfid.PICC_IsNewCardPresent()) {
      return;
    }

    if (!rfid.PICC_ReadCardSerial()) {
      return;
    }

    Serial.print(F("Card UID:"));
    printHex(rfid.uid.uidByte, rfid.uid.size);
    Serial.println();
    digitalWrite(buzzerPin, LOW); 
    digitalWrite(buzzerPin, HIGH); 
    delay(1000); 
    digitalWrite(buzzerPin, LOW); 
    if (isAuthorized(rfid.uid.uidByte)) {
        if (memcmp(lastCard, rfid.uid.uidByte, 4) == 0) {
            cardStatus = !cardStatus;
            
        } else {
            cardStatus = true;
            memcpy(lastCard, rfid.uid.uidByte, 4);
        }

        String nuidHex = getNuidHex(rfid.uid.uidByte);
        String name = getCardName(rfid.uid.uidByte);

        if (cardStatus) {
            if (WiFi.status() == WL_CONNECTED) {
                WiFiClient client;
                HTTPClient http;

                http.begin(client, "http://192.168.208.67:8000/rfid");
                http.addHeader("Content-Type", "application/json");

                String jsonPayload = "{\"api_key\":\"supersecure\",\"nuid\":\"" + nuidHex + "\",\"name\":\"" + name + "\"}";

                int httpResponseCode = http.POST(jsonPayload);

                Serial.print("HTTP Response code: ");
                Serial.println(httpResponseCode);

                http.end();
            } else {
                Serial.println("WiFi Disconnected");
            }

            Serial.println(F("Card logged in."));
            digitalWrite(buzzerPin, LOW);
            delay(200);
            digitalWrite(buzzerPin, HIGH);
        } else {
            Serial.println(F("Card logged out."));
            digitalWrite(buzzerPin, LOW);
            delay(200);
            digitalWrite(buzzerPin, HIGH);
        }
    } else {
        Serial.println(F("Unauthorized card."));
        digitalWrite(buzzerPin, LOW);
        delay(200);
        digitalWrite(buzzerPin, HIGH);
    }

    rfid.PICC_HaltA();
    rfid.PCD_StopCrypto1();
}

void printHex(byte *buffer, byte bufferSize) {
    for (byte i = 0; i < bufferSize; i++) {
        Serial.print(buffer[i] < 0x10 ? " 0" : " ");
        Serial.print(buffer[i], HEX);
    }
}

void handleRoot() {
    digitalWrite(led1Pin, HIGH);
    server.send(200, "text/plain", "hello from esp32!");
    digitalWrite(led1Pin, LOW);
}

void handleMelodyA() {
  playMelodyA();
  server.send(200, "text/plain", "Playing Susu Murni's Theme song");
}

void handleMelodyB() {
  playMelodyB();
  server.send(200, "text/plain", "Playing Mary had a little Lamb song");
}

void handleLedOn() {
    digitalWrite(led1Pin, HIGH);
    server.send(200, "text/plain", "LED turned on");
}

void handleLedOff() {
    digitalWrite(led1Pin, LOW);
    server.send(200, "text/plain", "LED turned off");
}

void handlePostRequest() {
    String message = "";
    if (server.hasArg("plain")) {
        message = server.arg("plain");
    } else {
        message = "No message received";
    }
    server.send(200, "text/plain", "POST message received: " + message);
}

void handleNotFound() {
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += server.uri();
    message += "\nMethod: ";
    message += (server.method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += server.args();
    message += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", message);
}

String printCurrentDate() {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char dateString[20];
    strftime(dateString, sizeof(dateString), "%Y-%m-%d", t);
    return String(dateString);
}

String printCurrentTime() {
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%H:%M:%S", t);
    return String(timeString);
}

String getNuidHex(byte *uid) {
    String nuid = "";
    for (byte i = 0; i < 4; i++) {
        if (uid[i] < 0x10) {
            nuid += "0";
        }
        nuid += String(uid[i], HEX);
    }
    nuid.toUpperCase(); 
    return nuid;
}

String getCardName(byte *uid) {
    if (memcmp(uid, authorizedCards[0], 4) == 0) {
        return "Card1";
    } else if (memcmp(uid, authorizedCards[1], 4) == 0) {
        return "Card2";
    } else {
        return "Unknown";
    }
}

void setup() {
    Serial.begin(115200);
    pinMode(LM35_Sensor1, INPUT);
    pinMode(LDR_Sensor2, INPUT);
    pinMode(LM35_Enable, OUTPUT);
    pinMode(LDR_Enable, OUTPUT);
    pinMode(led1Pin, OUTPUT);
    pinMode(led2Pin, OUTPUT);
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
    pinMode(buzzerPin, OUTPUT);

    digitalWrite(LM35_Enable, LOW);
    digitalWrite(LDR_Enable, LOW);

    WiFi.begin(ssid, password);
    Serial.println("Connecting to WiFi...");
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.print("Connected to WiFi network with IP Address: ");
    Serial.println(WiFi.localIP());

    configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);

    if (MDNS.begin("esp32")) {
        Serial.println("MDNS responder started");
    }

    server.on("/", HTTP_GET, handleRoot);
    server.on("/ledon", HTTP_GET, handleLedOn);
    server.on("/ledoff", HTTP_GET, handleLedOff);
    server.on("/melodyA", HTTP_GET, handleMelodyA);
    server.on("/melodyB", HTTP_GET, handleMelodyB);
    server.on("/post", HTTP_ANY, handlePostRequest);
    server.onNotFound(handleNotFound);

    server.begin();
    Serial.println("HTTP server started");

    SPI.begin();
    rfid.PCD_Init();
}

void loop() {
    digitalWrite(buzzerPin, LOW);

    digitalWrite(LM35_Enable, HIGH);
    delay(100);
    int LM35_Raw = analogRead(LM35_Sensor1);
    float Voltage = readADC_Cal(LM35_Raw);
    LM35_TempC = Voltage / 10;
    digitalWrite(LM35_Enable, LOW);

    digitalWrite(LDR_Enable, HIGH);
    delay(100);
    LDR_Raw = analogRead(LDR_Sensor2);
    digitalWrite(LDR_Enable, LOW);

    digitalWrite(trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);
    duration = pulseIn(echoPin, HIGH);
    distanceCm = duration * SOUND_SPEED / 2;

    if (distanceCm <5){
      digitalWrite(buzzerPin, HIGH);
      delay(100);
      digitalWrite(buzzerPin, LOW);
      delay(100);
    } else if(distanceCm >= 5 && distanceCm <=15){
      digitalWrite(buzzerPin, HIGH);
      delay(500);
      digitalWrite(buzzerPin, LOW);
      delay(500);
    } else if (distanceCm >15 && distanceCm <=30){
      digitalWrite(buzzerPin, HIGH);
      delay(1000);
      digitalWrite(buzzerPin, LOW);
      delay(1000);
    }

    Time = printCurrentTime();
    Date = printCurrentDate();

    if ((millis() - lastTime) > timerDelay) {
        if (WiFi.status() == WL_CONNECTED) {
            WiFiClient client;
            HTTPClient http;

            http.begin(client, "http://192.168.208.67:8000/post");
            http.addHeader("Content-Type", "application/json");

            String jsonPayload = "{\"api_key\":\"supersecure\",\"sensor\":\"LM35\",\"temperature\":\"" + String(LM35_TempC, 2) + "\",\"light\":\"" + String(LDR_Raw) + "\",\"date\":\"" + Date + "\",\"time\":\"" + Time + "\",\"distance\":\"" + String(distanceCm, 2) + "\"}";

            int httpResponseCode = http.POST(jsonPayload);

            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);

            http.end();
        } else {
            Serial.println("WiFi Disconnected");
        }
        lastTime = millis();
    }
    
    server.handleClient();
    rfidhandler();
    delay(10);
}

uint32_t readADC_Cal(int ADC_Raw) {
    esp_adc_cal_characteristics_t adc_chars;
    esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
    return (esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}