# mqtt
An exploration of mqtt protocol on an (a) esp8266 wifi microcontroller, (b) a node express server, mqtt broker and websockets server, (c) a web client for http and mqtt riding on websockets.

## tags
### 01-no-webconfig-non-indep
Working on /mqttMck/esp8266/mqqtMck mock data provider. Its a port of mqttHrs. Does not yet have web configuration and does not yet run independently since on every loop it keeps trying to reconnect instead of going about its business.

```c++
    void loop() {
      if (!client.connected()) {
        reconnect();
      }
      client.loop();
```


### 02-no-webconfig-indep
now runs independently of server and client. todo-webconfig then add sensor

### 03-rebuild-env
- rebuilt arduino esp8266 envirnment from scratch not using the board manager but by going into powershell admin `PS C:\Program Files (x86)\Arduino\hardware` and adding `\esp8266com` then `cd esp8266com
git clone https://github.com/esp8266/Arduino.git esp8266` then `\esp8266\tools>` and running `python get.py`. Otherwise the MXonewire library was fucking with regular onewire. All this was to get `https://github.com/bblanchon/ArduinoJson` library to work. Ugh Now the callback can parse the payload response into its json components.

### 04-webconfig
- remove hardcoded ssid/pwd and see if it is still in eeprom from that other sketch -YES 
- figure out a way to erase the eeprom ssid/pwd 
In `setup()` `setup_wifi()` is called which reads the ssid/pwd out of eeprom and tries to connect. 

If that works, the pubsub client is set up along with io setup and then the loop starts
 
If not, after 15 tries it goes to `setupAP()` which puts it in Station Mode and scans for available wifi networks and saves it to `st` then starts up an access point (AP) `ESP_OB6834` running on 192.168.4.1 serving up a page that lists `st`and takes input for ssid/pwd. Now the loop starts.

(The loop handles server input if it is in setupAP mode 

or 

checks if the mqtt client is running and tries to reconnect every 5 seconnds if not. If connected it runs a client.loop() to check for incoming mqqt messages.)

and 

does its own thing reading inputs and controlling relays. that keeps up no matter what the connected state of things.


