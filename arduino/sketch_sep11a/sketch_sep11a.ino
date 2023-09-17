#include "Network.h"
#include "TinyGPS++.h"
#include "TinyGPSPlus.h"
#include "SoftwareSerial.h"
#include "PN532_SWHSU.h"
#include "PN532.h"

#define LED_GPS 14
#define LED_NFC 2

Network* network;

SoftwareSerial GPSSerial(13, 12);  // RX, TX
SoftwareSerial NFCSerial(5, 4);    //RX, TX

TinyGPSPlus gps;

PN532_SWHSU pn532swhsu(NFCSerial);
PN532 nfc(pn532swhsu);
String tagId = "None", dispTag = "None";
byte nuidPICC[4];

const int maxCapacity = 100;
String* passengers = new String[maxCapacity];

void setup(void) {
  Serial.begin(9600);
  GPSSerial.begin(9600);
  //  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);

  initNetwork();

  nfc.begin();
  uint32_t versiondata = nfc.getFirmwareVersion();
  if (!versiondata) {
    Serial.print("Didn't Find PN53x Module");
    while (1);
  }
  nfc.SAMConfig();

  pinMode(LED_NFC, OUTPUT);
  pinMode(LED_GPS, OUTPUT);
}


void loop() {
  while (GPSSerial.available() > 0) {
    gps.encode(GPSSerial.read());
    if (gps.location.isUpdated()) {
      digitalWrite(LED_NFC, HIGH);
      network->firestoreUpdatePosition(gps.location.lng(), gps.location.lat());
      digitalWrite(LED_NFC, LOW);
    }
    readNFC();
  }
}

void initNetwork() {
  network = new Network();
  network->initWiFi();
}

void readNFC() {
  boolean success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
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
    delay(1000);
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