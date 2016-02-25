var mqtt = require('mqtt')
const deviceId ='AAAAA0'
const status = deviceId+'/status'
const cmd = deviceId +'/cmd'

client = mqtt.createClient(1883, 'localhost');
client.subscribe(status);
client.subscribe(cmd);
client.on('message', function(topic, payload) {
	console.log('['+topic+'] '+payload.toString())
});
console.log('Client started...');