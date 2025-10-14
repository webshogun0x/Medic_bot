#include "pins_arduino.h"
#include <iterator>
#include "esp32-hal-spi.h"
#include "rfid_module.h"

MFRC522 rfid(SS_PIN, RST_PIN);
MFRC522::MIFARE_Key key;
byte nuidPICC[4];
String tidString = "NIL";

void initRFID() {
  SPI.begin(CLK_PIN, MISO_PIN, MOSI_PIN, SS_PIN);
  rfid.PCD_Init();
  for (byte i = 0; i < 6; i++) {
    key.keyByte[i] = 0xFF;
    Serial.println("rfid initialized");
  }
}

void readRFID() {
  if (!rfid.PICC_IsNewCardPresent()) {
    return;
  }
  if (!rfid.PICC_ReadCardSerial()) {
    return;
  }

  tidString = " ";
  for (byte i = 0; i < rfid.uid.size; i++) {
    tidString += String(rfid.uid.uidByte[i] < 0x10 ? "0" : "");
    tidString += String(rfid.uid.uidByte[i], HEX);
  }
  tidString.toUpperCase();
  Serial.print("RFID UID: ");
  Serial.println(tidString);

  rfid.PICC_HaltA();
  rfid.PCD_StopCrypto1();
  delay(500);
}