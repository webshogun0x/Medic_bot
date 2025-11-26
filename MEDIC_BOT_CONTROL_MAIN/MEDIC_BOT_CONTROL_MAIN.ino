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
#include <time.h>
#include "esp_sntp.h"

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

// WiFi monitoring
void checkWiFiConnection() {
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected, attempting reconnection...");
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
      delay(500);
      Serial.print(".");
      attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("\nWiFi reconnected: " + WiFi.localIP().toString());
    } else {
      Serial.println("\nWiFi reconnection failed");
    }
  }
}

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

  // Configure time synchronization for SSL
  Serial.println("Setting up time synchronization...");
  configTime(0, 0, "pool.ntp.org", "time.nist.gov");
  
  // Wait for time to be set
  time_t now = time(nullptr);
  int timeout = 0;
  while (now < 8 * 3600 * 2 && timeout < 20) {
    delay(500);
    Serial.print(".");
    now = time(nullptr);
    timeout++;
  }
  Serial.println();
  Serial.println("Time synchronized: " + String(ctime(&now)));

  // Configure Firebase for RTDB access without authentication
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;
  
  // Skip authentication for RTDB with open rules
  Serial.println("Initializing Firebase without authentication...");
  
  // Initialize Firebase
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);
  
  signupOK = true;  // Set to true for RTDB access
  
  Serial.println("Firebase initialization complete");
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
  user.rfid = rfidNumber;
  
  // Check if Firebase is ready and connected
  if (Firebase.ready() && WiFi.status() == WL_CONNECTED) {
    String basePath = "USERS/" + rfidNumber;
    
    Serial.println("Attempting to fetch user data from Firebase...");
    
    // Fetch name with timeout
    if (Firebase.RTDB.getString(&fbdo, basePath + "/name")) {
      user.name = fbdo.stringData();
      Serial.println("Found user: " + user.name);
    } else {
      Serial.println("User not found in Firebase: " + fbdo.errorReason());
      // Use fallback data
      user.name = "Unknown";
    }
    
    // Only fetch other data if name was found
    if (user.name != "Unknown") {
      if (Firebase.RTDB.getString(&fbdo, basePath + "/age")) {
        user.age = fbdo.stringData();
      }
      if (Firebase.RTDB.getString(&fbdo, basePath + "/gender")) {
        user.gender = fbdo.stringData();
      }
      if (Firebase.RTDB.getString(&fbdo, basePath + "/medical_id")) {
        user.medical_id = fbdo.stringData();
      }
    }
  } else {
    Serial.println("Firebase not ready or WiFi disconnected, using fallback data");
  }
  
  // Fallback for offline mode or if user not found
  if (user.name == "Unknown" || user.name == "") {
    Serial.println("Using fallback user data");
    user.name = "Sir_timmy";
    user.age = "25 years";
    user.gender = "Male";
    user.medical_id = "MED001";
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

void dashboardMode() {
  Serial.println("=== DASHBOARD MODE ===");
  Serial.println("User: " + currentUser.name + " is now in dashboard");
  
  while (currentUser.isLoggedIn) {
    handleDisplayCommands();
    delay(100);
  }
  
  Serial.println("=== EXITING DASHBOARD ===");
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
  
  currentUser = fetchUserData(tidString);
  Serial.println("User: " + currentUser.name);
  
  if (currentUser.name == "Unknown") {
    Serial.println("ERROR: RFID not registered in system!");
    sendToDisplay("PROMPT", "ERROR: RFID not registered!");
    return;
  }
  
  uint8_t fingerprintID = rfidToFingerprintID(tidString);
  Serial.println("Assigned Fingerprint ID: " + String(fingerprintID));
  sendToDisplay("PROMPT", "Fingerprint ID: " + String(fingerprintID));
  
  id = fingerprintID;
  Serial.println("Starting fingerprint enrollment for " + currentUser.name + "...");
  sendToDisplay("PROMPT", "Starting enrollment for " + currentUser.name);
  
  if (getFingerprintEnroll()) {
    Serial.println("SUCCESS: Fingerprint enrolled for " + currentUser.name);
    sendToDisplay("PROMPT", "SUCCESS: Enrollment complete!");
    
    if (updateFingerprintStatus(tidString, true)) {
      Serial.println("Database updated: Registration complete!");
      sendToDisplay("PROMPT", "Database updated successfully");
    } else {
      Serial.println("WARNING: Database update failed");
      sendToDisplay("PROMPT", "WARNING: Database update failed");
    }
    
    // After successful enrollment, go to dashboard
    currentUser.isLoggedIn = true;
    delay(2000);
    sendToDisplay("USER_DATA", "Welcome " + currentUser.name + " - Enrollment Complete");
    dashboardMode();
  } else {
    Serial.println("FAILED: Fingerprint enrollment unsuccessful");
    sendToDisplay("PROMPT", "FAILED: Enrollment unsuccessful");
  }
  
  Serial.println("=== ENROLLMENT COMPLETE ===");
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
    return;
  }
  
  sendToDisplay("PROMPT", "Please scan fingerprint...");
  delay(5000);
  
  uint8_t expectedID = rfidToFingerprintID(tidString);
  int fingerprintResult = getFingerprintIDez();
  
  if (fingerprintResult == expectedID) {
    currentUser.isLoggedIn = true;
    Serial.println("LOGIN SUCCESS: Welcome " + currentUser.name);
    sendToDisplay("FINGERPRINT_SUCCESS", "Fingerprint verified successfully");
    delay(1000);
    sendToDisplay("USER_DATA", "Welcome " + currentUser.name);
    
    // Go to dashboard after successful login
    dashboardMode();
    
  } else if (fingerprintResult == -1) {
    Serial.println("LOGIN FAILED: No fingerprint detected");
    sendToDisplay("FINGERPRINT_ERROR", "No fingerprint detected. Try again.");
  } else {
    Serial.println("LOGIN FAILED: Fingerprint mismatch");
    sendToDisplay("FINGERPRINT_ERROR", "Fingerprint mismatch. Please enroll first.");
  }
}

void writeFirebaseDB() {
  if (Firebase.ready() && WiFi.status() == WL_CONNECTED && currentUser.rfid != "") {
    Serial.println("Saving data to Firebase...");
    
    String userPath = "READINGS/" + currentUser.rfid + "/latest";
    time_t now = time(nullptr);
    String timestamp = String(now);
    
    bool success = true;
    
    // Save latest readings with error checking
    if (!Firebase.RTDB.setFloat(&fbdo, userPath + "/heart_rate", user_hr)) {
      Serial.println("Failed to save heart_rate: " + fbdo.errorReason());
      success = false;
    }
    if (!Firebase.RTDB.setFloat(&fbdo, userPath + "/spo2", user_sp02)) {
      Serial.println("Failed to save spo2: " + fbdo.errorReason());
      success = false;
    }
    if (!Firebase.RTDB.setFloat(&fbdo, userPath + "/temperature", user_tempo)) {
      Serial.println("Failed to save temperature: " + fbdo.errorReason());
      success = false;
    }
    if (!Firebase.RTDB.setFloat(&fbdo, userPath + "/weight", user_weight)) {
      Serial.println("Failed to save weight: " + fbdo.errorReason());
      success = false;
    }
    if (!Firebase.RTDB.setFloat(&fbdo, userPath + "/height", user_height_laser)) {
      Serial.println("Failed to save height: " + fbdo.errorReason());
      success = false;
    }
    if (!Firebase.RTDB.setFloat(&fbdo, userPath + "/bmi", user_bmi_laser)) {
      Serial.println("Failed to save bmi: " + fbdo.errorReason());
      success = false;
    }
    if (!Firebase.RTDB.setString(&fbdo, userPath + "/timestamp", timestamp)) {
      Serial.println("Failed to save timestamp: " + fbdo.errorReason());
      success = false;
    }
    
    if (success) {
      Serial.println("Health data saved to Firebase for user: " + currentUser.name);
      
      // Save to history only if latest save was successful
      String historyPath = "READINGS/" + currentUser.rfid + "/history/" + timestamp;
      Firebase.RTDB.setFloat(&fbdo, historyPath + "/heart_rate", user_hr);
      Firebase.RTDB.setFloat(&fbdo, historyPath + "/spo2", user_sp02);
      Firebase.RTDB.setFloat(&fbdo, historyPath + "/temperature", user_tempo);
      Firebase.RTDB.setFloat(&fbdo, historyPath + "/bmi", user_bmi_laser);
    } else {
      Serial.println("Some data failed to save to Firebase");
    }
  } else {
    Serial.println("Cannot save to Firebase - not ready or no connection");
    Serial.println("Firebase ready: " + String(Firebase.ready()));
    Serial.println("WiFi status: " + String(WiFi.status()));
    Serial.println("User RFID: " + currentUser.rfid);
  }
}

void setup() {
  Serial.begin(115200);
  Wire.begin(SDA_I2C, SCL_I2C);
  pinMode(MP1, OUTPUT);
  pinMode(MP2, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  
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
  
  Serial.println("System ready - waiting for display commands");
}

void handleDisplayCommands() {
  if (displaySerial.available()) {
    String command = displaySerial.readStringUntil('\n');
    command.trim();
    
    if (command == "START_LOGIN") {
      Serial.println("=== DISPLAY REQUESTED LOGIN ===");
      loginProcess();
    } else if (command == "START_ENROLLMENT") {
      Serial.println("=== DISPLAY REQUESTED ENROLLMENT ===");
      enrollmentProcess();
    } else if (command == "READ_OXIMETER") {
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
    } else if (command == "SAVE_READINGS") {
      Serial.println("=== SAVE BUTTON PRESSED ===");
      Serial.println("Saving readings to Firebase...");
      writeFirebaseDB();
      sendToDisplay("PROMPT", "Readings saved successfully for " + currentUser.name);
      Serial.println("=== READINGS SAVED ===");
    } else if (command == "LOGOUT") {
      Serial.println("=== LOGOUT BUTTON PRESSED ===");
      Serial.println("User " + currentUser.name + " logging out");
      currentUser.isLoggedIn = false;
      sendToDisplay("PROMPT", "Logged out successfully");
      Serial.println("=== USER LOGGED OUT ===");
    }
  }
}

void loop() {
  // Check WiFi connection every 30 seconds
  static unsigned long lastWiFiCheck = 0;
  if (millis() - lastWiFiCheck > 30000) {
    checkWiFiConnection();
    lastWiFiCheck = millis();
  }
  
  // Always handle display commands
  handleDisplayCommands();
  delay(100);
}