
void setup_setting() {
  
  WiFi.softAP(APssid, APpassword);
  Serial.println("Access Point started");

  // Inisialisasi mDNS
  if (!MDNS.begin("esp32-setup")) {
    Serial.println("Error setting up MDNS responder!");
  } else {
    Serial.println("mDNS responder started");
  }

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    String ssid = readFile("/ssid.txt");
    String password = readFile("/password.txt");
    String ipServer = readFile("/ip.txt");
    String footerInfo = readFile("/footer.txt");
    String reset = readFile("/reset.txt");
    String send = readFile("/send.txt");
    String count = readFile("/count.txt");

    String htmlContent = readFile("/index.html");
    htmlContent.replace("%SSID%", ssid);
    htmlContent.replace("%PASSWORD%", password);
    htmlContent.replace("%IP%", ipServer);
    htmlContent.replace("%FOOTER_INFO%", footerInfo);
    htmlContent.replace("%RESET%", reset);
    htmlContent.replace("%SEND%", send);
    htmlContent.replace("%COUNT%", count);

    request->send(200, "text/html", htmlContent);
  });
  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/style.css", "text/css");
  });
  

  // Handler untuk permintaan POST dari formulir web
  server.on("/get", HTTP_GET, [](AsyncWebServerRequest *request){
    // Mendapatkan nilai yang dikirimkan dari formulir
    writeFile("/ssid.txt", request->arg("ssid"));
    writeFile("/password.txt", request->arg("password"));
    writeFile("/ip.txt", request->arg("ip"));
    writeFile("/reset.txt", request->arg("reset"));
    writeFile("/send.txt", request->arg("send"));
    writeFile("/count.txt", request->arg("count"));

    // Redirect kembali ke halaman utama setelah menyimpan nilai
    request->redirect("/");
  });

  server.begin();
}

void loop_setting() {
  // Tidak ada operasi khusus yang perlu dilakukan di loop
  delay(1000); // Delay pendek untuk mencegah CPU overload
}

//read from spiffs
String readFile(const char* filename) {
  File file = SPIFFS.open(filename, "r");
  if (!file) {
    Serial.println("Failed to open file for reading");
    return String();
  }

  String fileContent = file.readString();
  file.close();
  Serial.println(fileContent);
  return fileContent;
}

//write to spiffs
void writeFile(String path, String content){
  File file = SPIFFS.open(path, "w");
  file.print(content);
  file.close();
}
