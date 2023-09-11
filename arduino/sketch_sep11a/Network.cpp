#include "Network.h"
const char* WIFI_SSID = "0123";
const char* WIFI_PASSWORD = "00000000";

#define API_KEY "AIzaSyBVBXdwmnNYiv4wmzsAAukSlq6o6VaNszM"
#define FIREBASE_PROJECT_ID "bspoint"
#define USER_EMAIL "ilhamaulana24@gmail.com"
#define USER_PASSWORD "BsPointFirebase123"

String id = "IAJYrLix9U4MiGssZmxF";
String number = "11";

static Network* instance = NULL;

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

  Serial.print("Initializing Firebase.");
  instance->firebaseInit();

  while (!Firebase.ready()) {
    Serial.print(".");
  }

  Serial.print("Connected to Firebase!");
}

void Network::firebaseInit() {
  config.api_key = API_KEY;

  auth.user.email = USER_EMAIL;
  auth.user.password = USER_PASSWORD;

  Firebase.begin(&config, &auth);
}

void Network::firestoreDataUpdate(double longitude, double latitude) {
  while (WiFi.status() != WL_CONNECTED || !Firebase.ready()) {
    Serial.print(".");
    instance->initWiFi();

    delay(1000);
  }

  if (WiFi.status() == WL_CONNECTED && Firebase.ready()) {
    Serial.print("Updating bus position");

    String documentPath = "buses/"+id;

    Serial.print(documentPath);

    FirebaseJson content;

    content.set("fields/position/geoPointValue/latitude", latitude);
    content.set("fields/position/geoPointValue/longitude", longitude);

    if (Firebase.Firestore.patchDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw(), "position")) {
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      return;
    } else {
      Serial.println(fbdo.errorReason());
    }

    if (Firebase.Firestore.createDocument(&fbdo, FIREBASE_PROJECT_ID, "", documentPath.c_str(), content.raw())) {
      Serial.printf("ok\n%s\n\n", fbdo.payload().c_str());
      return;
    } else {
      Serial.println(fbdo.errorReason());
    }
  }
}