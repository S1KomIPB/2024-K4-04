// Function to create a card element
function createCard(sensor) {
    const card = document.createElement('div');
    card.className = 'bg-white px-12 py-6 rounded-lg shadow-md text-center text-2xl';
    card.innerHTML = `
        <h3 class="text-2xl font-bold mb-2">Latest</h3>
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
        const response = await fetch('/get?limit=50'); // Removed the limit parameter
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

// Function to create a table row element
function createTableRow(sensor, index) {
    const row = document.createElement('tr');
    row.innerHTML = `
        <td class="border px-4 py-2">${index + 1}</td>
        <td class="border px-4 py-2">${sensor.date}</td>
        <td class="border px-4 py-2">${sensor.time}</td>
        <td class="border px-4 py-2">${sensor.temperature}°C</td>
        <td class="border px-4 py-2">${sensor.light}%</td>
        <td class="border px-4 py-2">${sensor.distance}</td>
    `;
    return row;
}

// Function to create a chart for the given data and canvas ID
function createChart(ctx, labels, data, label, yAxisLabel, backgroundColor, borderColor) {
    return new Chart(ctx, {
        type: 'line',
        data: {
            labels: labels,
            datasets: [{
                label: label,
                data: data,
                borderColor: borderColor,
                backgroundColor: backgroundColor,
                fill: false
            }]
        },
        options: {
            scales: {
                x: {
                    title: {
                        display: true,
                        text: 'Time'
                    }
                },
                y: {
                    title: {
                        display: true,
                        text: yAxisLabel
                    }
                }
            }
        }
    });
}

// Function to initialize the dashboard
async function initDashboard() {
    const dashboard = document.getElementById('dashboard');
    const data = await fetchSensorData();
    const searchInput = document.getElementById('search');

    function renderDashboard(filteredData) {
        dashboard.innerHTML = '';

        if (filteredData.length > 0) {
            // Latest data section
            const newestSensor = filteredData[0];
            const newestCard = createCard(newestSensor);
            const newestWrapper = document.createElement('div');
            newestWrapper.className = 'flex justify-center mb-4';
            newestWrapper.appendChild(newestCard);
            dashboard.appendChild(newestWrapper);

            const otherSensors = filteredData.slice(1);

            // Extract data for charts
            const labels = otherSensors.map(sensor => sensor.time).reverse();
            const temperatureData = otherSensors.map(sensor => sensor.temperature).reverse();
            const lightData = otherSensors.map(sensor => sensor.light).reverse();
            const distanceData = otherSensors.map(sensor => sensor.distance).reverse();

            // Add Charts title
            const chartsTitle = document.createElement('div');
            chartsTitle.className = 'text-4xl font-bold mb-4 text-center';
            chartsTitle.textContent = 'Charts';
            dashboard.appendChild(chartsTitle);

            // Graph section
            const chartsContainer = document.createElement('div');
            chartsContainer.className = 'flex flex-row justify-between mb-8';

            // Create temperature chart
            const tempCanvasWrapper = document.createElement('div');
            tempCanvasWrapper.className = 'flex-1 text-center';
            const tempChartTitle = document.createElement('h4');
            tempChartTitle.className = 'text-2xl font-bold mb-2';
            tempChartTitle.textContent = 'Temperature';
            const tempCanvas = document.createElement('canvas');
            tempCanvas.id = 'temperatureChart';
            const tempCtx = tempCanvas.getContext('2d');
            tempCanvasWrapper.appendChild(tempChartTitle);
            tempCanvasWrapper.appendChild(tempCanvas);
            chartsContainer.appendChild(tempCanvasWrapper);
            createChart(tempCtx, labels, temperatureData, 'Temperature', 'Temperature (°C)', 'rgba(255, 99, 132, 0.2)', 'rgba(255, 99, 132, 1)');

            // Create light chart
            const lightCanvasWrapper = document.createElement('div');
            lightCanvasWrapper.className = 'flex-1 text-center';
            const lightChartTitle = document.createElement('h4');
            lightChartTitle.className = 'text-2xl font-bold mb-2';
            lightChartTitle.textContent = 'Light';
            const lightCanvas = document.createElement('canvas');
            lightCanvas.id = 'lightChart';
            const lightCtx = lightCanvas.getContext('2d');
            lightCanvasWrapper.appendChild(lightChartTitle);
            lightCanvasWrapper.appendChild(lightCanvas);
            chartsContainer.appendChild(lightCanvasWrapper);
            createChart(lightCtx, labels, lightData, 'Light', 'Light (%)', 'rgba(54, 162, 235, 0.2)', 'rgba(54, 162, 235, 1)');

            // Create distance chart
            const distanceCanvasWrapper = document.createElement('div');
            distanceCanvasWrapper.className = 'flex-1 text-center';
            const distanceChartTitle = document.createElement('h4');
            distanceChartTitle.className = 'text-2xl font-bold mb-2';
            distanceChartTitle.textContent = 'Distance';
            const distanceCanvas = document.createElement('canvas');
            distanceCanvas.id = 'distanceChart';
            const distanceCtx = distanceCanvas.getContext('2d');
            distanceCanvasWrapper.appendChild(distanceChartTitle);
            distanceCanvasWrapper.appendChild(distanceCanvas);
            chartsContainer.appendChild(distanceCanvasWrapper);
            createChart(distanceCtx, labels, distanceData, 'Distance', 'Distance', 'rgba(75, 192, 192, 0.2)', 'rgba(75, 192, 192, 1)');

            // Append the charts container to the dashboard
            dashboard.appendChild(chartsContainer);

            // Add Graphs title
            const tableTitle = document.createElement('div');
            tableTitle.className = 'text-4xl font-bold mb-4 text-center';
            tableTitle.textContent = 'History Table';
            dashboard.appendChild(tableTitle);

            // Table section
            const tableWrapper = document.createElement('div');
            tableWrapper.className = 'mb-8';

            const table = document.createElement('table');
            table.className = 'min-w-full bg-white mb-4';
            table.innerHTML = `
                <thead>
                    <tr>
                        <th class="bg-gray-200 text-gray-600 border px-4 py-2">Number</th>
                        <th class="bg-gray-200 text-gray-600 border px-4 py-2">Date</th>
                        <th class="bg-gray-200 text-gray-600 border px-4 py-2">Time</th>
                        <th class="bg-gray-200 text-gray-600 border px-4 py-2">Temperature</th>
                        <th class="bg-gray-200 text-gray-600 border px-4 py-2">Light</th>
                        <th class="bg-gray-200 text-gray-600 border px-4 py-2">Distance</th>
                    </tr>
                </thead>
                <tbody></tbody>
            `;

            const tbody = table.querySelector('tbody');
            otherSensors.forEach((sensor, index) => {
                const row = createTableRow(sensor, index);
                tbody.appendChild(row);
            });

            tableWrapper.appendChild(table);
            dashboard.appendChild(tableWrapper);
        }
    }

    renderDashboard(data);

    searchInput.addEventListener('input', (e) => {
        const query = e.target.value.toLowerCase();
        const filteredData = data.filter(sensor => 
            sensor.date.toLowerCase().includes(query) || 
            sensor.time.toLowerCase().includes(query) || 
            sensor.temperature.toString().toLowerCase().includes(query) || 
            sensor.light.toString().toLowerCase().includes(query) || 
            sensor.distance.toString().toLowerCase().includes(query)
        );
        renderDashboard(filteredData);
    });
}

// Initialize the dashboard when the DOM is fully loaded
document.addEventListener('DOMContentLoaded', initDashboard);
