//  these are all of the package dependendancies. 
const express = require('express')
const app = express()
var http = require('http').Server(app);
var io = require('socket.io')(http);
var port = process.env.PORT || 3000;


//this will host a public folder as static on the server, this for the script. 
app.use(express.static('public'))

//this will route the user to the file at the defualt route. 
app.get('/', function(req, res){
  res.sendFile(__dirname + '/index.html');
});

/*The getUsers method will get all of the clients that are connected.
With this object will get all of the socket information. 
Lastly will then map that to an array of users and return the array */
const getUsers = () => {
  let clients = io.sockets.clients().connected;
  let sockets = Object.values(clients);
  let users = sockets.map(s => s.user);
  return users
}

//this will emit the users, the client can listen to this event to know all of the connected users
const emitUsers = () => {
  io.emit("users", getUsers());
};


//this is the fucntion that runs as socket is connected. 
io.on('connection', function(socket){
  let username = "username"
  // the server will listen for a new user. 
  socket.on("new_user", user => {
    console.log("The user connected is: " + user)
    let username = user
    socket.user = user;
    emitUsers()
  })

  socket.on('typer', username =>{
    let clients = io.sockets.clients().connected;
    let sockets = Object.values(clients);
    sockets.forEach(socket => {
      if(socket.user != username)
        socket.emit("nosy", username);
    });
  });



  socket.on("disconnect", function(){
    console.log("the user: " + username + " disconnected")
    emitUsers()
  })
  socket.on('chat message', function(msg){
    io.emit('chat message', msg);
  });

});

http.listen(port, function(){
  console.log('listening on *:' + port);
});
