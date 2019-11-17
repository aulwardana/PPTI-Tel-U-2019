#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Adafruit_Fingerprint.h>

# define Finger_Rx 13 //D7
# define Finger_Tx 15 //D8

# define esp_Rx 3
# define esp_Tx 2

SoftwareSerial toNode(esp_Rx, esp_Tx);

SoftwareSerial toESP(Finger_Rx, Finger_Tx, false, 256);
Adafruit_Fingerprint finger = Adafruit_Fingerprint( & toESP);

const char* wifiSSID = "Your SSID";
const char* wifiPassword = "Your Password";

const char* mqttServerIP = "Your MQTT Server";
const int mqttPort = 1883;

char * device1 = "finger/found";
char * dataFinger = "finger/data"; 
char * pirOn = "pir/pirOn";
char * pirOff = "pir/pirOn";
char * stat = "status/device";

WiFiClient myESP;
PubSubClient client(myESP);

void wifiSetup(){
  WiFi.begin(wifiSSID,wifiPassword);
  while (WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.println("Waiting, connection to Wifi..");
    Serial.print("SSID : "); 
    Serial.println(wifiSSID);
  }
  Serial.println("Connected to the WiFI Network "); 
  Serial.print("Connected Network "); Serial.println(wifiSSID);
  Serial.print("IP Local "); Serial.println(WiFi.localIP());
}

char dataPublish[50];
void publishMQTT(char * topics, String data) {
  data.toCharArray(dataPublish, data.length() + 1);
  client.publish(topics, dataPublish);
}

void reconnect() {
  while(!client.connected()){
    Serial.println("Connecting to MQTT Server..");
    Serial.print("IP MQTT Server : "); Serial.println(mqttServerIP);
    bool hasConnection = client.connect("Esp-1"); 
    if(hasConnection){
      Serial.println("Success connected to MQTT Broker");
    } else {
      Serial.print("Failed connected");
      Serial.println(client.state());
      delay(2000);
      Serial.println("Try to connect...");
    }
  }
  client.publish(stat, "Initializing"); 
  client.subscribe(stat);

}

void callback(char * topic, byte * payload, unsigned int length) {
  Serial.println("--------");
  Serial.println("Message Arrived");
  Serial.print("Topic :");
  Serial.println(topic);
  Serial.print("Message : ");
  String pesan = "";
  for (int i = 0; i < length; i++) {
    Serial.print((char) payload[i]);

    pesan += (char) payload[i];

  }
  Serial.println();

  Serial.print("Pesan masuk :");
  Serial.println(pesan);
  Serial.println("--------");
}

uint8_t menu;
uint8_t id = 0;
uint8_t myArray[] = {
  2,
  3,
  4
};

void setup() {
  toNode.begin(57600);
  Serial.begin(57600);

  wifiSetup();

  Serial.print(F("WiFi connected! IP address: "));
  client.setServer(mqttServerIP, mqttPort);
  client.setCallback(callback); 
  
  while (!Serial); 
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
  }

}
uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}


uint8_t getFingerprintEnroll() {

  int p = -1;
  Serial.print("Waiting for valid finger to enroll as #");
  Serial.println(id);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.println(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.println("Remove finger");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID ");
  Serial.println(id);
  p = -1;
  Serial.println("Place same finger again");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
      case FINGERPRINT_OK:
        Serial.println("Image taken");
        break;
      case FINGERPRINT_NOFINGER:
        Serial.print(".");
        break;
      case FINGERPRINT_PACKETRECIEVEERR:
        Serial.println("Communication error");
        break;
      case FINGERPRINT_IMAGEFAIL:
        Serial.println("Imaging error");
        break;
      default:
        Serial.println("Unknown error");
        break;
    }
  }

  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  Serial.print("Creating model for #");
  Serial.println(id);

  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("Prints matched!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("Fingerprints did not match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("ID ");
  Serial.println(id);
  p = finger.storeModel(id);
  if (p == FINGERPRINT_OK) {
    Serial.println("Stored!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Could not store in that location");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Error writing to flash");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }
}

int getFingerprintIDez() {
  Serial.println("waiting");
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK) return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK) return -1;

  Serial.print("Found Jari with ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);
  
  return finger.fingerID;
}

void loop() {
  
  if (!client.connected()){
    reconnect();
  }
  client.loop();
  
  Serial.println("Please Choose the Menu ID  (from 1 to 3)!");
  Serial.println("1. Enroll fingerprint ");
  Serial.println("2. To log in");
  Serial.println("3. Empty fingerprint db ");
  Serial.println("4. Show fingerprint template ");

  menu = readnumber();
  if (menu == 1) {
    Serial.println("Ready to enroll a fingerprint!");
    id = readnumber();
    if (id > 0 && id < 120) { 
      Serial.println("enroll boz");
      getFingerprintEnroll();

    };

  } else if (menu == 2) {


    finger.getTemplateCount();
    Serial.print("Sensor contains ");
    Serial.print(finger.templateCount);
    Serial.println(" templates");
    Serial.println("Waiting for valid finger...");
    while (menu == 2) {
      int in1 = getFingerprintIDez();
      if (in1 != -1) {
        int triggerId = 1;
        int nomorFinger = finger.fingerID;
        Serial.print(nomorFinger);
        Serial.println(triggerId);
        char ketemuPir = client.subscribe(pirOn);
        String foundJari = "found_jari";
        Serial.println(ketemuPir);
        String dataSent = "";
        String dataFinger = "";
        if(triggerId==1){ 
          dataSent = String(foundJari);
          dataFinger = String(nomorFinger);
          publishMQTT(device1, dataFinger);
          break;
        }
        
        in1 = -1;
      }
    }

  } else if (menu == 3) {
    finger.emptyDatabase();
  } else if (menu == 4) {
    for (int finger = 1; finger < 10; finger++) {
      downloadFingerprintTemplate(finger);
    }
  } else {
    Serial.println("no choice");
    return;
  }
}

uint8_t getFingerprintID() {
  uint8_t p = finger.getImage();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image taken");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println("No finger detected");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Imaging error");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  p = finger.image2Tz();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Image converted");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Image too messy");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Could not find fingerprint features");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Could not find fingerprint features");
      return p;
    default:
      Serial.println("Unknown error");
      return p;
  }

  p = finger.fingerFastSearch();
  if (p == FINGERPRINT_OK) {
    Serial.println("Found a print match!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Communication error");
    return p;
  } else if (p == FINGERPRINT_NOTFOUND) {
    Serial.println("Did not find a match");
    return p;
  } else {
    Serial.println("Unknown error");
    return p;
  }

  Serial.print("Found ID #");
  Serial.print(finger.fingerID);
  Serial.print(" with confidence of ");
  Serial.println(finger.confidence);

  return finger.fingerID;
}

uint8_t downloadFingerprintTemplate(uint16_t id) {
  Serial.println("------------------------------------");
  Serial.print("Attempting to load #");
  Serial.println(id);
  uint8_t p = finger.loadModel(id);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Template ");
      Serial.print(id);
      Serial.println(" loaded");
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Communication error");
      return p;
    default:
      Serial.print("Unknown error ");
      Serial.println(p);
      return p;
  }

  Serial.print("Attempting to get #");
  Serial.println(id);
  p = finger.getModel();
  switch (p) {
    case FINGERPRINT_OK:
      Serial.print("Template ");
      Serial.print(id);
      Serial.println(" transferring:");
      break;
    default:
      Serial.print("Unknown error ");
      Serial.println(p);
      return p;
  }

  uint8_t bytesReceived[534];
  memset(bytesReceived, 0xff, 534);

  uint32_t starttime = millis();
  int i = 0;
  while (i < 534 && (millis() - starttime) < 20000) {
    if (toESP.available()) {
      bytesReceived[i++] = toESP.read();
    }
  }
  Serial.print(i);
  Serial.println(" bytes read.");
  Serial.println("Decoding packet...");

  uint8_t fingerTemplate[512];
  memset(fingerTemplate, 0xff, 512);

  int uindx = 9, index = 0;
  while (index < 534) {
    while (index < uindx) ++index;
    uindx += 256;
    while (index < uindx) {
      fingerTemplate[index++] = bytesReceived[index];
    }
    uindx += 2;
    while (index < uindx) ++index;
    uindx = index + 9;
  }
  for (int i = 0; i < 512; ++i) {
    printHex(fingerTemplate[i], 2);
  }
  Serial.println("\ndone.");
}

void printHex(int num, int precision) {
  char tmp[16];
  char format[128];

  sprintf(format, "%%.%dX", precision);

  sprintf(tmp, format, num);
  Serial.print(tmp);
}
