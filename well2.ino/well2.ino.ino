#include <SoftwareSerial.h>

#include <ESP8266WiFi.h>
#include <MicroGear.h>
#include "CMMC_Interval.hpp"

const char* ssid     = "Zenfone 2";
const char* password = "0870548627";

#define APPID   "HWsmarthouse"
#define KEY     "tRVWBRKu5TzLuTo"
#define SECRET  "vclR2zMJJzKyXUCs3wsauPpcm"
#define ALIAS   "pieled"


WiFiClient client;
CMMC_Interval timer001;

#define rxPin 5
#define txPin 4

SoftwareSerial sSerial(rxPin, txPin);
uint8_t lastByte;
char serialBuffer[100];
int count = 0;
int onboardLed = 13;

int timer = 0;
MicroGear microgear(client);
String msgglobald;
/* If a new message arrives, do this */
void onMsghandler(char *topic, uint8_t* msg, unsigned int msglen) {
  msg[msglen] = '\0';
  String _msg = String((char*)msg);
  msgglobald = _msg;
  /* print message */
  Serial.print("Incoming message --> ");
  Serial.print(_msg);
  if(_msg == "0")
     digitalWrite(LED_BUILTIN, HIGH); 
  else
     digitalWrite(LED_BUILTIN, LOW); 
  Serial.printf(" at [%lu] \r\n", millis());

}

void onFoundgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Found new member --> ");
  for (int i = 0; i < msglen; i++) {
    Serial.print((char)msg[i]);
  }
  Serial.println();
}

void onLostgear(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.print("Lost member --> ");
  for (int i = 0; i < msglen; i++) {
    Serial.print((char)msg[i]);
  }
  Serial.println();
}

/* When a microgear is connected, do this */
void onConnected(char *attribute, uint8_t* msg, unsigned int msglen) {
  Serial.println("Connected to NETPIE...");
  /*
    microgear can set its own alias, which to be used for others make a function call chat(). 
    The alias will appear on the key management portal of netpie.io
  */
  
  /* Set the alias of this microgear ALIAS */  
  microgear.setAlias(ALIAS);
}


void setup() {
  pinMode(rxPin, INPUT);
  pinMode(txPin, OUTPUT);

   /* Call onMsghandler() when new message arraives */
  microgear.on(MESSAGE, onMsghandler);
   pinMode(LED_BUILTIN, OUTPUT);

  /* Call onFoundgear() when new gear appear */
  microgear.on(PRESENT, onFoundgear); 

  /* Call onLostgear() when some gear goes offline */
  microgear.on(ABSENT, onLostgear);

  /* Call onConnected() when NETPIE connection is established */
  microgear.on(CONNECTED, onConnected);

  
  Serial.begin(115200);
  sSerial.begin(115200);
  pinMode(onboardLed, OUTPUT);
  digitalWrite(onboardLed, HIGH);

  if (WiFi.begin(ssid, password)) {
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
  }

   Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println("Connecting to NETPIE.io");
  /* Initial with KEY, SECRET and also set the ALIAS here */
  microgear.init(KEY, SECRET, ALIAS);

  /* connect to NETPIE to a specific APPID */
  microgear.connect(APPID);

}

void loop() {
  count = 0;
  while (sSerial.available() > 0) {
    serialBuffer[count] = sSerial.read();
    count++;
  }
   for (int i = 0; i < count; i++) {
    Serial.print(serialBuffer[i]);
   }
   /* Call this method regularly otherwise the connection may be lost */
   microgear.loop();
  /* To check if the microgear is still connected */
   if (microgear.connected()) {
    // call microgear.chat every 2 seconds
     timer001.every_ms(2000, []() {
      if(msgglobald == "0"){       
          if(serialBuffer[0] == 'O')
            microgear.chat("kuyfilm","OPEN");
          else
            microgear.chat("kuyfilm","CLOSE");
      }
      else
          microgear.chat("kuyfilm", "WaiiHair");
    });

  }
  else {
    Serial.println("connection lost, reconnect...");
    microgear.connect(APPID);
    delay(2000);
  }

}
