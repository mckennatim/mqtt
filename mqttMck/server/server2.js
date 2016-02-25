var mosca = require('mosca')

var pubsubsettings = {
  //using ascoltatore
  type: 'mongo',        
  url: 'mongodb://localhost:27017/mqtt',
  pubsubCollection: 'ascoltatori',
  mongo: {}
};

var moscaSettings = {
  port: 1883,           //mosca (mqtt) port
  backend: pubsubsettings,//pubsubsettings is the object we created above 
  http: {port: 3333, bundle: true, static: './'}    
};
var server = new mosca.Server(moscaSettings);   //here we start mosca
server.on('ready', setup);  //on init it fires up setup()

// fired when the mqtt server is ready
function setup() {
  console.log('Mosca server is up and running')
}
server.published = function(packet, client, cb) {
  if (packet.topic.indexOf('echo') === 0) {
    return cb();
  }
  var newPacket = {
    topic: 'echo/' + packet.topic,
    payload: packet.payload,
    retain: packet.retain || false,
    qos: packet.qos || 0
  };
  console.log('newPacket payload', packet.topic , newPacket.payload.toString());
  server.publish(newPacket, cb);
}
