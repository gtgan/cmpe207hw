// this is the script that is running on the client end. 

/* This will render the table body.
It does so by mapping the each visitor in the state array to a tabel row with each of it's properties.*/

username = prompt("Hey! welcome to the chat server! What is your username?")
$(function () {
    var socket = io();
    //the client will emit the new user event with the visitor info to the server
    socket.emit("new_user", username);

    //   THIS IS THE TYPING INDICATOR CODE
    var textarea = $('#m');
    var typingStatus = $('#typer');
    var lastTypedTime = new Date(0); // it's 01/01/1970, actually some time in the past
    var typingDelayMillis = 5000; // how long user can "think about his spelling" before we show "No one is typing -blank space." message

    function refreshTypingStatus() {

      if (!textarea.is(':focus') ||
          textarea.val() == '' ||
          new Date().getTime() - lastTypedTime.getTime() > typingDelayMillis) {
        // console.log("No one is typing")
      } else {
        console.log(username + " is typing")
        socket.emit('typer', username);
      }
    }

    function updateLastTypedTime() {
      lastTypedTime = new Date();
    }

    setInterval(refreshTypingStatus, 100);
    textarea.keypress(updateLastTypedTime);


    socket.on("nosy", user => {
      userHTML = "<p>"+user+" is typing</p>"
      userString = user+" is typing"
      var children = $('#typer').children();

      if($('#typer').children().length > 0){
        children.each(function () {
          if(!$(this).text().includes(userString)){
            $('#typer').append(userHTML)
          }

        });
      } else {
        $('#typer').append(userHTML)
      }
    
    })
    // end of the indicator code

    //the client will listen for users added or leaving
    socket.on("users", users =>{
        //Need to update the online users view. 
        $('#daBod').empty();
        users.forEach(user => {
            $('#daBod').append("<tr><td>"+ user+"</td><td>:)</td></tr>");
        });
    })

    // this function will call the 'chat message' event with the message when the user submits the form. 
    $('form').submit(function(){
      socket.emit('chat message', $('#m').val());
      $('#m').val('');
      $('#typer').empty()
      return false;
    });

  
    
    // the client is listening for the 'chat message' event to be emiited. once it is, it will append the message as a <li>
    socket.on('chat message', function(msg){
      $('#messages').append($('<li>').text(msg));
      window.scrollTo(0, document.body.scrollHeight);
    });   
  });
