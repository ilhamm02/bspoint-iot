#ifndef Network_H_
#define Network_H_

#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
class Network{
private:
  FirebaseData fbdo;
  FirebaseAuth auth;
  FirebaseConfig config;

  void firebaseInit();
  
public:
  Network();
  void initWiFi();
  void firestoreDataUpdate(double longitude, double latitude);
};


#endif