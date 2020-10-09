// var app = require('express')();
const express = require('express')
const app = express()
var http = require('http').Server(app);
var io = require('socket.io')(http);
const port = process.env.PORT || 3000;

app.use(express.static('public'))

app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
});

/*The getUsers method will get all of the clients that are connected.
With this object will get all of the socket information. 
Lastly will then map that to an array of users and return the array */
const getUsers = () => {
  let client = io.sockets.clients().connected;
  let sockets = Object.values(clients);
  let users = sockets.map(s => s.user);
  return users
}

const emitUsers = () => {
  io.emit("users", getUsers());
};

io.on('connection', function(socket){
  socket.on("new_user", user => {
    console.log("New User: ", user)
    socket.user = user
  });




  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
  });
});

http.listen(port, function(){
  console.log('listening on *:' + port);
});
