// webpage.h
#ifndef WEBPAGE_H
#define WEBPAGE_H

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE HTML>
<html>
<head>
  <meta charset="UTF-8">
  <title>Controlling LED with Local WiFi</title>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    body {
      text-align: center;
      font-family: Arial, sans-serif;
      margin: 20px;
      background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
      min-height: 100vh;
      padding: 20px;
    }
    
    .container {
      background: white;
      border-radius: 15px;
      padding: 30px;
      max-width: 500px;
      margin: 0 auto;
      box-shadow: 0 10px 30px rgba(0,0,0,0.3);
    }
    
    h1 {
      font-size: 28px;
      color: #333;
      margin-bottom: 10px;
    }
    
    h2 {
      font-size: 20px;
      color: #666;
      margin-top: 30px;
      margin-bottom: 15px;
    }
    
    .led-section {
      margin: 30px 0;
      padding: 20px;
      border-radius: 10px;
      background: #f8f9fa;
    }
    
    .button-group {
      display: flex;
      gap: 15px;
      justify-content: center;
      margin-top: 15px;
    }
    
    .button {
      height: 60px;
      width: 120px;
      text-align: center;
      cursor: pointer;
      font-size: 18px;
      border: none;
      border-radius: 8px;
      color: white;
      font-weight: bold;
      transition: all 0.3s ease;
      box-shadow: 0 4px 6px rgba(0,0,0,0.1);
      text-decoration: none;
      display: flex;
      align-items: center;
      justify-content: center;
    }
    
    .button:hover {
      transform: translateY(-2px);
      box-shadow: 0 6px 12px rgba(0,0,0,0.2);
    }
    
    .button:active {
      transform: translateY(0);
    }
    
    .red-on {
      background: linear-gradient(135deg, #ff6b6b 0%, #dc3545 100%);
    }
    
    .red-off {
      background: linear-gradient(135deg, #6c757d 0%, #495057 100%);
    }
    
    .yellow-on {
      background: linear-gradient(135deg, #ffd93d 0%, #ffc107 100%);
      color: #333;
    }
    
    .yellow-off {
      background: linear-gradient(135deg, #6c757d 0%, #495057 100%);
    }
  </style>
</head>
<body>
  <div class="container">
    <h1>🌐 LED Control Panel</h1>
    <p style="color: #666;">Local WiFi Network</p>
    
    <div class="led-section">
      <h2>🔴 RED LED</h2>
      <div class="button-group">
        <a href="/led-on-1" class="button red-on">ON</a>
        <a href="/led-off-1" class="button red-off">OFF</a>
      </div>
    </div>
    
    <div class="led-section">
      <h2>🟡 YELLOW LED</h2>
      <div class="button-group">
        <a href="/led-on-2" class="button yellow-on">ON</a>
        <a href="/led-off-2" class="button yellow-off">OFF</a>
      </div>
    </div>
  </div>
</body>
</html>
)=====";

#endif