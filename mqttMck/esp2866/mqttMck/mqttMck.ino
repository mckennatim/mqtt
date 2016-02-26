#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <EEPROM.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>

//#define deviceId "AAAAA0"
#define deviceId "CYURD001"
#define cmd "CYURD001/cmd"
//#define cmd "AAAAA0/cmd"
#define wifi_ssid "street_no_vale2"
#define wifi_password "jjjjjjjj"

#define mqtt_server "10.0.1.104"
#define HOAH 14
#define HOAA 13
#define ALED 5
#define CMD 12
#define ADC 0
#define ONE_WIRE_BUS 4  // DS18B20 pin
#define TEMP_MAX 85 // DS18B20 pin
#define TEMP_MIN 75  // DS18B20 pin
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature DS18B20(&oneWire);

ESP8266WebServer server(80);
String st;
String content;
int statusCode;
const char* ssid = "bobbles";
const char* passphrase = "test";

WiFiClient espClient;
PubSubClient client(espClient);

void createConfigServer(int webtype){
  server.on("/", []() {
      IPAddress ip = WiFi.softAPIP();
      String ipStr = String(ip[0]) + '.' + String(ip[1]) + '.' + String(ip[2]) + '.' + String(ip[3]);
      content = "<!DOCTYPE HTML>\r\n<html>Hello from ESP8266 at ";
      content += ipStr;
      content += "<p>";
      content += st;
      content += "</p><form method='get' action='setting'><label>SSID: </label><input name='ssid' length=32><input name='pass' length=64><input type='submit'></form>";
      content += "</html>";
      server.send(200, "text/html", content);  
  });
  server.on("/setting", []() {
      String qsid = server.arg("ssid");
      String qpass = server.arg("pass");
      if (qsid.length() > 0 && qpass.length() > 0) {
        Serial.println("clearing eeprom");
        for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
        Serial.println(qsid);
        Serial.println("");
        Serial.println(qpass);
        Serial.println("");
          
        Serial.println("writing eeprom ssid:");
        for (int i = 0; i < qsid.length(); ++i)
          {
            EEPROM.write(i, qsid[i]);
            Serial.print("Wrote: ");
            Serial.println(qsid[i]); 
          }
        Serial.println("writing eeprom pass:"); 
        for (int i = 0; i < qpass.length(); ++i)
          {
            EEPROM.write(32+i, qpass[i]);
            Serial.print("Wrote: ");
            Serial.println(qpass[i]); 
          }    
        EEPROM.commit();
        content = "{\"Success\":\"saved to eeprom... reset to boot into new wifi\"}";
        statusCode = 200;
      } else {
        content = "{\"Error\":\"404 not found\"}";
        statusCode = 404;
        Serial.println("Sending 404");
      }
      server.send(statusCode, "application/json", content);
  });  
}

void launchWeb(int webtype) {
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("Local IP: ");
  Serial.println(WiFi.localIP());
  Serial.print("SoftAP IP: ");
  Serial.println(WiFi.softAPIP());
  // Start the server
  createConfigServer(webtype);
  server.begin();
  Serial.println("Server started"); 
}

void setupAP(void) {
  Serial.println("scan done");
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(100);
  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0)
    Serial.println("no networks found");
  else
  {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i)
     {
      // Print SSID and RSSI for each network found
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*");
      delay(10);
     }
  }
  Serial.println(""); 
  st = "<ol>";
  for (int i = 0; i < n; ++i)
    {
      // Print SSID and RSSI for each network found
      st += "<li>";
      st += WiFi.SSID(i);
      st += " (";
      st += WiFi.RSSI(i);
      st += ")";
      st += (WiFi.encryptionType(i) == ENC_TYPE_NONE)?" ":"*";
      st += "</li>";
    }
  st += "</ol>";
  delay(100);
  WiFi.softAP(ssid, passphrase, 6);
  Serial.println("softap");
  launchWeb(1);
  Serial.println("over");
}


void setup_wifi() {
  delay(10);
  Serial.println("Reading EEPROM ssid");
  String esid;
  for (int i = 0; i < 32; ++i)
    {
      esid += char(EEPROM.read(i));
    }
  Serial.print("SSID: ");
  Serial.println(esid);
  Serial.println("Reading EEPROM pass");
  String epass = "";
  for (int i = 32; i < 96; ++i)
    {
      epass += char(EEPROM.read(i));
    }
  Serial.print("PASS: ");
  Serial.println(epass);   
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(esid);
  WiFi.begin(esid.c_str(), epass.c_str());
  int tries =0;
  int success=1;
  while (WiFi.status() != WL_CONNECTED ) {
    delay(500);
    Serial.print(".");
    tries++;
    if (tries==15){
      success=0;
      setupAP();
      break;
    }
  }
  if (success){
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());    
  } else {
    Serial.println("WiFi not connected-goneto Station Mode");    
  }

}
char incoming[40];
int relay;
char c;
int oldLed;

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    c = (char)payload[i];
    incoming[i] = c;
  }
  incoming[length] = '\0';
  String sinc = String(incoming).c_str();  
  StaticJsonBuffer<200> jsonBuffer;
  Serial.println(incoming);
  // "{\"heat\":1,\"src\":1,\"empty\":1}"
  JsonObject& root = jsonBuffer.parseObject(incoming);
  relay = root["heat"];
  int empty = root["empty"];
  if(relay<2){
    digitalWrite(ALED, relay);
    oldLed = !digitalRead(ALED);
  } else {
    oldLed=2;
  }
  if (empty==1){
    Serial.println("clearing eeprom");
    for (int i = 0; i < 96; ++i) { EEPROM.write(i, 0); }
    EEPROM.commit();  
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);      
  }
  Serial.println(empty);
}
void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe(cmd);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
void reconn() {
  Serial.print("Attempting MQTT connection...");
  // Attempt to connect
  // If you do not want to use a username and password, change next line to
  // if (client.connect("ESP8266Client")) {
  if (client.connect("ESP8266Client")) {
    Serial.println("connected");
    client.subscribe(cmd);
    return;
  } else {
    Serial.print("failed, rc=");
    Serial.print(client.state());
    
    // Wait 5 seconds before retrying
    delay(5000);
    Serial.println(" try again in 5 seconds");
  }
}

long lastMsg = 0;
int hoa ;
int haa ;
int hoah = 0;
int hoaa = 0;
int oldHoah;
int oldHoaa;
int oldtemp1;
char astr[80] ;
int aled;
char status[80];
char payload[100];

void setup() {
  Serial.begin(115200);
  EEPROM.begin(512);
  delay(10);
  Serial.println();
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);
  pinMode(HOAH, INPUT);//pullup
  pinMode(HOAA, INPUT);//pullup
  pinMode(ALED, OUTPUT);
  digitalWrite(ALED, HIGH);
  oldLed = digitalRead(ALED);
  pinMode(CMD, INPUT);
  char arr[ ] = "This is a test";
  String sarr = String(arr).c_str();
  Serial.println(sarr + 456);

}

void loop() {
  server.handleClient();
  if (!client.connected()) {
    reconn();
  }
  if (client.connected()){
    client.loop();
  }
  float temp;
  int temp1;
  long now = millis();
  if (now - lastMsg > 1000) {
    lastMsg = now; //reset timer
    oldHoah = hoah;
    oldHoaa = hoaa;
    hoah=digitalRead(HOAH);
    hoaa=digitalRead(HOAA);
    DS18B20.requestTemperatures(); 
    temp = DS18B20.getTempFByIndex(0);
    temp1 = (int)temp;
    if (oldtemp1 != temp1 || oldLed != digitalRead(ALED)) {
      aled = digitalRead(ALED);
      if (temp1 >= TEMP_MAX){
        aled = 0;
      } else if (temp1 <= TEMP_MIN) {
        aled = 1;
      }
      digitalWrite(ALED, aled);
      oldtemp1= temp1;      
      oldLed = digitalRead(ALED);
      (hoah==1) ? hoa=0 : hoa=1; //if ho switch is open O else H 
      if(hoaa==0) hoa=2; //if ha is closed A
      digitalRead(ALED) ? haa=1 : haa=0;
      sprintf(astr, "{\"temp1\":%d, \"hoa\":%d, \"heat\":%d }", temp1, hoa, aled);
      Serial.print(astr);
      sprintf(status, "%s/status", deviceId);
      sprintf(payload,"{ payload:%s, topic:'%s' }", astr, status);
      Serial.println(payload);
      if (client.connected()){
        client.publish(status, astr, true);
      }
    }
  }
}