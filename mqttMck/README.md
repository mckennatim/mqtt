## mqttHrs
### intro
Based on <a href="https://github.com/mqttjs/MQTT.js">MQTT.js and <a href="https://github.com/mcollina/mosca"></a>mosca</a> as combined in <a href="https://github.com/arvindr21/mqttApp">mqttHrs</a> and using code as described in <a href="http://thejackalofjavascript.com/getting-started-mqtt/">getting-started-mqtt/</a> for node servers and clients. The esp8266 #includes `ESP8266WiFi.h` and `PubSubClient.h`


### tags
#### 0-mqtt-to_browser_and_back
server + 2 node clients vii `mosca` and a webclient using `browserMqtt.js` as described in <a href="https://www.npmjs.com/package/mqtt#browser">mqtt browser</a> and another webclient using <a href="https://www.eclipse.org/paho/clients/js/">paho</a>
#### 1-express+mosca-012-mob
Integrated `express` + `mosca` .  The esp8266/mqttHrs/mqttHrs.ino (now in mqttHrs) sends status whenever something changes. Received cmd's always cause a status event. 

Incoming stream of bytes gets changed to characters and stored in the `incoming[]` character array. That gets converted to a String `sinc` . Now you can use `indexOf` to find the first `:` as in auto:1 which for some reason is still a `byte` code for the number. Subtracting the byte code for `0` gets you the integer value. Since the led is pulled up to vcc, a zero turns it ON, 1 turns it  OFF. 0/1 get immediately sent to that output `digitalWrite(ALED, relay)`. `oldLed` is a variable that is compared to `digitalRead(ALED)` in the main loop line 121 `if (oldHoah != hoah || oldHoaa != hoaa || oldLed != digitalRead(ALED))` . Whenever it is different it causes the `status` to get published to the mqtt broker and beyond. It is toggled on a command <2 (as seen below) and set to 2 otherwise. 


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
`auto:2` is for quering the microcontroller for its status. It will cause a publish event. `oldLed` gets set to match the output then to keep the esp8266 quiet until something else happens. (like maybe somebody turns the HOA switch).

Now works on mobile devices. (change locahost to 10.0.1.102). Android complained about naming a variable `status`. 

### questions
My <a href="https://github.com/mcollina/mosca/issues/399">github issue 339</a> and are resolved

###refs
@mateocollina has a <a href="http://mcollina.github.io/mqtt_and_nodejs/#1">slide show</a>. Here is a <a href="https://www.youtube.com/watch?v=xmKd2lYqEPA">youtube video</a>

Besides the arduino development environment there is a <a href="http://nodemcu.com/index_en.html">NodeMCU</a>. Besides moca there is <a href="http://test.mosquitto.org/">mosquitto</a> 

<a href="http://blog.thingstud.io/getting-started/free-mqtt-brokers-for-thingstudio/">Thing studio</a>(<a href="http://blog.thingstud.io/recipes/how-to-make-your-raspberry-pi-the-ultimate-iot-hub/">+ Pi tutorial</a>) and <a href="https://home-assistant.io/blog/2015/10/11/measure-temperature-with-esp8266-and-report-to-mqtt/">Home Assistant</a> use mqtt brokers.

<a href="https://www.npmjs.com/package/mqtt#browser">mqtt browser</a>
<a href="https://github.com/mcollina/mosca/wiki/MQTT-over-Websockets">MQTT-over-Websockets</a>
<a href=""></a>
 
