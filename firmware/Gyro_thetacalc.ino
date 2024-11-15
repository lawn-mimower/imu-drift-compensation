#include <ESP8266WiFi.h>
#include <Wire.h>
#include <WiFiClient.h>

// Wi-Fi credentials
const char* ssid = "YOUR_WIFI_SSID";            // Your Wi-Fi SSID
const char* password = "YOUR_WIFI_PASSWORD";  // Your Wi-Fi password
const char* host = "192.168.1.x";       // IP address of your MacBook (make sure it's on the same network)
const uint16_t port = 23456;            // Port number to connect to on the MacBook

// I2C address of the MPU-6050
const int MPU_addr = 0x68;              // I2C address of the MPU-6050
int16_t GyZ;                           // Z-axis gyroscope raw data
float gyroZ_deg_per_sec;               // Gyroscope data in degrees per second
float angleZ = 0;                      // Angular displacement along the Z-axis

unsigned long previousTime = 0;
unsigned long currentTime;
float deltaTime;  // Time difference between samples

WiFiClient client;

void setup() {
  Serial.begin(115200);   // Start Serial Monitor for debugging
  Wire.begin();
  
  // Initialize MPU-6050
  Wire.beginTransmission(MPU_addr);
  Wire.write(0x6B);  // Power management register
  Wire.write(0);     // Wake up the MPU-6050
  Wire.endTransmission(true);

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
}

void loop() {
  // Calculate the time difference (deltaTime) in seconds
  currentTime = millis();
  deltaTime = (currentTime - previousTime) / 1000.0;  // Convert to seconds
  
  // Update data every 10ms (100Hz sample rate)
  if (deltaTime >= 0.01) {  
    previousTime = currentTime;  // Update previousTime for next loop

    // Read gyroscope data from the MPU-6050
    Wire.beginTransmission(MPU_addr);
    Wire.write(0x43);  // Starting register for gyroscope data (Z-axis)
    Wire.endTransmission(false);
    Wire.requestFrom(MPU_addr, 6, true);  // Request 6 bytes of gyroscope data

    GyZ = (Wire.read() << 8 | Wire.read());  // Read Z-axis gyroscope data (raw format)
  
    // Convert raw gyroscope data to degrees per second (assuming ±250°/s range)
    gyroZ_deg_per_sec = GyZ / 131.0;  // Conversion factor (131.0 for ±250°/s)

    // Calculate angular displacement (integrating angular velocity)
    angleZ += gyroZ_deg_per_sec * deltaTime;

    // Print the current angular displacement to the Serial Monitor (optional)
    Serial.print("AngleZ: ");
    Serial.println(angleZ);

    // Connect to the server (MacBook) on port 23456
    if (client.connect(host, port)) {
      String data = String(angleZ);  // Prepare the angular displacement data
      client.println(data);          // Send the angleZ data to the server

      // Optionally, wait for the server's response
      while (client.available()) {
        char c = client.read();
        Serial.print(c);
      }

      client.stop();  // Close the connection
    } else {
      Serial.println("Connection failed");
    }
  }
}
