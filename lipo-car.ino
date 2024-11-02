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

void setup()
{
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

void loop()
{
  server.handleClient();

  if (motorSpeed > -30 && motorSpeed < 30)
  {
    analogWrite(ENA, 0);
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, LOW);
    return;
  }

  if (motorSpeed < 0)
  {
    digitalWrite(IN1, LOW);
    digitalWrite(IN2, HIGH);
    analogWrite(ENA, abs(motorSpeed));
  }
  else
  {
    digitalWrite(IN1, HIGH);
    digitalWrite(IN2, LOW);
    analogWrite(ENA, motorSpeed);
  }
}
void handleRoot()
{
  String html = R"====(
    <html>
  <style>
    body {
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 0;
    }
    input[type="range"] {
      height: 49px;
      -webkit-appearance: none;
      margin: 10px 0;
      width: 100%;
    }
    input[type="range"]:focus {
      outline: none;
    }
    input[type="range"]::-webkit-slider-runnable-track {
      width: 100%;
      height: 20px;
      cursor: pointer;
      animate: 0.2s;
      box-shadow: 1px 1px 1px #000000;
      background: #3071a9;
      border-radius: 14px;
      border: 2px solid #000000;
    }
    input[type="range"]::-webkit-slider-thumb {
      box-shadow: 1px 1px 1px #000000;
      border: 2px solid #000000;
      height: 40px;
      width: 40px;
      border-radius: 50px;
      background: #ffffff;
      cursor: pointer;
      -webkit-appearance: none;
      margin-top: -12px;
    }
    input[type="range"]:focus::-webkit-slider-runnable-track {
      background: #3071a9;
    }
    input[type="range"]::-moz-range-track {
      width: 100%;
      height: 20px;
      cursor: pointer;
      animate: 0.2s;
      box-shadow: 1px 1px 1px #000000;
      background: #3071a9;
      border-radius: 14px;
      border: 2px solid #000000;
    }
    input[type="range"]::-moz-range-thumb {
      box-shadow: 1px 1px 1px #000000;
      border: 2px solid #000000;
      height: 40px;
      width: 40px;
      border-radius: 50px;
      background: #ffffff;
      cursor: pointer;
    }
    input[type="range"]::-ms-track {
      width: 100%;
      height: 20px;
      cursor: pointer;
      animate: 0.2s;
      background: transparent;
      border-color: transparent;
      color: transparent;
    }
    input[type="range"]::-ms-fill-lower {
      background: #3071a9;
      border: 2px solid #000000;
      border-radius: 28px;
      box-shadow: 1px 1px 1px #000000;
    }
    input[type="range"]::-ms-fill-upper {
      background: #3071a9;
      border: 2px solid #000000;
      border-radius: 28px;
      box-shadow: 1px 1px 1px #000000;
    }
    input[type="range"]::-ms-thumb {
      margin-top: 1px;
      box-shadow: 1px 1px 1px #000000;
      border: 2px solid #000000;
      height: 40px;
      width: 40px;
      border-radius: 50px;
      background: #ffffff;
      cursor: pointer;
    }
    input[type="range"]:focus::-ms-fill-lower {
      background: #3071a9;
    }
    input[type="range"]:focus::-ms-fill-upper {
      background: #3071a9;
    }
  </style>
  <body>
    <h1 style="text-align: center; padding: 2rem">LiPo CAR</h1>

    <div
      style="
        display: flex;
        justify-content: space-between;
        align-items: center;
        padding-left: 10rem;
        padding-right: 10rem;
        padding-top: 5rem;
      "
    >
      <div style="display: flex; flex-direction: column; align-items: center">
        <input
          type="range"
          min="0"
          max="180"
          step="10"
          value="90"
          id="steeringSlider"
          oninput="updateSteeringAngle(this.value)"
          style="width: 300px"
        />
      </div>
      <div style="display: flex; flex-direction: column; align-items: center">
        <input
          type="range"
          min="-51"
          max="51"
          value="0"
          id="speedSlider"
          oninput="updateSpeed(this.value)"
          style="width: 300px; transform: rotate(270deg)"
        />
      </div>
    </div>

    <script>
      function updateSpeed(val) {
        fetch("/setSpeed?value=" + val * 5);
      }
      function updateSteeringAngle(angle) {
        fetch("/setSteeringAngle?angle=" + angle);
        document.getElementById("steeringSlider").value = angle;
      }
    </script>
  </body>
</html>

    )====";
    server.send(200, "text/html", html);
}

void handleSetSpeed()
{
  if (server.hasArg("value"))
  {
    motorSpeed = server.arg("value").toInt();
    analogWrite(ENA, motorSpeed);
    server.send(200, "text/plain", "Speed set to " + String(motorSpeed));
  }
  else
  {
    server.send(400, "text/plain", "Invalid request");
  }
}

void handleSetSteeringAngle()
{
  if (server.hasArg("angle"))
  {
    steeringAngle = server.arg("angle").toInt();
    myServo.write(steeringAngle);
    server.send(200, "text/plain", "Steering angle set to " + String(steeringAngle));
  }
  else
  {
    server.send(400, "text/plain", "Invalid request");
  }
}
