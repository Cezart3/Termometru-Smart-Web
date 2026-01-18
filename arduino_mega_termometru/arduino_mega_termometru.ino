#include <DHT.h>

#define DHTPIN 2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

void setup() {
  Serial.begin(115200);   
  Serial3.begin(9600);    
  dht.begin();
  Serial.println("MEGA: Sistem Serial pornit pe Pin 14 (TX3)");
}

void loop() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (!isnan(h) && !isnan(t)) {
    
    Serial3.print(t, 1);
    Serial3.print(",");
    Serial3.println(h, 1);
    
    Serial.print("Trimis catre ESP32: T="); Serial.print(t, 1);
    Serial.print(" H="); Serial.println(h, 1);
  } else {
    Serial.println("ERROR: Senzor deconectat");
  }
  delay(2000); 
}