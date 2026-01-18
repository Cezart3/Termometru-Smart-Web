#include <WiFi.h>
#include <WebServer.h>

// WiFi Config
const char* ssid = "iPhoneCezar_14";      
const char* password = "12345678";  

WebServer server(80);

// Variabile globale
float temperatura = 0.0;
float umiditate = 0.0;
String ultimaActualizare = "Așteptare date...";

// Folosim Hardware Serial 2 (Pin 16 pentru RX)
HardwareSerial MySerial(2);

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  // Inițializare Serial pe pinii siguri (RX=16, TX=17)
  MySerial.begin(9600, SERIAL_8N1, 16, 17); 
  
  conecteazaWiFi();
  
  server.on("/", handleRoot);
  server.on("/data", handleData);
  server.on("/style.css", handleCSS);
  
  server.begin();
  Serial.println("\nESP32: Server pornit pe Serial IO16!");
}

void loop() {
  server.handleClient();

  // Verificăm dacă avem date primite de la Mega
  if (MySerial.available()) {
    String linie = MySerial.readStringUntil('\n');
    int virgulaIdx = linie.indexOf(',');
    
    if (virgulaIdx > 0) {
      temperatura = linie.substring(0, virgulaIdx).toFloat();
      umiditate = linie.substring(virgulaIdx + 1).toFloat();
      ultimaActualizare = getTimestamp();
      
      Serial.print("Update Serial: T="); Serial.print(temperatura);
      Serial.print(" H="); Serial.println(umiditate);
    }
  }
}

void conecteazaWiFi() {
  Serial.print("Conectare WiFi la: "); Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500); 
    Serial.print(".");
  }
  Serial.println("\nConectat cu succes!");
  Serial.print("Adresă IP: "); Serial.println(WiFi.localIP());
}

String getTimestamp() {
  unsigned long s = millis() / 1000;
  char buf[20];
  sprintf(buf, "%02lu:%02lu:%02lu", (s/3600)%24, (s/60)%60, s%60);
  return String(buf);
}

// --- HANDLERS WEB SERVER (Design original păstrat) ---

void handleRoot() {
  String html = "<!DOCTYPE html><html lang='ro'><head><meta charset='UTF-8'>";
  html += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>";
  html += "<title>Termometru Inteligent</title>";
  html += "<link rel='stylesheet' href='/style.css'></head><body>";
  html += "<div class='container'><header><h1>🌡️ Termometru Inteligent</h1>";
  html += "<p class='subtitle'>Arduino Mega + ESP32 (Serial)</p></header>";
  html += "<div class='cards-container'>";
  html += "<div class='card temperatura-card'><h2>Temperatură</h2><div class='value' id='temperatura'>--</div><div class='unit'>°C</div></div>";
  html += "<div class='card umiditate-card'><h2>Umiditate</h2><div class='value' id='umiditate'>--</div><div class='unit'>%</div></div></div>";
  html += "<div class='info-card'><h3>📊 Informații</h3>";
  html += "<div class='info-item'><span>Confort:</span><span id='confort'>-</span></div>";
  html += "<div class='info-item'><span>Actualizat:</span><span id='updateTime'>-</span></div></div>";
  html += "<footer><p>Proiect Arduino © 2026</p></footer></div>";
  html += "<script>function update(){fetch('/data').then(r=>r.json()).then(d=>{";
  html += "document.getElementById('temperatura').textContent=d.temperatura.toFixed(1);";
  html += "document.getElementById('umiditate').textContent=d.umiditate.toFixed(1);";
  html += "document.getElementById('updateTime').textContent=d.timestamp;";
  html += "let c='';if(d.temperatura>=20&&d.temperatura<=26&&d.umiditate>=40&&d.umiditate<=60)c='✓ Confortabil';";
  html += "else if(d.temperatura<18)c='❄️ Rece';else if(d.temperatura>28)c='🔥 Cald';else c='~ OK';";
  html += "document.getElementById('confort').textContent=c;});}setInterval(update,2000);update();</script></body></html>";
  server.send(200, "text/html", html);
}

void handleData() {
  String json = "{\"temperatura\":" + String(temperatura, 1) + ",\"umiditate\":" + String(umiditate, 1) + ",\"timestamp\":\"" + ultimaActualizare + "\"}";
  server.send(200, "application/json", json);
}

void handleCSS() {
  String css = "*{margin:0;padding:0;box-sizing:border-box;}body{font-family:Arial,sans-serif;background:linear-gradient(135deg,#667eea 0%,#764ba2 100%);min-height:100vh;padding:20px;}";
  css += ".container{max-width:900px;margin:0 auto;}header{text-align:center;color:white;margin-bottom:30px;}header h1{font-size:2.5em;margin-bottom:10px;}.subtitle{font-size:1.1em;opacity:0.9;}";
  css += ".cards-container{display:grid;grid-template-columns:repeat(auto-fit,minmax(250px,1fr));gap:25px;margin-bottom:30px;}";
  css += ".card{background:white;border-radius:20px;padding:30px;box-shadow:0 10px 30px rgba(0,0,0,0.2);text-align:center;}";
  css += ".temperatura-card{background:linear-gradient(135deg,#ff6b6b 0%,#ee5a6f 100%);color:white;}.umiditate-card{background:linear-gradient(135deg,#4facfe 0%,#00f2fe 100%);color:white;}";
  css += ".value{font-size:4em;font-weight:bold;margin:10px 0;}.unit{font-size:1.5em;opacity:0.8;}";
  css += ".info-card{background:white;border-radius:20px;padding:25px;margin-bottom:20px;}.info-card h3{color:#667eea;margin-bottom:15px;}";
  css += ".info-item{display:flex;justify-content:space-between;padding:12px;background:#f8f9fa;border-radius:8px;margin-bottom:10px;}footer{text-align:center;color:white;padding:20px;}";
  server.send(200, "text/css", css);
}