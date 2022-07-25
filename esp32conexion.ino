#include <HTTPClient.h>
#include <WiFi.h>
#include <DHT.h>
#include <LiquidCrystal.h>

#define DHTPIN 13
#define DHTTYPE DHT11

#define bomba 14

LiquidCrystal lcd(22, 23, 5, 18, 19, 21);
DHT dht(DHTPIN, DHTTYPE);

const char* ssid = "Emi";
const char* password = "Torres203453";

float temp, humF;
int waterLvl, hum, humFN;
String regado;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  digitalWrite(bomba, LOW);
  pinMode(35, INPUT);
  pinMode(25, OUTPUT);
  pinMode(33, INPUT);
  pinMode(bomba, OUTPUT);
  lcd.begin(16, 2);
  dht.begin();

  WiFi.begin(ssid, password);

  Serial.print("Conectando...");
  lcd.setCursor(0, 0);
  lcd.println("Iniciando...");
  while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }

  Serial.print("Conectado con éxito, la IP es: ");
  Serial.println(WiFi.localIP());
  Serial.println("La dirección MAC es: ");
  Serial.println(WiFi.macAddress());
}

float get_distance(){
  int duration, distance;
  digitalWrite(25, HIGH);
  delayMicroseconds(1000);
  digitalWrite(25, LOW);
  duration = pulseIn(33, HIGH);
  distance =(duration/2)/29 ; //cm
  return distance;
}

void loop() {
  // put your main code here, to run repeatedly:     
      humF = analogRead(35);
      int distance = get_distance();
      temp = dht.readTemperature();
      hum = dht.readHumidity();

      
      humFN = map(humF, 4095, 0, 0, 100);
      if (humFN > 100) {
          humFN = 100;
        }
        
      waterLvl = map(distance, 13, 4, 0, 100);
      
      if (waterLvl > 100) {
           waterLvl = 100;
        }

      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("T:" + String(temp) + "C");
      lcd.setCursor(0, 1);
      lcd.print("H:"+ String(hum) + "%");
      lcd.setCursor(9, 0);
      lcd.print("HS:"+ String(humFN) + "%");
      lcd.setCursor(9, 1);
      lcd.print("W:"+ String(waterLvl) + "%");
      
      if (humFN <= 5) {
          digitalWrite(bomba, HIGH);
          delay(4000);
          digitalWrite(bomba, LOW);
          regado = "si";
      } else { regado = "no"; }

  if (WiFi.status() == WL_CONNECTED) {
      HTTPClient http;

      String datos = "temp=" + (String)temp + "&hum=" + (String)hum + "&humF=" + (String)humFN + "&waterLvl=" + (String)waterLvl + "&regado=" + regado;

      http.begin("http://172.20.10.5/datos.php");
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      
      int codeResponse = http.POST(datos);
      String respuesta = http.getString();
      
      if (codeResponse > 0) {
          Serial.println("Código HTTP = " + String(codeResponse));

          if (codeResponse == 200) {
              Serial.println("El servidor respondió!");
              Serial.println(respuesta);
            }
        } else {
            Serial.print("Error enviando POST, código: ");
            Serial.println(codeResponse);
          }

          http.end();
        
    } else {
        Serial.println("Error en la conexión WiFi");
      }

      delay(5000);

}
