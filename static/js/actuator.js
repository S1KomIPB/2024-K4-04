document.getElementById('ledOn').addEventListener('click', fetchLedOn);
document.getElementById('ledOff').addEventListener('click', fetchLedOff);

function getIpAddress() {
    return document.getElementById('ipAddress').value.trim();
}

function fetchLedOn() {
    const ipAddress = getIpAddress();
    if (!ipAddress) {
        document.getElementById('content').innerText = 'Please enter a valid IP address.';
        return;
    }
    fetch(`http://${ipAddress}/led/on`)
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok ' + response.statusText);
            }
            document.getElementById('content').innerText = 'LED is on';
        })
        .catch(error => {
            document.getElementById('content').innerText = 'Fetch error: ' + error;
        });
}

function fetchLedOff() {
    const ipAddress = getIpAddress();
    if (!ipAddress) {
        document.getElementById('content').innerText = 'Please enter a valid IP address.';
        return;
    }
    fetch(`http://${ipAddress}/led/off`)
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok ' + response.statusText);
            }
            document.getElementById('content').innerText = 'LED is off';
        })
        .catch(error => {
            document.getElementById('content').innerText = 'Fetch error: ' + error;
        });
}

function fetchMelodyA() {
    const ipAddress = getIpAddress();
    if (!ipAddress) {
        document.getElementById('content').innerText = 'Please enter a valid IP address.';
        return;
    }
    fetch(`http://${ipAddress}/melodyA`)
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok ' + response.statusText);
            }
            document.getElementById('content').innerText = 'Melody A is on';
        })
        .catch(error => {
            document.getElementById('content').innerText = 'Fetch error: ' + error;
        });
}

function fetchMelodyB() {
    const ipAddress = getIpAddress();
    if (!ipAddress) {
        document.getElementById('content').innerText = 'Please enter a valid IP address.';
        return;
    }
    fetch(`http://${ipAddress}/melodyB`)
        .then(response => {
            if (!response.ok) {
                throw new Error('Network response was not ok ' + response.statusText);
            }
            document.getElementById('content').innerText = 'Melody B is on';
        })
        .catch(error => {
            document.getElementById('content').innerText = 'Fetch error: ' + error;
        });
}