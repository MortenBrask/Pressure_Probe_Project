$(document).ready(function() {
    const slider = new Slider("#slider", { reversed : true });
    const startButton = document.getElementById('start');
    const stopButton = document.getElementById('stop');      
    const sound = document.getElementById('sound');

    //const play = setInterval(audio, 3000);

    function audio(sec) {
      play = setInterval(function() {
        $('audio')[0].play();
      }, sec);
    }

    slider.on("change", function(sliderValue) {
        document.getElementById("slider-value").textContent = sliderValue.newValue;
        vas_value = sliderValue.newValue;
    });
    
    startButton.addEventListener('click', function(e) {
      e.preventDefault;
      startButton.style.display = 'none';
      stopButton.style.display = 'block';
      audio(3000);
      sendStartData();
    });

    stopButton.addEventListener('click', function(e) {
      e.preventDefault;
      startButton.style.display = 'block';
      stopButton.style.display = 'none';
      clearInterval(play);
      sendStopData();
      sendData(2);
    });

    
    
  });
  
   
  
    
    
    
  
  
  
  
  
  
  