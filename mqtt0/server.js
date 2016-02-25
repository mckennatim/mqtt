var mqtt = require('mqtt')
var client = mqtt.connect();
client.subscribe('my/shit')
client.on('message', function(topic, payload){
	alert([topic,paylod].join(": "));
	client.end();
})

client.publish('my/shit', "hello voking")