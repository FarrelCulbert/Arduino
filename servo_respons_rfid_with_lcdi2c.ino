#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

#define SS_PIN 10
#define RST_PIN 9

MFRC522 mfrc522(SS_PIN, RST_PIN); // Buat objek MFRC522 
Servo myServo;   // Buat objek Servo
LiquidCrystal_I2C lcd(0x27, 16, 2);  // Inisialisasi objek LCD I2C

void setup() {
  Serial.begin(9600);   // Inisialisasi komunikasi serial
  SPI.begin();   // Inisialisasi SPI bus
  mfrc522.PCD_Init();  // Inisialisasi modul RFID
  lcd.begin();   // Inisialisasi LCD
  lcd.backlight();  // Hidupkan backlight LCD
  
  myServo.attach(7);   // Menghubungkan servo ke pin 11
  
  lcd.setCursor(0,0);
  lcd.print("Siap membaca RFID");
}

void loop() {
  bool tagDetected = mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial();
  
  // Memeriksa apakah ada kartu RFID
  if (tagDetected) {
    Serial.print("Kartu terdeteksi: ");
    String id = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      id.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
      id.concat(String(mfrc522.uid.uidByte[i], HEX));
      id.toUpperCase();
    }
    Serial.println(id);
    
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("ID Kartu:");
    lcd.setCursor(0,1);
    lcd.print(id);
    
    // Periksa apakah ID kartu adalah C4 11 72 29
    if (id == "C4117229") {
      Serial.println("ID kartu cocok");
      myServo.write(90);   // Menggerakkan servo ke posisi 90 derajat
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Terbuka");
      lcd.setCursor(0,1);
      lcd.print("90 derajat");
    }
    // Periksa apakah ID kartu adalah E3 56 43 1C
    else if (id == "E356431C") {
      Serial.println("ID kartu cocok");
      myServo.write(180);   // Menggerakkan servo ke posisi 180 derajat
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Terbuka");
      lcd.setCursor(0,1);
      lcd.print("180 derajat");
    }
    else {
      Serial.println("ID kartu tidak cocok");
      myServo.write(0);  // Menggerakkan servo ke posisi 0 derajat
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("Tertolak");
      lcd.setCursor(0,1);
      lcd.print("tag salah");
      
    }
    delay(1000);   // Tahan posisi

  }

  if (!tagDetected) {
    myServo.write(0);  // Menggerakkan servo ke posisi 0 derajat
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("membaca RFID");
  }
  
  mfrc522.PICC_HaltA(); // Menutup koneksi kartu RFID
}
