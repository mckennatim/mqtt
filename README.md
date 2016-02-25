# mqtt
An exploration of mqtt protocol on an (a) esp8266 wifi microcontroller, (b) a node express server, mqtt broker and websockets server, (c) a web client for http and websockets.

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

  