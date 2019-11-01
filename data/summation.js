$(document).ready(function() {
    const slider = new Slider("#slider", { reversed : true });
    const startButton = document.getElementById('start');
    const stopButton = document.getElementById('stop');
    const nextButton = document.getElementById('next');      
    const sound = document.getElementById('sound');

    //const play = setInterval(audio, 3000);

    function audio(sec) {
      play = setInterval(function() {
        $('audio')[0].play();
      }, sec);
    }
    function tx_VAS(sec) {
      tx_interval = setInterval(function() {
        sendData(2)
      },sec);
    }

    slider.on("change", function(sliderValue) {
        document.getElementById("slider-value").textContent = sliderValue.newValue;
        vas_value = sliderValue.newValue;
    });
    
    startButton.addEventListener('click', function(e) {
      e.preventDefault;
      startButton.style.display = 'none';
      stopButton.style.display = 'block';
      audio(vas_freq_value * 1000);
      sendStartData();
      tx_VAS(vas_freq_value * 1000);
    });

    stopButton.addEventListener('click', function(e) {
      e.preventDefault;
      //startButton.style.display = 'block';
      stopButton.style.display = 'none';
      nextButton.style.display = 'block';
      clearInterval(play);
      clearInterval(tx_interval);      
      sendData(2);
      sendStopData();
    });
       
    
  });
  
   
  
    
    
    
  
  
  
  
  
  
  