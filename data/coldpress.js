document.addEventListener("DOMContentLoaded", function(event) {

  //Selectors
  const painButton = document.querySelector('#pain-button');
  const stopButton = document.querySelector('#stop-button');
  const readyButton = document.querySelector('#ready-button');
  const startButton = document.querySelector('#start-button');
  const resetButton = document.querySelector('#reset-button');
  const nextButton = document.querySelector('#next');
  const vas = document.querySelector('#test');
  const sec = document.querySelector('#seconds');
  const measureOne = document.querySelector('#measure-one');
  
  const slider = new Slider("#slider", { reversed : true });
        
  //Selectors
  document.addEventListener('WS_connected', connectedHandler, false);
  document.addEventListener('WS_stop', stopHandler, false);
  //Variables
  let counter = 0;
  let countdown;
  let interval;
  //Variables

  startButton.setAttribute = 'disabled';

  function connectedHandler(){
    startButton.removeAttribute = 'disabled';
  }
  function stopHandler(){
    clear();
    nextButton.style.display = 'inline-block';
  }

  slider.on("change", function(sliderValue) {
    document.getElementById("slider-value").textContent = sliderValue.newValue;
    vas_value = sliderValue.newValue;
  });

  startButton.addEventListener('click', function(e) {
    e.preventDefault;
    startButton.style.display = 'none';
    stopButton.style.display = 'block';
    vas.style.display = 'block';
    sendStartData();
    
    dataTimer(vas_freq_value * 1000);
    timer(120);    
  });

  readyButton.addEventListener('click', function(e) {
    e.preventDefault;
    readyButton.style.display = 'none';
    stopButton.style.display = 'block';
    vas.style.display = 'block';
    sec.textContent = 0;    
  });

  stopButton.addEventListener('click', function(e) {
    e.preventDefault;

    stopButton.style.display = 'none';
    stopButton.setAttribute = 'disabled';
    painButton.style.display = 'block';
    sendResetData();
    clear();
  });

  painButton.addEventListener('click', function(e) {
    e.preventDefault;

    counter++;
    clear();

    switch (counter <= 3) {
      case counter === 1:
        measureOne.style.display = 'block';
        painButton.style.display = 'none';        
        nextButton.style.display = 'inline-block';

        sendStopData();
        break;      
    }
    return counter;
  });

  resetButton.addEventListener('click', function(e) {
    switch (counter <= 3) {
      case counter === 1:
        measureOne.style.display = 'none';
        resetButton.style.display = 'none';
        counter--;
        break;      
    }
  });

  //Functions
  function toggleButtons() {
    toggle.forEach(function(element) {
      if (element.style.display === "none") {
        element.style.display = "inline-block";
        painButton.setAttribute('disabled', 'disabled');
      }
    });
  }
  
  function dataTimer(sec) {
    interval = setInterval(function() {
      sendData(2)
    },sec);
  }

  function timer(seconds) {

    countdown = setInterval(() => {
      var secondsPassed = ++seconds % 120;

      if (secondsPassed === 100) {
        stopButton.style.display = 'none';        
        painButton.style.display = 'block';
        sendSetData();
      }

      if (secondsPassed === 119) {
        clear();
        secondsPassed++;
        nextButton.style.display = 'inline-block';
        sendStopData();
      }
      displayTimeLeft(secondsPassed);
    }, 1000);
  }

  function displayTimeLeft(seconds) {
    const remainingSeconds = seconds;
    const display = `${remainingSeconds}`;
    sec.textContent = display;
    
  }

  function clear() {
    clearInterval(countdown);
    clearInterval(interval);
  }
  //Functions

});
