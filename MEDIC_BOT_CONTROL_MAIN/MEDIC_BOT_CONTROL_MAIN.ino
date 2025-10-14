/*
  iDEPP PROJECTS 2024: MEDIC-BOT 2024
  MEDIC-BOT CONTROL UNIT FIRMWARE
  DATE: 10/30/2024
*/

#include "config.h"
#include "esp_now_module.h"
#include "fingerprint_module.h"
#include "rfid_module.h"
#include "oximeter_module.h"
#include <Wire.h>

// Firebase includes - only in main file
#include <Arduino.h>
#if defined(ESP32)
#include <WiFi.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#endif
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"

// Firebase objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
unsigned long sendDataPrevMillis = 0;
int count = 0;
bool signupOK = false;

// User data structure
typedef struct {
  String name;
  String rfid;
  String age;
  String gender;
  String medical_id;
  bool isLoggedIn;
} UserData;

// Display data structure for ESP-NOW
typedef struct {
  String message_type;  // "PROMPT", "USER_DATA", "SENSOR_DATA"
  String message;
  String user_name;
  String user_age;
  String user_gender;
  double heart_rate;
  double spo2;
  double temperature;
  double weight;
  double height;
  double bmi;
} DisplayData;

// Global system variables
int fingerprint_count, rfid_count;
double oximeter_temp, user_height_laser, user_height_sonar, user_weight;
double user_bmi_laser, user_bmi_sonar, user_tempa, user_tempo, motor_height;
String weightStatus, tempaStatus, tempoStatus, heightLaserStatus, heightSonarStatus;
String bmiLaserStatus, bmiSonarStatus;
UserData currentUser;
DisplayData displayData;


// Display UART communication (GPIO 6=TX, GPIO 7=RX)
HardwareSerial displaySerial(2);

void initFirebase() {
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.print("Connected with IP: ");
  Serial.println(WiFi.localIP());

  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Firebase sign-up successful");
    signupOK = true;
  } else {
    Serial.printf("Error: %s\n", config.signer.signupError.message.c_str());
  }
  config.token_status_callback = tokenStatusCallback;
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
}

void writePath(String data_path, String status_path, String data, String status) {
  if (Firebase.RTDB.setString(&fbdo, data_path, data)) {
    Serial.println("PASSED");
  } else {
    Serial.println("FAILED: " + fbdo.errorReason());
  }
  if (Firebase.RTDB.setString(&fbdo, status_path, status)) {
    Serial.println("PASSED");
  } else {
    Serial.println("FAILED: " + fbdo.errorReason());
  }
}

UserData fetchUserData(String rfidNumber) {
  UserData user;
  user.isLoggedIn = false;
  
  if (Firebase.ready()) {
    String basePath = "USERS/" + rfidNumber;
    
    // Fetch name
    if (Firebase.RTDB.getString(&fbdo, basePath + "/name")) {
      user.name = fbdo.stringData();
    } else {
      user.name = "Unknown";
      return user;
    }
    
    // Fetch other data
    if (Firebase.RTDB.getString(&fbdo, basePath + "/age")) {
      user.age = fbdo.stringData();
    }
    if (Firebase.RTDB.getString(&fbdo, basePath + "/gender")) {
      user.gender = fbdo.stringData();
    }
    if (Firebase.RTDB.getString(&fbdo, basePath + "/medical_id")) {
      user.medical_id = fbdo.stringData();
    }
    
    user.rfid = rfidNumber;
  }
  return user;
}

void sendToDisplay(String msgType, String message) {
  // Create JSON-like string for display
  String data = "{";
  data += "\"type\":\"" + msgType + "\",";
  data += "\"message\":\"" + message + "\",";
  data += "\"user_name\":\"" + currentUser.name + "\",";
  data += "\"user_age\":\"" + currentUser.age + "\",";
  data += "\"user_gender\":\"" + currentUser.gender + "\",";
  data += "\"heart_rate\":" + String(user_hr, 1) + ",";
  data += "\"spo2\":" + String(user_sp02, 1) + ",";
  data += "\"temperature\":" + String(user_tempo, 1) + ",";
  data += "\"weight\":" + String(user_weight, 1) + ",";
  data += "\"height\":" + String(user_height_laser, 2) + ",";
  data += "\"bmi\":" + String(user_bmi_laser, 1);
  data += "}\n";
  
  displaySerial.print(data);
  Serial.println("Sent to display: " + message);
}

void sendModeSwitchState() {
  String data = "{";
  data += "\"type\":\"MODE_SWITCH\",";
  data += "\"message\":\"Mode switch updated\",";
  data += "\"mode_switch\":" + String(digitalRead(MODE_SWITCH) == HIGH ? "true" : "false");
  data += "}\n";
  
  displaySerial.print(data);
  Serial.println("Mode switch state sent: " + String(digitalRead(MODE_SWITCH)));
}

bool updateFingerprintStatus(String rfidNumber, bool status) {
  if (Firebase.ready()) {
    String path = "USERS/" + rfidNumber + "/fingerprint_registered";
    return Firebase.RTDB.setBool(&fbdo, path, status);
  }
  return false;
}

uint8_t rfidToFingerprintID(String rfidNumber) {
  // Convert RFID hex to number within 1-127 range
  unsigned long rfidHash = 0;
  for (int i = 0; i < rfidNumber.length(); i++) {
    rfidHash += rfidNumber[i];
  }
  return (rfidHash % 127) + 1;
}

void enrollmentProcess() {
  Serial.println("=== ENROLLMENT MODE ===");
  sendToDisplay("PROMPT", "Please scan your RFID card...");
  
  tidString = "NIL";
  while (tidString == "NIL") {
    readRFID();
    delay(100);
  }
  
  Serial.println("RFID Detected: " + tidString);
  sendToDisplay("PROMPT", "RFID Detected: " + tidString);
  
  UserData user = fetchUserData(tidString);
  Serial.println("User: " + user.name);
  
  if (user.name == "Unknown") {
    Serial.println("ERROR: RFID not registered in system!");
    sendToDisplay("PROMPT", "ERROR: RFID not registered!");
    return;
  }
  
  uint8_t fingerprintID = rfidToFingerprintID(tidString);
  Serial.println("Assigned Fingerprint ID: " + String(fingerprintID));
  sendToDisplay("PROMPT", "Fingerprint ID: " + String(fingerprintID));
  
  id = fingerprintID;
  Serial.println("Starting fingerprint enrollment for " + user.name + "...");
  sendToDisplay("PROMPT", "Starting enrollment for " + user.name);
  
  if (getFingerprintEnroll()) {
    Serial.println("SUCCESS: Fingerprint enrolled for " + user.name);
    sendToDisplay("PROMPT", "SUCCESS: Enrollment complete!");
    
    if (updateFingerprintStatus(tidString, true)) {
      Serial.println("Database updated: Registration complete!");
      sendToDisplay("PROMPT", "Database updated successfully");
    } else {
      Serial.println("WARNING: Database update failed");
      sendToDisplay("PROMPT", "WARNING: Database update failed");
    }
  } else {
    Serial.println("FAILED: Fingerprint enrollment unsuccessful");
    sendToDisplay("PROMPT", "FAILED: Enrollment unsuccessful");
  }
  
  Serial.println("=== ENROLLMENT COMPLETE ===");
  delay(2000);
}

// void checkDatabaseStructure() {
  // Serial.println("=== CHECKING DATABASE STRUCTURE ===");
  // 
  // if (Firebase.ready()) {
    // //Check if USERS node exists
    // if (Firebase.RTDB.get(&fbdo, "USERS")) {
      // Serial.println("USERS node exists");
      // Serial.println("Data type: " + fbdo.dataType());
      // 
      // if (fbdo.dataType() == "json") {
        // Serial.println("Available users in database:");
        // Serial.println(fbdo.jsonString());
      // }
    // } else {
      // Serial.println("USERS node does not exist!");
      // Serial.println("Error: " + fbdo.errorReason());
    // }
    // 
    // //Try to list all children under USERS
    // Serial.println("\nTrying to get USERS children...");
    // if (Firebase.RTDB.getJSON(&fbdo, "USERS")) {
      // Serial.println("USERS JSON data:");
      // Serial.println(fbdo.jsonString());
    // }
  // } else {
    // Serial.println("Firebase not ready for database check!");
  // }
  // 
  // Serial.println("=== DATABASE CHECK COMPLETE ===");
// }


void loginProcess() {
  Serial.println("=== LOGIN MODE ===");
  sendToDisplay("PROMPT", "Please scan your RFID card...");
  
  tidString = "NIL";
  while (tidString == "NIL") {
    readRFID();
    delay(100);
  }
  
  Serial.println("RFID Detected: " + tidString);
  sendToDisplay("PROMPT", "RFID Detected: " + tidString);
  
  currentUser = fetchUserData(tidString);
  
  if (currentUser.name == "Unknown") {
    Serial.println("ERROR: User not found!");
    sendToDisplay("PROMPT", "ERROR: User not found!");
    // return;
  }
  
  // Serial.println("User found: " + currentUser.name);
  sendToDisplay("PROMPT", "Please scan fingerprint...");
  delay(2000);
  
  uint8_t expectedID = rfidToFingerprintID(tidString);
  int fingerprintResult = getFingerprintIDez();
  
  if (fingerprintResult == expectedID) {
    currentUser.isLoggedIn = true;
    Serial.println("LOGIN SUCCESS: Welcome " + currentUser.name);
    sendToDisplay("FINGERPRINT_SUCCESS", "Fingerprint verified successfully");
    delay(1000);
    sendToDisplay("USER_DATA", "Welcome " + currentUser.name);
    
    // Initial sensor reading after login
    // readOximeter();
    // readESPNowData();
    // writeFirebaseDB();
    
    // Wait for display commands after login
    Serial.println("Waiting for display commands...");
    // while (currentUser.isLoggedIn) {
      if (displaySerial.available()) {
        String command = displaySerial.readStringUntil('\n');
        command.trim();
        
        if (command == "READ_OXIMETER") {
          Serial.println("=== READ BUTTON PRESSED ===");
          Serial.println("Display requested sensor reading");
          
          Serial.println("Reading oximeter...");
          readOximeter();
          Serial.println("Heart Rate: " + String(user_hr));
          Serial.println("SpO2: " + String(user_sp02));
          
          Serial.println("Reading ESP-NOW data...");
          readESPNowData();
          Serial.println("Temperature: " + String(user_tempo));
          Serial.println("Weight: " + String(user_weight));
          Serial.println("Height: " + String(user_height_laser));
          Serial.println("BMI: " + String(user_bmi_laser));
          
          sendToDisplay("SENSOR_DATA", "All sensors read successfully");
          Serial.println("=== SENSOR DATA SENT TO DISPLAY ===");
        } else if (command == "GET_MODE_SWITCH") {
          Serial.println("Display requested MODE_SWITCH state");
          sendModeSwitchState();
        }
      }
      delay(100); // Small delay to prevent busy waiting
    // }
    
  } else if (fingerprintResult == -1) {
    Serial.println("LOGIN FAILED: No fingerprint detected");
    sendToDisplay("FINGERPRINT_ERROR", "No fingerprint detected. Try again.");
  } else {
    Serial.println("LOGIN FAILED: Fingerprint mismatch");
    sendToDisplay("FINGERPRINT_ERROR", "Fingerprint mismatch. Please enroll first.");
  }
}

void writeFirebaseDB() {
  if (Firebase.ready()) {
    writePath("MBOT-READINGS/MBOT-HLASER/VALUE", "MBOT-READINGS/MBOT-HLASER/STATUS", String(user_height_laser, 2), heightLaserStatus);
    writePath("MBOT-READINGS/MBOT-HSONAR/VALUE", "MBOT-READINGS/MBOT-HSONAR/STATUS", String(user_height_sonar, 2), heightSonarStatus);
    writePath("MBOT-READINGS/MBOT-WEIGHT/VALUE", "MBOT-READINGS/MBOT-WEIGHT/STATUS", String(user_weight, 2), weightStatus);
    writePath("MBOT-READINGS/MBOT-TEMPA/VALUE", "MBOT-READINGS/MBOT-TEMPA/STATUS", String(user_tempa, 2), tempaStatus);
    writePath("MBOT-READINGS/MBOT-TEMPO/VALUE", "MBOT-READINGS/MBOT-TEMPO/STATUS", String(user_tempo, 2), tempoStatus);
    writePath("MBOT-READINGS/MBOT-BMI_LASER/VALUE", "MBOT-READINGS/MBOT-BMI_LASER/STATUS", String(user_bmi_laser, 2), bmiLaserStatus);
    writePath("MBOT-READINGS/MBOT-BMI_SONAR/VALUE", "MBOT-READINGS/MBOT-BMI_SONAR/STATUS", String(user_bmi_sonar, 2), bmiSonarStatus);
    writePath("MBOT-READINGS/MBOT-HR/VALUE", "MBOT-READINGS/MBOT-HR/STATUS", String(user_hr, 2), hrStatus);
    writePath("MBOT-READINGS/MBOT-SP02/VALUE", "MBOT-READINGS/MBOT-SP02/STATUS", String(user_sp02, 2), sp02Status);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_I2C, SCL_I2C);
  pinMode(MP1, OUTPUT);
  pinMode(MP2, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(MODE_SWITCH, INPUT_PULLDOWN);
  
  digitalWrite(MP1, LOW);
  digitalWrite(MP2, LOW);
  
  delay(1000);

  Serial.println("...Starting Medic-Bot");
  
  // Initialize display UART (GPIO 6=TX, GPIO 7=RX)
  displaySerial.begin(115200, SERIAL_8N1, 7, 6);
  
  initESPNow();
  initFingerprint();
  delay(1000);
  initRFID();
  initOximeter();
  delay(1000);
  initFirebase();
  
  // Send initial mode switch state to display after boot screen
  delay(3000); // Wait for display to reach instruction screen
  sendModeSwitchState();
}

void loop() {
  Serial.println(digitalRead(MODE_SWITCH));

  if (digitalRead(MODE_SWITCH) == HIGH) {
    enrollmentProcess();
  }
  
  if (digitalRead(MODE_SWITCH) == LOW) {
    loginProcess();
  }
}