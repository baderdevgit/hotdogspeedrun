#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

// ====== WiFi Settings ======
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ====== Hardware Pins ======
Servo servo1;
Servo servo2;

int servo1Pin = 27;
int servo2Pin = 26;
int relay1Pin = 18;
int relay2Pin = 19;

// ====== Web Server ======
WebServer server(80);

// Track if sequence should run
volatile bool runSequence = false;

// ====== Sequence Function ======
void runActionSequence() {
  // Servo 1: 0 -> 180
  servo1.write(0);
  delay(500);
  servo1.write(180);
  delay(1000);

  // Servo 1: 180 -> 0
  servo1.write(0);
  delay(1000);

  // Relay1 ON 1s
  digitalWrite(relay1Pin, HIGH);
  delay(1000);
  digitalWrite(relay1Pin, LOW);
  delay(1000);

  // Relay1 + Relay2 ON 1s
  digitalWrite(relay1Pin, HIGH);
  digitalWrite(relay2Pin, HIGH);
  delay(1000);
  digitalWrite(relay1Pin, LOW);
  digitalWrite(relay2Pin, LOW);
  delay(1000);

  // Servo 2: 0 -> 90
  servo2.write(0);
  delay(500);
  servo2.write(90);
}

// ====== Web Handlers ======
void handleRoot() {
  String html = R"rawliteral(
  <!DOCTYPE html>
  <html>
  <head>
    <title>ESP32 Stopwatch</title>
    <style>
      body { font-family: Arial; text-align: center; margin-top: 50px; }
      button { font-size: 20px; padding: 10px 20px; margin: 10px; }
      #time { font-size: 30px; margin-top: 20px; }
    </style>
  </head>
  <body>
    <div id="time">0.000s</div>
    <div>
      <button id="startBtn" onclick="startStopwatch()">Start</button>
      <button id="stopBtn" onclick="stopStopwatch()">Stop</button>
    </div>

    <!-- Audio element -->
    <audio id="song" src="music/song.mp3"></audio>

    <script>
      let timer = null;
      let startTime = 0;
      const song = document.getElementById("song");

      function updateTime() {
        const now = Date.now();
        const elapsed = ((now - startTime) / 1000).toFixed(3); // 3 decimals
        document.getElementById("time").textContent = elapsed + "s";
      }

      function startStopwatch() {
        fetch('/start'); // tell ESP32 to run sequence
        startTime = Date.now();
        timer = setInterval(updateTime, 10); // update every 10ms
        document.getElementById("startBtn").disabled = true; // disable start

        // Play song
        song.currentTime = 0;
        song.play();
      }

      function stopStopwatch() {
        clearInterval(timer);
        timer = null;
        document.getElementById("startBtn").disabled = false; // re-enable start

        // Pause song
        song.pause();
      }
    </script>
  </body>
  </html>
  )rawliteral";

  server.send(200, "text/html", html);
}

void handleStart() {
  runSequence = true;   // set flag to run sequence
  server.send(200, "text/plain", "Sequence started");
}

void setup() {
  // Serial debug
  Serial.begin(115200);

  // Hardware setup
  servo1.attach(servo1Pin);
  servo2.attach(servo2Pin);
  pinMode(relay1Pin, OUTPUT);
  pinMode(relay2Pin, OUTPUT);
  digitalWrite(relay1Pin, LOW);
  digitalWrite(relay2Pin, LOW);

  // WiFi setup
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected! IP address: ");
  Serial.println(WiFi.localIP());

  // Web server setup
  server.on("/", handleRoot);
  server.on("/start", handleStart);
  server.begin();
}

void loop() {
  server.handleClient();

  // If "Start" pressed → run sequence once
  if (runSequence) {
    runSequence = false;
    runActionSequence();
  }
}
