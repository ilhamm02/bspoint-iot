#include <Ticker.h>
#include "Network.h"

Network *network;

void setup()
{
  Serial.begin(9600);

  initNetwork();

  network->firestoreDataUpdate(-6.287389, 77.777079);
}

void loop() {
}

void initNetwork(){
  network = new Network();
  network->initWiFi();
}

// #include <ESP8266WiFi.h>
// // Network SSID
// const char* ssid = "R-11";
// const char* password = "RepublicSukatani11";
 
// void setup() {
  
//   Serial.begin(9600);
//   delay(10);
 
//   // Connect WiFi
//   Serial.println();
//   Serial.println();
//   Serial.print("Connecting to ");
//   Serial.println(ssid);
//   WiFi.hostname("Name");
//   WiFi.begin(ssid, password);
 
//   while (WiFi.status() != WL_CONNECTED) {
//     delay(500);
//     Serial.print(".");
//   }
//   Serial.println("");
//   Serial.println("WiFi connected");
 
//   // Print the IP address
//   Serial.print("IP address: ");
//   Serial.print(WiFi.localIP());
// }
 
// void loop() {
  
// //Add your project's loop code here
// }
