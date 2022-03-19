 #include <WiFi.h>

#include <WiFiMulti.h>

#include "DHT.h"

#define DHTPIN 4    
#define DHTTYPE DHT22  

DHT dht(DHTPIN, DHTTYPE);
 
WiFiMulti WiFiMulti;

const char* ssid     = "Saad"; // Your SSID (Name of your WiFi)
const char* password = "google123"; //Your Wifi password

const char* host = "api.thingspeak.com";
String api_key = "WTF95XO5BNF0U3A2"; // Your API Key provied by thingspeak

void setup(){
  Serial.begin(9600);
   Serial.println(F("DHTxx test!"));
  dht.begin();
  Connect_to_Wifi();
}

void loop(){

    float h = dht.readHumidity();
  float t = dht.readTemperature();
  float f = dht.readTemperature(true);

  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

   Serial.print(F("Humidity: "));
  Serial.print(h);
  Serial.print(F("%  Temperature: "));
  Serial.print(t);
  Serial.println(F("°C "));

  
 // call function to send data to Thingspeak
  Send_Data(t,h);

  delay(5000);
 

}

void Connect_to_Wifi()
{

  // We start by connecting to a WiFi network
  WiFiMulti.addAP(ssid, password);

  Serial.println();
  Serial.println();
  Serial.print("Wait for WiFi... ");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

}

void Send_Data(float t, float h)
{

  // map the moist to 0 and 100% for a nice overview in thingspeak.
  Serial.println("");
  Serial.println("Prepare to send data");

  // Use WiFiClient class to create TCP connections
  WiFiClient client;

  const int httpPort = 80;

  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  else
  {
    String data_to_send = api_key;
    data_to_send += "&field1=";
    data_to_send += String(t);
    data_to_send += "&field2=";
    data_to_send += String(h);
    // data_to_send += "&field3=";
    // data_to_send += String(pressure);
    data_to_send += "\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + api_key + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(data_to_send.length());
    client.print("\n\n");
    client.print(data_to_send);

    delay(1000);
  }

  client.stop();

}
