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
  void firestoreUpdatePosition(double longitude, double latitude);
  void firestoreUpdateCapacity(String idBus, bool increment);
  void firestoreUpdatePassenger(String tagId);
};