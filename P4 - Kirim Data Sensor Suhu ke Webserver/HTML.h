const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html>
<html>
  <head>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <link href="https://use.fontawesome.com/releases/v5.7.2/css/all.css" rel="stylesheet">
    <style>
      html {
        font-family: Arial;
        display: inline-block;
        margin: 0px auto;
        text-align: center;
      }
      h1 { font-size: 2.0rem; }
      p { font-size: 2.0rem; }
      .units { font-size: 1.2rem; }
      .dht-labels{
        font-size: 1.5rem;
        vertical-align:middle;
        padding-bottom: 15px;
      }
      .btn {
        background-color: #4CAF50;
        border: none;
        color: white;
        padding: 15px 32px;
        text-align: center;
        text-decoration: none;
        display: inline-block;
        font-size: 16px;
        margin: 10px 5px;
        cursor: pointer;
        border-radius: 4px;
      }
      .btn-blue {
        background-color: #008CBA;
      }
      .btn-red {
        background-color: #f44336;
      }
      #dataCount {
        font-size: 1.2rem;
        color: #666;
        margin: 10px 0;
      }
    </style>
  </head>
  <body>
    <h1>NodeMCU ESP8266 Monitoring Sensor DHT11</h1>
    <p>
      <i class="fa fa-thermometer-half" style="font-size:3.0rem;color:#62a1d3;"></i> 
      <span class="dht-labels">Temperature : </span> 
      <span id="TemperatureValue">0</span>
      <sup class="units">&deg;C</sup>
    </p>
    <p>
      <i class="fa fa-tint" style="font-size:3.0rem;color:#75e095;"></i>
      <span class="dht-labels">Humidity : </span>
      <span id="HumidityValue">0</span>
      <sup class="units">%</sup>
    </p>
    <p>
      <i class="far fa-clock" style="font-size:1.0rem;color:#e3a8c7;"></i>
      <span style="font-size:1.0rem;">Time </span>
      <span id="time" style="font-size:1.0rem;"></span>
      
      <i class="far fa-calendar-alt" style="font-size:1.0rem;color:#f7dc68";></i>
      <span style="font-size:1.0rem;">Date </span>
      <span id="date" style="font-size:1.0rem;"></span>
    </p>
    <div id="dataCount">Data tersimpan: 0</div>
    <div>
      <button class="btn btn-blue" onclick="downloadExcel()">
        <i class="fa fa-download"></i> Download Excel
      </button>
      <button class="btn btn-red" onclick="clearData()">
        <i class="fa fa-trash"></i> Hapus Data
      </button>
    </div>
    <script src="https://cdnjs.cloudflare.com/ajax/libs/xlsx/0.18.5/xlsx.full.min.js"></script>
    <script>
      var dataLog = [];
      
      setInterval(function() {
        getTemperatureData();
        getHumidityData();
      }, 2000); 
      
      setInterval(function() {
        Time_Date();
      }, 1000); 
      
      function getTemperatureData() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("TemperatureValue").innerHTML = this.responseText;
            window.currentTemp = this.responseText;
            saveData();
          }
        };
        xhttp.open("GET", "readTemperature", true);
        xhttp.send();
      }
      
      function getHumidityData() {
        var xhttp = new XMLHttpRequest();
        xhttp.onreadystatechange = function() {
          if (this.readyState == 4 && this.status == 200) {
            document.getElementById("HumidityValue").innerHTML = this.responseText;
            window.currentHum = this.responseText;
          }
        };
        xhttp.open("GET", "readHumidity", true);
        xhttp.send();
      }
      
      function Time_Date() {
        var t = new Date();
        document.getElementById("time").innerHTML = t.toLocaleTimeString();
        var d = new Date();
        const dayNames = ["Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday","Saturday"];
        const monthNames = ["January", "February", "March", "April", "May", "June","July", "August", "September", "October", "November", "December"];
        document.getElementById("date").innerHTML = dayNames[d.getDay()] + ", " + d.getDate() + "-" + monthNames[d.getMonth()] + "-" + d.getFullYear();
      }
      
      function saveData() {
        if (window.currentTemp && window.currentHum) {
          var now = new Date();
          var dataEntry = {
            'Tanggal': now.toLocaleDateString('id-ID'),
            'Waktu': now.toLocaleTimeString('id-ID'),
            'Suhu (°C)': window.currentTemp,
            'Kelembaban (%)': window.currentHum
          };
          dataLog.push(dataEntry);
          document.getElementById("dataCount").innerHTML = "Data tersimpan: " + dataLog.length;
        }
      }
      
      function downloadExcel() {
        if (dataLog.length === 0) {
          alert("Tidak ada data untuk diunduh!");
          return;
        }
        
        var ws = XLSX.utils.json_to_sheet(dataLog);
        var wb = XLSX.utils.book_new();
        XLSX.utils.book_append_sheet(wb, ws, "Data DHT11");
        
        var filename = "Data_DHT11_" + new Date().toISOString().slice(0,10) + ".xlsx";
        XLSX.writeFile(wb, filename);
      }
      
      function clearData() {
        if (confirm("Apakah Anda yakin ingin menghapus semua data?")) {
          dataLog = [];
          document.getElementById("dataCount").innerHTML = "Data tersimpan: 0";
          alert("Data berhasil dihapus!");
        }
      }
    </script>
  </body>
</html>
)=====";
