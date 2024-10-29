#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

ESP8266WebServer server(80);
Servo myServo;

int ENA = D3;
int IN1 = D1;
int IN2 = D2;
int SERVO_PIN = D0;

int motorSpeed = 0;
int steeringAngle = 90;

const char *ssid = "LiPo CAR";
const char *password = "12345678";

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("Access Point IP: ");
  Serial.println(IP);

  pinMode(ENA, OUTPUT);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);

  myServo.attach(SERVO_PIN);
  myServo.write(steeringAngle); 

  server.on("/", handleRoot);
  server.on("/setSpeed", handleSetSpeed);
  server.on("/setSteeringAngle", handleSetSteeringAngle);

  server.begin();
  Serial.println("HTTP server started.");
}

void loop() {
  server.handleClient();

  if(motorSpeed > -30 && motorSpeed < 30) {
    analogWrite(ENA, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    return;
  }

  if (motorSpeed < 0) {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, abs(motorSpeed));
  } else {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, motorSpeed);
  }
}
void handleRoot() {
  String html = "<html><body>";
  html += "<h1>LiPo CAR</h1>";
  html += "<div style='margin: 50px;'>";
  html += "<label>Speed: </label><input type='range' min='-51' max='51' value='" + String(motorSpeed) + "' id='speedSlider' oninput='updateSpeed(this.value)' style='transform: rotate(270deg); width: 200px; margin-top: 50px;'>";
  html += "</div>";
  html += "<br><br>";
  html += "<label>Steering Angle: </label><input type='range' min='0' max='' step='10'  value='" + String(steeringAngle) + "' id='steeringSlider' oninput='updateSteeringAngle(this.value)'>";
  html += "<script>";
  html += "function updateSpeed(val) {";
  html += "  fetch('/setSpeed?value=' + val * 5);";
  html += "}";
  html += "function updateSteeringAngle(angle) {";
  html += "  fetch('/setSteeringAngle?angle=' + angle);";
  html += "  document.getElementById('steeringSlider').value = angle;";
  html += "}";
  html += "</script>";
  html += "</body></html>";
  server.send(200, "text/html", html);
}


void handleSetSpeed() {
  if (server.hasArg("value")) {
    motorSpeed = server.arg("value").toInt();
    analogWrite(ENA, motorSpeed);
    server.send(200, "text/plain", "Speed set to " + String(motorSpeed));
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}


void handleSetSteeringAngle() {
  if (server.hasArg("angle")) {
    steeringAngle = server.arg("angle").toInt();
    myServo.write(steeringAngle);
    server.send(200, "text/plain", "Steering angle set to " + String(steeringAngle));
  } else {
    server.send(400, "text/plain", "Invalid request");
  }
}
