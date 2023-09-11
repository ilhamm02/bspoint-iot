#include "Network.h"

Network *network;

void setup()
{
  Serial.begin(9600);

  initNetwork();

  // network->firestoreUpdatePosition(105.85313727623817, -6.163592203837075);
  // network->firestoreUpdateCapacity(false);
}

void loop() {
}

void initNetwork(){
  network = new Network();
  network->initWiFi();
}