#include <ESP8266WiFi.h>

const char* ssid = "*****";
const char* password = "******";

void jsonResponse(WiFiClient &client, String json) {
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: application/json");
	client.println(""); //  do not forget this one
	client.println(json);
	client.stop();
}

void htmlResponse(WiFiClient &client){
	client.println("HTTP/1.1 200 OK");
	client.println("Content-Type: text/html");
	client.println(""); //  do not forget this one
	client.println("<!DOCTYPE html><html><head lang=\"en\"> <meta charset=\"UTF-8\"> <title>Robo Car Controller</title> <script src=\"https://ajax.googleapis.com/ajax/libs/jquery/1.12.0/jquery.min.js\"></script> <script>var keydowned=false; $(document).ready(function (){$(\".key\").mousedown(function (){console.log(\"mouse down\"); restCall(\"\"+$(this).attr(\"data-key\"));}); $(\".key\").mouseup(function (){console.log(\"mouse up\"); restCall(999);}); $(document).keydown(function (event){if (!keydowned){keydowned=true; console.log(\"keydown=\" + event.which); restCall(\"\"+event.which);}}); $(document).keyup(function (){console.log(\"keyup\"); keydowned=false; restCall(999);});}); function restCall(action){switch (action){case \"37\": case 37: a=\"l\"; break; case \"38\": case 38: a=\"u\"; break; case \"39\": case 39: a=\"r\"; break; case \"40\": case 40: a=\"d\"; break; case \"999\": case 999: a=\"s\"; break; default : return;}var uri=\"http://192.168.1.50/api/?a=\" + a; $.get(uri, function (data){console.log(data);});}</script> <style>.btn{font-family: Arial, serif; color: #ffffff; font-size: 20px; background: #3498db; padding: 10px 20px 10px 20px; text-decoration: none;}.btn:hover{background: #3cb0fd; text-decoration: none;}.btn-wrp{background-color: #eef6ff; border: solid 1px #373c52; height: 400px; width: 400px; position: relative;}.key{position: absolute; height: 48px; width: 108px;}.key.up{top: 0px; left: 150px;}.key.down{bottom: 0px; left: 150px;}.key.left{top: 170px; left: 0px;}.key.right{top: 170px; right: 0px;}</style></head><body><div class=\"btn-wrp\"> <button class=\"btn key up\" data-key=\"38\">UP</button> <button class=\"btn key left\" data-key=\"37\">LEFT</button> <button class=\"btn key right\" data-key=\"39\">RIGHT</button> <button class=\"btn key down\" data-key=\"40\">DOWN</button></div></body></html>");
	client.stop();
}

WiFiServer server(80);

void setup() {
	Serial.begin(9600);
	delay(10);

	// Connect to WiFi network
	Serial.println();
	Serial.println();
	Serial.print("Connecting to ");
	Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}
	Serial.println("");
	Serial.println("WiFi connected");

	// Start the server
	server.begin();
	Serial.println("Server started");

	// Print the IP address
	Serial.print("Use this URL to connect: ");
	Serial.print("http://");
	Serial.print(WiFi.localIP());
	Serial.println("/");

}

void loop() {
	// Check if a client has connected
	WiFiClient client = server.available();
	if (!client) {
		return;
	}

	// Wait until the client sends some data
	Serial.println("new client");
	while (!client.available()) {
		delay(1);
	}

	// Read the first line of the request
	String request = client.readStringUntil('\r');
	Serial.println(request);
	client.flush();

	// Set ledPin according to the request
	//digitalWrite(ledPin, value);

	if (request.indexOf("api") != -1) {
		jsonResponse(client, "{\"status\":\"succsess\"}");
	}
	else{
		htmlResponse(client);
	}

	delay(1);
	Serial.println("Client disonnected");
	Serial.println("");

}




