document.getElementById('ledOn').addEventListener('click', fetchLedOn);
document.getElementById('ledOff').addEventListener('click', fetchLedOff);


function fetchLedOn() {
    fetch('http://192.168.208.91/led/on')  // Example URL
        .then(response => {
            document.getElementById('content').innerText = 'led is on';
            if (!response.ok) {
                throw new Error('Network response was not ok ' + response.statusText);
            }
            document.getElementById('content').innerText = 'led is on';
        });
}

function fetchLedOff() {
    fetch('http://192.168.208.91/led/off')  // Example URL
        .then(response => {
            document.getElementById('content').innerText = 'led is off';
            if (!response.ok) {
                throw new Error('Network response was not ok ' + response.statusText);
            }
            document.getElementById('content').innerText = 'led is off';
        });
}
