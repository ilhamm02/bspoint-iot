#include "Network.h"
#include "SoftwareSerial.h"
#include "TinyGPS++.h"
#include "TinyGPSPlus.h"
#include "SoftwareSerial.h"

Network *network;

static const int RXPin = 13, TXPin = 12;
static const uint32_t GPSBaud = 9600;

TinyGPSPlus gps;

SoftwareSerial ss(RXPin, TXPin);

void setup() {
  Serial.begin(9600);
  ss.begin(GPSBaud);

  initNetwork();
}

void loop() {
  while (ss.available() > 0) {
    gps.encode(ss.read());
    if (gps.location.isUpdated()) {
      network->firestoreUpdatePosition(gps.location.lng(), gps.location.lat());
    }
  }
}

void initNetwork(){
  network = new Network();
  network->initWiFi();
}