#include "Network.h"
#include "TinyGPS++.h"
#include "TinyGPSPlus.h"
#include "SoftwareSerial.h"
#include "PN532_SWHSU.h"
#include "PN532.h"

#define LED_GPS 14
#define LED_NFC 2

Network* network;

SoftwareSerial GPSSerial(12, 13);  // RX, TX
SoftwareSerial NFCSerial(5, 4);    //RX, TX

TinyGPSPlus gps;

PN532_SWHSU pn532swhsu(NFCSerial);
PN532 nfc(pn532swhsu);
String tagId = "None", dispTag = "None";
byte nuidPICC[4];

void setup(void) {
  Serial.begin(9600);
  GPSSerial.begin(9600);


  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't Find PN53x Module");
    while (1)
      ;
  }
  nfc.SAMConfig();

  initNetwork();

  pinMode(LED_NFC, OUTPUT);
  pinMode(LED_GPS, OUTPUT);
}

void loop() {
  if (GPSSerial.available() > 0) {
    gps.encode(GPSSerial.read());
    // Serial.write(GPSSerial.read());
    // Serial.println("CHECKING");
    if (gps.location.isUpdated()) {
      readNFC();
      Serial.println("UPDATING LOCATION");
      digitalWrite(LED_GPS, HIGH);
      network->firestoreUpdatePosition(gps.location.lng(), gps.location.lat());
      digitalWrite(LED_GPS, LOW);
    } else {
      // Serial.println("NOT UPDATED");
      digitalWrite(LED_GPS, LOW);
    }
  }
}

void initNetwork() {
  network = new Network();
  network->initWiFi();
}

void readNFC() {
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  if (nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength)) {
    for (uint8_t i = 0; i < uidLength; i++) {
      nuidPICC[i] = uid[i];
    }
    Serial.println();
    tagId = tagToString(nuidPICC);
    dispTag = tagId;
    Serial.print(F("tagId is : "));
    Serial.println(tagId);
    digitalWrite(LED_NFC, HIGH);
    network->firestoreUpdatePassenger(tagId);
    digitalWrite(LED_NFC, LOW);
  }
}

String tagToString(byte id[4]) {
  String tagId = "";
  for (byte i = 0; i < 4; i++) {
    if (i < 3) tagId += String(id[i]) + ".";
    else tagId += String(id[i]);
  }
  return tagId;
}