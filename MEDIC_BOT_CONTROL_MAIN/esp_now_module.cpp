#include "esp_now_module.h"
#include "config.h"

struct_message myData;
struct_message board1, board2, board3;
struct_message boardsStruct[3] = { board1, board2, board3 };

extern double user_weight, user_tempo, user_tempa, user_height_sonar, user_height_laser, motor_height;
extern double user_bmi_laser, user_bmi_sonar;
extern String weightStatus, tempaStatus, tempoStatus, heightLaserStatus, heightSonarStatus;
extern String bmiLaserStatus, bmiSonarStatus;

// MAC address of HEIGHT_WEIGHT_MODULE ESP32-S3
// TODO: Replace with actual MAC address from HEIGHT_WEIGHT_MODULE
uint8_t heightWeightModuleMAC[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

void initESPNow() {
  WiFi.mode(WIFI_STA);
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    while (true);
  }
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));
  
  // Add HEIGHT_WEIGHT_MODULE as peer
  esp_now_peer_info_t peerInfo = {};
  memcpy(peerInfo.peer_addr, heightWeightModuleMAC, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;
  
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add HEIGHT_WEIGHT_MODULE peer");
  }
  
  Serial.print("Main Controller MAC: ");
  Serial.println(WiFi.macAddress());
}

void OnDataRecv(const uint8_t *mac_addr, const uint8_t *incomingData, int len) {
  char macStr[18];
  Serial.print("Packet received from: ");
  snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
  Serial.println(macStr);
  
  // Check if it's measurement data from HEIGHT_WEIGHT_MODULE
  if (len == sizeof(measurement_data_t)) {
    measurement_data_t* data = (measurement_data_t*)incomingData;
    Serial.println("Received HEIGHT_WEIGHT data:");
    Serial.print("Weight: "); Serial.print(data->weight_kg); Serial.println(" kg");
    Serial.print("Height: "); Serial.print(data->height_cm); Serial.println(" cm");
    Serial.print("BMI: "); Serial.println(data->bmi);
    
    // Store in global variables
    user_weight = data->weight_kg;
    user_height_laser = data->height_cm / 100.0; // Convert to meters
    user_bmi_laser = data->bmi;
  } else {
    // Legacy format for other boards
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
    
    boardsStruct[myData.id - 1].a = myData.a;
    boardsStruct[myData.id - 1].b = myData.b;
    boardsStruct[myData.id - 1].c = myData.c;
  }
}

// Data structure matching HEIGHT_WEIGHT_MODULE
typedef struct {
  float weight_kg;
  float height_cm;
  float bmi;
  uint32_t timestamp;
} measurement_data_t;

void requestESPNowData() {
  Serial.println("Requesting data from HEIGHT_WEIGHT_MODULE...");
  
  // Send request command (0xAA = request code)
  uint8_t requestCmd = 0xAA;
  esp_err_t result = esp_now_send(heightWeightModuleMAC, &requestCmd, 1);
  
  if (result == ESP_OK) {
    Serial.println("Request sent successfully");
  } else {
    Serial.println("Error sending request");
  }
  
  // Wait for response (data will arrive via OnDataRecv callback)
  delay(500);
}

void readESPNowData() {
  Serial.println("W-BODY = " + String(boardsStruct[0].a));
  Serial.println("W-TOLR = " + String(boardsStruct[0].b));
  Serial.println("W-STAT = " + String(boardsStruct[0].c));

  Serial.println("T-BODY = " + String(boardsStruct[2].a));
  Serial.println("T-AMBT = " + String(boardsStruct[2].b));
  Serial.println("B-DIST = " + String(boardsStruct[2].c));

  Serial.println("H-SONAR = " + String(boardsStruct[1].a));
  Serial.println("H-LASER = " + String(boardsStruct[1].b));
  Serial.println("H-MOTOR = " + String(boardsStruct[1].c));
  Serial.println();

  user_weight = (double)boardsStruct[0].a;
  user_tempo = (double)boardsStruct[1].a;
  user_tempa = (double)boardsStruct[1].b;
  user_height_sonar = HEIGHT_REF - (double)boardsStruct[2].a;
  user_height_laser = HEIGHT_REF - (double)boardsStruct[2].b;
  motor_height = (double)boardsStruct[2].c;
  
  user_bmi_laser = (double)(user_weight) / (user_height_laser * user_height_laser);
  user_bmi_sonar = (double)(user_weight) / (user_height_sonar * user_height_sonar);

  // Status mapping for height laser
  if (!isnan(user_height_laser)) {
    if (user_height_laser < 1.45) heightLaserStatus = "DWARF";
    else if (user_height_laser < 1.65) heightLaserStatus = "SHORT";
    else if (user_height_laser < 1.78) heightLaserStatus = "AVG";
    else if (user_height_laser < 2.0) heightLaserStatus = "TALL";
    else heightLaserStatus = "GIGA";
  } else heightLaserStatus = "NIL";

  // Status mapping for height sonar
  if (!isnan(user_height_sonar)) {
    if (user_height_sonar < 1.45) heightSonarStatus = "DWARF";
    else if (user_height_sonar < 1.65) heightSonarStatus = "SHORT";
    else if (user_height_sonar < 1.78) heightSonarStatus = "AVG";
    else if (user_height_sonar < 2.0) heightSonarStatus = "TALL";
    else heightSonarStatus = "GIGA";
  } else heightSonarStatus = "NIL";

  // Status mapping for weight
  if (!isnan(user_weight)) {
    if (user_weight < 50) weightStatus = "UNDER";
    else if (user_weight < 70) weightStatus = "NORM";
    else if (user_weight < 85) weightStatus = "OVER";
    else if (user_weight < 120) weightStatus = "OBES1";
    else weightStatus = "OBES2";
  } else weightStatus = "NIL";

  // Status mapping for BMI laser
  if (!isnan(user_bmi_laser)) {
    if (user_bmi_laser < 18.5) bmiLaserStatus = "UNDER";
    else if (user_bmi_laser < 24.9) bmiLaserStatus = "NORM";
    else if (user_bmi_laser < 30) bmiLaserStatus = "OVER";
    else if (user_bmi_laser < 34.9) bmiLaserStatus = "OBES1";
    else if (user_bmi_laser < 39.9) bmiLaserStatus = "OBES2";
    else bmiLaserStatus = "OBES3";
  } else bmiLaserStatus = "NIL";

  // Status mapping for BMI sonar
  if (!isnan(user_bmi_sonar)) {
    if (user_bmi_sonar < 18.5) bmiSonarStatus = "UNDER";
    else if (user_bmi_sonar < 24.9) bmiSonarStatus = "NORM";
    else if (user_bmi_sonar < 30) bmiSonarStatus = "OVER";
    else if (user_bmi_sonar < 34.9) bmiSonarStatus = "OBES1";
    else if (user_bmi_sonar < 39.9) bmiSonarStatus = "OBES2";
    else bmiSonarStatus = "OBES3";
  } else bmiSonarStatus = "NIL";

  // Status mapping for ambient temperature
  if (!isnan(user_tempa)) {
    if (user_tempa < 25) tempaStatus = "LOW";
    else if (user_tempa < 30) tempaStatus = "ROOM";
    else if (user_tempa < 37.5) tempaStatus = "NORM";
    else if (user_tempa < 40) tempaStatus = "HIGH";
    else tempaStatus = "EXTR";
  } else tempaStatus = "NIL";

  // Status mapping for body temperature
  if (!isnan(user_tempo)) {
    if (user_tempo < 28) tempoStatus = "LHYP";
    else if (user_tempo < 32) tempoStatus = "LOW";
    else if (user_tempo < 37.5) tempoStatus = "NORM";
    else if (user_tempo < 40) tempoStatus = "HIGH";
    else tempoStatus = "HHYP";
  } else tempoStatus = "NIL";
}