#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <ESP32Servo.h>

void ResetPlateServo();
void MoveBunServo(bool reset);
void MoveWeinerServo(bool reset);
void MovePlateDown();

// ====== WiFi Settings ======
const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

// ====== Hardware Pins ======
Servo bunServo;
Servo weinerServo;
Servo plateServo;

int bunServoPin = 14;
int weinerServoPin = 27;
int plateServoPin = 26;
int flamethrowerRelayPin = 13;
int taserRelayPin = 12;
int tensRelayPin = 33;

// // ====== Web Server ======
// WebServer server(80);

// // Track if sequence should run
// volatile bool runSequence = false;

// // ====== Sequence Function ======
// void runActionSequence() {

// }

// // ====== Web Handlers ======
// void handleRoot() {
//   String html = R"rawliteral(
//   <!DOCTYPE html>
//   <html>
//   <head>
//     <title>ESP32 Stopwatch</title>
//     <style>
//       body { font-family: Arial; text-align: center; margin-top: 50px; }
//       button { font-size: 20px; padding: 10px 20px; margin: 10px; }
//       #time { font-size: 30px; margin-top: 20px; }
//     </style>
//   </head>
//   <body>
//     <div id="time">0.000s</div>
//     <div>
//       <button id="startBtn" onclick="startStopwatch()">Start</button>
//       <button id="stopBtn" onclick="stopStopwatch()">Stop</button>
//     </div>

//     <!-- Audio element -->
//     <audio id="song" src="music/song.mp3"></audio>

//     <script>
//       let timer = null;
//       let startTime = 0;
//       const song = document.getElementById("song");

//       function updateTime() {
//         const now = Date.now();
//         const elapsed = ((now - startTime) / 1000).toFixed(3); // 3 decimals
//         document.getElementById("time").textContent = elapsed + "s";
//       }

//       function startStopwatch() {
//         fetch('/start'); // tell ESP32 to run sequence
//         startTime = Date.now();
//         timer = setInterval(updateTime, 10); // update every 10ms
//         document.getElementById("startBtn").disabled = true; // disable start

//         // Play song
//         song.currentTime = 0;
//         song.play();
//       }

//       function stopStopwatch() {
//         clearInterval(timer);
//         timer = null;
//         document.getElementById("startBtn").disabled = false; // re-enable start

//         // Pause song
//         song.pause();
//       }
//     </script>
//   </body>
//   </html>
//   )rawliteral";

//   server.send(200, "text/html", html);
// }

// void handleStart() {
//   runSequence = true;   // set flag to run sequence
//   server.send(200, "text/plain", "Sequence started");
// }

void setup() {
  // Serial debug
  Serial.begin(115200);

  // Hardware setup
  bunServo.attach(bunServoPin);
  weinerServo.attach(weinerServoPin);
  plateServo.attach(plateServoPin);

  pinMode(flamethrowerRelayPin, OUTPUT);
  pinMode(taserRelayPin, OUTPUT);
  pinMode(tensRelayPin, OUTPUT);
  digitalWrite(flamethrowerRelayPin, HIGH);
  digitalWrite(taserRelayPin, HIGH);
  digitalWrite(tensRelayPin, LOW);

  MoveBunServo(true);
  MoveWeinerServo(true);
  ResetPlateServo();

  // WiFi setup
  // WiFi.begin(ssid, password);
  // Serial.print("Connecting to WiFi");
  // while (WiFi.status() != WL_CONNECTED) {
  //   delay(500);
  //   Serial.print(".");
  // }
  // Serial.println("\nConnected! IP address: ");
  // Serial.println(WiFi.localIP());

  // Web server setup
  // server.on("/", handleRoot);
  // server.on("/start", handleStart);
  // server.begin();
}

void loop() {
  // server.handleClient();

  // // If "Start" pressed → run sequence once
  // if (runSequence) {
  //   runSequence = false;
  //   runActionSequence();
  // }

  if (Serial.available() > 0) {
    char key = Serial.read();
    if (key == 'd') {
      Serial.println("You pressed D!");
      ResetPlateServo();

      MoveBunServo(false);
      delay(500);
      MoveBunServo(true);
      delay(100);
      MoveWeinerServo(false);
      delay(600);
      MoveWeinerServo(true);
      delay(500);

      digitalWrite(taserRelayPin, LOW);
      delay(200);
      digitalWrite(flamethrowerRelayPin, LOW);

      delay(2000);

      digitalWrite(flamethrowerRelayPin, HIGH);
      digitalWrite(taserRelayPin, HIGH);

      delay(1000);

      digitalWrite(tensRelayPin, HIGH);
      delay(2000);
      digitalWrite(tensRelayPin, LOW);


      delay(500);
      plateServo.write(0);



    }

    else if (key == 'f') {
      Serial.println("You pressed F!");

      MoveBunServo(true);
      MoveWeinerServo(true);
      ResetPlateServo();
    }
    else if (key == 'g') {
      MovePlateDown();
    }

  }
}

void MoveBunServo(bool reset) {
  if(reset) {
    bunServo.write(130);
  }
  else
  {
    bunServo.write(10);
  }
}

void MoveWeinerServo(bool reset) {
  if(reset) {
    weinerServo.write(45);
  }
  else
  {
    weinerServo.write(160);
  }
}

void ResetPlateServo() {
    plateServo.write(15);
}

void MovePlateDown() {
      plateServo.write(0);
}
