#include "Network.h"
#include "SoftwareSerial.h"
#include "PN532_SWHSU.h"
#include "PN532.h"
#include "queue"
#include "array"

Network* network;

SoftwareSerial SWSerial(8, 7);  // RX, TX
PN532_SWHSU pn532swhsu(SWSerial);
PN532 nfc(pn532swhsu);
String tagId = "None", dispTag = "None";
byte nuidPICC[4];

const int maxCapacity = 100;

String* passengers = new String[maxCapacity];

void setup() {
  Serial.begin(9600);

  initNetwork();

  nfc.begin();
  nfc.SAMConfig();

  // network->firestoreUpdatePosition(105.85313727623817, -6.163592203837075);
  // network->firestoreUpdateCapacity(false);
}

void loop() {
  readNFC();
  delay(1000);
}

void initNetwork() {
  network = new Network();
  network->initWiFi();
}

void readNFC() {
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };
  uint8_t uidLength;
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, &uid[0], &uidLength);
  if (success) {
    for (uint8_t i = 0; i < uidLength; i++) {
      nuidPICC[i] = uid[i];
    }
    tagId = tagToString(nuidPICC);
    Serial.print(tagId);
    network->firestoreUpdateCapacity(findPassanger(tagId));
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

bool findPassanger(String tagId) {
  bool newComer = true;
  String* currentPassenger = new String[maxCapacity];
  int capacity = 0;
  for (int i = 0; i < maxCapacity; i++) {
    if (passengers[i] == tagId) newComer = false;
    else {
      currentPassenger[capacity] = tagId;
      capacity++;
    }
  }

  passengers = currentPassenger;

  return newComer;
}