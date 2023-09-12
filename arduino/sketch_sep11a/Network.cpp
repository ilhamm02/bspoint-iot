#include "Network.h"
#include "NTPClient.h"
#include "WiFiUdp.h"
const char* WIFI_SSID = "0123";
const char* WIFI_PASSWORD = "00000000";

#define API_KEY "AIzaSyBVBXdwmnNYiv4wmzsAAukSlq6o6VaNszM"
#define FIREBASE_PROJECT_ID "bspoint"
#define USER_EMAIL "ilhamaulana24@gmail.com"
#define USER_PASSWORD "BsPointFirebase123"

String idBus = "IAJYrLix9U4MiGssZmxF";
String idList = "0v9ABoP0PukSrI9X9mbp";
String busNumber = "14";

static Network* instance = NULL;

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP);

String timestamp;

Network::Network() {
  instance = this;
}

void Network::initWiFi() {
  WiFi.disconnect();
  WiFi.hostname("BSPoint");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Checking if WiFi Connected
  Serial.print("Connecting .");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.print("Connected! Local IP: ");
  Serial.println(WiFi.localIP());

  Serial.print("Intializing NTP. ");
  timeClient.begin();

  Serial.print("Initializing Firebase. ");
  instance->firebaseInit();

  while (!Firebase.ready()) {
    Serial.print(".");
  }

  Serial.print("Connected to Firebase! ");
}

void Network::firebaseInit() {
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
}

void Network::firestoreUpdatePosition(double longitude, double latitude) {
  while (!timeClient.update()) {
    timeClient.forceUpdate();
  }

  while (WiFi.status() != WL_CONNECTED || !Firebase.ready()) {
    Serial.print(".");
    instance->initWiFi();

    delay(1000);
  }

  if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {
    String documentPath = "buses/" + idBus;

    timestamp = timeClient.getFormattedDate();
    Serial.print(timestamp);

    FirebaseJson content;

    content.set("fields/busNumber/stringValue", busNumber);
    content.set("fields/position/geoPointValue/latitude", latitude);
    content.set("fields/position/geoPointValue/longitude", longitude);
    content.set("fields/lastUpdate/timestampValue", timestamp);

    if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "busNumber, position, lastUpdate")) {
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      return;
    } else {
      Serial.println(fbdo.errorReason());
    }
  }
}

void Network::firestoreUpdateCapacity(bool increment) {
  while (WiFi.status() != WL_CONNECTED || !Firebase.ready()) {
    Serial.print(".");
    instance->initWiFi();

    delay(1000);
  }

  if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {
    String documentPath = "listbust/" + idList;

    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), "")) {
      FirebaseJson payload;
      payload.setJsonData(fbdo.payload().c_str());

      FirebaseJsonData jsonData;
      payload.get(jsonData, "fields/capacity/integerValue", true);

      int prevCapacity = jsonData.intValue;

      FirebaseJson content;

      if (!increment && prevCapacity > 0) content.set("fields/capacity/integerValue", prevCapacity - 1);
      else if (increment) content.set("fields/capacity/integerValue", prevCapacity + 1);
      else content.set("fields/capacity/integerValue", prevCapacity);

      if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "capacity")) {
        Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
        return;
      } else {
        Serial.println(fbdo.errorReason());
      }
    } else {
      Serial.println(fbdo.errorReason());
    }
  }
}