#include <SPI.h>
#include <MFRC522.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Konfigurasi RFID
#define SS_PIN D4
#define RST_PIN D3
MFRC522 mfrc522(SS_PIN, RST_PIN);

// Konfigurasi Wi-Fi
char auth[] = "K_WWakXZ1MezAS6u-1hIl9R5PMDZS535";
char ssid[] = "PencariKontrakan";
char pass[] = "kontrakan_itts";

void setup() {
  Serial.begin(9600);

  // Inisialisasi RFID
  SPI.begin();
  mfrc522.PCD_Init();

  // Inisialisasi Wi-Fi dan Blynk
  WiFi.begin(ssid, pass);
  Blynk.begin(auth, ssid, pass);

  // Tampilkan status koneksi Wi-Fi pada Serial Monitor
  Serial.print("Connecting to ");
  Serial.print(ssid);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
}

void loop() {
  // Periksa apakah tag RFID terdeteksi
  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    // Baca ID tag RFID
    String tagID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      tagID.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : ""));
      tagID.concat(String(mfrc522.uid.uidByte[i], HEX));
    }

    // Cek apakah ID tag dikenali
    if (tagID == "c4117229") {
      Serial.println("Satpam 1");

      // Kirim pesan "Satpam 1" ke terminal Blynk pada pin V1
      Blynk.virtualWrite(V1, "Satpam 1\n");
    } else if (tagID == "e356431c") {
      Serial.println("Satpam 2");

      // Kirim pesan "Satpam 2" ke terminal Blynk pada pin V1
      Blynk.virtualWrite(V1, "Satpam 2\n");
    } else {
      Serial.println("Tidak dikenali");

      // Kirim pesan "Tidak dikenali" ke terminal Blynk pada pin V1
      Blynk.virtualWrite(V1, "Tidak dikenali\n");
    }

    // Tunggu hingga tag RFID dihapus dari koneksi
    mfrc522.PICC_HaltA();
  }
  delay(1000);
  Blynk.run();
}
