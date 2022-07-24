
/*****************************************************
 * Board: NodeMCU 1.0 (ESP-12E Module)
 *
 * Date: 23 July 2022
 * Written by: Palash Bhowmick
 * ***************************************************/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

#include "html.h"
#include "secrets.h"
#include "MotorController.h"

#define IR_SENSOR_PIN 4 // GPIO-4, D2 of nodemcu esp8266

const char *ssid = SECRET_SSID;
const char *password = SECRET_PASS;

ESP8266WebServer server(80);

MotorController motor(15, // GPIO-15, D8 of nodemcu esp8266
                      13, // GPIO-13, D7 of nodemcu esp8266
                      12, // GPIO-12, D6 of nodemcu esp8266
                      14  // GPIO-14, D5 of nodemcu esp8266
);

boolean forwarding = false;

void setup()
{
  Serial.begin(115200);
  delay(10);
  // Declaring L293D control pins as Output
  motor.init();

  // IR Sensor pin
  pinMode(IR_SENSOR_PIN, INPUT);

  // Connect to WiFi network
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.on("/", handleRoot);
  server.on("/action/", handleAction);
  // Start the server
  server.begin();
  Serial.println("Server started");

  // Print the IP address on serial monitor
  Serial.print("Use this URL to connect: ");
  Serial.print("http://"); // URL IP to be typed in mobile/desktop browser
  Serial.print(WiFi.localIP());
  Serial.println("/");
}

void handleRoot()
{
  server.send(200, "text/html", html);
}

void handleAction()
{
  String value = server.arg("a"); // this lets you access a query param (http://x.x.x.x/action?a=fwd)

  if (value == "fwd")
  {
    if (obstacle())
    {
      motor.stop();
      forwarding = false;
      sendJSON(value, "obstacle ahead");
    }
    else
    {
      motor.forward();
      forwarding = true;
      sendJSON(value, "moving forward");
    }
  }
  else if (value == "bkw")
  {
    motor.backward();
    forwarding = false;
    sendJSON(value, "moving backward");
  }
  else if (value == "right")
  {
    motor.right();
    forwarding = false;
    sendJSON(value, "turning right");
  }
  else if (value == "left")
  {
    motor.left();
    forwarding = false;
    sendJSON(value, "turning left");
  }
  else
  {
    motor.stop();
    forwarding = false;
    sendJSON(value, "stopped");
  }
}

void sendJSON(String action, String message)
{
  Serial.println(message);
  server.send(200, "application/json", "{\"action\":\"" + action + "\",\"message\":\"" + message + "\"}");
}

boolean obstacle()
{
  return (digitalRead(IR_SENSOR_PIN) == 1);
}

void loop()
{
  server.handleClient();

  if (obstacle() && forwarding)
  {
    motor.stop();
  }
}
