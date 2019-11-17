#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include <ArduinoJson.h>
const char* wifiSSID = "Your SSID";
const char* wifiPassword = "Your Password SSID";

const char* mqttServerIP = "Your MQTT Server";
const int mqttPort = 1883;
char * devGerak = "gerak/gerak";
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

const int trigPin = 2;  //D4
const int echoPin = 4;  //D2

const unsigned int jarak = 140;  
const byte toleransi = 5;        
const unsigned int  jeda = 10,           
                    jeda2 = 1000,            
                    waktuKonfirmasi = 80; 

long duration;
int distance;
bool triggered;
unsigned long ct = 0;

void setup() {
  pinMode(trigPin, OUTPUT); 
  pinMode(echoPin, INPUT); 
  Serial.begin(57600); 

  wifiSetup();
  Serial.print(F("WiFi connected! IP address: "));
  client.setServer(mqttServerIP, mqttPort);
  client.setCallback(callback); 
  
  digitalWrite(trigPin, LOW);

  triggered = false;
}

void loop() {
  if (!client.connected()){
    reconnect();
  }
  client.loop();
  delayMicroseconds(2);

  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH);

  distance = duration * 0.034 / 2;
  String foundGerak = "gerak_found";
  String dataSent = "";
  if ( ((float) distance) < jarak * (1.0 - toleransi / 100.0) ) {    
    ct += jeda;
    if (ct >= waktuKonfirmasi) {
      if (!triggered) {
        dataSent = String(foundGerak);
        Serial.println(dataSent);
        triggered = true;
        publishMQTT(devGerak, dataSent);
      }
      ct = 0;
      delay(jeda2);
    }
  } else {
    triggered = false;
    ct = 0;
  }
  if (jeda > 0) delay(jeda);
}
