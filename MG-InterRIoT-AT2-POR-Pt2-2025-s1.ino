/** 
*
* Assessment Title: Portfolio Part 2  
* Cluster:          Intermediate RIoT  
* Qualification:    ICT50220 Diploma of Information Technology (Advanced Programming) 
* Name:             Michiko Goto 
* Student ID:       20126861
* Year/Semester:    2025/S1 
* 
* YOUR SUMMARY OF PORTFOLIO ACTIVITY 
* The implementation of a Wirelessly connected IoT Device and it’s controlling software. 
* 
*/ 

#include <WiFi.h> // to use functions for wifi connect of esp32 
#include <secrets.h>
#include <Adafruit_MQTT.h>
#include <Adafruit_MQTT_Client.h>

#define RETRY_PERIOD 1000 // wait time at first  
#define MAX_ATTEMPTS 10

WiFiClient client;
Adafruit_MQTT_Client mqtt(&client, MQTT_SERVER, MQTT_SERVERPORT, MQTT_USERNAME, MQTT_PASSWORD);

void setup() { 
  Serial.begin(115200); // serial connection and speed 
  if (wiFiConnect()) { 
    Serial.println("WiFi Connected"); 
    wiFiDetails();

    bool mqttConnected = mqttConnected();

    if (mqttConnected) {
      Serial.Println("MQTT Connected")
    } else {
      Serial.Println("MQTT Connection Failed")
    }

  } else {
    Serial.println("Connection Failed");
  }
} 

void loop() {

}

// wifi connection status, return wifi connected or not 
bool wiFiConnect() { 
  int attempts = 0; 

  WiFi.mode(WIFI_STA); // make the board connect to wifi 
  WiFi.begin(ssid, password); // try to connect with ssid and password 
  Serial.println("Connecting to WiFi..."); 

  // wait connection 
  while (WiFi.status() != WL_CONNECTED) { 
    Serial.print("."); 
    delay(RETRY_PERIOD); 
    attempts++;
    if (attempts == 10) {
      Serial.println();  // new line every 10 dots
      attempts = 0;
    }
  }
  return attempts;
} 

bool mqttConnect() {

  uint8_t attempts = 0;
  int8_t mqttConnectionResult;

  Serial.println();
  Serial.println("Connecting to MQTT Broker/Server...");

  while ((mqttConnectionResult = mqtt.connect()) != 0 && attempts < MAX_ATTEMPTS) {

    Serial.print(".");
    
    delay(RETRY_PERIOD);
    attempts++;

    if (mqttConnectionResult != 0) {
      mqtt.disconnect();
    }
  }

  return attempts < MAX_ATTEMPTS;
}


// Display the wifi details 
void wiFiDetails() { 
  Serial.println(); 
  Serial.print("Local IP: "); 
  Serial.println(WiFi.localIP()); 
  Serial.print("Gateway IP: "); 
  Serial.println(WiFi.gatewayIP()); 
  Serial.print("Signal Strength: "); 
  Serial.println(WiFi.RSSI()); 
  Serial.print("Hostname: "); 
  Serial.println(WiFi.getHostname());
} 

 