// FOR DEBUGING ONLY
// ```````````````````````
// NOTE : Pins 0 and 1 also used for modbus communication ==> enable (DEBUG) only when needed
#define DEBUG 1 // set 1 to enable debuging
#if DEBUG
  #define PRINT(s)    { Serial.print(s); }
  #define PRINTLN(s)  { Serial.println(s); }
#else
  #define PRINT(s)
  #define PRINTLN(s)
#endif

// fn get size of int array
// ````````````````````````````
#define ARRSIZE(x)   (sizeof(x) / sizeof(x[0]))

const int server_port = 80;  // Port HTTP server MySQL lokal
const char *file_name = "/PMOnline/php/insert_log.php";

float VAB, VBC, VCA, VLL, VAN, VBN, VCN, VLN;
float CA, CB, CC, CN, CG, CR;
float APT, RPT, APP, PFT, FRQ;
float Power_Factor(){
  if(PFT > 1.0){
    return(2-PFT);
  }
  else if(PFT < -1.0){
    return(-2.0-PFT);
  }
  else if(abs(PFT)==1.0){
    return(PFT);
  }
  else
    return(PFT);
}

uint64_t ENN;

#define detik 1841
#define vab 3020
#define vbc (vab+2)
#define vca (vbc+2)
#define vll (vca+2)
#define van (vll+2)
#define vbn (van+2)
#define vcn (vbn+2)
#define vln (vcn+4)

#define ca 3000
#define cB 3002
#define cC 3004
#define cN 3006
#define cg 3008
#define cr 3010

#define apt 3060    //active (real) power total kW
#define rpt 3068    //reactive power total kVAR
#define appr 3076    //apparent power total kVA
#define pft 3084
#define frq 3110
#define enn 3204    //Active Energy Delivered (Into Load) wh

#define pin_send 19
#define pin_succ 18
#define pin_fail 5
#define modePin 4

String hostName = "AS07";
String deviceName = String(hostName) + "_PM";

String ssid, password, ipServer, send, footerInfo, DEVICE_ID;
unsigned long send_interval;
bool sendMode;
String ssidAP = "esp32-PMOnline Setup";
String passwordAP = "12345678";

#include <WiFi.h>
#include <ESPmDNS.h>
#include <DNSServer.h>
#include <SPIFFS.h>

#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <HTTPClient.h>

#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>

const byte DNS_PORT = 53;
DNSServer dnsServer;
// AsyncWebServer asyncServer(81);
WiFiServer server(80);
AsyncWebServer asyncServer(81);

String Content = R"(
  <html>
    <head>
      <style>
        /* CSS untuk mengatur posisi tengah */
        .center {
          position: absolute;
          top: 50%;
          left: 50%;
          transform: translate(-50%, -50%);
        }
      </style>
    </head>
    <body>
      <div class="center">
        <h3>Redirecting...</h3>
      </div>
      <meta http-equiv="refresh" content="0;URL='http://%IP%:81'" />
    </body>
  </html>
)";

#include "ronnModbus.h"
#include "ronnEthernet.h"

unsigned long timer;

void setup(){
  Serial.begin(115200);
  pinMode(pin_send, OUTPUT);
  pinMode(pin_succ, OUTPUT);
  pinMode(pin_fail, OUTPUT);
  pinMode(modePin, INPUT_PULLUP);

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  DEVICE_ID = readFile("/deviceID.txt");
  ssid = readFile("/ssid.txt");
  password = readFile("/password.txt");
  ipServer = readFile("/ip.txt");
  send_interval = atol(readFile("/send.txt").c_str());

  WIFIinit(digitalRead(modePin));
  modbusInit();
  
  OTAinit();
  ArduinoOTA.begin();

  delay(1000);
}

void loop(){
  //handel OTA request
  ArduinoOTA.handle();

  //if sendMode = true, then device send modbus data to database. this state changed in WIFIinit()
  if(sendMode){
    connection_status = modbus_update(packets);
    // Send every 5000ms
    if(millis() - timer >= send_interval || timer > millis()){
      timer = millis();
      modbusData();   // update data modbus convert to float
      sendData();     // sendToServer(); // send to server over HTTP request (POST)
    }
    if(WiFi.status() != WL_CONNECTED) WIFIinit(true);
  }
  //sendMode = false, the device will provide access point network then a captive portal will be open to set several parameter on device
  else{
    dnsServer.processNextRequest();
    WiFiClient client = server.available();   // listen for incoming clients

    if (client) {
      String currentLine = "";
      while (client.connected()) {
        if (client.available()) {
          char c = client.read();
          if (c == '\n') {
            if (currentLine.length() == 0) {
              client.println("HTTP/1.1 200 OK");
              client.println("Content-type:text/html");
              client.println();
              IPAddress aipi = WiFi.softAPIP();
              Content.replace("%IP%", aipi.toString());
              client.print(Content);
              break;
            } else {
              currentLine = "";
            }
          } else if (c != '\r') {
            currentLine += c;
          }
        }
      }
      client.stop();
    }
  }
  //check for change in the state of the modePin, if there is a change then wait 3s and restart device
  if(digitalRead(modePin) != sendMode){
    delay(3000);
    if(digitalRead(modePin) != sendMode){
      Serial.println("Change Mode, restarting");
      ESP.restart();
    }
  }
}
