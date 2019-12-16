document.addEventListener("DOMContentLoaded", function(event) {

    //Selectors 
    const calibrationButton = document.querySelector('#calibration-button');   
    const calVal = document.querySelector('#cal-val');  
    const factorVal = document.querySelector('#factor-val');
    const factorButton = document.querySelector('#factor-button');  
    const plusButton = document.querySelector('#plus-button');
    const minusButton = document.querySelector('#minus-button');
    const weightButton = document.querySelector('#weight-button');
    const weightValue = document.querySelector('#known-val');
    const resetWifiButton = document.querySelector('#reset-wifi-button');
    const saveButton = document.querySelector('#save-button');
    //Selectors

    let factor_once = true;    

    const webSocket = new WebSocket('ws://'+ document.location.host +'/settings'); //Fill in URL    

        webSocket.onopen = () => {
        console.log('Connection established');      
        }

        webSocket.onclose = () => {
        console.log('Connection closed');
        }
        webSocket.onmessage = function(event){
            console.log('data received');        
            var msg = JSON.parse(event.data);
            console.log(msg);

            switch(msg.type) {
                case "measurement":
                    calVal.value = msg.measurement;
                    console.log('in switch');
                    console.log(msg);
                    console.log(msg.measurement);
                    if(factor_once){
                        factorVal.value = msg.factor;
                        factor_once = false;
                    }
                    break;
            }
            //create a JSON object
            //var jsonObject = JSON.parse(event.data);
            //var value = jsonObject.value;
            //var message = jsonObject.message;       
            
            //calVal.value = value;
            //console.log(value);         
        };
        webSocket.onerror = (error) => {
        console.error(error);
        } 
    
    function sendCalStartData() {
        var data = {
        type: "Start"        
        };
        console.log('start sent function');
        webSocket.send(JSON.stringify(data));
        }
    function sendFactorData() {
        var data = {
        type: "factor",
        factor: factorVal.value        
        };        
        console.log('factor sent function');
        webSocket.send(JSON.stringify(data));
    }
    function sendPlusData() {
        var data = {
        type: "+"       
        };
        factor_once = true;
        console.log('plus sent function');
        webSocket.send(JSON.stringify(data));
    }
    function sendMinusData() {
        var data = {
        type: "-"      
        };
        factor_once = true;
        console.log('minus sent function');
        webSocket.send(JSON.stringify(data));
    }
    function sendKnownWeightData(){
        var data = {
        type: "calWeight",
        weight: weightValue.value
        };
        console.log('known weight sent function');
        console.log(data);
        webSocket.send(JSON.stringify(data));
    }

    function sendSaveData() {
        var data = {
        type: "Stop"      
        };
        console.log('Save sent function');
        webSocket.send(JSON.stringify(data));
    }
    function sendWifiResetData() {
        var data = {
        type: "Reset"       
        };
        console.log('reset wifi sent function');
        webSocket.send(JSON.stringify(data));
    }  
  
    calibrationButton.addEventListener('click', function(e) {
        sendCalStartData();
    });

    factorButton.addEventListener('click', function(e) {
        sendFactorData();
    });

    plusButton.addEventListener('click', function(e) {
        sendPlusData();
    });

    minusButton.addEventListener('click', function(e) {
        sendMinusData();
    });

    weightButton.addEventListener('click', function(e) {
        sendKnownWeightData();
    });
  
    resetWifiButton.addEventListener('click', function(e) {
      sendWifiResetData();
    });

    saveButton.addEventListener('click', function(e) {
        sendSaveData();
      });
  
  
      
  });
  
  //Functions
  
  //Functions
  