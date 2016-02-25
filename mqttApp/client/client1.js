var mqtt = require('mqtt')
client = mqtt.createClient(1883, 'localhost');
client.subscribe('presence');
console.log('Client publishing.. ');
client.publish('presence', 'Client 10 is alive.. Test Ping! ' + Date());
client.end();