//------------------------------setup wifi and change mode----------------------
//read spiffs file
String readFile(const char* filename) {
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return String();
  }

  String fileContent = file.readString();
  file.close();
  // Serial.println(fileContent);
  return fileContent;
}

//write to spiffs
void writeFile(String path, String content){
  File file = SPIFFS.open(path, "w");
  file.print(content);
  file.close();
}

void WIFIinit(bool state){
  //if state true, wifi set to station mode
  if(state){
    sendMode = true;
    WiFi.setHostname(deviceName.c_str());
  konek:
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password);
    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < 20000) {
      Serial.print(".");
      delay(100);  // Tunggu setengah detik
    }
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("Connected!");
      Serial.println(WiFi.localIP());
      //WiFi.setAutoConnect(1);
      WiFi.setAutoReconnect(1);
      WiFi.persistent(1);
    }
    else {
      goto konek;
    }
  }
  //else, wifi set to ap mode, mdns server seted
  else{
    sendMode = false;
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssidAP, passwordAP);
    Serial.println("Access Point started");

    IPAddress aipi = WiFi.softAPIP();
    dnsServer.start(DNS_PORT, "*", aipi);

    server.begin();

    asyncServer.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      String DEVICE_ID = readFile("/deviceID.txt");
      String ssid = readFile("/ssid.txt");
      String password = readFile("/password.txt");
      String ipServer = readFile("/ip.txt");
      String footerInfo = readFile("/footer.txt");
      String send = readFile("/send.txt");

      String htmlContent = readFile("/index.html");
      htmlContent.replace("%DEVICE_ID%", DEVICE_ID);
      htmlContent.replace("%SSID%", ssid);
      htmlContent.replace("%PASSWORD%", password);
      htmlContent.replace("%IP%", ipServer);
      htmlContent.replace("%FOOTER_INFO%", footerInfo);
      htmlContent.replace("%SEND%", send);

      request->send(200, "text/html", htmlContent);
    });
    asyncServer.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(SPIFFS, "/style.css", "text/css");
    });
    

    // Handler untuk permintaan POST dari formulir web
    asyncServer.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
      // Mendapatkan nilai yang dikirimkan dari formulir
      writeFile("/deviceID.txt", request->arg("deviceID"));
      writeFile("/ssid.txt", request->arg("ssid"));
      writeFile("/password.txt", request->arg("password"));
      writeFile("/ip.txt", request->arg("ip"));
      writeFile("/send.txt", request->arg("send"));

      // Redirect kembali ke halaman utama setelah menyimpan nilai
      request->redirect("/");
    });

    asyncServer.begin();
  }
}

void OTAinit(){
  ArduinoOTA.setHostname(deviceName.c_str());
  ArduinoOTA
    .onStart([]() {
      String type;
      if (ArduinoOTA.getCommand() == U_FLASH)
        type = "sketch";
      else // U_SPIFFS

        type = "filesystem";

      // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
      Serial.println("Start updating " + type);
    })
    .onEnd([]() {
      Serial.println("\nEnd");
    })
    .onProgress([](unsigned int progress, unsigned int total) {
      Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
    })
    .onError([](ota_error_t error) {
      Serial.printf("Error[%u]: ", error);
      if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
      else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
      else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
      else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
      else if (error == OTA_END_ERROR) Serial.println("End Failed");
    });
}

void sendData(){
    // Membuat objek HTTPClient
    HTTPClient http;

    // Membuat URL untuk permintaan POST
    String url = "http://" + String(ipServer) + ":" + String(server_port) + String(file_name);

    // Membuat payload untuk data yang akan disimpan di database
    String payload = 
    "&id_device=" + String(DEVICE_ID) +

    "&ir=" + String(CA) +
    "&is=" + String(CB) +
    "&it=" + String(CC) +
    "&imean=" + String(CR) +

    "&vrs=" + String(VAB) +
    "&vst=" + String(VBC) +
    "&vtr=" + String(VCA) +
    "&vrn=" + String(VAN) +
    "&vsn=" + String(VBN) +
    "&vtn=" + String(VCN) +
    "&vmean=" + String(VLL) +

    "&pw=" + String(APT) +
    "&pvar=" + String(RPT) +
    "&pva=" + String(APP) +
    "&pf=" + String(Power_Factor()) +
    "&freq=" + String(FRQ) +
    "&day=" + String("0") +

    // "&pr=" + String() +
    // "&ps=" + String() +
    // "&pt=" + String() +

    "&ener=" + String(ENN);
    // Memulai permintaan HTTP POST
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");

    // Mengirimkan data sebagai payload
    int httpCode = http.POST(payload);

    // Memproses respons server
    if (httpCode > 0) {
      String response = http.getString();
      Serial.println("Server response: " + response);
    } 
    else {
      if(WiFi.status() != WL_CONNECTED) Serial.println("Wifi disconnected");
      else {
        Serial.println("HTTP request failed");
      }
    }

    // Menutup koneksi HTTP
    http.end();
}