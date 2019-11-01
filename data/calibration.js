//Selectors
const calibrationButton = document.querySelector('#calibrate-button');
const startButton = document.querySelector('#start-button');
const measureVal = document.querySelector('#measure-val');
const factorVal = document.querySelector('#factor-val');
const factorButton = document.querySelector('#factor-button');
const plusButton = document.querySelector('#plus-button');
const minusButton = document.querySelector('#minus-button');
const weightButton = document.querySelector('#weight-button');
const weightValue = document.querySelector('#known-val');
const resetButton = document.querySelector('#restart-calibration-button');
const saveButton = document.querySelector('#save-button');
const heading = document.querySelector('#heading');
const calText = document.querySelector('#calibration-text');
//Selectors


document.addEventListener('WS_factor', factorHandler, false);

function factorHandler() {
  factorVal.value = factor_value;
  //console.log("send id");
}

document.addEventListener('WS_weight', weightHandler, false);

function weightHandler() {
  weightValue.value = weight_value;
  //console.log("send id");
}

document.addEventListener('WS_measure', measureHandler, false);

function measureHandler() {
  measureVal.value = measure_value;
}

document.addEventListener("DOMContentLoaded", function(event) {             

    startButton.addEventListener('click', function(e) {
        sendStartData();
        heading.innerHTML = "Initializing calibration"
        calText.innerHTML = "Place the known weight on the probe tip, using the optionally supplied platform decree. Press 'CALIBRATE' when ready";
        calibrationButton.style.display = 'block';
        startButton.style.display = 'none';
    });

    calibrationButton.addEventListener('click', function(e) {
      sendSetData();
      heading.innerHTML = "Calibration running"
      calText.innerHTML = "Using '+' and '-' buttons, it is possible to fine tune the calibration, or simply input a value in the calibration factor field, and press 'SET'. In most cases this is not necessary. Press 'Save' when finished";
      calibrationButton.style.display = 'none';
  });

    factorButton.addEventListener('click', function(e) {
      factor_value = factorVal.value;  
      sendData(5);
    });

    plusButton.addEventListener('click', function(e) {
        sendPlusData();
    });

    minusButton.addEventListener('click', function(e) {
        sendMinusData();
    });

    weightButton.addEventListener('click', function(e) {
      weight_value = weightValue.value;  
      sendData(6);
    });

    resetButton.addEventListener('click', function(e) {
      sendResetData();
      heading.innerHTML = "Calibration";
      calText.innerHTML = "The calibration procedure is initiated when pressing start, make sure there is NO weight on the probe. Follow the instruction shown here for correct calibration procedure."
      startButton.style.display = 'block';
      calibrationButton.style.display = 'none';
    });

    saveButton.addEventListener('click', function(e) {
        sendStopData();
      });
  });
