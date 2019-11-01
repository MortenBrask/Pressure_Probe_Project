document.addEventListener("DOMContentLoaded", function(event) {
  
  //const webSocket = new WebSocket('ws://'+ document.location.host +'/video'); //Fill in URL
  
  webSocket.onopen = () => {
    console.log('Connection established');      
  }

  webSocket.onclose = () => {
    console.log('Connection closed');
  }
  webSocket.onmessage = function(event){
    console.log('data received');        
              
  }
  webSocket.onerror = (error) => {
    console.error(error);
  }   
  
});

  