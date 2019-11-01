
const saveButton = document.querySelector('#save-button');
const targetROC = document.querySelector('#target-roc');
const maxPressure = document.querySelector('#max-pressure');
const vasFrequency = document.querySelector('#vas-frequence');



document.addEventListener('WS_settings', settingsHandler, false);

let once = 0;

function settingsHandler() {
  targetROC.value = roc_value;
  maxPressure.value = max_press_value;
  vasFrequency.value = vas_freq_value;  
}

(function() {
  'use strict';
  window.addEventListener('load', function() {
  // Fetch all the forms we want to apply custom Bootstrap validation styles to
  var forms = document.getElementsByClassName('needs-validation');
  // Loop over them and prevent submission
  var validation = Array.prototype.filter.call(forms, function(form) {
  form.addEventListener('submit', function(event) {
    event.preventDefault();
    event.stopPropagation();
  if (form.checkValidity() === false) {    
  event.preventDefault();
  event.stopPropagation();
  }
  else if (form.checkValidity() === true) {  
  roc_value = targetROC.value;
  max_press_value = maxPressure.value;
  vas_freq_value = vasFrequency.value;
  save(); 
  }
  form.classList.add('was-validated');
  }, false);
  });
  }, false);
  })();


function save(event) {
    $('#alert').addClass('show');
    $.blockUI({ message: '<h1>CONFIGURATION SUCCESFULLY SAVED!</h1>'});
    sendData(3);

    setTimeout(function() {
      $("#alert").alert('close');
    }, 2000);

    $('#alert').on('closed.bs.alert', function () {
      window.location.href = "index.html";
    });
  };

