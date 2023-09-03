#include <SPI.h>
#include <MFRC522.h>

#define SS_PIN 10   // Pin Chip Select untuk modul RFID
#define RST_PIN 9   // Pin Reset untuk modul RFID

MFRC522 mfrc522(SS_PIN, RST_PIN); // Inisialisasi MFRC522

void setup() {
  Serial.begin(9600); // Inisialisasi komunikasi serial
  SPI.begin();        // Inisialisasi SPI bus
  mfrc522.PCD_Init(); // Inisialisasi modul RFID
  Serial.println("Siap untuk membaca kartu RFID...");
}

void loop() {
  // Cek apakah ada tag RFID yang terdeteksi
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.print("ID Kartu: ");

    // Baca dan tampilkan ID kartu dalam bentuk hexadecimal
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
      Serial.print(mfrc522.uid.uidByte[i], HEX);
    }
    
    Serial.println(); // Pindah ke baris baru
    mfrc522.PICC_HaltA(); // Menonaktifkan tag RFID saat selesai
  }
}
