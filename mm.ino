/***************************************************
 * BARA - Ultimate Wi-Fi Pentesting Tool
 * Developer: Ahmed Nour Ahmed from Qena
 * Version: 2.0.0
 * Description: Advanced Wi-Fi Deauthentication Tool
 * For Educational and Authorized Testing Only
 ***************************************************/

#include <WiFi.h>
#include <WebServer.h>
#include <esp_wifi.h>
#include <ArduinoJson.h>
#include <SPIFFS.h>

// Audio and Visual Effects
#include "DFRobotDFPlayerMini.h"

// Configuration
#define AP_SSID "bara"
#define AP_PASSWORD "A7med@Elshab7"
#define SERIAL_BAUD 115200

// Global Variables
WebServer server(80);
DFRobotDFPlayerMini audioPlayer;

// Hacking-themed HTML with Bloody Design
const char* bloodyHTML = R"rawliteral(
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
            color: #ff0000;
            font-family: 'Orbitron', monospace;
            overflow-x: hidden;
            background-image: 
                radial-gradient(circle at 10% 20%, rgba(255, 0, 0, 0.1) 0%, transparent 20%),
                radial-gradient(circle at 90% 80%, rgba(139, 0, 0, 0.1) 0%, transparent 20%),
                linear-gradient(45deg, #000000 0%, #1a0000 50%, #000000 100%);
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
                repeating-linear-gradient(0deg, transparent, transparent 2px, rgba(255, 0, 0, 0.03) 2px, rgba(255, 0, 0, 0.03) 4px),
                repeating-linear-gradient(90deg, transparent, transparent 2px, rgba(139, 0, 0, 0.02) 2px, rgba(139, 0, 0, 0.02) 4px);
            pointer-events: none;
            z-index: -1;
        }
        
        .blood-drip {
            position: fixed;
            top: 0;
            width: 4px;
            height: 100px;
            background: linear-gradient(to bottom, rgba(255, 0, 0, 0.8), transparent);
            animation: drip 8s infinite;
            z-index: 9999;
        }
        
        @keyframes drip {
            0% { transform: translateY(-100px); opacity: 0; }
            10% { opacity: 1; }
            90% { opacity: 1; }
            100% { transform: translateY(100vh); opacity: 0; }
        }
        
        .header {
            background: linear-gradient(135deg, #200000 0%, #000000 50%, #200000 100%);
            padding: 30px 20px;
            text-align: center;
            border-bottom: 3px solid #8b0000;
            position: relative;
            overflow: hidden;
        }
        
        .header::before {
            content: '';
            position: absolute;
            top: 0;
            left: 0;
            right: 0;
            bottom: 0;
            background: 
                radial-gradient(circle at 20% 50%, rgba(255, 0, 0, 0.3) 0%, transparent 50%),
                radial-gradient(circle at 80% 20%, rgba(139, 0, 0, 0.2) 0%, transparent 50%);
            animation: pulse 4s ease-in-out infinite;
        }
        
        @keyframes pulse {
            0%, 100% { opacity: 0.5; }
            50% { opacity: 1; }
        }
        
        .title {
            font-family: 'Bloody', 'Orbitron', sans-serif;
            font-size: 4em;
            font-weight: 900;
            text-transform: uppercase;
            color: #ff0000;
            text-shadow: 
                0 0 10px #ff0000,
                0 0 20px #ff0000,
                0 0 40px #8b0000,
                0 0 80px #8b0000;
            margin-bottom: 10px;
            position: relative;
            animation: glitch 3s infinite;
        }
        
        @keyframes glitch {
            0% { transform: translate(0); }
            20% { transform: translate(-2px, 2px); }
            40% { transform: translate(-2px, -2px); }
            60% { transform: translate(2px, 2px); }
            80% { transform: translate(2px, -2px); }
            100% { transform: translate(0); }
        }
        
        .subtitle {
            font-size: 1.2em;
            color: #ff4444;
            text-shadow: 0 0 10px rgba(255, 0, 0, 0.5);
            margin-bottom: 20px;
        }
        
        .developer {
            font-size: 1em;
            color: #ff6666;
            font-style: italic;
        }
        
        .container {
            max-width: 1200px;
            margin: 0 auto;
            padding: 20px;
        }
        
        .panel {
            background: rgba(20, 0, 0, 0.8);
            border: 2px solid #8b0000;
            border-radius: 10px;
            padding: 25px;
            margin: 20px 0;
            box-shadow: 
                0 0 20px rgba(255, 0, 0, 0.3),
                inset 0 0 20px rgba(0, 0, 0, 0.5);
            position: relative;
            overflow: hidden;
        }
        
        .panel::before {
            content: '';
            position: absolute;
            top: -50%;
            left: -50%;
            width: 200%;
            height: 200%;
            background: linear-gradient(45deg, transparent, rgba(255, 0, 0, 0.1), transparent);
            animation: shine 6s infinite;
            pointer-events: none;
        }
        
        @keyframes shine {
            0% { transform: translateX(-100%) translateY(-100%) rotate(45deg); }
            100% { transform: translateX(100%) translateY(100%) rotate(45deg); }
        }
        
        .panel-title {
            font-size: 1.8em;
            color: #ff0000;
            margin-bottom: 20px;
            text-shadow: 0 0 10px rgba(255, 0, 0, 0.5);
            border-bottom: 2px solid #8b0000;
            padding-bottom: 10px;
        }
        
        .btn {
            background: linear-gradient(135deg, #8b0000 0%, #ff0000 50%, #8b0000 100%);
            color: white;
            border: none;
            padding: 15px 30px;
            font-size: 1.1em;
            font-weight: bold;
            border-radius: 5px;
            cursor: pointer;
            margin: 10px 5px;
            text-transform: uppercase;
            font-family: 'Orbitron', monospace;
            transition: all 0.3s ease;
            box-shadow: 0 0 15px rgba(255, 0, 0, 0.5);
            position: relative;
            overflow: hidden;
        }
        
        .btn::before {
            content: '';
            position: absolute;
            top: 0;
            left: -100%;
            width: 100%;
            height: 100%;
            background: linear-gradient(90deg, transparent, rgba(255, 255, 255, 0.2), transparent);
            transition: left 0.5s;
        }
        
        .btn:hover::before {
            left: 100%;
        }
        
        .btn:hover {
            transform: translateY(-2px);
            box-shadow: 0 0 25px rgba(255, 0, 0, 0.8);
        }
        
        .btn:active {
            transform: translateY(0);
        }
        
        .btn-danger {
            background: linear-gradient(135deg, #600000 0%, #ff0000 50%, #600000 100%);
            box-shadow: 0 0 20px rgba(255, 0, 0, 0.7);
        }
        
        .btn-success {
            background: linear-gradient(135deg, #006000 0%, #00ff00 50%, #006000 100%);
            box-shadow: 0 0 20px rgba(0, 255, 0, 0.5);
        }
        
        .network-list {
            display: grid;
            grid-template-columns: repeat(auto-fill, minmax(300px, 1fr));
            gap: 15px;
            margin: 20px 0;
        }
        
        .network-item {
            background: rgba(40, 0, 0, 0.8);
            border: 1px solid #8b0000;
            border-radius: 8px;
            padding: 15px;
            transition: all 0.3s ease;
            position: relative;
        }
        
        .network-item:hover {
            transform: translateY(-3px);
            box-shadow: 0 5px 15px rgba(255, 0, 0, 0.3);
            border-color: #ff0000;
        }
        
        .network-ssid {
            font-weight: bold;
            color: #ff4444;
            font-size: 1.2em;
            margin-bottom: 8px;
        }
        
        .network-info {
            color: #ff6666;
            font-size: 0.9em;
            margin: 3px 0;
        }
        
        .signal-strength {
            height: 10px;
            background: #330000;
            border-radius: 5px;
            margin: 10px 0;
            overflow: hidden;
        }
        
        .signal-bar {
            height: 100%;
            background: linear-gradient(90deg, #00ff00, #ffff00, #ff0000);
            border-radius: 5px;
            transition: width 0.5s ease;
        }
        
        .status-panel {
            text-align: center;
            padding: 20px;
        }
        
        .status-indicator {
            display: inline-block;
            width: 20px;
            height: 20px;
            border-radius: 50%;
            margin-right: 10px;
            animation: pulse-status 2s infinite;
        }
        
        .status-active {
            background: #00ff00;
            box-shadow: 0 0 10px #00ff00;
        }
        
        .status-scanning {
            background: #ffff00;
            box-shadow: 0 0 10px #ffff00;
        }
        
        .status-error {
            background: #ff0000;
            box-shadow: 0 0 10px #ff0000;
        }
        
        @keyframes pulse-status {
            0%, 100% { opacity: 1; }
            50% { opacity: 0.5; }
        }
        
        .console {
            background: #000000;
            border: 2px solid #8b0000;
            border-radius: 5px;
            padding: 15px;
            height: 200px;
            overflow-y: auto;
            font-family: 'Courier New', monospace;
            font-size: 0.9em;
            color: #00ff00;
            text-shadow: 0 0 5px #00ff00;
            margin: 20px 0;
        }
        
        .console-line {
            margin: 5px 0;
            animation: typewriter 0.1s ease-in;
        }
        
        @keyframes typewriter {
            from { opacity: 0; }
            to { opacity: 1; }
        }
        
        .scan-progress {
            width: 100%;
            height: 10px;
            background: #330000;
            border-radius: 5px;
            margin: 20px 0;
            overflow: hidden;
        }
        
        .progress-bar {
            height: 100%;
            background: linear-gradient(90deg, #ff0000, #ffff00, #00ff00);
            border-radius: 5px;
            width: 0%;
            transition: width 0.3s ease;
        }
        
        .matrix-rain {
            position: fixed;
            top: 0;
            left: 0;
            width: 100%;
            height: 100%;
            pointer-events: none;
            z-index: -1;
            opacity: 0.1;
        }
        
        .audio-controls {
            position: fixed;
            bottom: 20px;
            right: 20px;
            z-index: 1000;
        }
        
        .audio-btn {
            background: rgba(139, 0, 0, 0.8);
            color: white;
            border: none;
            padding: 10px 15px;
            border-radius: 50%;
            cursor: pointer;
            font-size: 1.2em;
            box-shadow: 0 0 10px rgba(255, 0, 0, 0.5);
        }
        
        @media (max-width: 768px) {
            .title { font-size: 2.5em; }
            .network-list { grid-template-columns: 1fr; }
        }
    </style>
</head>
<body>
    <div class="matrix-rain" id="matrixRain"></div>
    
    <!-- Blood drips -->
    <div class="blood-drip" style="left: 10%; animation-delay: 0s;"></div>
    <div class="blood-drip" style="left: 20%; animation-delay: 1s;"></div>
    <div class="blood-drip" style="left: 30%; animation-delay: 2s;"></div>
    <div class="blood-drip" style="left: 40%; animation-delay: 3s;"></div>
    <div class="blood-drip" style="left: 50%; animation-delay: 4s;"></div>
    <div class="blood-drip" style="left: 60%; animation-delay: 5s;"></div>
    <div class="blood-drip" style="left: 70%; animation-delay: 6s;"></div>
    <div class="blood-drip" style="left: 80%; animation-delay: 7s;"></div>
    <div class="blood-drip" style="left: 90%; animation-delay: 0s;"></div>
    
    <div class="header">
        <h1 class="title">BARA</h1>
        <p class="subtitle">ULTIMATE WI-FI PENTESTING TOOL</p>
        <p class="developer">Developed by Ahmed Nour Ahmed - Qena</p>
    </div>
    
    <div class="container">
        <div class="panel">
            <h2 class="panel-title">SYSTEM STATUS</h2>
            <div class="status-panel">
                <span class="status-indicator status-active" id="statusIndicator"></span>
                <span id="statusText">SYSTEM READY</span>
            </div>
            <div class="scan-progress">
                <div class="progress-bar" id="scanProgress"></div>
            </div>
            <button class="btn" onclick="startScan()">START NETWORK SCAN</button>
            <button class="btn btn-danger" onclick="stopScan()">STOP SCAN</button>
            <button class="btn btn-success" onclick="systemInfo()">SYSTEM INFO</button>
        </div>
        
        <div class="panel">
            <h2 class="panel-title">DETECTED NETWORKS</h2>
            <div class="network-list" id="networkList">
                <div class="network-item">
                    <div class="network-ssid">Scanning...</div>
                    <div class="network-info">Waiting for scan results</div>
                </div>
            </div>
        </div>
        
        <div class="panel">
            <h2 class="panel-title">DEAUTH ATTACK</h2>
            <div id="attackControls" style="display: none;">
                <input type="text" id="targetBSSID" placeholder="Target BSSID" style="width: 100%; padding: 10px; margin: 10px 0; background: #330000; border: 1px solid #8b0000; color: white; border-radius: 5px;">
                <input type="text" id="targetChannel" placeholder="Channel (optional)" style="width: 100%; padding: 10px; margin: 10px 0; background: #330000; border: 1px solid #8b0000; color: white; border-radius: 5px;">
                <button class="btn btn-danger" onclick="startDeauth()">LAUNCH DEAUTH ATTACK</button>
                <button class="btn" onclick="stopDeauth()">STOP ATTACK</button>
            </div>
            <div id="noTarget" style="text-align: center; color: #ff6666;">
                Select a network from above to enable attack controls
            </div>
        </div>
        
        <div class="panel">
            <h2 class="panel-title">SYSTEM CONSOLE</h2>
            <div class="console" id="consoleOutput">
                <div class="console-line">> BARA System Initialized</div>
                <div class="console-line">> Welcome to Ultimate Wi-Fi Pentesting Tool</div>
                <div class="console-line">> Developed by Ahmed Nour Ahmed</div>
                <div class="console-line">> System Ready for Operations</div>
            </div>
        </div>
    </div>
    
    <div class="audio-controls">
        <button class="audio-btn" onclick="toggleAudio()">🔊</button>
    </div>
    
    <script>
        // Matrix Rain Effect
        const matrixRain = document.getElementById('matrixRain');
        const canvas = document.createElement('canvas');
        const ctx = canvas.getContext('2d');
        matrixRain.appendChild(canvas);
        
        canvas.width = window.innerWidth;
        canvas.height = window.innerHeight;
        
        const chars = '01バラデスクトップハッキングウィファイ';
        const charArray = chars.split('');
        const fontSize = 14;
        const columns = canvas.width / fontSize;
        const drops = [];
        
        for(let i = 0; i < columns; i++) {
            drops[i] = 1;
        }
        
        function drawMatrix() {
            ctx.fillStyle = 'rgba(0, 0, 0, 0.04)';
            ctx.fillRect(0, 0, canvas.width, canvas.height);
            
            ctx.fillStyle = '#0f0';
            ctx.font = fontSize + 'px monospace';
            
            for(let i = 0; i < drops.length; i++) {
                const text = charArray[Math.floor(Math.random() * charArray.length)];
                ctx.fillText(text, i * fontSize, drops[i] * fontSize);
                
                if(drops[i] * fontSize > canvas.height && Math.random() > 0.975) {
                    drops[i] = 0;
                }
                drops[i]++;
            }
        }
        
        setInterval(drawMatrix, 35);
        
        // Network Scanning
        let isScanning = false;
        let selectedNetwork = null;
        
        function startScan() {
            if(isScanning) return;
            
            isScanning = true;
            document.getElementById('statusText').textContent = 'SCANNING NETWORKS...';
            document.getElementById('statusIndicator').className = 'status-indicator status-scanning';
            
            addConsoleLine('> Starting network scan...');
            playSound('scan_start');
            
            fetch('/scan')
                .then(response => response.json())
                .then(data => {
                    displayNetworks(data.networks);
                    isScanning = false;
                    document.getElementById('statusText').textContent = 'SCAN COMPLETE';
                    document.getElementById('statusIndicator').className = 'status-indicator status-active';
                    addConsoleLine('> Network scan completed: ' + data.networks.length + ' networks found');
                    playSound('scan_complete');
                })
                .catch(error => {
                    console.error('Error:', error);
                    isScanning = false;
                    document.getElementById('statusText').textContent = 'SCAN FAILED';
                    document.getElementById('statusIndicator').className = 'status-indicator status-error';
                    addConsoleLine('> ERROR: Scan failed - ' + error);
                    playSound('error');
                });
            
            // Simulate progress
            let progress = 0;
            const progressBar = document.getElementById('scanProgress');
            const progressInterval = setInterval(() => {
                if(progress < 100) {
                    progress += 2;
                    progressBar.style.width = progress + '%';
                } else {
                    clearInterval(progressInterval);
                }
            }, 50);
        }
        
        function stopScan() {
            if(!isScanning) return;
            
            fetch('/stop-scan')
                .then(() => {
                    isScanning = false;
                    document.getElementById('statusText').textContent = 'SCAN STOPPED';
                    document.getElementById('statusIndicator').className = 'status-indicator status-active';
                    addConsoleLine('> Network scan stopped by user');
                    playSound('action');
                });
        }
        
        function displayNetworks(networks) {
            const networkList = document.getElementById('networkList');
            networkList.innerHTML = '';
            
            if(networks.length === 0) {
                networkList.innerHTML = '<div class="network-item"><div class="network-ssid">No networks found</div></div>';
                return;
            }
            
            networks.forEach(network => {
                const networkItem = document.createElement('div');
                networkItem.className = 'network-item';
                networkItem.onclick = () => selectNetwork(network);
                
                const signalPercent = Math.min(100, Math.max(0, (network.rssi + 100) * 2));
                
                networkItem.innerHTML = `
                    <div class="network-ssid">${network.ssid || 'Hidden Network'}</div>
                    <div class="network-info">BSSID: ${network.bssid}</div>
                    <div class="network-info">Channel: ${network.channel}</div>
                    <div class="network-info">RSSI: ${network.rssi} dBm</div>
                    <div class="network-info">Encryption: ${network.encryption}</div>
                    <div class="signal-strength">
                        <div class="signal-bar" style="width: ${signalPercent}%"></div>
                    </div>
                    <button class="btn" onclick="event.stopPropagation(); selectNetwork(${JSON.stringify(network).replace(/"/g, '&quot;')})">SELECT</button>
                `;
                
                networkList.appendChild(networkItem);
            });
        }
        
        function selectNetwork(network) {
            selectedNetwork = network;
            document.getElementById('targetBSSID').value = network.bssid;
            document.getElementById('targetChannel').value = network.channel;
            document.getElementById('attackControls').style.display = 'block';
            document.getElementById('noTarget').style.display = 'none';
            
            addConsoleLine('> Selected network: ' + (network.ssid || 'Hidden') + ' (' + network.bssid + ')');
            playSound('select');
        }
        
        function startDeauth() {
            const bssid = document.getElementById('targetBSSID').value;
            const channel = document.getElementById('targetChannel').value;
            
            if(!bssid) {
                addConsoleLine('> ERROR: No target BSSID specified');
                playSound('error');
                return;
            }
            
            addConsoleLine('> Starting deauth attack on: ' + bssid);
            playSound('attack_start');
            
            fetch('/deauth', {
                method: 'POST',
                headers: {
                    'Content-Type': 'application/json',
                },
                body: JSON.stringify({
                    bssid: bssid,
                    channel: channel || 1
                })
            })
            .then(response => response.json())
            .then(data => {
                if(data.success) {
                    addConsoleLine('> Deauth attack launched successfully');
                    document.getElementById('statusText').textContent = 'DEAUTH ATTACK ACTIVE';
                    document.getElementById('statusIndicator').className = 'status-indicator status-error';
                    playSound('attack_active');
                } else {
                    addConsoleLine('> ERROR: ' + data.error);
                    playSound('error');
                }
            })
            .catch(error => {
                addConsoleLine('> ERROR: ' + error);
                playSound('error');
            });
        }
        
        function stopDeauth() {
            addConsoleLine('> Stopping deauth attack');
            playSound('action');
            
            fetch('/stop-deauth')
                .then(response => response.json())
                .then(data => {
                    if(data.success) {
                        addConsoleLine('> Deauth attack stopped');
                        document.getElementById('statusText').textContent = 'SYSTEM READY';
                        document.getElementById('statusIndicator').className = 'status-indicator status-active';
                        playSound('action');
                    }
                });
        }
        
        function systemInfo() {
            addConsoleLine('> Requesting system information...');
            playSound('info');
            
            fetch('/system-info')
                .then(response => response.json())
                .then(data => {
                    addConsoleLine('> System: ' + data.chip_model);
                    addConsoleLine('> CPU Freq: ' + data.cpu_freq + ' MHz');
                    addConsoleLine('> Free Heap: ' + data.free_heap + ' bytes');
                    addConsoleLine('> Flash Size: ' + data.flash_size + ' MB');
                });
        }
        
        function addConsoleLine(text) {
            const console = document.getElementById('consoleOutput');
            const line = document.createElement('div');
            line.className = 'console-line';
            line.textContent = text;
            console.appendChild(line);
            console.scrollTop = console.scrollHeight;
        }
        
        function playSound(type) {
            // Simulate sound effects - in real implementation, this would control audio
            const audioContext = new (window.AudioContext || window.webkitAudioContext)();
            
            switch(type) {
                case 'scan_start':
                    createBeep(audioContext, 800, 0.1);
                    break;
                case 'scan_complete':
                    createBeep(audioContext, 1200, 0.2);
                    createBeep(audioContext, 1600, 0.1);
                    break;
                case 'attack_start':
                    createSiren(audioContext);
                    break;
                case 'attack_active':
                    createPulse(audioContext);
                    break;
                case 'select':
                    createBeep(audioContext, 600, 0.05);
                    break;
                case 'action':
                    createBeep(audioContext, 400, 0.1);
                    break;
                case 'error':
                    createErrorSound(audioContext);
                    break;
                case 'info':
                    createBeep(audioContext, 1000, 0.1);
                    break;
            }
        }
        
        function createBeep(audioContext, frequency, duration) {
            const oscillator = audioContext.createOscillator();
            const gainNode = audioContext.createGain();
            
            oscillator.connect(gainNode);
            gainNode.connect(audioContext.destination);
            
            oscillator.frequency.value = frequency;
            oscillator.type = 'sine';
            
            gainNode.gain.setValueAtTime(0.3, audioContext.currentTime);
            gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + duration);
            
            oscillator.start(audioContext.currentTime);
            oscillator.stop(audioContext.currentTime + duration);
        }
        
        function createSiren(audioContext) {
            const oscillator = audioContext.createOscillator();
            const gainNode = audioContext.createGain();
            
            oscillator.connect(gainNode);
            gainNode.connect(audioContext.destination);
            
            oscillator.frequency.setValueAtTime(800, audioContext.currentTime);
            oscillator.frequency.exponentialRampToValueAtTime(1600, audioContext.currentTime + 0.5);
            oscillator.type = 'sawtooth';
            
            gainNode.gain.setValueAtTime(0.3, audioContext.currentTime);
            gainNode.gain.exponentialRampToValueAtTime(0.01, audioContext.currentTime + 0.5);
            
            oscillator.start(audioContext.currentTime);
            oscillator.stop(audioContext.currentTime + 0.5);
        }
        
        function createPulse(audioContext) {
            for(let i = 0; i < 3; i++) {
                setTimeout(() => {
                    createBeep(audioContext, 500, 0.1);
                }, i * 200);
            }
        }
        
        function createErrorSound(audioContext) {
            createBeep(audioContext, 200, 0.3);
            setTimeout(() => createBeep(audioContext, 150, 0.3), 100);
        }
        
        function toggleAudio() {
            // Toggle audio mute/unmute
            const btn = event.target;
            if(btn.textContent === '🔊') {
                btn.textContent = '🔇';
                addConsoleLine('> Audio muted');
            } else {
                btn.textContent = '🔊';
                addConsoleLine('> Audio enabled');
            }
        }
        
        // Initialize
        window.onload = function() {
            addConsoleLine('> Web interface loaded successfully');
            addConsoleLine('> Ready for pentesting operations');
        };
        
        // Handle window resize
        window.onresize = function() {
            canvas.width = window.innerWidth;
            canvas.height = window.innerHeight;
        };
    </script>
</body>
</html>
)rawliteral";

// Network scanning variables
bool scanComplete = false;
int networkCount = 0;
String scanResults = "";

// Deauth attack variables
bool deauthActive = false;
String targetBSSID = "";
int targetChannel = 1;

void setup() {
  Serial.begin(SERIAL_BAUD);
  
  // Initialize SPIFFS
  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS Mount Failed");
    return;
  }

  // Set WiFi to station mode and start access point
  WiFi.mode(WIFI_AP_STA);
  
  // Start access point
  WiFi.softAP(AP_SSID, AP_PASSWORD);
  
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Setup server routes
  setupServerRoutes();
  
  // Start server
  server.begin();
  
  Serial.println("BARA System Initialized Successfully");
  Serial.println("Web server started");
}

void loop() {
  server.handleClient();
  
  // Handle deauth attack if active
  if(deauthActive) {
    performDeauthAttack();
    delay(100);
  }
}

void setupServerRoutes() {
  server.on("/", HTTP_GET, [](){
    server.send(200, "text/html", bloodyHTML);
  });
  
  server.on("/scan", HTTP_GET, handleScan);
  server.on("/stop-scan", HTTP_GET, handleStopScan);
  server.on("/deauth", HTTP_POST, handleDeauth);
  server.on("/stop-deauth", HTTP_GET, handleStopDeauth);
  server.on("/system-info", HTTP_GET, handleSystemInfo);
}

void handleScan() {
  scanComplete = false;
  networkCount = 0;
  
  // Start WiFi scan in async mode
  int scanResult = WiFi.scanNetworks(true, true);
  
  if(scanResult == WIFI_SCAN_RUNNING) {
    // Wait for scan to complete with timeout
    unsigned long startTime = millis();
    while(WiFi.scanComplete() == WIFI_SCAN_RUNNING && millis() - startTime < 30000) {
      delay(100);
    }
  }
  
  int networkCount = WiFi.scanComplete();
  
  DynamicJsonDocument doc(4096);
  JsonArray networks = doc.createNestedArray("networks");
  
  if(networkCount > 0) {
    for(int i = 0; i < networkCount; i++) {
      JsonObject network = networks.createNestedObject();
      network["ssid"] = WiFi.SSID(i);
      network["bssid"] = WiFi.BSSIDstr(i);
      network["rssi"] = WiFi.RSSI(i);
      network["channel"] = WiFi.channel(i);
      network["encryption"] = getEncryptionType(WiFi.encryptionType(i));
    }
  }
  
  WiFi.scanDelete();
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void handleStopScan() {
  WiFi.scanDelete();
  server.send(200, "application/json", "{\"success\": true}");
}

void handleDeauth() {
  if(server.hasArg("plain")) {
    DynamicJsonDocument doc(256);
    deserializeJson(doc, server.arg("plain"));
    
    targetBSSID = doc["bssid"].as<String>();
    targetChannel = doc["channel"].as<int>();
    
    // Set WiFi channel
    esp_wifi_set_channel(targetChannel, WIFI_SECOND_CHAN_NONE);
    
    deauthActive = true;
    
    server.send(200, "application/json", "{\"success\": true}");
  } else {
    server.send(400, "application/json", "{\"success\": false, \"error\": \"No data provided\"}");
  }
}

void handleStopDeauth() {
  deauthActive = false;
  server.send(200, "application/json", "{\"success\": true}");
}

void handleSystemInfo() {
  DynamicJsonDocument doc(512);
  
  doc["chip_model"] = "ESP32";
  doc["cpu_freq"] = ESP.getCpuFreqMHz();
  doc["free_heap"] = ESP.getFreeHeap();
  doc["flash_size"] = ESP.getFlashChipSize() / (1024 * 1024);
  doc["sdk_version"] = ESP.getSdkVersion();
  
  String response;
  serializeJson(doc, response);
  server.send(200, "application/json", response);
}

void performDeauthAttack() {
  // This is a simplified deauth attack implementation
  // Note: Actual deauth packet sending requires low-level WiFi operations
  // and may be restricted in some environments
  
  Serial.println("Deauth attack active on: " + targetBSSID);
  
  // In a real implementation, you would send deauth packets here
  // This is just a simulation for educational purposes
}

String getEncryptionType(wifi_auth_mode_t encryptionType) {
  switch(encryptionType) {
    case WIFI_AUTH_OPEN:
      return "Open";
    case WIFI_AUTH_WEP:
      return "WEP";
    case WIFI_AUTH_WPA_PSK:
      return "WPA";
    case WIFI_AUTH_WPA2_PSK:
      return "WPA2";
    case WIFI_AUTH_WPA_WPA2_PSK:
      return "WPA/WPA2";
    case WIFI_AUTH_WPA2_ENTERPRISE:
      return "WPA2 Enterprise";
    case WIFI_AUTH_WPA3_PSK:
      return "WPA3";
    case WIFI_AUTH_WPA2_WPA3_PSK:
      return "WPA2/WPA3";
    default:
      return "Unknown";
  }
}
