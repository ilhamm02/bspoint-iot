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

    content.set("fields/position/geoPointValue/latitude", latitude);
    content.set("fields/position/geoPointValue/longitude", longitude);
    content.set("fields/lastUpdate/timestampValue", timestamp);

    if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "position, lastUpdate")) {
      Serial.println("Updated bus position");
      return;
    } else {
      Serial.println(fbdo.errorReason());
    }
  }
}

void Network::firestoreUpdateCapacity(String idBus, bool increment) {
  while (WiFi.status() != WL_CONNECTED && !Firebase.ready()) {
    Serial.print(".");
    instance->initWiFi();

    delay(1000);
  }

  if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {
    String documentPath = "buses/" + idBus;

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
        Serial.println("Updated bus capacity");
        return;
      } else {
        Serial.println(fbdo.errorReason());
      }
    } else {
      Serial.println(fbdo.errorReason());
    }
  }
}

void Network::firestoreUpdatePassenger(String tagId) {
  while (WiFi.status() != WL_CONNECTED && !Firebase.ready()) {
    Serial.print(".");
    instance->initWiFi();

    delay(1000);
  }

  if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {
    String documentPath = "passengers/" + tagId;

    if (Firebase.Firestore.getDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), "")) {
      FirebaseJson payload;
      payload.setJsonData(fbdo.payload().c_str());

      FirebaseJsonData jsonData;
      payload.get(jsonData, "fields/inBusNumber/stringValue", true);

      String currentBus = jsonData.stringValue;

      FirebaseJson content;

      if (currentBus.length() > 0) {
        if (currentBus == idBus) {
          instance->firestoreUpdateCapacity(idBus, false);
          currentBus = "";
        } else {
          instance->firestoreUpdateCapacity(currentBus, false);
          currentBus = idBus;
          instance->firestoreUpdateCapacity(currentBus, true);
        }
      } else {
        instance->firestoreUpdateCapacity(idBus, true);
        currentBus = idBus;
      }

      content.set("fields/inBusNumber/stringValue", currentBus);

      if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "inBusNumber")) {
        Serial.println("Updated passenger");
        return;
      } else {
        Serial.println(fbdo.errorReason());
      }
    } else {
      Serial.println(fbdo.errorReason());
    }
  }
}