#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <OneWire.h>
#include <DallasTemperature.h>

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


WiFiClient espClient;
PubSubClient client(espClient);

void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(wifi_ssid);
  WiFi.begin(wifi_ssid, wifi_password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
char incoming[40];
char rela;
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
  rela = sinc[sinc.indexOf(':')+1];
  relay = rela - '0';
  if(relay<2){
    digitalWrite(ALED, relay);
    oldLed = !digitalRead(ALED);
  } else {
    oldLed=2;
  }
  Serial.println(sinc + relay);
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
void reconn(int num) {
  int cnt = num;
  while (cnt>0) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    // If you do not want to use a username and password, change next line to
    // if (client.connect("ESP8266Client")) {
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      client.subscribe(cmd);
      break;
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
      cnt--;
    }
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
  if (!client.connected()) {
    reconn(1);
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