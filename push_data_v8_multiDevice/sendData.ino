//============================================MAIN PROGRAM=======================================

void setup_sendData() {
  
  wifiSetup();
  OTAsetup();
  
  ArduinoOTA.begin();
  
  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  //-------------------------------------setup main program-----------------------------------
  pinMode(pin_maindrive, INPUT);
  pinMode(pin_mainsensor, INPUT);
  attachInterrupt(pin_mainsensor, isr, RISING);
  }

  //-----------------------------------loop program to execute--------------------------------
void loop_sendData() {
  ArduinoOTA.handle();
  
  currentMillis = millis();

//  current_state = digitalRead(pin_mainsensor);
//  if(current_state != prev_state && (currentMillis - state_prevMillis <= state_interval)){
//    buat_debug = currentMillis - state_prevMillis;
//    buat++;
//    state_prevMillis = currentMillis;
//    prev_state = current_state;
//    mainsensor = true;
//  }
//  else if (currentMillis - state_prevMillis > state_interval) {
//    mainsensor = false;
//  } 
  
//  if(prev_motor != digitalRead(pin_maindrive)){
//    maindrive = (!digitalRead(pin_maindrive));
//    prev_motor = digitalRead(pin_maindrive);
//    sendData();
//    previousMillis = millis();
//  }
  
  // Melakukan pengiriman data setiap interval
  if (currentMillis - previousMillis > send_interval) {
    // Simpan waktu terakhir pengiriman data
    i++;
    if(i==6){
      kirim_cpm = cpm;
      cpm=0;
      i=0;
    }
    if (buat) mainsensor = true;
    else if (!buat) mainsensor = false;
    previousMillis = currentMillis;
    maindrive = (!digitalRead(pin_maindrive) && mainsensor);
    kirim_buat += buat; buat=0;
    sendData();
  }
//  else{
//    current_state = digitalRead(pin_mainsensor);
//    if(current_state) apa_on_terus = 0;
//    if(current_state != prev_state){
//      buat++;
//      apa_on_terus = 0;
//      prev_state = current_state;
//    }
//    buat = cpm;
//  }

//  if(millis() - lst > 60000){
//    kirim_cpm = cpm;
//    cpm = 0;
//    lst = millis();
//  }

  if(WiFi.status() != WL_CONNECTED) wifiSetup();
}

//=======================================ADDITIONAL FUNCTION===================================

//--------------------------Setup wifi untuk mode pengiriman data------------------------------
void wifiSetup(){
  Serial.println("Booting");
  //wifi configuration
  WiFi.setHostname(namaota.c_str());
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }
}

//----------------------------------Setup konfigurasi OTA-------------------------------------
void OTAsetup(){
  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  ArduinoOTA.setHostname(namaota.c_str());

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

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

//----------------------------------kirim data ke mysql----------------------------------------
void sendData(){
    // Membuat objek HTTPClient
    HTTPClient http;

    // Membuat URL untuk permintaan POST
    String url = "http://" + String(ip) + ":" + String(server_port) + String(file_name);

    // Membuat payload untuk data yang akan disimpan di database
    String payload = 
    "&mesin=" + String(nama_mesin) +
    "&maindrive=" + String(maindrive) + 

    "&lampu=" + String(!digitalRead(pin_maindrive)) +
    "&count=" + String(kirim_cpm) +
//    "&mainsensor=" + String(mainsensor) +
//    "&waktu=" + String(buat_debug) +
    "&jumlah=" + String(kirim_buat);
    if(kirim_cpm!=0)kirim_cpm=0;
    // Memulai permintaan HTTP POST
    http.begin(url);
    http.addHeader("Content-Type", "application/x-www-form-urlencoded");
    
    // Mengirimkan data sebagai payload
    int httpCode = http.POST(payload);

    // Memproses respons server
    if (httpCode > 0) {
      kirim_buat = 0;
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
