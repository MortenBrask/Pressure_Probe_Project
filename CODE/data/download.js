//Selectors
const downloadTest = document.querySelector('#dropTest');

//Selectors
document.addEventListener('WS_set', setHandler, false);

function setHandler() {
    window.location.href = "downloadtest.html";
}

document.addEventListener("DOMContentLoaded", function(event) {

    downloadTest.addEventListener('click', function(e) {
        e.preventDefault;
        sendStartData();
    });  
  //Functions
});