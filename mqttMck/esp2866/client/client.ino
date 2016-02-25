#include <ESP8266WiFi.h>
#include <WiFiClient.h>

// WiFi information
const char WIFI_SSID[] = "street_no_vale2";
const char WIFI_PSK[] = "jjjjjjjj";

// Remote site information
const char http_site[] = "10.0.1.104";
const int http_port = 3072;

// Pin definitions
const int LED_PIN = 5;
const int ST_AP =4;


// Global variables
WiFiClient client;


// Attempt to connect to WiFi
void connectWiFi() {
  
  byte led_status = 0;
  
  // Set WiFi mode to station (client)
  WiFi.mode(WIFI_STA);
  
  // Initiate connection with SSID and PSK
  WiFi.begin(WIFI_SSID, WIFI_PSK);
  
  // Blink LED while we wait for WiFi connection
  while ( WiFi.status() != WL_CONNECTED ) {
    digitalWrite(LED_PIN, led_status);
    led_status ^= 0x01;
    delay(100);
  }
  
  // Turn LED on when we are connected
  digitalWrite(LED_PIN, HIGH);
}

// Perform an HTTP GET request to a remote page
bool getPage() {
  
  // Attempt to make a connection to the remote server
  if ( !client.connect(http_site, http_port) ) {
    return false;
  }
  
  // Make an HTTP GET request
  client.println("GET / HTTP/1.1");
  client.print("Host: ");
  client.println(http_site);
  client.println("Connection: close");
  client.println();
  
  return true;
}

void setup() {
  
  // Set up serial console to read web page
  Serial.begin(9600);
  Serial.print("Thing GET Example");
  
  // Set up LED for debugging
  pinMode(LED_PIN, OUTPUT);
  pinMode(ST_AP, INPUT);
  
  // Connect to WiFi
  connectWiFi();
  
  // Attempt to connect to website
  if ( !getPage() ) {
    Serial.println("GET request failed");
  }
}

void loop() {
  
  // If there are incoming bytes, print them
  if ( client.available() ) {
    char c = client.read();
    Serial.print(c);
  }
  
  // If the server has disconnected, stop the client and WiFi
  if ( !client.connected() ) {
    Serial.println();
    
    // Close socket and wait for disconnect from WiFi
    client.stop();
    if ( WiFi.status() != WL_DISCONNECTED ) {
      WiFi.disconnect();
    }
    
    // Turn off LED
    digitalWrite(LED_PIN, LOW);
    Serial.println(digitalRead(ST_AP));
    // Do nothing
    Serial.println("Finished Thing GET test");
    while(true){
      delay(1000);
    }
  }
}
