#include <SPI.h>
#include <MFRC522.h>

#define RST_PIN D3
#define SS_PIN D4

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

void setup() {
  Serial.begin(9600);  // Initialize serial communication

  SPI.begin();  // Initialize SPI bus
  mfrc522.PCD_Init();  // Initialize MFRC522
  Serial.println("Waiting for card...");
}

void loop() {
  // Check if a card is present
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Get the UID of the card
    String uid = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      uid += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      uid += String(mfrc522.uid.uidByte[i], HEX);
    }

    // Print the UID to serial monitor
    Serial.print("ID Terbaca: ");
    Serial.println(uid);
    Serial.println();

    // Halt PICC
    mfrc522.PICC_HaltA();
    // Stop encryption on PCD
    mfrc522.PCD_StopCrypto1();
  }
}
