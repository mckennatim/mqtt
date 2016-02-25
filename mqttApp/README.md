## mqttApp
### refs
Based on <a href="https://github.com/mqttjs/MQTT.js">MQTT.js and <a href="https://github.com/mcollina/mosca"></a>mosca</a> as combined in <a href="https://github.com/arvindr21/mqttApp">mqttApp</a> and using code as described in <a href="http://thejackalofjavascript.com/getting-started-mqtt/">getting-started-mqtt/</a> for node servers and clients. 

Web clients are working because of <a href="https://www.eclipse.org/paho/clients/js/">paho</a>. (still can't get browserMqtt.js to work)

@mateocollina has a <a href="http://mcollina.github.io/mqtt_and_nodejs/#1">slide show</a>. Here is a <a href="https://www.youtube.com/watch?v=xmKd2lYqEPA">youtube video</a>

Besides the arduino development environment there is a <a href="http://nodemcu.com/index_en.html">NodeMCU</a>. Besides moca there is <a href="http://test.mosquitto.org/">mosquitto</a> 

<a href="http://blog.thingstud.io/getting-started/free-mqtt-brokers-for-thingstudio/">Thing studio</a>(<a href="http://blog.thingstud.io/recipes/how-to-make-your-raspberry-pi-the-ultimate-iot-hub/">+ Pi tutorial</a>) and <a href="https://home-assistant.io/blog/2015/10/11/measure-temperature-with-esp8266-and-report-to-mqtt/">Home Assistant</a> use mqtt brokers.

<a href="https://www.npmjs.com/package/mqtt#browser">mqtt browser</a>
<a href="https://github.com/mcollina/mosca/wiki/MQTT-over-Websockets">MQTT-over-Websockets</a>
<a href=""></a>

### questions
My <a href="https://github.com/mcollina/mosca/issues/399">github issue 339</a> and <a href="http://stackoverflow.com/questions/34868620/mosca-and-mqtt-dont-get-messages-from-browserfied-mqtt-web-client">stackoverflow - mosca-and-mqtt-dont-get-messages-from-browserfied-mqtt-web-client</a>
 
Mosca and mqtt don't get messages from browserfied mqtt web client.

Following the tips from <a href="https://github.com/mcollina/mosca/wiki/MQTT-over-Websockets">MQTT-over-Websockets</a> I tried to add to this alredy working setup.
server(broker)

    var mosca = require('mosca')
    var settings = {
      port: 1883,
      persistence: mosca.persistence.Memory,
      http: {port: 3333, bundle: true, static: './'}  
    };
    var server = new mosca.Server(settings, function() {
      console.log('Mosca server is up and running')
    });
    server.published = function(packet, client, cb) {
      if (packet.topic.indexOf('echo') === 0) {
        return cb();
      }
      var newPacket = {
        topic: 'echo/' + packet.topic,
        payload: packet.payload,
        retain: packet.retain,
        qos: packet.qos
      };
      console.log('newPacket', newPacket);
      server.publish(newPacket, cb);
    }

client2.js

    var mqtt = require('mqtt')
    client = mqtt.createClient(1883, 'localhost');
    client.subscribe('presence');
    client.on('message', function(topic, message) {
        console.log(message.toString());
    });
    console.log('Client started...');    
client1.js

    var mqtt = require('mqtt')
    client = mqtt.createClient(1883, 'localhost');
    client.subscribe('presence');
    console.log('Client publishing.. ');
    client.publish('presence', 'Client 10 is alive.. Test Ping! ' + Date());
    client.end();
So I want to get a web client working and I followed the instructions in <a href="https://www.npmjs.com/package/mqtt#browser">mqtt in browser w webpack</a> to create browserMqtt.js

    cd node_modules/mqtt
    npm install . // install dev dependencies 
    webpack mqtt.js ./browserMqtt.js --output-library mqtt

and used it in a webpage mimiking what the node client2.js already does.

    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>webmqtt</title>
        <script src="./dist/browserMqtt.js"></script>   
    </head>
    <body>
        <h1>hello</h1>
        <script>
            client = mqtt.connect({ host: 'localhost', port: 3333 });
            client.subscribe('presence');
            client.on('message', function(topic, payload) {
                console.log(message.toString())
            });
            client.publish('presence', 'Web Client is alive.. Test Ping! ' + Date());
        </script>
    </body>
    </html>  
It's publish message isn't showing up on the other clients and it doesn't get the messages it is subscribed to. It does however cause a new packet to arrive at the server with what looks like the browser client's id. 

Ok with that change...

    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>webmqtt</title>
        <script src="./dist/browserMqtt.js"></script>   
    </head>
    <body>
        <h1>hellop</h1>
        <script>
            var client = mqtt.connect('ws://localhost:3333');
            console.log('maybe connected')
            client.subscribe('presence');
            client.on('message', function(topic, payload) {
                console.log(message.toString())
            });
            client.publish('presence', 'Web Client is alive.. Test Ping! ' + Date());
            //client.end()
        </script>
    </body>
    </html>
It stil sends the id but doesn't send or recieve messages

I thought maybe I wasn't allowing for a callback so I also tried this to no avail.

    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>webmqtt</title>
        <script src="./dist/browserMqtt.js"></script>   
    </head>
    <body>
        <h1>hellop</h1>
        <script>
            mqtt.connect('ws://localhost:3333', function(client){
                console.log('maybe connected')
                client.subscribe('presence');
                client.on('message', function(topic, payload) {
                    console.log(message.toString())
                });
                client.publish('presence', 'Web Client is alive.. Test Ping! ' + Date());
            });
            //client.end()
        </script>
    </body>
    </html>

payload instead of message didn't help

    client.on('message', function(topic, payload) {
        console.log(payload.toString())
        console.log(payload)
        console.log(topic)
    });

    client.on('payload', function(topic, payload) {
        console.log(payload.toString())
        console.log(payload)
        console.log(topic)
    });    
@psorowka, I added server defaults and went back to the async web client

    <!DOCTYPE html>
    <html lang="en">
    <head>
        <meta charset="UTF-8">
        <title>webmqtt</title>
        <script src="./dist/browserMqtt.js"></script>   
    </head>
    <body>
        <h1>hellop</h1>
        <script>
            var client = mqtt.connect('ws://localhost:3333')
            client.on('connect', function(){
                console.log('maybe connected')
                client.subscribe('presence');
                client.on('message', function(topic, payload) {
                    console.log(payload)
                    console.log(payload.toString())
                });
                client.publish('presence', 'Web Client is alive.. Test Ping! ' + Date());
            });
        </script>
    </body>
    </html>    