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
    retain: packet.retain || false,
    qos: packet.qos || 0
  };
  console.log('newPacket payload', newPacket.payload.toString());
  server.publish(newPacket, cb);
}