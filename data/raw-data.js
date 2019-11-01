const kpa_output = document.querySelector('#probe-kpa');
const kg_output = document.querySelector('#probe-kg');
const ms_output = document.querySelector('#probe-ms');
const roc_output = document.querySelector('#probe-roc');
const save_data = document.querySelector('#save');
const max_onoff = document.querySelector('#max');
//const roc_onoff = document.querySelector('#roc');
const downloadButton = document.querySelector('#download');
const frontpageButton = document.querySelector('#frontpage');

document.addEventListener('WS_raw', rawHandler, false);

function rawHandler() {
    kpa_output.value = measure_value;
    kg_output.value = measure_value / 98.0665 //divide to go from kpa to kilogram pr cm^2
    ms_output.value = ms_value;
    roc_output.value = roc_value;
}

document.addEventListener("DOMContentLoaded", function(event) {

    save_data.addEventListener('change', function(){
        //e.preventDefault;
        if(save_data.checked == true){
            sendStartData();
        }
        else{
            sendStopData();
        }
    });

    max_onoff.addEventListener('change', function(){
        //e.preventDefault;
        if(max_onoff.checked == true){
            sendMaxOnData();
        }
        else {
            sendMaxOffData();
        }        
    });

    // roc_onoff.addEventListener('change', function(){
    //     //e.preventDefault;
    //     if(roc_onoff.checked == true){
    //         sendRocOnData();
    //     }
    //     else {
    //         sendRocOffData();
    //     }
    // });

    downloadButton.addEventListener('click', function(){
        sendStopData();
        setTimeout(function() {
            window.location.href = "download.html";
          }, 2000);               
    });

    frontpageButton.addEventListener('click', function(){
        sendStopData();
        window.location.href = "index.html";        
    });

});