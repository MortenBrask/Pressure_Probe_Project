//Selectors
const toggle = document.querySelectorAll('.toggle-buttons');
const painButton = document.querySelector('#pain-button');
const readyButton = document.querySelector('#ready-button');
const resetButton = document.querySelector('#reset-button');
const nextButton = document.querySelector('#next');
const measureOne = document.querySelector('#measure-one');
const measureTwo = document.querySelector('#measure-two');
const measureThree = document.querySelector('#measure-three');
//Selectors

document.addEventListener('WS_stop', stopHandler, false);

function stopHandler() {
	painButton.click();
}

document.addEventListener("DOMContentLoaded", function(event) {  

  //Variables
  let counter = 0;
  let countdown;
  //Variables

  // Events
  readyButton.addEventListener('click', function(e) {
    e.preventDefault;
    readyButton.style.display = 'none';
    painButton.style.display = 'block';
    sendStartData();
  });

  painButton.addEventListener('click', function(e) {
    e.preventDefault;

    counter++;

    switch (counter <= 3) {
      case counter === 1:
        measureOne.style.display = 'block';
        painButton.style.display = 'none';
        readyButton.textContent = 'I am ready for the test';
        readyButton.style.display = 'block';
        resetButton.style.display = 'inline-block';
        readyButton.removeAttribute('disabled');
        sendStopData();
        
        break;
      case counter === 2:
        measureTwo.style.display = 'block';
        painButton.style.display = 'none';
        readyButton.textContent = 'I am ready for the test';
        readyButton.style.display = 'block';
        resetButton.style.display = 'inline-block';
        readyButton.removeAttribute('disabled');
        sendStopData();
        break;
      case counter === 3:
        measureThree.style.display = 'block';
        painButton.textContent = 'Finished test';
        painButton.setAttribute('disabled', 'disabled');
        resetButton.style.display = 'inline-block';
        nextButton.style.display = 'inline-block';
        sendStopData();
    }

    return counter;
  });

  resetButton.addEventListener('click', function(e) {
    switch (counter <= 3) {
      case counter === 1:
        measureOne.style.display = 'none';
        resetButton.style.display = 'none';
        counter--;
        sendResetData();
        
        break;
      case counter === 2:
        measureTwo.style.display = 'none';
        resetButton.style.display = 'none';
        counter--;
        sendResetData();
        
        break;
      case counter === 3:
        measureThree.style.display = 'none';
        resetButton.style.display = 'none';
        painButton.style.display = 'none';
        painButton.textContent = 'I start to feel pain';
        painButton.removeAttribute('disabled');
        readyButton.textContent = 'I am ready for the test';
        readyButton.style.display = 'block';
        nextButton.style.display = 'none';
        counter--;
        sendResetData();
        
        break;
    }
  });
  // Events

  //Functions
  function toggleButtons() {
    toggle.forEach(function(element) {
      if (element.style.display === "none") {
        element.style.display = "inline-block";
        painButton.setAttribute('disabled', 'disabled');
      }
    });
  }  
});





