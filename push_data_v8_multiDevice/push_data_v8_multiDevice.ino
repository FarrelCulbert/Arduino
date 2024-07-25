
const char *nama_mesin = "AS07";

//general library
#include <WiFi.h>
#include <ESPmDNS.h>
#include <SPIFFS.h>
//send data mode library
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>
//setting mode library
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
int buat_debug, apa_on_terus=1, prev_motor, kirim_cpm, kirim_buat, i;
volatile int cpm, buat;
unsigned long lst;
#define pin_program 22

bool stateMode = false;
long reset_interval;

//==============================================================================================
//--------------------------------- Push Data to mysql, OTA ------------------------------------
//==============================================================================================

//=========untuk mode client, mengirimkan data ke mysql dan upload program via OTA==============
const int server_port = 80;  // Port HTTP server MySQL lokal
const char *file_name = "/project/push_data.php";


//=====================================Global variabel dan fungsi===============================
// konfigurasi delay pengiriman data
unsigned long previousMillis = 0;
long send_interval;  // Interval waktu antara pengiriman data (dalam milidetik)

//deklarasi variabel
bool maindrive, mainsensor;
bool current_state, prev_state=0;
long state_interval;
unsigned long state_prevMillis = 0;

unsigned long currentMillis;

#define pin_maindrive 17
#define pin_mainsensor 5

//==============================================================================================
//---------------------------------- Setting Mode, DNS Server ----------------------------------
//==============================================================================================

//konfigurasi access point esp32
const char* APssid = "ESP32-Setup-AP";
const char* APpassword = "esp32password";

String ssid, password, ip, namaota;

AsyncWebServer server(80);

//==============================================================================================
//---------------------------------- Program yang dijalankan -----------------------------------
//==============================================================================================

void IRAM_ATTR isr() {
  cpm++; buat++;
}

void setup() {
  Serial.begin(115200);
  namaota = String(nama_mesin)+"_produk";
  pinMode(pin_program, INPUT_PULLUP);
  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  reset_interval = atol(readFile("/reset.txt").c_str());

  // Check initial state of GPIO 2
  if (digitalRead(pin_program) == HIGH) {
    // ssid = readFile("/ssid.txt");
    ssid = "PENS";
    // password = readFile("/password.txt");
    password = "penspens";
    // ip = readFile("/ip.txt");
    ip = "192.168.0.136";
    prev_motor = digitalRead(pin_maindrive);
    // send_interval = atol(readFile("/send.txt").c_str());
    send_interval = 10000;
    state_interval = atol(readFile("/count.txt").c_str());

    setup_sendData(); // If HIGH, execute setup A
    stateMode = true;
    Serial.println("send");
  } else {
    setup_setting(); // If LOW, execute setup B
    stateMode = false;
    Serial.println("setting");
  }
}

void loop() {
  if (stateMode) {
    loop_sendData(); // Execute loop A if stateA is true
//    Serial.println("loopsend");
  } else {
    loop_setting(); // Otherwise, execute loop B
    // Serial.println("loopsetting");
  }

  if (digitalRead(pin_program) != stateMode){
    delay(reset_interval);
    if (digitalRead(pin_program) != stateMode){
     Serial.println("Change Mode, restarting");
     ESP.restart();
    }
  }
}
