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
#define MAX_ATTEMPTS 5

#define uS_TO_S_FACTOR 1000000 // microseconds per second
#define TIME_TO_SLEEP 1800        // seconds, 30min

#define SENSOR 25

#define LED_RED_1 4
#define LED_GREEN_1 18
#define LED_BLUE_1 19

WiFiClient wifiClient;
Adafruit_MQTT_Client mqtt(&wifiClient, IO_SERVER, IO_SERVERPORT, IO_USERNAME, IO_KEY);
Adafruit_MQTT_Publish presence = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/inter-riot-devices");
Adafruit_MQTT_Publish sensor = Adafruit_MQTT_Publish(&mqtt, IO_USERNAME "/feeds/inter-riot-at2-data");


void setup() { 
  pinMode(SENSOR, INPUT);
  int sensor_value = analogRead(SENSOR);
    
  Serial.begin(115200);

  pinMode (LED_RED_1, OUTPUT); 
  pinMode (LED_GREEN_1, OUTPUT); 
  pinMode (LED_BLUE_1, OUTPUT); 
 
  ledOn(LED_RED_1); 
  ledOff(LED_GREEN_1); 
  ledOff(LED_BLUE_1);

  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);  // wake the device once every 30 minutes
  Serial.println();
  Serial.println("Set up ESP32 to sleep every " + String(TIME_TO_SLEEP) + " seconds");
  Serial.println();

  if (wiFiConnect()) { 
    Serial.println("WiFi Connected"); 
    wiFiDetails();

    ledOff(LED_RED_1);
    ledOff(LED_BLUE_1);
    ledFlash(2);

    bool mqttConnected = mqttConnect();
        
    if (mqttConnected) {
      Serial.println("MQTT Connected");
      ledFlash(3);

      presence.publish("Online");
      Serial.println("Sent message");

      flashBlue();

      Serial.print("Sensor value: ");
      Serial.println(String(sensor_value));
      sensor.publish(String(sensor_value).c_str());

      flashBlue();

      delay(5000);

      ledOff(LED_RED_1); 
      ledOff(LED_GREEN_1); 
      ledOff(LED_BLUE_1);

      pinMode (LED_RED_1, INPUT); 
      pinMode (LED_GREEN_1, INPUT); 
      pinMode (LED_BLUE_1, INPUT); 
 
      esp_deep_sleep_start();
    } else {
      delay(1000);
      // sleep if no connection to either MQTT 
      Serial.println("Going to sleep");
      delay(1000);
      Serial.flush();

      ledOff(LED_RED_1); 
      ledOff(LED_GREEN_1); 
      ledOff(LED_BLUE_1);
      
      pinMode (LED_RED_1, INPUT); 
      pinMode (LED_GREEN_1, INPUT); 
      pinMode (LED_BLUE_1, INPUT);

      esp_deep_sleep_start();
      Serial.println("MQTT Connection Failed");  // never excuted
    }

  } else {
    // sleep if no connection to WiFi
    Serial.println("Going to sleep");
    delay(1000);
    Serial.flush();

    ledOff(LED_RED_1); 
    ledOff(LED_GREEN_1); 
    ledOff(LED_BLUE_1);
    
    pinMode (LED_RED_1, INPUT); 
    pinMode (LED_GREEN_1, INPUT); 
    pinMode (LED_BLUE_1, INPUT);

    esp_deep_sleep_start();
    Serial.println("WiFi Connection Failed");  // never excuted
  }
} 

void loop() {

}

// wifi connection status, return wifi connected or not 
bool wiFiConnect() { 
  int attempts = 0; 
  int8_t wifiConnectionResult;

  WiFi.mode(WIFI_STA); // make the board connect to wifi 
  WiFi.begin(ssid, password); // try to connect with ssid and password 
  Serial.println("Connecting to WiFi..."); 

  // wait connection 
  while (WiFi.status() != WL_CONNECTED && attempts < MAX_ATTEMPTS) { 
    Serial.print("."); 
    delay(RETRY_PERIOD); 
    attempts++;
    if (attempts == MAX_ATTEMPTS) {
      wiFiErrors(wifiConnectionResult);
      delay(1000);
      attempts++;
    }
  }
  return attempts < MAX_ATTEMPTS;
} 

// MQTT connection status, return MQTT connected or not
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
      if (attempts == MAX_ATTEMPTS){
        mqttErrors(mqttConnectionResult);
      }
    }
  }

  return attempts < MAX_ATTEMPTS;
}

void ledOff(int ledPin){
  digitalWrite(ledPin, LOW);
}

void ledOn(int ledPin){
  digitalWrite(ledPin, HIGH);
}

void ledFlash(int flash){
  int attempts = 0;
  while(attempts < flash){
    ledOff(LED_RED_1);
    ledOff(LED_BLUE_1);
    ledOn(LED_GREEN_1);
    delay(100);
    ledOff(LED_GREEN_1);
    delay(100);
    attempts++;
  }
}

void flashBlue(){
  ledOn(LED_BLUE_1);
  delay(100);
  ledOff(LED_BLUE_1);
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

void wiFiErrors(uint8_t errorNumber) {
  // https://randomnerdtutorials.com/esp32-useful-wi-fi-functions-arduino/#4
  switch (errorNumber) {
    case WL_IDLE_STATUS:
      {
        Serial.println("Temporary status on Wi-Fi Begin");
        break;
      }
    case WL_NO_SSID_AVAIL:
      {
        Serial.println("No SSID are available");
        break;
      }
    case WL_SCAN_COMPLETED:
      {
        Serial.println("Scan of networks is complete");
        break;
      }
    case WL_CONNECTED:
      {
        Serial.println("Connected to Wi-Fi");
        break;
      }
    case WL_CONNECT_FAILED:
      {
        Serial.println("Wi-Fi Connection failed");
        break;
      }
    case WL_CONNECTION_LOST:
      {
        Serial.println("Wi-Fi Connection lost");
        break;
      }
    case WL_DISCONNECTED:
      {
        Serial.println("Wi-Fi Disconnected");
        break;
      }
    default:
      {
        Serial.println("Unknown Wi-Fi Error.");
        break;
      }
  }
}

void mqttErrors(uint8_t errorNumber) {
  // http://www.steves-internet-guide.com/client-connections-python-mqtt/
  switch (errorNumber) {
    case 0:
      {
        Serial.println("MQTT Connection successful");
        break;
      }
    case 1:
      {
        Serial.println("MQTT Connection refused – incorrect protocol version");
        break;
      }
    case 2:
      {
        Serial.println("MQTT Connection refused – invalid client identifier");
        break;
      }
    case 3:
      {
        Serial.println("MQTT Connection refused – server unavailable");
        break;
      }
    case 4:
      {
        Serial.println("MQTT Connection refused – bad username or password");
        break;
      }
    case 5:
      {
        Serial.println("MQTT Connection refused – not authorised");
        break;
      }
    case 6 - 255:
      {
        Serial.println("MQTT Currently unused.");
        break;
      }
    default:
      {
        Serial.println("MQTT Unknown error.");
        break;
      }
  }
}

 
