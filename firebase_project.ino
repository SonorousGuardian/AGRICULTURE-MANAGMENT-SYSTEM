#define WIFI_SSID "amritesh123"
#define WIFI_PASSWORD "amritesh@123"

// Firebase database URL and secret
#define FIREBASE_HOST "https://console.firebase.google.com/project/iot-hid/database/iot-hid-default-rtdb/data/~2F"
#define FIREBASE_AUTH "FyRoqm2kZsnt2xRYKwuVZ7BgG1nKyygWVXDxg6Mm" // Replace with your Firebase Database Secret

#include <WiFi.h>
#include <FirebaseESP32.h>
#include <DHT.h>

// DHT 11 sensor type
#define DHTPIN 32       // Pin connected to the DHT sensor
#define DHTTYPE DHT11

#define SOIL_MOISTURE_PIN 26  // Changed to GPIO26 for analog input
#define RELAY_PIN 13
#define MOTOR_PIN 33
#define BUZZER_PIN 14
#define LED_PIN 2
#define LDR_PIN 36     // LDR connected to GPIO36
#define RGB_PIN_RED 4

DHT dht(DHTPIN, DHTTYPE);

// Define Firebase objects
FirebaseData firebaseData;
FirebaseConfig firebaseConfig;
FirebaseAuth firebaseAuth;

void setup() {
  Serial.begin(115200);
  
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(MOTOR_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(SOIL_MOISTURE_PIN, INPUT);
  pinMode(LED_PIN, OUTPUT);
  pinMode(LDR_PIN, INPUT);

  digitalWrite(RELAY_PIN, HIGH);
  digitalWrite(MOTOR_PIN, LOW);
  digitalWrite(BUZZER_PIN, LOW);
  digitalWrite(LED_PIN, HIGH);
  
  dht.begin();

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("Connected to Wi-Fi");

  // Connect to Firebase
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
  Firebase.reconnectWiFi(true);
}

void loop() {
  // Read sensors
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature();
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN); // Read analog value
  int ldrValue = analogRead(LDR_PIN) / 10;  // Read LDR value

  if (isnan(humidity) || isnan(temperature)) {
    Serial.println("Failed to read from sensors!");
    return;
  }

  Serial.print("Humidity: ");
  Serial.print(humidity);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(temperature);
  Serial.print(" *C\t");
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisture);
  Serial.print("\tLDR Value: ");
  Serial.println(ldrValue);

  // Send data to Firebase
  Firebase.setFloat(firebaseData, "/humidity", humidity);
  Firebase.setFloat(firebaseData, "/temperature", temperature);
  Firebase.setInt(firebaseData, "/soilMoisture", soilMoisture);
  Firebase.setInt(firebaseData, "/ldrValue", ldrValue);

  // Control relay, motor, and buzzer based on temperature and soil moisture
  if (temperature < 30 || soilMoisture < 300) {
    digitalWrite(MOTOR_PIN, HIGH);  // Turn on water pump
    digitalWrite(BUZZER_PIN, HIGH); // Turn on buzzer
    Firebase.setInt(firebaseData, "/relayStatus", 1);
  } else {
    digitalWrite(MOTOR_PIN, LOW);    // Turn off water pump
    digitalWrite(BUZZER_PIN, LOW);  // Turn off buzzer
    Firebase.setInt(firebaseData, "/relayStatus", 0);
  }

  delay(5000); // Adjust the delay as needed
}
