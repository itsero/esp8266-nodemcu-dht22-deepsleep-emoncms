//simple sketch reading from DHT22 and posting up to emoncms.org using the ESP8266 wifi module.
// This file is part of OpenEnergyMonitor project.
// This sketch is expand with the folloing Option



#include <ESP8266WiFi.h>
#include "DHT.h"
#define DHTPIN 4   


#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321


DHT dht(DHTPIN, DHTTYPE);


const char* ssid     = "WIFI-NAME"; //WIFI NAME
const char* password = "WIFI-PASSWORD"; //WIFI PASSWORD

const char* host = "EMONCMS-URL";


void setup() {
  Serial.begin(9600);//Baud rate
  delay(10);

  

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  
  IPAddress ip(192, 168, 1, 220);         // Static IP of NodeMCU
  IPAddress gateway(192, 168, 1, 1);      // Internet Gateway
  IPAddress subnet(255, 255, 255, 0);     // Subnet
  IPAddress dns(192, 168, 1, 1);          // DNS-Server Address
  WiFi.config(ip, dns, gateway, subnet);

  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("");
  Serial.println("WiFi connected");  
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  
  dht.begin();

  int value = 0;

  delay(2000);
  ++value;
//  DHT22_ERROR_t errorCode;


  
  Serial.print("Requesting data...");
//  errorCode = DHT.readData();
//  switch(errorCode)
  {
   

  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  
  float f = dht.readTemperature(true);

  // Check if any reads failed and exit early (to try again).
  if (isnan(h) || isnan(t) || isnan(f)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  Serial.print("connecting to ");
  Serial.println(host);
  
  // Use WiFiClient class to create TCP connections
  WiFiClient client;
  const int httpPort = 80;
  if (!client.connect(host, httpPort)) {
    Serial.println("connection failed");
    return;
  }
  
  // Compute heat index in Fahrenheit (the default)
  float hif = dht.computeHeatIndex(f, h);
  // Compute heat index in Celsius (isFahreheit = false)
  float hic = dht.computeHeatIndex(t, h, false);
  Serial.print("\n");
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\n");
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" *C\n ");

  // We now create a URI for the request
  String url = "/input/post.json?csv="+String(h)+","+String(t)+"&apikey=XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"; //Enter api key here

  Serial.print("Requesting URL: ");
  Serial.println(url);
  
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n" +
               "Host: " + host + "\r\n" + 
               "Connection: close\r\n\r\n");
  unsigned long timeout = millis();
  while (client.available() == 0) {
    if (millis() - timeout > 5000) {
      Serial.println(">>> Client Timeout !");
      client.stop();
      return;
    }
  }
  
  // Read all the lines of the reply from server and print them to Serial
  while(client.available()){
    String line = client.readStringUntil('\r');
    Serial.print(line);
  }
  
  Serial.println();
  Serial.println("closing connection");

  Serial.println("Going into deep sleep for 300 seconds");
  ESP.deepSleep(300*1000000); // deep sleep for 300 seconds (30 * 1 000 000)
  }

}

void loop() {

}
