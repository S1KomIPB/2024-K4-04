// Function to create a card element
function createCard(sensor, isNewest = false) {
    const card = document.createElement('div');
    card.className = isNewest ? 'bg-white px-12 py-6 rounded-lg shadow-md text-center text-2xl' : 'bg-white p-4 rounded-lg shadow-md';
    card.innerHTML = isNewest ? `
        <h3 class="text-2xl font-bold mb-2">Latest</h3>
        <p>Temperature: ${sensor.temperature}°C</p>
        <p>Light: ${sensor.light}%</p>
        <p>Distance: ${sensor.distance}</p>
        <p>Date: ${sensor.date}</p>
        <p>Time: ${sensor.time}</p>
    ` : `
        <h3 class="text-xl font-bold mb-2">Sensor: ${sensor.sensor}</h3>
        <p>Temperature: ${sensor.temperature}°C</p>
        <p>Light: ${sensor.light}%</p>
        <p>Distance: ${sensor.distance}</p>
        <p>Date: ${sensor.date}</p>
        <p>Time: ${sensor.time}</p>
    `;
    return card;
}

// Function to fetch sensor data from the API
async function fetchSensorData() {
    try {
        const response = await fetch('/get');
        if (!response.ok) {
            throw new Error('Network response was not ok ' + response.statusText);
        }
        const data = await response.json();
        return data;
    } catch (error) {
        console.error('Failed to fetch sensor data:', error);
        return [];
    }
}

// Function to initialize the dashboard
async function initDashboard() {
    const dashboard = document.getElementById('dashboard');
    const data = await fetchSensorData();
    const searchInput = document.getElementById('search');

    function renderDashboard(filteredData) {
        dashboard.innerHTML = '';

        if (filteredData.length > 0) {
            const newestSensor = filteredData[0];
            const newestCard = createCard(newestSensor, true);
            const newestWrapper = document.createElement('div');
            newestWrapper.className = 'flex justify-center mb-4';
            newestWrapper.appendChild(newestCard);
            dashboard.appendChild(newestWrapper);

            const otherSensors = filteredData.slice(1);
            const gridWrapper = document.createElement('div');
            gridWrapper.className = 'grid grid-cols-1 md:grid-cols-2 lg:grid-cols-3 gap-4';
            otherSensors.forEach(sensor => {
                const card = createCard(sensor);
                gridWrapper.appendChild(card);
            });
            dashboard.appendChild(gridWrapper);
        }
    }

    renderDashboard(data);

    searchInput.addEventListener('input', (e) => {
        const query = e.target.value.toLowerCase();
        const filteredData = data.filter(sensor => sensor.sensor.toLowerCase().includes(query));
        renderDashboard(filteredData);
    });
}

// Initialize the dashboard when the DOM is fully loaded
document.addEventListener('DOMContentLoaded', initDashboard);
