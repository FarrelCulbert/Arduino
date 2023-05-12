#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

#define SS_PIN D4
#define RST_PIN D3

MFRC522 rfid(SS_PIN, RST_PIN);

LiquidCrystal_I2C lcd(0x27, 16, 2);

const int buzzerPin = D8;

char auth[] = "K_WWakXZ1MezAS6u-1hIl9R5PMDZS535";
char ssid[] = "tes"; 
char pass[] = "12345678";

void setup() {
  Serial.begin(9600);
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Ready!");

  pinMode(buzzerPin, OUTPUT);

  SPI.begin();
  rfid.PCD_Init();

  Blynk.begin(auth, ssid, pass);

  Serial.println("Scan an RFID tag");
}

void loop() {
  Blynk.run();

  if (!rfid.PICC_IsNewCardPresent() || !rfid.PICC_ReadCardSerial()) {
    delay(50);
    return;
  }

  String tagID = "";
  for (byte i = 0; i < rfid.uid.size; i++) {
    tagID.concat(String(rfid.uid.uidByte[i] < 0x10 ? "0" : ""));
    tagID.concat(String(rfid.uid.uidByte[i], HEX));
  }

  lcd.clear();
  lcd.setCursor(0, 0);

  if (tagID == "c4117229") {
    lcd.print("Satpam 1");
    tone(buzzerPin, 1000, 1000);
    delay(1000);
    noTone(buzzerPin);
    Serial.println("Satpam 1");
    Blynk.virtualWrite(V1, "Satpam 1\n");
  } else if (tagID == "e356431c") {
    lcd.print("Satpam 2");
    tone(buzzerPin, 1000, 1000);
    delay(1000);
    noTone(buzzerPin);
    Serial.println("Satpam 2");
    Blynk.virtualWrite(V1, "Satpam 2\n");
  } else {
    lcd.print("Tidak dikenali");
    Serial.println("Tidak dikenali");
    Blynk.virtualWrite(V1, "Tidak dikenali\n");
  }

  delay(1000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("tempelkan  kartu");
}
