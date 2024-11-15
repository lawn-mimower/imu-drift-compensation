#include <ESP8266WiFi.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <MPU6050.h>

// Wi-Fi credentials (using character arrays instead of const char*)
char mihir_wifiSsid[] = "YOUR_WIFI_SSID";            // Your Wi-Fi SSID
char mihir_wifiPassword[] = "YOUR_WIFI_PASSWORD";  // Your Wi-Fi password
char mihir_serverHost[] = "192.168.19";      // IP address of your server (MacBook)
uint16_t mihir_serverPort = 23457;           // Port to connect to on the server

// I2C address of the MPU-6050
#define mihir_MPU_ADDR 0x68  // MPU-6050 I2C address

// Create an instance of the MPU6050 sensor
MPU6050 mihir_mpu;

// Declare WiFiClient object globally (only once)
WiFiClient mihir_client;

float mihir_angularDisplacement = 0;  // Initialize angular displacement (in degrees)
unsigned long mihir_previousTime = 0;
unsigned long mihir_currentTime;
float mihir_deltaTime;  // Time difference between readings in seconds

void setup() {
  Serial.begin(115200);   // Start Serial Monitor for debugging
  delay(10);

  // Connect to Wi-Fi
  Serial.println("Connecting to Wi-Fi...");
  WiFi.begin(mihir_wifiSsid, mihir_wifiPassword);
  unsigned long mihir_startMillis = millis();
  unsigned long mihir_timeout = 10000;  // 10 seconds timeout
  while (WiFi.status() != WL_CONNECTED && millis() - mihir_startMillis < mihir_timeout) {
    delay(500);
    Serial.print(".");
  }
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect to Wi-Fi within timeout");
    return;  // Exit if Wi-Fi connection fails
  }
  Serial.println("\nWi-Fi connected");

  // Connect to the server
  if (!mihir_client.connect(mihir_serverHost, mihir_serverPort)) {
    Serial.println("Connection to server failed");
    while (1);
  }
  Serial.println("Connected to server");

  // Initialize I2C and MPU-6050 sensor
  Wire.begin(D2, D1);  // Custom I2C pins (D2 = GPIO4, D1 = GPIO5)
  mihir_mpu.initialize();
  if (!mihir_mpu.testConnection()) {
    Serial.println("Failed to initialize MPU6050 sensor!");
    while (1);
  }
}

void loop() {
  // Calculate deltaTime for accurate angular displacement calculation
  mihir_currentTime = millis();
  mihir_deltaTime = (mihir_currentTime - mihir_previousTime) / 1000.0;  // Convert to seconds
  mihir_previousTime = mihir_currentTime;

  // Read gyroscope data (Z-axis)
  int16_t mihir_gx, mihir_gy, mihir_gz;
  mihir_mpu.getRotation(&mihir_gx, &mihir_gy, &mihir_gz);  // Get raw gyroscope data

  // Convert raw gyroscope data to degrees per second (±250°/s range)
  float mihir_gyroZ_deg_per_sec = mihir_gz / 131.0;  // Conversion factor for ±250°/s

  // Calculate angular displacement by integrating angular velocity over time
  mihir_angularDisplacement += mihir_gyroZ_deg_per_sec * mihir_deltaTime;

  // Create a data string to send to the server
  String mihir_data = String(mihir_angularDisplacement);  // Send angular displacement

  // Send data to server if connected
  if (mihir_client.connected()) {
    mihir_client.println(mihir_data);  // Send the angular displacement to the server
    Serial.println("Data sent: " + mihir_data); // Debugging print
  } else {
    Serial.println("Disconnected from server. Reconnecting...");
    reconnectToServer();  // Handle reconnection
  }

  delay(10);  // Delay to prevent watchdog reset and give the system time to perform tasks
}

void reconnectToServer() {
  // Attempt to reconnect to the server until successful
  while (!mihir_client.connected()) {
    Serial.print("Attempting to reconnect...");
    if (mihir_client.connect(mihir_serverHost, mihir_serverPort)) {
      Serial.println("Reconnected!");
    } else {
      Serial.println("Failed to reconnect. Retrying in 2 seconds...");
      delay(2000);  // Retry every 2 seconds
    }
  }
}
