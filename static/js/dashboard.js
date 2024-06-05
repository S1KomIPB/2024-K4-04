// Function to create a card element
function createCard(sensor) {
    const card = document.createElement('div');
    card.className = 'bg-white p-4 rounded-lg shadow-md';
    card.innerHTML = `
        <h2 class="text-xl font-bold mb-2">Sensor: ${sensor.sensor}</h2>
        <p>API Key: ${sensor.api_key}</p>
        <p>Temperature: ${sensor.temperature}°C</p>
        <p>Humidity: ${sensor.humidity}%</p>
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
        filteredData.forEach(sensor => {
            const card = createCard(sensor);
            dashboard.appendChild(card);
        });
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
