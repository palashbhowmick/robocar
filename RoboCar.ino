
/*****************************************************
 * Date: 23 july 2022
 * Written by: Palash Bhowmick 
 * ***************************************************/
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char *ssid = "Palash-Wifi";
const char *password = "palash-wifi@2019";

// Motor driver L293D pin congifuration
int a0 = 15; // GPIO-15, D8 of nodemcu esp8266
int a1 = 13; // GPIO-13, D7 of nodemcu esp8266
int a2 = 12; // GPIO-12, D6 of nodemcu esp8266
int a3 = 14; // GPIO-14, D5 of nodemcu esp8266

ESP8266WebServer server(80);

void setup()
{
  Serial.begin(115200);
  delay(10);
  // Declaring L293D control pins as Output
  pinMode(a0, OUTPUT);
  pinMode(a1, OUTPUT);
  pinMode(a2, OUTPUT);
  pinMode(a3, OUTPUT);

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
  String value = server.arg("a"); // this lets you access a query param (http://x.x.x.x/?a=fwd)

  if (value == "fwd")
  {
    digitalWrite(a0, HIGH); // Start first motor
    digitalWrite(a1, LOW);

    digitalWrite(a2, HIGH); // Start second motor
    digitalWrite(a3, LOW);
    Serial.println("forward");
  }
  else if (value == "bkw")
  {
    digitalWrite(a0, LOW); // Start first motor
    digitalWrite(a1, HIGH);

    digitalWrite(a2, LOW); // Start second motor
    digitalWrite(a3, HIGH);
    Serial.println("backward");
  }
  else if (value == "right")
  {
    digitalWrite(a0, HIGH); // Start first motor
    digitalWrite(a1, LOW);

    digitalWrite(a2, LOW); // Start second motor
    digitalWrite(a3, HIGH);
    Serial.println("right");
  }
  else if (value == "left")
  {
    digitalWrite(a0, LOW); // Start first motor
    digitalWrite(a1, HIGH);

    digitalWrite(a2, HIGH); // Start second motor
    digitalWrite(a3, LOW);
    Serial.println("left");
  }
  else
  {
    digitalWrite(a0, LOW); // Stop first motor
    digitalWrite(a1, LOW);

    digitalWrite(a2, LOW); // Stop second motor
    digitalWrite(a3, LOW);
    Serial.println("stop");
  }
  server.send(200, "text/html", "<a href=\"/?a=fwd\"><span style=\'font-size:5em;padding-left:1.4em\'>&#9195;</span></a><br>\r\n<br><a href=\"/?a=left\"><span style=\'font-size:5em;\'>&#9194;</span></a>\r\n<a href=\"/?a=stop\"><span style=\'font-size:5em;\'>&#9209;</span></a>\r\n<a href=\"/?a=right\"><span style=\'font-size:5em;\'>&#9193;</span></a>\r\n<br><a href=\"/?a=bkw\"><span style=\'font-size:5em;padding-left:1.4em\'>&#9196;</span></a><style>a:link {text-decoration: none;}</style>");
}

void loop()
{
  server.handleClient();
}
