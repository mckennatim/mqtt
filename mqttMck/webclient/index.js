var mqtt = require('mqtt')

// var client = mqtt.connect('http://localhost:1883');
console.log(mqtt)
client = mqtt.connect('http://localhost:1883');


client.subscribe('presence');

client.on("message", function(topic, payload) {
alert([topic, payload].join(": "));
client.end();
});

client.publish('presence', 'Web Client is alive.. Test Ping! ' + Date());
console.log('might be working')