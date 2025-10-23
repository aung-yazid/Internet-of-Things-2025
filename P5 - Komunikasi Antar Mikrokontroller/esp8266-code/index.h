#ifndef INDEX_H
#define INDEX_H

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
<head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Arduino ESP8266 Control</title>
    <style>
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            min-height: 100vh;
            display: flex;
            justify-content: center;
            align-items: center;
            padding: 20px;
        }
        
        .container {
            max-width: 600px;
            width: 100%;
            background: white;
            padding: 40px;
            border-radius: 20px;
            box-shadow: 0 20px 60px rgba(0,0,0,0.3);
            animation: fadeIn 0.5s ease-in;
        }
        
        @keyframes fadeIn {
            from {
                opacity: 0;
                transform: translateY(20px);
            }
            to {
                opacity: 1;
                transform: translateY(0);
            }
        }
        
        h1 {
            color: #333;
            text-align: center;
            margin-bottom: 30px;
            font-size: 28px;
        }
        
        .sensor-card {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            padding: 25px;
            border-radius: 15px;
            margin-bottom: 25px;
            color: white;
            box-shadow: 0 5px 15px rgba(102, 126, 234, 0.4);
        }
        
        .sensor-item {
            display: flex;
            justify-content: space-between;
            align-items: center;
            margin: 15px 0;
            font-size: 18px;
        }
        
        .sensor-item:first-child {
            margin-top: 0;
        }
        
        .sensor-item:last-child {
            margin-bottom: 0;
        }
        
        .sensor-label {
            font-weight: 500;
            display: flex;
            align-items: center;
            gap: 8px;
        }
        
        .sensor-value {
            font-size: 24px;
            font-weight: bold;
            text-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        
        .led-control {
            background: #f8f9fa;
            padding: 25px;
            border-radius: 15px;
            text-align: center;
            margin-bottom: 20px;
        }
        
        .led-status {
            margin-bottom: 20px;
            font-size: 18px;
            color: #333;
            font-weight: 500;
        }
        
        .led-status-badge {
            display: inline-block;
            padding: 8px 20px;
            border-radius: 20px;
            font-weight: bold;
            font-size: 16px;
            margin-left: 10px;
            transition: all 0.3s ease;
        }
        
        .status-on {
            background: #d4edda;
            color: #155724;
            box-shadow: 0 0 10px rgba(76, 175, 80, 0.3);
        }
        
        .status-off {
            background: #f8d7da;
            color: #721c24;
        }
        
        .button-group {
            display: flex;
            gap: 15px;
            justify-content: center;
            margin-bottom: 15px;
        }
        
        .button {
            flex: 1;
            padding: 15px 25px;
            font-size: 16px;
            font-weight: bold;
            cursor: pointer;
            border: none;
            border-radius: 10px;
            color: white;
            transition: all 0.3s ease;
            box-shadow: 0 4px 10px rgba(0,0,0,0.2);
        }
        
        .button:hover {
            transform: translateY(-2px);
            box-shadow: 0 6px 15px rgba(0,0,0,0.3);
        }
        
        .button:active {
            transform: translateY(0);
            box-shadow: 0 2px 5px rgba(0,0,0,0.2);
        }
        
        .btn-on {
            background: linear-gradient(135deg, #11998e 0%, #38ef7d 100%);
        }
        
        .btn-on:hover {
            background: linear-gradient(135deg, #0d8072 0%, #2dd46b 100%);
        }
        
        .btn-off {
            background: linear-gradient(135deg, #ee0979 0%, #ff6a00 100%);
        }
        
        .btn-off:hover {
            background: linear-gradient(135deg, #d10867 0%, #e65f00 100%);
        }
        
        .download-section {
            background: #f8f9fa;
            padding: 25px;
            border-radius: 15px;
            margin-bottom: 20px;
        }
        
        .download-section h3 {
            color: #333;
            margin-bottom: 15px;
            font-size: 18px;
            text-align: center;
        }
        
        .download-buttons {
            display: flex;
            gap: 10px;
            justify-content: center;
        }
        
        .btn-download {
            flex: 1;
            padding: 12px 20px;
            font-size: 14px;
            font-weight: bold;
            cursor: pointer;
            border: none;
            border-radius: 8px;
            color: white;
            transition: all 0.3s ease;
            box-shadow: 0 3px 8px rgba(0,0,0,0.15);
        }
        
        .btn-excel {
            background: linear-gradient(135deg, #1e7e34 0%, #28a745 100%);
        }
        
        .btn-excel:hover {
            background: linear-gradient(135deg, #155724 0%, #1e7e34 100%);
            transform: translateY(-2px);
        }
        
        .btn-pdf {
            background: linear-gradient(135deg, #c82333 0%, #dc3545 100%);
        }
        
        .btn-pdf:hover {
            background: linear-gradient(135deg, #a71d2a 0%, #c82333 100%);
            transform: translateY(-2px);
        }
        
        .history-section {
            background: #ffffff;
            padding: 20px;
            border-radius: 15px;
            border: 2px solid #e9ecef;
            margin-bottom: 20px;
        }
        
        .history-section h3 {
            color: #333;
            margin-bottom: 15px;
            font-size: 18px;
            text-align: center;
        }
        
        .history-list {
            max-height: 200px;
            overflow-y: auto;
            font-size: 13px;
        }
        
        .history-item {
            padding: 8px;
            border-bottom: 1px solid #e9ecef;
            display: flex;
            justify-content: space-between;
        }
        
        .history-item:last-child {
            border-bottom: none;
        }
        
        .icon {
            margin-right: 5px;
        }
        
        .footer {
            text-align: center;
            margin-top: 20px;
            color: #666;
            font-size: 12px;
        }
        
        .connection-status {
            position: fixed;
            top: 20px;
            right: 20px;
            padding: 8px 15px;
            border-radius: 20px;
            font-size: 12px;
            font-weight: bold;
            background: #d4edda;
            color: #155724;
            box-shadow: 0 2px 5px rgba(0,0,0,0.1);
        }
        
        @media (max-width: 480px) {
            .container {
                padding: 25px;
            }
            
            h1 {
                font-size: 24px;
            }
            
            .sensor-value {
                font-size: 20px;
            }
            
            .button-group,
            .download-buttons {
                flex-direction: column;
            }
            
            .connection-status {
                top: 10px;
                right: 10px;
                font-size: 11px;
                padding: 6px 12px;
            }
        }
    </style>
</head>
<body>
    <div class='connection-status' id='connStatus'>● Connected</div>
    
    <div class='container'>
        <h1>🌡️ IoT Dashboard</h1>
        
        <div class='sensor-card'>
            <div class='sensor-item'>
                <span class='sensor-label'>
                    <span>🌡️</span>
                    <span>Temperature</span>
                </span>
                <span class='sensor-value' id='temp'>--</span>
            </div>
            <div class='sensor-item'>
                <span class='sensor-label'>
                    <span>💧</span>
                    <span>Humidity</span>
                </span>
                <span class='sensor-value' id='hum'>--</span>
            </div>
        </div>
        
        <div class='led-control'>
            <div class='led-status'>
                LED Status: 
                <span class='led-status-badge status-off' id='ledBadge'>OFF</span>
            </div>
            
            <div class='button-group'>
                <button class='button btn-on' onclick='controlLED("on")'>
                    <span class='icon'>💡</span> Turn ON
                </button>
                <button class='button btn-off' onclick='controlLED("off")'>
                    <span class='icon'>⚫</span> Turn OFF
                </button>
            </div>
        </div>
        
        <div class='history-section'>
            <h3>📊 Data History (<span id='recordCount'>0</span> records)</h3>
            <div class='history-list' id='historyList'>
                <div style='text-align: center; color: #999; padding: 20px;'>No data yet</div>
            </div>
        </div>
        
        <div class='download-section'>
            <h3>📥 Download Data</h3>
            <div class='download-buttons'>
                <button class='btn-download btn-excel' onclick='downloadExcel()'>
                    <span class='icon'>📊</span> Excel (CSV)
                </button>
                <button class='btn-download btn-pdf' onclick='downloadPDF()'>
                    <span class='icon'>📄</span> PDF Report
                </button>
            </div>
        </div>
        
        <div class='footer'>
            <p>Arduino UNO ⟷ ESP8266 Communication</p>
            <p>Last update: <span id='lastUpdate'>--</span></p>
        </div>
    </div>

    <script src='https://cdnjs.cloudflare.com/ajax/libs/jspdf/2.5.1/jspdf.umd.min.js'></script>
    <script>
        let dataHistory = [];
        let isConnected = true;
        
        function updateConnectionStatus(connected) {
            const status = document.getElementById('connStatus');
            if (connected) {
                status.textContent = '● Connected';
                status.style.background = '#d4edda';
                status.style.color = '#155724';
            } else {
                status.textContent = '● Disconnected';
                status.style.background = '#f8d7da';
                status.style.color = '#721c24';
            }
            isConnected = connected;
        }
        
        function addToHistory(temp, hum, led) {
            const now = new Date();
            const timestamp = now.toLocaleString('id-ID');
            
            dataHistory.push({
                timestamp: timestamp,
                temperature: temp,
                humidity: hum,
                ledStatus: led
            });
            
            // Batasi history maksimal 100 data
            if (dataHistory.length > 100) {
                dataHistory.shift();
            }
            
            updateHistoryDisplay();
        }
        
        function updateHistoryDisplay() {
            const historyList = document.getElementById('historyList');
            const recordCount = document.getElementById('recordCount');
            
            recordCount.textContent = dataHistory.length;
            
            if (dataHistory.length === 0) {
                historyList.innerHTML = '<div style="text-align: center; color: #999; padding: 20px;">No data yet</div>';
                return;
            }
            
            // Tampilkan 10 data terakhir
            const recentData = dataHistory.slice(-10).reverse();
            let html = '';
            
            recentData.forEach(item => {
                html += `
                    <div class='history-item'>
                        <span>${item.timestamp}</span>
                        <span>${item.temperature}°C | ${item.humidity}% | LED: ${item.ledStatus}</span>
                    </div>
                `;
            });
            
            historyList.innerHTML = html;
        }
        
        function updateData() {
            fetch('/data')
                .then(response => {
                    if (!response.ok) throw new Error('Network error');
                    return response.json();
                })
                .then(data => {
                    // Update temperature dan humidity
                    document.getElementById('temp').innerText = data.temperature + ' °C';
                    document.getElementById('hum').innerText = data.humidity + ' %';
                    
                    // Update LED status
                    const ledBadge = document.getElementById('ledBadge');
                    ledBadge.innerText = data.led;
                    
                    if (data.led === 'ON') {
                        ledBadge.className = 'led-status-badge status-on';
                    } else {
                        ledBadge.className = 'led-status-badge status-off';
                    }
                    
                    // Tambahkan ke history
                    addToHistory(data.temperature, data.humidity, data.led);
                    
                    // Update last update time
                    const now = new Date();
                    const timeString = now.toLocaleTimeString('id-ID');
                    document.getElementById('lastUpdate').innerText = timeString;
                    
                    // Update connection status
                    updateConnectionStatus(true);
                })
                .catch(error => {
                    console.error('Error fetching data:', error);
                    updateConnectionStatus(false);
                });
        }
        
        function controlLED(state) {
            fetch('/led/' + state)
                .then(response => {
                    if (!response.ok) throw new Error('Network error');
                    return response.text();
                })
                .then(() => {
                    setTimeout(updateData, 200);
                })
                .catch(error => {
                    console.error('Error controlling LED:', error);
                    alert('Failed to control LED. Please check connection.');
                });
        }
        
        function downloadExcel() {
            if (dataHistory.length === 0) {
                alert('No data to download!');
                return;
            }
            
            // Buat CSV content
            let csv = 'Timestamp,Temperature (°C),Humidity (%),LED Status\n';
            
            dataHistory.forEach(item => {
                csv += `${item.timestamp},${item.temperature},${item.humidity},${item.ledStatus}\n`;
            });
            
            // Download CSV
            const blob = new Blob([csv], { type: 'text/csv;charset=utf-8;' });
            const link = document.createElement('a');
            const url = URL.createObjectURL(blob);
            const filename = `sensor_data_${new Date().toISOString().slice(0,10)}.csv`;
            
            link.setAttribute('href', url);
            link.setAttribute('download', filename);
            link.style.visibility = 'hidden';
            document.body.appendChild(link);
            link.click();
            document.body.removeChild(link);
        }
        
        function downloadPDF() {
            if (dataHistory.length === 0) {
                alert('No data to download!');
                return;
            }
            
            const { jsPDF } = window.jspdf;
            const doc = new jsPDF();
            
            // Header
            doc.setFontSize(18);
            doc.text('IoT Sensor Data Report', 105, 15, { align: 'center' });
            
            doc.setFontSize(10);
            doc.text(`Generated: ${new Date().toLocaleString('id-ID')}`, 105, 22, { align: 'center' });
            doc.text(`Total Records: ${dataHistory.length}`, 105, 28, { align: 'center' });
            
            // Summary Statistics
            const temps = dataHistory.map(d => parseFloat(d.temperature));
            const hums = dataHistory.map(d => parseFloat(d.humidity));
            
            const avgTemp = (temps.reduce((a, b) => a + b, 0) / temps.length).toFixed(2);
            const maxTemp = Math.max(...temps).toFixed(2);
            const minTemp = Math.min(...temps).toFixed(2);
            
            const avgHum = (hums.reduce((a, b) => a + b, 0) / hums.length).toFixed(2);
            const maxHum = Math.max(...hums).toFixed(2);
            const minHum = Math.min(...hums).toFixed(2);
            
            doc.setFontSize(12);
            doc.text('Summary Statistics', 14, 40);
            doc.setFontSize(10);
            doc.text(`Temperature: Avg ${avgTemp}°C | Max ${maxTemp}°C | Min ${minTemp}°C`, 14, 47);
            doc.text(`Humidity: Avg ${avgHum}% | Max ${maxHum}% | Min ${minHum}%`, 14, 53);
            
            // Table Header
            doc.setFontSize(9);
            doc.text('Timestamp', 14, 65);
            doc.text('Temp', 85, 65);
            doc.text('Humidity', 115, 65);
            doc.text('LED', 155, 65);
            doc.line(14, 67, 196, 67);
            
            // Table Data
            let y = 73;
            const maxRows = 30; // Maksimal baris per halaman
            
            dataHistory.slice(-maxRows).forEach((item, index) => {
                if (y > 280) {
                    doc.addPage();
                    y = 20;
                }
                
                doc.text(item.timestamp, 14, y);
                doc.text(item.temperature + '°C', 85, y);
                doc.text(item.humidity + '%', 115, y);
                doc.text(item.ledStatus, 155, y);
                y += 7;
            });
            
            // Footer
            const pageCount = doc.internal.getNumberOfPages();
            for (let i = 1; i <= pageCount; i++) {
                doc.setPage(i);
                doc.setFontSize(8);
                doc.text(`Page ${i} of ${pageCount}`, 105, 290, { align: 'center' });
            }
            
            // Save PDF
            const filename = `sensor_report_${new Date().toISOString().slice(0,10)}.pdf`;
            doc.save(filename);
        }
        
        // Update data pertama kali saat halaman dimuat
        updateData();
        
        // Auto-update setiap 2 detik
        setInterval(updateData, 2000);
    </script>
</body>
</html>
)=====";

#endif