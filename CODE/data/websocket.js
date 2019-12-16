//$.blockUI({ message: null });

var start = false;
var stop = false;
var set = false;
var reset = false;
var plus = false;
var minus = false;
var trx = false;
var id_value;
var prev_id_value;
var vas_value = 0;
var measure_value = 0;
var ms_value = 0;
var roc_value = 0;
var max_press_value = 0;
var vas_freq_value = 0;
var factor_value = 0;
var weight_value = 0;


function connect(){
  webSocket = new WebSocket('ws://'+ document.location.host +':81');

  webSocket.onopen = () => {
    $.unblockUI();
    console.log('Connection established');
    var connected_event = new CustomEvent('WS_connected');
    document.dispatchEvent(connected_event);
  };
  
  webSocket.onclose = () => {
    $.blockUI({ message: null });
    console.log('Connection closed');
    setTimeout(function() {
      connect();
    }, 1000);
  };

  webSocket.onmessage = function(event){
    console.log('data received');        
    var msg = JSON.parse(event.data);
    console.log(msg);
  
    switch(msg.type) {
        case "start":
            //start = true;
            var start_event = new CustomEvent('WS_start');
            document.dispatchEvent(start_event);
            break;
        case "stop":
            //stop = true;
            var stop_event = new CustomEvent('WS_stop');
            document.dispatchEvent(stop_event);
            break;
        case "set":
          var set_event = new CustomEvent('WS_set');
          document.dispatchEvent(set_event);
            break;
        case "reset":
            var reset_event = new CustomEvent('WS_reset');
            document.dispatchEvent(reset_event);
            break;
        case "plus":
            var plus_event = new CustomEvent('WS_plus');
            document.dispatchEvent(plus_event);
            break;
        case "minus":
            var minus_event = new CustomEvent('WS_minus');
            document.dispatchEvent(minus_event);
            break;
        case "factor":
            factor_value = msg.factor;
            var factor_event = new CustomEvent('WS_factor');
            document.dispatchEvent(factor_event);
            break;
        case "measure":
            measure_value = msg.measure;
            var measure_event = new CustomEvent('WS_measure');
            document.dispatchEvent(measure_event);
            break;
        case "vas_freq":
            vas_freq_value = msg.vas_freq;
            var vas_freq_event = new CustomEvent('WS_vas_freq');
            document.dispatchEvent(vas_freq_event);
            break;
        case "measurement":
            measure_value = msg.measurement;
            factor_value = msg.factor;
            var cal_data_event = new CustomEvent('WS_cal_data');
            document.dispatchEvent(cal_data_event);
        case "raw":
            measure_value = msg.measure;
            ms_value = msg.milli;
            roc_value = msg.roc;
            var raw_event = new CustomEvent('WS_raw');
            document.dispatchEvent(raw_event);
            break;  
        case "weight":
            weight_value = msg.weight;
            var weight_event = new CustomEvent('WS_weight');
            document.dispatchEvent(weight_event);
            break;
        case "settings":
            roc_value = msg.target_roc;
            max_press_value = msg.max_pressure;
            vas_freq_value = msg.vas_freq;
            var settings_event = new CustomEvent('WS_settings');            
            document.dispatchEvent(settings_event);
            break;
        case "id":
            id_value = msg.id;
            prev_id_value = msg.prev_id;
            var id_event = new CustomEvent('WS_id');
            document.dispatchEvent(id_event);
            break;
    }        
  };

  webSocket.onerror = (error) => {
    console.error('Socket encountered error: ' + error.message + '. Closing socket.');
    webSocket.onclose();        
  };
};


function sendStartData() {
  var data = {
  type: "Start",    
  //reading: measurementNumber 
  };
  console.log('Start sent function');
  webSocket.send(JSON.stringify(data));
}  
function sendStopData() {
  var data = {
  type: "Stop"    
  };
  console.log('Stop sent function');
  webSocket.send(JSON.stringify(data));
}
function sendSetData() {
  var data = {
  type: "Set"    
  };
  console.log('Set sent function');
  webSocket.send(JSON.stringify(data));
}
function sendResetData() {
  var data = {
  type: "Reset"    
  };
  console.log('Reset sent function');
  webSocket.send(JSON.stringify(data));
}
function sendPlusData() {
  var data = {
  type: "Plus"
  };
  factor_once = true;
  console.log('plus sent function');
  webSocket.send(JSON.stringify(data));
}
function sendMinusData() {
  var data = {
  type: "Minus"
  };
  factor_once = true;
  console.log('minus sent function');
  webSocket.send(JSON.stringify(data));
}
function sendMaxOnData() {
  var data = {
  type: "Raw_max_on"
  };
  console.log('max on sent function');
  webSocket.send(JSON.stringify(data));
}
function sendMaxOffData() {
  var data = {
  type: "Raw_max_off"
  };
  console.log('max off sent function');
  webSocket.send(JSON.stringify(data));
}
function sendRocOnData() {
  var data = {
  type: "Raw_roc_on"
  };
  console.log('roc on sent function');
  webSocket.send(JSON.stringify(data));
}
function sendRocOffData() {
  var data = {
  type: "Raw_roc_off"
  }; 
  console.log('roc off sent function');
  webSocket.send(JSON.stringify(data));
}

function sendData(data_type) {
  if(data_type === 1){

    var data = {
      type: "Measurement",
      kPa: measure_value, //kPa measurement from strain gauge
      ms: ms_value
      };

      console.log('Measurement data sent function');
      console.log(data);
  }
  else if(data_type === 2){
    var data = {
      type: "VAS",
      vas: vas_value
      };

      console.log('VAS data sent function');
      console.log(data);
  } 
  else if(data_type === 3){
    var data = {
      type: "Settings",
      roc: roc_value,
      max_press: max_press_value,
      vas_freq: vas_freq_value
    }
  }
  else if(data_type === 4){
    var data = {
      type: "ID",
      id: id_value
    }
  }
  else if(data_type === 5) {
    var data = {
    type: "factor",
    factor: factor_value
    };
    console.log('factor sent function');
    //webSocket.send(JSON.stringify(data));
  }
  else if(data_type === 6){
    var data = {
      type: "weight",
      weight: weight_value
      };
      console.log('known weight sent function');
      console.log(data);
      //webSocket.send(JSON.stringify(data));
  }
  webSocket.send(JSON.stringify(data));
}




document.addEventListener("DOMContentLoaded", function(event) {
  
  //const painButton = document.querySelector('#pain-button');
  //Invoke WebSocket
  ws();

  //WebSocket connection
  function ws () {
    if ('WebSocket' in window) {    
      connect();
    } else {
      // The browser doesn't support WebSocket
      alert('WebSocket not supported by your browser');
    }
  }  
});

