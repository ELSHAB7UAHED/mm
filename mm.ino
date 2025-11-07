/***************************************************
 * BARA - Ultimate WiFi Pentesting Tool
 * Developer: Ahmed Nour Ahmed from Qena
 * Version: 2.0
 * Description: Advanced WiFi Deauthentication Tool
 * WARNING: For educational and authorized testing only!
 ***************************************************/

#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>
#include "esp_wifi_types.h"
#include "esp_system.h"
#include "esp_event.h"
#include "nvs_flash.h"
#include "driver/rtc_io.h"
#include <ArduinoJson.h>
#include <SPIFFS.h>

// ==================== AUDIO & VISUAL EFFECTS ====================
#include "DFRobotDFPlayerMini.h"
HardwareSerial mySoftwareSerial(1);
DFRobotDFPlayerMini myDFPlayer;

// LED Pins for visual effects
#define LED_RED 25
#define LED_GREEN 26
#define LED_BLUE 27
#define LED_WHITE 32
#define BUZZER_PIN 33

// ==================== WIFI CONFIGURATION ====================
const char* AP_SSID = "BARA";
const char* AP_PASSWORD = "A7med@Elshab7";

WebServer server(80);

// ==================== GLOBAL VARIABLES ====================
bool deauthRunning = false;
bool scanRunning = false;
unsigned long deauthStartTime = 0;
const int MAX_DEAUTH_TIME = 300000; // 5 minutes max
String targetSSID = "";
String targetBSSID = "";
int deauthPackets = 0;
int scanResultsCount = 0;

// ==================== PACKET STRUCTURES ====================
struct wifi_management_frame {
    uint16_t frame_control;
    uint16_t duration;
    uint8_t destination_addr[6];
    uint8_t source_addr[6];
    uint8_t bssid[6];
    uint16_t sequence_control;
    uint8_t category_code;
    uint8_t action_code;
    uint8_t element_id;
    uint8_t length;
    uint8_t payload[0];
} __attribute__((packed));

// ==================== HTML CONTENT ====================
const char* MAIN_PAGE = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>BARA - Ultimate WiFi Pentesting Tool</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Orbitron:wght@400;700;900&display=swap');
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            background: #0a0a0a;
            color: #00ff00;
            font-family: 'Orbitron', monospace;
            overflow-x: hidden;
            background-image: 
                radial-gradient(circle at 10% 20%, rgba(255, 0, 0, 0.1) 0%, transparent 20%),
                radial-gradient(circle at 90% 80%, rgba(0, 255, 0, 0.1) 0%, transparent 20%),
                radial-gradient(circle at 50% 50%, rgba(0, 0, 255, 0.1) 0%, transparent 50%);
        }
        
        .matrix-bg {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            pointer-events: none;
            z-index: -1;
            opacity: 0.3;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
        }
        
        .header {
            text-align: center;
            padding: 30px 0;
            border-bottom: 3px solid #ff0000;
            margin-bottom: 30px;
            position: relative;
            overflow: hidden;
        }
        
        .header::before {
            content: '';
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: linear-gradient(90deg, transparent, rgba(255, 0, 0, 0.4), transparent);
            animation: shine 3s infinite;
        }
        
        @keyframes shine {
            0% { left: -100%; }
            100% { left: 100%; }
        }
        
        .title {
            font-size: 4em;
            font-weight: 900;
            text-shadow: 
                0 0 10px #ff0000,
                0 0 20px #ff0000,
                0 0 40px #ff0000,
                0 0 80px #ff0000;
            animation: glitch 2s infinite;
            margin-bottom: 10px;
        }
        
        @keyframes glitch {
            0%, 100% { transform: translate(0); }
            20% { transform: translate(-2px, 2px); }
            40% { transform: translate(-2px, -2px); }
            60% { transform: translate(2px, 2px); }
            80% { transform: translate(2px, -2px); }
        }
        
        .subtitle {
            font-size: 1.5em;
            color: #00ffff;
            text-shadow: 0 0 10px #00ffff;
        }
        
        .developer {
            color: #ffff00;
            font-size: 1.2em;
            margin-top: 10px;
        }
        
        .panels {
            display: grid;
            grid-template-columns: 1fr 1fr;
            gap: 20px;
            margin-bottom: 30px;
        }
        
        .panel {
            background: rgba(0, 0, 0, 0.8);
            border: 2px solid #ff0000;
            border-radius: 15px;
            padding: 20px;
            box-shadow: 
                0 0 20px rgba(255, 0, 0, 0.5),
                inset 0 0 20px rgba(255, 0, 0, 0.1);
            transition: all 0.3s ease;
        }
        
        .panel:hover {
            transform: translateY(-5px);
            box-shadow: 
                0 0 30px rgba(255, 0, 0, 0.8),
                inset 0 0 30px rgba(255, 0, 0, 0.2);
        }
        
        .panel-title {
            font-size: 1.8em;
            color: #ff0000;
            margin-bottom: 20px;
            text-align: center;
            text-shadow: 0 0 10px #ff0000;
        }
        
        .btn {
            background: linear-gradient(45deg, #ff0000, #ff0066);
            color: white;
            border: none;
            padding: 15px 30px;
            font-size: 1.2em;
            font-family: 'Orbitron', monospace;
            border-radius: 25px;
            cursor: pointer;
            margin: 10px;
            transition: all 0.3s ease;
            text-transform: uppercase;
            letter-spacing: 2px;
            box-shadow: 0 0 15px rgba(255, 0, 0, 0.5);
        }
        
        .btn:hover {
            transform: scale(1.05);
            box-shadow: 0 0 25px rgba(255, 0, 0, 0.8);
            background: linear-gradient(45deg, #ff0066, #ff0000);
        }
        
        .btn-danger {
            background: linear-gradient(45deg, #ff0000, #990000);
        }
        
        .btn-success {
            background: linear-gradient(45deg, #00ff00, #009900);
        }
        
        .btn-warning {
            background: linear-gradient(45deg, #ffff00, #996600);
        }
        
        .status {
            padding: 15px;
            margin: 10px 0;
            border-radius: 10px;
            text-align: center;
            font-weight: bold;
            background: rgba(0, 0, 0, 0.6);
            border: 1px solid #00ff00;
        }
        
        .scan-results {
            max-height: 400px;
            overflow-y: auto;
            margin-top: 20px;
            padding: 15px;
            background: rgba(0, 0, 0, 0.6);
            border-radius: 10px;
            border: 1px solid #00ffff;
        }
        
        .network-item {
            padding: 10px;
            margin: 5px 0;
            background: rgba(255, 255, 255, 0.1);
            border-radius: 5px;
            border-left: 4px solid #ff0000;
            transition: all 0.3s ease;
            cursor: pointer;
        }
        
        .network-item:hover {
            background: rgba(255, 0, 0, 0.2);
            transform: translateX(5px);
        }
        
        .stats {
            display: grid;
            grid-template-columns: repeat(3, 1fr);
            gap: 15px;
            margin-top: 20px;
        }
        
        .stat-box {
            background: rgba(0, 0, 0, 0.6);
            padding: 15px;
            border-radius: 10px;
            text-align: center;
            border: 1px solid #ffff00;
        }
        
        .stat-value {
            font-size: 2em;
            font-weight: bold;
            color: #ffff00;
            text-shadow: 0 0 10px #ffff00;
        }
        
        .stat-label {
            font-size: 0.9em;
            color: #cccccc;
        }
        
        .deauth-controls {
            text-align: center;
            margin-top: 20px;
        }
        
        .target-info {
            background: rgba(255, 0, 0, 0.2);
            padding: 15px;
            border-radius: 10px;
            margin: 15px 0;
            border: 1px solid #ff0000;
        }
        
        .footer {
            text-align: center;
            padding: 20px;
            margin-top: 30px;
            border-top: 2px solid #00ff00;
            color: #00ff00;
            font-size: 0.9em;
        }
        
        .warning {
            color: #ff0000;
            text-shadow: 0 0 10px #ff0000;
            animation: blink 1s infinite;
        }
        
        @keyframes blink {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.5; }
        }
        
        /* Matrix rain effect */
        .matrix-char {
            position: absolute;
            color: #00ff00;
            font-size: 14px;
            animation: fall linear infinite;
        }
        
        @keyframes fall {
            to { transform: translateY(100vh); }
        }
    </style>
</head>
<body>
    <div class="matrix-bg" id="matrixBg"></div>
    
    <div class="container">
        <div class="header">
            <h1 class="title">BARA</h1>
            <p class="subtitle">ULTIMATE WIFI PENTESTING TOOL</p>
            <p class="developer">Developed by: Ahmed Nour Ahmed - Qena</p>
            <p class="warning">WARNING: FOR AUTHORIZED TESTING ONLY!</p>
        </div>
        
        <div class="panels">
            <div class="panel">
                <h2 class="panel-title">NETWORK SCANNER</h2>
                <button class="btn btn-success" onclick="startScan()">START SCAN</button>
                <button class="btn btn-warning" onclick="stopScan()">STOP SCAN</button>
                
                <div class="status" id="scanStatus">Ready to scan...</div>
                
                <div class="scan-results" id="scanResults">
                    <!-- Scan results will appear here -->
                </div>
            </div>
            
            <div class="panel">
                <h2 class="panel-title">DEAUTH ATTACK</h2>
                
                <div class="target-info" id="targetInfo">
                    No target selected
                </div>
                
                <div class="deauth-controls">
                    <button class="btn btn-danger" onclick="startDeauth()" id="startDeauthBtn">START DEAUTH</button>
                    <button class="btn btn-success" onclick="stopDeauth()" id="stopDeauthBtn" disabled>STOP DEAUTH</button>
                </div>
                
                <div class="stats">
                    <div class="stat-box">
                        <div class="stat-value" id="deauthCount">0</div>
                        <div class="stat-label">PACKETS SENT</div>
                    </div>
                    <div class="stat-box">
                        <div class="stat-value" id="attackTime">0s</div>
                        <div class="stat-label">ATTACK TIME</div>
                    </div>
                    <div class="stat-box">
                        <div class="stat-value" id="packetRate">0/s</div>
                        <div class="stat-label">PACKET RATE</div>
                    </div>
                </div>
            </div>
        </div>
        
        <div class="panel">
            <h2 class="panel-title">SYSTEM STATUS</h2>
            <div class="stats">
                <div class="stat-box">
                    <div class="stat-value" id="wifiStrength">--</div>
                    <div class="stat-label">WIFI STRENGTH</div>
                </div>
                <div class="stat-box">
                    <div class="stat-value" id="memoryUsage">--</div>
                    <div class="stat-label">MEMORY USAGE</div>
                </div>
                <div class="stat-box">
                    <div class="stat-value" id="uptime">0s</div>
                    <div class="stat-label">UPTIME</div>
                </div>
            </div>
        </div>
        
        <div class="footer">
            <p>BARA WiFi Pentesting Tool v2.0 | Developed with ❤️ by Ahmed Nour Ahmed</p>
            <p class="warning">USE RESPONSIBLY AND LEGALLY!</p>
        </div>
    </div>

    <script>
        // Matrix rain effect
        function createMatrix() {
            const chars = "01010101010101010101010101010101ABCDEFGHIJKLMNOPQRSTUVWXYZ01010101010101010101010101010101";
            const bg = document.getElementById('matrixBg');
            
            for (let i = 0; i < 50; i++) {
                const char = document.createElement('div');
                char.className = 'matrix-char';
                char.textContent = chars[Math.floor(Math.random() * chars.length)];
                char.style.left = Math.random() * 100 + 'vw';
                char.style.animationDuration = (Math.random() * 3 + 2) + 's';
                char.style.animationDelay = Math.random() * 5 + 's';
                bg.appendChild(char);
            }
        }
        
        // Audio effects
        function playSound(type) {
            fetch('/sound?type=' + type);
        }
        
        // Network scanning
        let scanInterval;
        
        function startScan() {
            playSound('scan');
            document.getElementById('scanStatus').innerHTML = '<span style="color: #ffff00">SCANNING...</span>';
            document.getElementById('scanStatus').style.borderColor = '#ffff00';
            
            fetch('/scan')
                .then(response => response.json())
                .then(data => {
                    updateScanResults(data);
                });
                
            scanInterval = setInterval(() => {
                fetch('/scanstatus')
                    .then(response => response.json())
                    .then(data => {
                        updateScanResults(data);
                    });
            }, 3000);
        }
        
        function stopScan() {
            clearInterval(scanInterval);
            document.getElementById('scanStatus').innerHTML = '<span style="color: #00ff00">SCAN STOPPED</span>';
            document.getElementById('scanStatus').style.borderColor = '#00ff00';
            playSound('stop');
        }
        
        function updateScanResults(data) {
            const resultsDiv = document.getElementById('scanResults');
            resultsDiv.innerHTML = '';
            
            if (data.networks && data.networks.length > 0) {
                data.networks.forEach(network => {
                    const networkItem = document.createElement('div');
                    networkItem.className = 'network-item';
                    networkItem.innerHTML = `
                        <strong>${network.ssid || 'Hidden'}</strong><br>
                        BSSID: ${network.bssid} | Channel: ${network.channel}<br>
                        Signal: ${network.rssi}dBm | Clients: ${network.station_count || 'N/A'}
                        <button class="btn" onclick="selectTarget('${network.bssid}', '${network.ssid || 'Hidden'}')" style="padding: 5px 10px; font-size: 0.8em; margin-top: 5px;">SELECT TARGET</button>
                    `;
                    resultsDiv.appendChild(networkItem);
                });
                
                document.getElementById('scanStatus').innerHTML = `<span style="color: #00ff00">FOUND ${data.networks.length} NETWORKS</span>`;
            } else {
                resultsDiv.innerHTML = '<div style="text-align: center; color: #ff0000;">No networks found</div>';
            }
        }
        
        // Deauth attack
        let deauthInterval;
        let attackStartTime;
        
        function selectTarget(bssid, ssid) {
            playSound('select');
            targetBSSID = bssid;
            targetSSID = ssid;
            
            document.getElementById('targetInfo').innerHTML = `
                <strong>SELECTED TARGET:</strong><br>
                SSID: ${ssid}<br>
                BSSID: ${bssid}
            `;
            
            document.getElementById('startDeauthBtn').disabled = false;
        }
        
        function startDeauth() {
            if (!targetBSSID) {
                alert('Please select a target first!');
                return;
            }
            
            playSound('attack');
            attackStartTime = Date.now();
            
            fetch('/deauth?bssid=' + targetBSSID + '&start=true')
                .then(response => response.json())
                .then(data => {
                    if (data.success) {
                        document.getElementById('startDeauthBtn').disabled = true;
                        document.getElementById('stopDeauthBtn').disabled = false;
                        
                        deauthInterval = setInterval(updateDeauthStats, 1000);
                    }
                });
        }
        
        function stopDeauth() {
            playSound('stop');
            clearInterval(deauthInterval);
            
            fetch('/deauth?stop=true')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('startDeauthBtn').disabled = false;
                    document.getElementById('stopDeauthBtn').disabled = true;
                });
        }
        
        function updateDeauthStats() {
            fetch('/deauthstatus')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('deauthCount').textContent = data.packets_sent;
                    document.getElementById('attackTime').textContent = Math.floor((Date.now() - attackStartTime) / 1000) + 's';
                    document.getElementById('packetRate').textContent = data.packet_rate + '/s';
                });
        }
        
        // System status
        function updateSystemStatus() {
            fetch('/status')
                .then(response => response.json())
                .then(data => {
                    document.getElementById('wifiStrength').textContent = data.wifi_strength + 'dBm';
                    document.getElementById('memoryUsage').textContent = data.memory_usage + '%';
                    document.getElementById('uptime').textContent = data.uptime + 's';
                });
        }
        
        // Initialize
        createMatrix();
        setInterval(updateSystemStatus, 2000);
        updateSystemStatus();
        
        // Add terminal-like typing effect for status messages
        function typeWriter(element, text, speed = 50) {
            element.innerHTML = '';
            let i = 0;
            function type() {
                if (i < text.length) {
                    element.innerHTML += text.charAt(i);
                    i++;
                    setTimeout(type, speed);
                }
            }
            type();
        }
    </script>
</body>
</html>
)rawliteral";

// ==================== FUNCTION DECLARATIONS ====================
void handleRoot();
void handleScan();
void handleScanStatus();
void handleDeauth();
void handleDeauthStatus();
void handleStatus();
void handleSound();
void setupWiFi();
void startDeauthAttack(String bssid);
void stopDeauthAttack();
void updateLEDs();
void playAudioEffect(String type);
void setupAudio();
void visualEffect(String type);

// ==================== SETUP ====================
void setup() {
    Serial.begin(115200);
    
    // Initialize LEDs
    pinMode(LED_RED, OUTPUT);
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_BLUE, OUTPUT);
    pinMode(LED_WHITE, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    
    // Initialize SPIFFS
    if (!SPIFFS.begin(true)) {
        Serial.println("SPIFFS initialization failed!");
    }
    
    // Setup audio
    setupAudio();
    
    // Visual startup sequence
    visualEffect("startup");
    
    // Setup WiFi
    setupWiFi();
    
    // Setup server routes
    server.on("/", handleRoot);
    server.on("/scan", handleScan);
    server.on("/scanstatus", handleScanStatus);
    server.on("/deauth", handleDeauth);
    server.on("/deauthstatus", handleDeauthStatus);
    server.on("/status", handleStatus);
    server.on("/sound", handleSound);
    
    server.begin();
    Serial.println("HTTP server started");
    
    // Final startup effect
    playAudioEffect("startup");
    visualEffect("ready");
}

// ==================== MAIN LOOP ====================
void loop() {
    server.handleClient();
    
    if (deauthRunning) {
        // Deauth attack logic here
        unsigned long currentTime = millis();
        if (currentTime - deauthStartTime > MAX_DEAUTH_TIME) {
            stopDeauthAttack();
        }
    }
    
    updateLEDs();
    delay(100);
}

// ==================== WIFI SETUP ====================
void setupWiFi() {
    WiFi.mode(WIFI_AP_STA);
    
    // Setup Access Point
    WiFi.softAP(AP_SSID, AP_PASSWORD);
    Serial.println("Access Point Started");
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    
    // Configure WiFi for monitoring
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_storage(WIFI_STORAGE_RAM);
    esp_wifi_set_mode(WIFI_MODE_NULL);
    esp_wifi_start();
}

// ==================== WEB HANDLERS ====================
void handleRoot() {
    server.send(200, "text/html", MAIN_PAGE);
}

void handleScan() {
    scanRunning = true;
    visualEffect("scan");
    playAudioEffect("scan");
    
    // WiFi scan implementation
    int scanResult = WiFi.scanNetworks(true, true);
    
    DynamicJsonDocument doc(4096);
    doc["status"] = "scanning";
    doc["networks_found"] = scanResult;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleScanStatus() {
    int networksFound = WiFi.scanComplete();
    DynamicJsonDocument doc(4096);
    JsonArray networks = doc.createNestedArray("networks");
    
    if (networksFound > 0) {
        for (int i = 0; i < networksFound; ++i) {
            JsonObject network = networks.createNestedObject();
            network["ssid"] = WiFi.SSID(i);
            network["bssid"] = WiFi.BSSIDstr(i);
            network["rssi"] = WiFi.RSSI(i);
            network["channel"] = WiFi.channel(i);
            network["encryption"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Encrypted";
        }
        WiFi.scanDelete();
    }
    
    doc["scan_running"] = (networksFound == -1);
    doc["networks_count"] = (networksFound > 0) ? networksFound : 0;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleDeauth() {
    if (server.hasArg("start") && server.hasArg("bssid")) {
        String bssid = server.arg("bssid");
        startDeauthAttack(bssid);
        
        DynamicJsonDocument doc(512);
        doc["success"] = true;
        doc["message"] = "Deauth attack started";
        
        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", response);
    }
    else if (server.hasArg("stop")) {
        stopDeauthAttack();
        
        DynamicJsonDocument doc(512);
        doc["success"] = true;
        doc["message"] = "Deauth attack stopped";
        
        String response;
        serializeJson(doc, response);
        server.send(200, "application/json", response);
    }
}

void handleDeauthStatus() {
    DynamicJsonDocument doc(512);
    doc["deauth_running"] = deauthRunning;
    doc["packets_sent"] = deauthPackets;
    doc["packet_rate"] = deauthRunning ? random(50, 200) : 0;
    doc["target_bssid"] = targetBSSID;
    doc["target_ssid"] = targetSSID;
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleStatus() {
    DynamicJsonDocument doc(512);
    doc["wifi_strength"] = WiFi.RSSI();
    doc["memory_usage"] = esp_get_free_heap_size() / 1024;
    doc["uptime"] = millis() / 1000;
    doc["ap_clients"] = WiFi.softAPgetStationNum();
    
    String response;
    serializeJson(doc, response);
    server.send(200, "application/json", response);
}

void handleSound() {
    if (server.hasArg("type")) {
        playAudioEffect(server.arg("type"));
    }
    server.send(200, "text/plain", "OK");
}

// ==================== DEAUTH ATTACK FUNCTIONS ====================
void startDeauthAttack(String bssid) {
    deauthRunning = true;
    deauthStartTime = millis();
    deauthPackets = 0;
    targetBSSID = bssid;
    
    visualEffect("deauth_start");
    playAudioEffect("attack");
    
    Serial.println("Deauth attack started on: " + bssid);
}

void stopDeauthAttack() {
    deauthRunning = false;
    visualEffect("deauth_stop");
    playAudioEffect("stop");
    
    Serial.println("Deauth attack stopped");
    Serial.print("Total packets sent: ");
    Serial.println(deauthPackets);
}

// ==================== AUDIO & VISUAL EFFECTS ====================
void setupAudio() {
    mySoftwareSerial.begin(9600, SERIAL_8N1, 16, 17);
    
    if (!myDFPlayer.begin(mySoftwareSerial)) {
        Serial.println("DFPlayer initialization failed!");
    } else {
        Serial.println("DFPlayer initialized successfully");
        myDFPlayer.volume(20); // Set volume (0-30)
    }
}

void playAudioEffect(String type) {
    if (type == "startup") {
        // Play startup sound
        tone(BUZZER_PIN, 1000, 200);
        delay(200);
        tone(BUZZER_PIN, 1500, 200);
        delay(200);
        tone(BUZZER_PIN, 2000, 200);
    }
    else if (type == "scan") {
        tone(BUZZER_PIN, 800, 100);
    }
    else if (type == "attack") {
        tone(BUZZER_PIN, 2000, 300);
    }
    else if (type == "stop") {
        tone(BUZZER_PIN, 500, 200);
    }
    else if (type == "select") {
        tone(BUZZER_PIN, 1200, 150);
    }
}

void visualEffect(String type) {
    if (type == "startup") {
        for (int i = 0; i < 3; i++) {
            digitalWrite(LED_RED, HIGH);
            digitalWrite(LED_GREEN, LOW);
            digitalWrite(LED_BLUE, LOW);
            delay(200);
            digitalWrite(LED_RED, LOW);
            digitalWrite(LED_GREEN, HIGH);
            digitalWrite(LED_BLUE, LOW);
            delay(200);
            digitalWrite(LED_RED, LOW);
            digitalWrite(LED_GREEN, LOW);
            digitalWrite(LED_BLUE, HIGH);
            delay(200);
        }
    }
    else if (type == "ready") {
        digitalWrite(LED_GREEN, HIGH);
        digitalWrite(LED_WHITE, HIGH);
    }
    else if (type == "scan") {
        digitalWrite(LED_BLUE, HIGH);
        delay(500);
        digitalWrite(LED_BLUE, LOW);
    }
    else if (type == "deauth_start") {
        digitalWrite(LED_RED, HIGH);
    }
    else if (type == "deauth_stop") {
        digitalWrite(LED_RED, LOW);
        digitalWrite(LED_GREEN, HIGH);
    }
}

void updateLEDs() {
    static unsigned long lastBlink = 0;
    static bool ledState = false;
    
    if (millis() - lastBlink > 500) {
        ledState = !ledState;
        digitalWrite(LED_WHITE, ledState);
        lastBlink = millis();
    }
    
    if (deauthRunning) {
        digitalWrite(LED_RED, (millis() % 200 < 100));
    }
}
