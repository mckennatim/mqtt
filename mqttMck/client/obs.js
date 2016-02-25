var mqtt = require('mqtt');
const deviceId ='AAAAA0';
//const status = deviceId+'/status'
const cmd = deviceId +'/cmd'
var thecmd =  "{ \"auto\":2, \"src\":1 }"
client = mqtt.createClient(1883, 'localhost');
client.subscribe(cmd);
console.log('Client publishing.. ');
client.publish(cmd, thecmd);
client.end();