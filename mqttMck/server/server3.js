var mosca = require('mosca')
var http = require('http')
var express = require('express');
var logger = require('morgan');//logs GET/ 304 12ms

var app = express();
var currentPacket;

app.use(logger('dev'));

app.get('/', function(req, res) {
  res.send(currentPacket);
});

app.get('/api', function (req,res){
  res.send("<h4>in /api</h4>")
});

app.set('port', process.env.PORT || 3332);
var server = http.createServer(app);

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
var moserver = new mosca.Server(moscaSettings);   //here we start mosca
moserver.on('ready', setup);  //on init it fires up setup()

// fired when the mqtt server is ready
function setup() {
  console.log('Mosca server is up and running')
}
moserver.published = function(packet, client, cb) {
  if (packet.topic.indexOf('echo') === 0) {
    return cb();
  }
  var newPacket = {
    topic: 'echo/' + packet.topic,
    payload: packet.payload,
    retain: packet.retain || false,
    qos: packet.qos || 0
  };
  currentPacket= newPacket.payload.toString();
  console.log('newPacket payload', packet.topic , newPacket.payload.toString());
  moserver.publish(newPacket, cb);
}

server.listen(app.get('port'), function(){
  console.log('Express server listening on port ' + server.address().port);
  console.log('MQTT broker operating on port ' + moscaSettings.port);
  console.log('WebClient server MQTT through WS on port ' + moscaSettings.http.port);
});