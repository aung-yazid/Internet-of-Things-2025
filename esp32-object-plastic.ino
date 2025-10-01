#include "esp_camera.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <WebServer.h>
#include <WiFiClient.h>
#include "esp_timer.h"
#include "img_converters.h"
#include "Arduino.h"
#include "fb_gfx.h"
#include "soc/soc.h"
#include "soc/rtc_cntl_reg.h"
#include "esp_http_server.h"

// WiFi credentials
const char* ssid = "ungs";
const char* password = "wuwungrangers";

// Flask server endpoint
const char* flaskServerURL = "http://192.168.100.13:5000/detect";

// Web server
WebServer server(80);

// ESP32 Wrover CAM Pin Configuration
#define PWDN_GPIO_NUM     -1
#define RESET_GPIO_NUM    -1
#define XCLK_GPIO_NUM     21
#define SIOD_GPIO_NUM     26
#define SIOC_GPIO_NUM     27
#define Y9_GPIO_NUM       35
#define Y8_GPIO_NUM       34
#define Y7_GPIO_NUM       39
#define Y6_GPIO_NUM       36
#define Y5_GPIO_NUM       19
#define Y4_GPIO_NUM       18
#define Y3_GPIO_NUM        5
#define Y2_GPIO_NUM        4
#define VSYNC_GPIO_NUM    25
#define HREF_GPIO_NUM     23
#define PCLK_GPIO_NUM     22

// Global variables
bool detection_enabled = false;
unsigned long last_detection = 0;
unsigned long detection_interval = 5000; // Increased to 5 seconds
float confidence_threshold = 0.5;
int total_detections = 0;
String last_detection_result = "{}";

// FPS optimization variables
bool stream_active = false;
unsigned long last_frame_time = 0;
int fps_counter = 0;
float current_fps = 0;

// HTTP server for streaming
httpd_handle_t stream_httpd = NULL;

#define PART_BOUNDARY "123456789000000000000987654321"
static const char* _STREAM_CONTENT_TYPE = "multipart/x-mixed-replace;boundary=" PART_BOUNDARY;
static const char* _STREAM_BOUNDARY = "\r\n--" PART_BOUNDARY "\r\n";
static const char* _STREAM_PART = "Content-Type: image/jpeg\r\nContent-Length: %u\r\n\r\n";

void setup() {
    Serial.begin(115200);
    Serial.setDebugOutput(false);
    Serial.println("🚀 Starting ESP32-CAM Bottle Detection System (Optimized)...");

    // Disable brownout detector
    WRITE_PERI_REG(RTC_CNTL_BROWN_OUT_REG, 0);

    // Initialize camera
    if (!initCamera()) {
        Serial.println("❌ Camera initialization failed!");
        return;
    }

    // Initialize WiFi
    if (!initWiFi()) {
        Serial.println("❌ WiFi initialization failed!");
        return;
    }

    // Initialize web server
    initWebServer();
    
    // Initialize streaming server
    startStreamServer();

    // Test Flask server connection
    testFlaskServerConnection();

    Serial.println("✅ System ready!");
    Serial.printf("🌐 Web interface: http://%s\n", WiFi.localIP().toString().c_str());
    Serial.printf("📹 Camera stream: http://%s:81/stream\n", WiFi.localIP().toString().c_str());
}

void loop() {
    server.handleClient();
    
    // Auto detection if enabled
    if (detection_enabled && (millis() - last_detection > detection_interval)) {
        performDetection();
        last_detection = millis();
    }
    
    // Calculate FPS
    if (millis() - last_frame_time > 1000) {
        current_fps = fps_counter;
        fps_counter = 0;
        last_frame_time = millis();
    }
    
    delay(1);  // Minimal delay
}

bool initCamera() {
    camera_config_t config;
    config.ledc_channel = LEDC_CHANNEL_0;
    config.ledc_timer = LEDC_TIMER_0;
    config.pin_d0 = Y2_GPIO_NUM;
    config.pin_d1 = Y3_GPIO_NUM;
    config.pin_d2 = Y4_GPIO_NUM;
    config.pin_d3 = Y5_GPIO_NUM;
    config.pin_d4 = Y6_GPIO_NUM;
    config.pin_d5 = Y7_GPIO_NUM;
    config.pin_d6 = Y8_GPIO_NUM;
    config.pin_d7 = Y9_GPIO_NUM;
    config.pin_xclk = XCLK_GPIO_NUM;
    config.pin_pclk = PCLK_GPIO_NUM;
    config.pin_vsync = VSYNC_GPIO_NUM;
    config.pin_href = HREF_GPIO_NUM;
    config.pin_sscb_sda = SIOD_GPIO_NUM;
    config.pin_sscb_scl = SIOC_GPIO_NUM;
    config.pin_pwdn = PWDN_GPIO_NUM;
    config.pin_reset = RESET_GPIO_NUM;
    config.xclk_freq_hz = 20000000;
    config.pixel_format = PIXFORMAT_JPEG;
    config.grab_mode = CAMERA_GRAB_LATEST; // Always get latest frame

    // Optimized settings for streaming
    if (psramFound()) {
        config.frame_size = FRAMESIZE_QVGA;  // 320x240 - MUCH BETTER for streaming
        config.jpeg_quality = 12;             // Lower quality = faster streaming (10-15 good range)
        config.fb_count = 2;                  // Double buffering
        Serial.println("✅ PSRAM found, using QVGA resolution for optimal streaming");
    } else {
        config.frame_size = FRAMESIZE_QVGA;
        config.jpeg_quality = 15;
        config.fb_count = 1;
        Serial.println("⚠️ PSRAM not found, limited buffering");
    }

    // Initialize camera
    esp_err_t err = esp_camera_init(&config);
    if (err != ESP_OK) {
        Serial.printf("❌ Camera init failed with error 0x%x\n", err);
        return false;
    }

    // Camera sensor settings optimized for speed
    sensor_t * s = esp_camera_sensor_get();
    if (s != NULL) {
        s->set_brightness(s, 0);
        s->set_contrast(s, 0);
        s->set_saturation(s, 0);
        s->set_special_effect(s, 0);
        s->set_whitebal(s, 1);
        s->set_awb_gain(s, 1);
        s->set_wb_mode(s, 0);
        s->set_exposure_ctrl(s, 1);
        s->set_aec2(s, 1);           // Enable AEC2 for faster adjustment
        s->set_ae_level(s, 0);
        s->set_aec_value(s, 300);
        s->set_gain_ctrl(s, 1);
        s->set_agc_gain(s, 0);
        s->set_gainceiling(s, (gainceiling_t)2);  // Lower ceiling for less noise
        s->set_bpc(s, 1);
        s->set_wpc(s, 1);
        s->set_raw_gma(s, 1);
        s->set_lenc(s, 1);
        s->set_hmirror(s, 0);
        s->set_vflip(s, 0);
        s->set_dcw(s, 1);
        s->set_colorbar(s, 0);
        
        // CRITICAL: Set frame rate to maximum
        s->set_framesize(s, FRAMESIZE_QVGA);
    }

    Serial.println("✅ Camera initialized with optimized settings!");
    return true;
}

bool initWiFi() {
    WiFi.begin(ssid, password);
    WiFi.setSleep(false);  // Disable WiFi sleep for better performance
    Serial.print("🔗 Connecting to WiFi");
    
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("\n❌ WiFi connection failed!");
        return false;
    }
    
    Serial.println();
    Serial.println("✅ WiFi connected!");
    Serial.printf("📡 IP address: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("📶 Signal strength: %d dBm\n", WiFi.RSSI());
    return true;
}

// MJPEG Streaming handler - MUCH FASTER than single image requests
static esp_err_t stream_handler(httpd_req_t *req) {
    camera_fb_t * fb = NULL;
    esp_err_t res = ESP_OK;
    size_t _jpg_buf_len = 0;
    uint8_t * _jpg_buf = NULL;
    char * part_buf[64];

    res = httpd_resp_set_type(req, _STREAM_CONTENT_TYPE);
    if (res != ESP_OK) {
        return res;
    }

    httpd_resp_set_hdr(req, "Access-Control-Allow-Origin", "*");

    stream_active = true;
    
    while (stream_active) {
        fb = esp_camera_fb_get();
        if (!fb) {
            Serial.println("Camera capture failed");
            res = ESP_FAIL;
        } else {
            _jpg_buf_len = fb->len;
            _jpg_buf = fb->buf;

            if (res == ESP_OK) {
                size_t hlen = snprintf((char *)part_buf, 64, _STREAM_PART, _jpg_buf_len);
                res = httpd_resp_send_chunk(req, (const char *)part_buf, hlen);
            }
            if (res == ESP_OK) {
                res = httpd_resp_send_chunk(req, (const char *)_jpg_buf, _jpg_buf_len);
            }
            if (res == ESP_OK) {
                res = httpd_resp_send_chunk(req, _STREAM_BOUNDARY, strlen(_STREAM_BOUNDARY));
            }
            
            esp_camera_fb_return(fb);
            fb = NULL;
            _jpg_buf = NULL;
            
            fps_counter++;
        }
        
        if (res != ESP_OK) {
            break;
        }
    }

    stream_active = false;
    return res;
}

void startStreamServer() {
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    config.server_port = 81;
    config.ctrl_port = 8081;
    config.max_open_sockets = 1;  // Limit concurrent connections
    config.task_priority = 5;      // Lower priority than main tasks
    config.stack_size = 4096;      // Smaller stack
    config.max_uri_handlers = 8;
    config.recv_wait_timeout = 10;
    config.send_wait_timeout = 10;

    httpd_uri_t stream_uri = {
        .uri       = "/stream",
        .method    = HTTP_GET,
        .handler   = stream_handler,
        .user_ctx  = NULL
    };

    Serial.printf("Starting stream server on port: '%d'\n", config.server_port);
    if (httpd_start(&stream_httpd, &config) == ESP_OK) {
        httpd_register_uri_handler(stream_httpd, &stream_uri);
        Serial.println("✅ Stream server started successfully!");
    } else {
        Serial.println("❌ Failed to start stream server");
    }
}

void initWebServer() {
    // Serve main web interface
    server.on("/", HTTP_GET, []() {
        server.send(200, "text/html", getWebInterface());
    });

    // Single frame capture (high quality for detection)
    server.on("/capture", HTTP_GET, []() {
        // Temporarily switch to higher resolution for detection
        sensor_t * s = esp_camera_sensor_get();
        s->set_framesize(s, FRAMESIZE_VGA);  // 640x480 for detection
        delay(100);  // Let camera adjust
        
        camera_fb_t * fb = esp_camera_fb_get();
        if (!fb) {
            server.send(500, "text/plain", "Camera capture failed");
            s->set_framesize(s, FRAMESIZE_QVGA);  // Back to streaming resolution
            return;
        }
        
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send_P(200, "image/jpeg", (const char *)fb->buf, fb->len);
        
        esp_camera_fb_return(fb);
        
        // Return to streaming resolution
        s->set_framesize(s, FRAMESIZE_QVGA);
    });

    // Detection control endpoints
    server.on("/api/start", HTTP_GET, []() {
        detection_enabled = true;
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "application/json", "{\"status\":\"Detection started\",\"enabled\":true}");
    });

    server.on("/api/stop", HTTP_GET, []() {
        detection_enabled = false;
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "application/json", "{\"status\":\"Detection stopped\",\"enabled\":false}");
    });

    // Manual detection trigger
    server.on("/api/detect", HTTP_GET, []() {
        String result = performDetectionAPI();
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "application/json", result);
    });

    // Settings endpoints
    server.on("/api/settings", HTTP_GET, []() {
        String settings = "{";
        settings += "\"confidence_threshold\":" + String(confidence_threshold) + ",";
        settings += "\"detection_interval\":" + String(detection_interval) + ",";
        settings += "\"detection_enabled\":" + String(detection_enabled ? "true" : "false") + ",";
        settings += "\"total_detections\":" + String(total_detections) + ",";
        settings += "\"fps\":" + String(current_fps) + ",";
        settings += "\"uptime\":" + String(millis()) + ",";
        settings += "\"free_heap\":" + String(ESP.getFreeHeap()) + ",";
        settings += "\"wifi_rssi\":" + String(WiFi.RSSI());
        settings += "}";
        
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "application/json", settings);
    });

    // Update settings
    server.on("/api/settings", HTTP_POST, []() {
        if (server.hasArg("confidence")) {
            confidence_threshold = server.arg("confidence").toFloat();
        }
        if (server.hasArg("interval")) {
            detection_interval = server.arg("interval").toInt();
        }
        
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "application/json", "{\"status\":\"Settings updated\"}");
    });

    // Get last detection results
    server.on("/api/lastdetection", HTTP_GET, []() {
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "application/json", last_detection_result);
    });

    // System info
    server.on("/api/info", HTTP_GET, []() {
        String info = "{";
        info += "\"chip_model\":\"" + String(ESP.getChipModel()) + "\",";
        info += "\"chip_revision\":" + String(ESP.getChipRevision()) + ",";
        info += "\"cpu_freq\":" + String(ESP.getCpuFreqMHz()) + ",";
        info += "\"flash_size\":" + String(ESP.getFlashChipSize()) + ",";
        info += "\"free_heap\":" + String(ESP.getFreeHeap()) + ",";
        info += "\"psram_size\":" + String(ESP.getPsramSize()) + ",";
        info += "\"fps\":" + String(current_fps) + ",";
        info += "\"wifi_mac\":\"" + WiFi.macAddress() + "\",";
        info += "\"wifi_ip\":\"" + WiFi.localIP().toString() + "\"";
        info += "}";
        
        server.sendHeader("Access-Control-Allow-Origin", "*");
        server.send(200, "application/json", info);
    });

    server.begin();
    Serial.println("✅ Web server started on port 80");
}

String performDetectionAPI() {
    // Switch to high resolution for detection
    sensor_t * s = esp_camera_sensor_get();
    s->set_framesize(s, FRAMESIZE_VGA);
    delay(100);
    
    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
        s->set_framesize(s, FRAMESIZE_QVGA);
        return "{\"error\":\"Camera capture failed\"}";
    }
    
    String result = detectBottlesHTTP(fb);
    esp_camera_fb_return(fb);
    
    // Return to streaming resolution
    s->set_framesize(s, FRAMESIZE_QVGA);
    
    return result;
}

void performDetection() {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ WiFi not connected");
        return;
    }

    // Switch to high resolution for detection
    sensor_t * s = esp_camera_sensor_get();
    s->set_framesize(s, FRAMESIZE_VGA);
    delay(100);

    camera_fb_t * fb = esp_camera_fb_get();
    if (!fb) {
        Serial.println("❌ Camera capture failed");
        s->set_framesize(s, FRAMESIZE_QVGA);
        return;
    }
    
    Serial.printf("📸 Image captured: %dx%d, %d bytes\n", fb->width, fb->height, fb->len);
    
    String result = detectBottlesHTTP(fb);
    last_detection_result = result;
    
    esp_camera_fb_return(fb);
    
    // Return to streaming resolution
    s->set_framesize(s, FRAMESIZE_QVGA);
}

String detectBottlesHTTP(camera_fb_t* fb) {
    if (WiFi.status() != WL_CONNECTED) {
        Serial.println("❌ WiFi not connected");
        return "{\"error\":\"WiFi not connected\"}";
    }

    // Log WiFi status
    Serial.printf("📡 WiFi Status: Connected to %s\n", ssid);
    Serial.printf("📡 Signal Strength: %d dBm\n", WiFi.RSSI());
    Serial.printf("📡 Local IP: %s\n", WiFi.localIP().toString().c_str());

    HTTPClient http;
    
    // Test connection first
    Serial.printf("🔗 Attempting to connect to: %s\n", flaskServerURL);
    
    if (!http.begin(flaskServerURL)) {
        Serial.println("❌ Failed to begin HTTP connection");
        return "{\"error\":\"HTTP begin failed\"}";
    }
    
    http.addHeader("Content-Type", "image/jpeg");
    http.addHeader("Content-Length", String(fb->len));
    http.setTimeout(20000); // 20 second timeout

    Serial.printf("📸 Sending image to Flask server...\n");
    Serial.printf("   Image size: %d bytes (%dx%d)\n", fb->len, fb->width, fb->height);
    Serial.printf("   Flask URL: %s\n", flaskServerURL);
    
    unsigned long start_time = millis();
    
    int httpResponseCode = http.POST(fb->buf, fb->len);
    
    unsigned long end_time = millis();
    unsigned long inference_time = end_time - start_time;

    String response = "{}";
    
    if (httpResponseCode == 200) {
        response = http.getString();
        Serial.printf("✅ Server response received in %lu ms\n", inference_time);
        Serial.printf("📦 Response length: %d bytes\n", response.length());
        parseAndLogDetections(response);
    } else if (httpResponseCode > 0) {
        Serial.printf("❌ Server error: HTTP %d\n", httpResponseCode);
        String errorBody = http.getString();
        if (errorBody.length() > 0) {
            Serial.printf("❌ Error details: %s\n", errorBody.c_str());
        }
        response = "{\"error\":\"Server error: " + String(httpResponseCode) + "\"}";
    } else {
        Serial.printf("❌ Connection error: %s (code: %d)\n", 
                     http.errorToString(httpResponseCode).c_str(), httpResponseCode);
        Serial.println("💡 Check:");
        Serial.println("   1. Flask server is running");
        Serial.println("   2. Flask server IP is correct: 192.168.100.13");
        Serial.println("   3. Port 5000 is accessible");
        Serial.println("   4. No firewall blocking connection");
        response = "{\"error\":\"Connection failed\"}";
    }

    http.end();
    return response;
}

void parseAndLogDetections(String jsonResponse) {
    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, jsonResponse);
    
    if (error) {
        Serial.printf("❌ JSON parsing failed: %s\n", error.c_str());
        return;
    }

    JsonArray detections = doc["detections"];
    int detection_count = detections.size();

    if (detection_count > 0) {
        total_detections += detection_count;
        Serial.printf("🍼 Found %d bottle(s):\n", detection_count);
        
        for (int i = 0; i < detection_count; i++) {
            JsonObject detection = detections[i];
            float confidence = detection["confidence"];
            JsonObject bbox = detection["bbox"];
            
            Serial.printf("  📍 Bottle %d: conf=%.2f, pos=(%d,%d), size=%dx%d\n", 
                         i + 1, confidence, 
                         (int)bbox["x"], (int)bbox["y"], 
                         (int)bbox["width"], (int)bbox["height"]);
        }
    } else {
        Serial.println("🚫 No bottles detected");
    }
}

void testFlaskServerConnection() {
    Serial.println("🔍 Testing Flask server connection...");
    Serial.println("======================================");
    
    HTTPClient http;
    String testURL = "http://192.168.100.13:5000/health";
    
    Serial.printf("🌐 Testing URL: %s\n", testURL.c_str());
    Serial.printf("📡 ESP32 IP: %s\n", WiFi.localIP().toString().c_str());
    Serial.printf("📡 WiFi RSSI: %d dBm\n", WiFi.RSSI());
    
    if (!http.begin(testURL.c_str())) {
        Serial.println("❌ Failed to initialize HTTP connection");
        Serial.println("   Check if URL is correct");
        return;
    }
    
    http.setTimeout(10000);
    
    Serial.println("⏳ Sending GET request to /health endpoint...");
    int httpCode = http.GET();
    
    if (httpCode == 200) {
        Serial.println("✅ Flask server is reachable and healthy!");
        String response = http.getString();
        Serial.println("📄 Server response:");
        Serial.println(response);
        Serial.println("========================================");
    } else if (httpCode > 0) {
        Serial.printf("⚠️ Flask server responded with HTTP %d\n", httpCode);
        String response = http.getString();
        if (response.length() > 0) {
            Serial.printf("Response: %s\n", response.c_str());
        }
    } else {
        Serial.printf("❌ Cannot reach Flask server!\n");
        Serial.printf("   Error: %s (code: %d)\n", 
                     http.errorToString(httpCode).c_str(), httpCode);
        Serial.println("\n💡 Troubleshooting steps:");
        Serial.println("   1. Check Flask server is running:");
        Serial.println("      python app.py");
        Serial.println("   2. Verify Flask server IP: 192.168.100.13");
        Serial.println("   3. Check Flask is listening on 0.0.0.0:5000");
        Serial.println("   4. Ping Flask server from ESP32 network");
        Serial.println("   5. Check firewall settings");
        Serial.println("   6. Verify both devices on same network");
    }
    
    http.end();
}

String getWebInterface() {
    return R"rawliteral(
<!DOCTYPE html>
<html>
<head>
    <meta charset="UTF-8">
    <title>ESP32-CAM Detection (Optimized)</title>
    <meta name="viewport" content="width=device-width, initial-scale=1">
    <style>
        * { margin: 0; padding: 0; box-sizing: border-box; }
        body { font-family: 'Segoe UI', Arial; background: #0f172a; color: #e2e8f0; }
        .container { max-width: 1400px; margin: 0 auto; padding: 20px; }
        .header { 
            text-align: center; 
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%); 
            padding: 30px; 
            border-radius: 15px; 
            margin-bottom: 20px;
            box-shadow: 0 10px 30px rgba(102, 126, 234, 0.3);
        }
        .header h1 { font-size: 2.5em; margin-bottom: 10px; }
        .controls { 
            background: #1e293b; 
            padding: 20px; 
            border-radius: 15px; 
            margin-bottom: 20px;
            box-shadow: 0 4px 15px rgba(0,0,0,0.3);
        }
        .video-section { 
            background: #1e293b; 
            padding: 20px; 
            border-radius: 15px; 
            margin-bottom: 20px;
            box-shadow: 0 4px 15px rgba(0,0,0,0.3);
        }
        #stream { 
            width: 100%; 
            max-width: 640px; 
            border-radius: 10px;
            border: 3px solid #667eea;
            box-shadow: 0 4px 15px rgba(0,0,0,0.3);
        }
        .btn { 
            padding: 12px 24px; 
            margin: 5px; 
            border: none; 
            border-radius: 8px; 
            cursor: pointer; 
            font-weight: bold;
            font-size: 14px;
            transition: all 0.3s ease;
            text-transform: uppercase;
            letter-spacing: 1px;
        }
        .btn:hover { transform: translateY(-2px); box-shadow: 0 6px 20px rgba(0,0,0,0.3); }
        .btn-primary { background: #3b82f6; color: white; }
        .btn-success { background: #10b981; color: white; }
        .btn-danger { background: #ef4444; color: white; }
        .stats { 
            display: grid; 
            grid-template-columns: repeat(auto-fit, minmax(200px, 1fr)); 
            gap: 15px; 
            margin: 20px 0; 
        }
        .stat-card { 
            background: linear-gradient(135deg, #1e293b 0%, #334155 100%);
            padding: 20px; 
            border-radius: 12px; 
            text-align: center;
            box-shadow: 0 4px 15px rgba(0,0,0,0.3);
            border: 1px solid #334155;
        }
        .stat-card h3 { color: #94a3b8; font-size: 0.9em; margin-bottom: 10px; text-transform: uppercase; }
        .stat-card div { font-size: 2em; font-weight: bold; color: #667eea; }
        .status { 
            padding: 15px; 
            border-radius: 8px; 
            margin: 10px 0;
            font-weight: 500;
        }
        .status.success { background: rgba(16, 185, 129, 0.2); color: #10b981; border: 1px solid #10b981; }
        .status.error { background: rgba(239, 68, 68, 0.2); color: #ef4444; border: 1px solid #ef4444; }
        .fps-indicator {
            position: fixed;
            top: 20px;
            right: 20px;
            background: rgba(16, 185, 129, 0.9);
            color: white;
            padding: 10px 20px;
            border-radius: 8px;
            font-weight: bold;
            box-shadow: 0 4px 15px rgba(0,0,0,0.3);
            z-index: 1000;
        }
        h3 { color: #94a3b8; margin-bottom: 15px; }
    </style>
</head>
<body>
    <div class="fps-indicator" id="fpsIndicator">FPS: 0</div>
    
    <div class="container">
        <div class="header">
            <h1>ESP32-CAM Detection</h1>
            <p>Optimized Real-time Detection System</p>
        </div>
        
        <div class="controls">
            <button class="btn btn-primary" onclick="startDetection()">▶️ Start Auto Detection</button>
            <button class="btn btn-danger" onclick="stopDetection()">⏹️ Stop Detection</button>
            <button class="btn btn-success" onclick="detectNow()">🔍 Detect Now</button>
        </div>
        
        <div class="stats">
            <div class="stat-card">
                <h3>📊 Current FPS</h3>
                <div id="currentFPS">0</div>
            </div>
            <div class="stat-card">
                <h3>🔢 Total Detections</h3>
                <div id="totalDetections">0</div>
            </div>
            <div class="stat-card">
                <h3>⚡ System Status</h3>
                <div id="systemStatus" style="font-size: 1.5em;">Ready</div>
            </div>
            <div class="stat-card">
                <h3>📡 WiFi Signal</h3>
                <div id="wifiSignal">-</div>
            </div>
        </div>
        
        <div class="video-section">
            <h3>📹 Live Camera Stream (MJPEG)</h3>
            <img id="stream" src="" alt="Loading stream...">
        </div>
        
        <div id="statusArea"></div>
        <div id="detectionResults"></div>
    </div>
    
    <script>
        let detectionEnabled = false;
        const streamUrl = 'http://' + window.location.hostname + ':81/stream';
        
        function showStatus(message, type = 'success') {
            const statusArea = document.getElementById('statusArea');
            statusArea.innerHTML = `<div class="status ${type}">${message}</div>`;
            setTimeout(() => statusArea.innerHTML = '', 5000);
        }
        
        function startStream() {
            const streamImg = document.getElementById('stream');
            streamImg.src = streamUrl + '?' + new Date().getTime();
            streamImg.onload = () => showStatus('🎥 Stream connected at ' + streamUrl, 'success');
            streamImg.onerror = () => {
                showStatus('❌ Stream failed. Retrying...', 'error');
                setTimeout(startStream, 2000);
            };
        }
        
        function startDetection() {
            fetch('/api/start')
                .then(response => response.json())
                .then(data => {
                    detectionEnabled = true;
                    showStatus('✅ Auto detection started!');
                    document.getElementById('systemStatus').textContent = 'Detecting';
                })
                .catch(error => showStatus('❌ Failed to start detection', 'error'));
        }
        
        function stopDetection() {
            fetch('/api/stop')
                .then(response => response.json())
                .then(data => {
                    detectionEnabled = false;
                    showStatus('⏹️ Detection stopped');
                    document.getElementById('systemStatus').textContent = 'Ready';
                })
                .catch(error => showStatus('❌ Failed to stop detection', 'error'));
        }
        
        function detectNow() {
            showStatus('🔍 Detecting...', 'success');
            fetch('/api/detect')
                .then(response => response.json())
                .then(data => {
                    displayDetectionResults(data);
                    if (data.detections && data.detections.length > 0) {
                        showStatus(`✅ Found ${data.detections.length} bottle(s)!`);
                    } else {
                        showStatus('🚫 No bottles detected');
                    }
                })
                .catch(error => {
                    console.error('Detection error:', error);
                    showStatus('❌ Detection failed', 'error');
                });
        }
        
        function displayDetectionResults(data) {
            const resultsDiv = document.getElementById('detectionResults');
            
            if (data.detections && data.detections.length > 0) {
                let html = '<div class="video-section"><h3>🎯 Detection Results:</h3><ul style="list-style: none; padding: 0;">';
                
                data.detections.forEach((detection, index) => {
                    html += `<li style="padding: 10px; margin: 5px 0; background: #334155; border-radius: 8px;">
                        🍼 Bottle ${index + 1}: <strong>${(detection.confidence * 100).toFixed(1)}%</strong> confidence 
                        at position (${detection.bbox.x}, ${detection.bbox.y})
                    </li>`;
                });
                
                html += '</ul></div>';
                resultsDiv.innerHTML = html;
            } else if (data.error) {
                resultsDiv.innerHTML = `<div class="status error">❌ ${data.error}</div>`;
            } else {
                resultsDiv.innerHTML = '<div class="video-section"><p>🚫 No bottles detected in current frame.</p></div>';
            }
        }
        
        function updateStats() {
            fetch('/api/settings')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('totalDetections').textContent = data.total_detections;
                    document.getElementById('currentFPS').textContent = data.fps.toFixed(1);
                    document.getElementById('fpsIndicator').textContent = 'FPS: ' + data.fps.toFixed(1);
                    document.getElementById('wifiSignal').textContent = data.wifi_rssi + ' dBm';
                    
                    // Update FPS indicator color
                    const fpsEl = document.getElementById('fpsIndicator');
                    if (data.fps >= 15) {
                        fpsEl.style.background = 'rgba(16, 185, 129, 0.9)';
                    } else if (data.fps >= 10) {
                        fpsEl.style.background = 'rgba(251, 191, 36, 0.9)';
                    } else {
                        fpsEl.style.background = 'rgba(239, 68, 68, 0.9)';
                    }
                })
                .catch(error => console.error('Stats update failed:', error));
        }
        
        // Start stream immediately
        startStream();
        
        // Update stats every 2 seconds
        setInterval(updateStats, 2000);
        
        // Initial stats load
        updateStats();
        
        // Auto-refresh detection results if auto-detection is enabled
        setInterval(() => {
            if (detectionEnabled) {
                fetch('/api/lastdetection')
                    .then(response => response.json())
                    .then(data => {
                        if (data.detections) {
                            displayDetectionResults(data);
                        }
                    })
                    .catch(error => console.error('Last detection fetch failed:', error));
            }
        }, 5000);
    </script>
</body>
</html>
)rawliteral";
}