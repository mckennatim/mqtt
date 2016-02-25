# esp8266 development Wemos
#### narrative
<a href="http://www.wemos.cc/wiki/doku.php?id=en:d1_mini#schematics">schematic</a>


### tasks
- read pin 4 and update when changed
- set pin that setup() can read then decide to be AP or St
- store ssid and password in eeprom
- read ssid and password from eeprom

#### read pin 4 and update when changed

basic delay between checking inputs

    long now = millis();
    if (now - lastMsg > 10000) {
        lastMsg = now;
    }

every 1000 read HOA switch and auto relay setting

      if (now - lastMsg > 1000) {
        lastMsg = now; //reset timer
        oldHoah = hoah;
        oldHoaa = hoaa;
        hoah=digitalRead(HOAH);
        hoaa=digitalRead(HOAA);
        if (oldHoah != hoah || oldHoaa != hoaa) {
          digitalWrite(ALED, !digitalRead(ALED)); //temporary toggle sim input
          (hoah==1) ? hoa=0 : hoa=1; //if ho switch is open O else H 
          if(hoaa==0) hoa=2; //if ha is closed A
          digitalRead(ALED) ? haa=1 : haa=0;
          sprintf(astr, "{hoa:%d, auto:%d}", hoa, digitalRead(ALED));
          Serial.print(astr);
          client.publish("presence", astr, true);
        }
      }

#### refs
- <a href="http://www.esp8266.com/wiki/doku.php?id=esp8266_gpio_pin_allocations">gpio_pin_allocations</a> 
- <a href="http://www.forward.com.au/pfod/ESP8266/GPIOpins/index.html">How to use GPIO0, GPIO2 and GPIO15 pins as Outputs and Inputs on ESP8266 modules</a>
- <a href="https://cdn.sparkfun.com/datasheets/Wireless/WiFi/ESP8266-Thing-Dev-v10.pdf">thing dev schematic</a>
- <a href="http://www.wemos.cc/wiki/lib/exe/fetch.php?media=en:d1_mini.pdf">wemos mini schematic</a>