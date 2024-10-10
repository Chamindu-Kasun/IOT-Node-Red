#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>  // For MQTT communication

// LED pin
int ledPin = 27;

// WiFi credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// MQTT broker credentials
const char* mqttServer = "192.168.8.198"; // Mosquitto public broker
const int mqttPort = 1883;
const char* mqttTopic = "ESP32_MQTT";

//Create an instance of WiFiClient
WiFiClient WOKWI_Client;
//Create an instance of PubSubClient
PubSubClient client(WOKWI_Client);


// Connect to WiFi
void setup_wifi() {
  delay(10);
  Serial.println("Connecting to WiFi...");
  WiFi.mode(WIFI_STA);    // Set ESP32 as WiFi station (client)
  WiFi.begin(ssid, password);  // Connect to the Wi-Fi

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  Serial.println("IP Address: ");
  Serial.println(WiFi.localIP()); // Display the assigned IP
}

// Connect to the MQTT broker
void connectToMQTT() {
  while (!client.connected()) {
    Serial.println("Connecting to MQTT...");
    if (client.connect("ESP32Client")) {   // Attempt to connect to the MQTT broker
      Serial.println("Connected to MQTT broker");
      client.subscribe(mqttTopic);  // Subscribe to the topic
    } else {
      Serial.print("Failed MQTT connection, state=");
      Serial.println(client.state());  // Show the error state
      delay(2000);  // Wait 2 seconds before retrying
    }
  }
}

// MQTT callback function
void callback(char* topic, byte* message, unsigned int length) {
  Serial.print("Message arrived on topic: ");
  Serial.println(topic);
  
  char msg[length + 1];  // Create a buffer to hold the message
  for (int i = 0; i < length; i++) {
    msg[i] = (char)message[i];
  }
  msg[length] = '\0';  // Null-terminate the message string

  // Check the received message and control the LED accordingly
  if (strcmp(msg, "ON") == 0) {
    digitalWrite(ledPin, HIGH);  // Turn LED ON
    Serial.println("LED ON");
  } else if (strcmp(msg, "OFF") == 0) {
    digitalWrite(ledPin, LOW);   // Turn LED OFF
    Serial.println("LED OFF");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(ledPin, OUTPUT);  // Set the LED pin as output
  setup_wifi();  // Connect to Wi-Fi

  // Connect to MQTT broker
  client.setServer(mqttServer, mqttPort);   // Set the MQTT broker
  client.setCallback(callback);             // Set the callback function
  connectToMQTT();                          // Connect to MQTT broker
}

void loop() {
  if (!client.connected()) {
    connectToMQTT();  // Reconnect to MQTT if disconnected
  }
  client.loop();  // Maintain the MQTT connection and handle messages
}
