#include <Servo.h>
#include <MFRC522.h>
#include <SPI.h>

#define SS_PIN 10   // Pin Chip Select untuk modul RFID
#define RST_PIN 9   // Pin Reset untuk modul RFID
#define LED_PIN 13  // Pin LED built-in Arduino (Opsional)
#define SERVO_PIN 2 // Pin untuk servo

MFRC522 mfrc522(SS_PIN, RST_PIN); // Inisialisasi MFRC522
Servo myservo; // Inisialisasi servo

// Daftar ID kartu yang diizinkan
char* allowedCardIDs[] = {
  "XXXXXXXXXX", // Ganti dengan ID kartu yang diizinkan
  "YYYYYYYYYY"  // Ganti dengan ID kartu lainnya jika diperlukan
  //ID Kartu bisa di dapatkan dengan menjalankan program ID_Read
};

void setup() {
  Serial.begin(9600); // Inisialisasi komunikasi serial
  SPI.begin();        // Inisialisasi SPI bus
  mfrc522.PCD_Init(); // Inisialisasi modul RFID

  myservo.attach(SERVO_PIN); // Menghubungkan servo ke pin 2
  myservo.write(0);          // Menggerakkan servo ke posisi awal (0 derajat)
  delay(1000);
  digitalWrite(LED_PIN, LOW); // Matikan LED (Opsional)
}

void loop() {
  // Cek apakah ada tag RFID yang terdeteksi
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    Serial.println("Tag RFID Terdeteksi!");

    // Baca ID kartu dari tag RFID
    String cardID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cardID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      cardID += String(mfrc522.uid.uidByte[i], HEX);
    }

    // Periksa apakah ID kartu ada dalam daftar yang diizinkan
    if (isCardAllowed(cardID)) {
      Serial.println("Akses Diberikan!");
      myservo.write(90); // Menggerakkan servo ke posisi 90 derajat
      delay(1000);       // Tunggu selama 1 detik
      myservo.write(0);  // Mengembalikan servo ke posisi awal (0 derajat)
    } else {
      Serial.println("Akses Ditolak!");
    }

    mfrc522.PICC_HaltA(); // Menonaktifkan tag RFID saat selesai
  }
}

bool isCardAllowed(String cardID) {
  // Memeriksa apakah ID kartu ada dalam daftar yang diizinkan
  for (int i = 0; i < sizeof(allowedCardIDs) / sizeof(allowedCardIDs[0]); i++) {
    if (cardID == allowedCardIDs[i]) {
      return true;
    }
  }
  return false;
}
