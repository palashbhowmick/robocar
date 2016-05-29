#include "RequestParser.h"
#include <Servo.h>

#define DEBUG true

//PIN configurations

// distance sensor pin config
#define DIST_TRG_PIN 2
#define DIST_ECH_PIN 3

#define WIFI_SWITCH_PIN 12

#define LED_LEFT_PIN 4
#define LED_RIGHT_PIN 5

// motor pin config
byte motor_left[] = { 8, 9 };
byte motor_right[] = { 10, 11 };

//servo pin config
#define SERVO_PIN 6

//other settings
const int threshold = 8; // 8 inches
#define MAX_DIST 50


//global variables

int currState = 0; // 0-stop,1-frd,2-bck,3-lt,4-rt

int leftDist = 0, rightDist = 0;

long lastStateTime = 0;
int avgDist = MAX_DIST;

int mode = 0;//0=stop,1=wifi rc, 2=auto

int wifiSwitchPrevState=-1;
Servo myservo;
RequestParser p;

void setup() {
	Serial.begin(9600);

	if (DEBUG){
		Serial.println("Starting RoboCar..");
	}
	// random seed
	randomSeed(analogRead(0));

	// Setup motors
	for (int i = 0; i < 2; i++) {
		pinMode(motor_left[i], OUTPUT);
		pinMode(motor_right[i], OUTPUT);
	}

	pinMode(LED_LEFT_PIN, OUTPUT);
	pinMode(LED_RIGHT_PIN, OUTPUT);

	pinMode(WIFI_SWITCH_PIN, INPUT_PULLUP);

	//setup servo;
	myservo.attach(SERVO_PIN);
	
	look_straight();

	flashLED();
	if (DEBUG){
		Serial.println("Ready!");
	}
}


void loop() {	
	
	unsigned long t = millis();	

	if ((t - lastStateTime) > 250 && (mode==2 || (currState!=0 && currState!=2))) {
		avgDist = getAvgDistance();
		lastStateTime = t;
		if (DEBUG) {
			Serial.print("avg distance:");
			Serial.println(avgDist);
		}
	}

	int wifiSwitch = digitalRead(WIFI_SWITCH_PIN);
	
	if (wifiSwitchPrevState == -1 || wifiSwitch != wifiSwitchPrevState){
		//wifi switch toggled
		wifiSwitchPrevState = wifiSwitch;
		if (wifiSwitch == LOW) { //auto via switch		
			mode = 2;//auto
			if (DEBUG){
				Serial.println("AUTO Mode enabled");
			}
			driveAuto(avgDist);
			return;
		}
		else{
			if (DEBUG){
				Serial.println("AUTO Mode disabled");
			}
			mode = 0;//stop
		}
	}

	


	int rqstState = -1;
	boolean obstacle = false;
	if (avgDist == 0) {
		avgDist = MAX_DIST;
	}

	if (avgDist <= threshold) { // obstacle ahead
		obstacle = true;
	}


	if (obstacle && currState == 1) {
		flashLED();
		if (currState != 0){
			currState = 0;
			motor_stop();
		}		
	} 

	// get remote command/request (if any)
	if (Serial.available()) {
		
		String request = Serial.readStringUntil('\r');		
		if (DEBUG){
			Serial.print("raw request=");
			Serial.println(request);
		}
		
		boolean t = p.parse(request);		    
		if (t)
		{						
			String key = p.getKey();
			String val = p.getValue();
			if (DEBUG){
				Serial.print("key=");
				Serial.println(key);
				Serial.print("value=");
				Serial.println(val);
			}
			if (key == "a"){
				mode = 1; //wifi rc
				if (val=="u") {
					rqstState = 1;					
				}
				else if (val == "d") {
					rqstState = 2;					
				}
				else if (val == "l") {					
					rqstState = 3;
				}
				else if (val == "r") {					
					rqstState = 4;
				}
				else if (val == "s") {
					mode = 0;
					rqstState = 0;
				}
			}
			else if (key=="l"){
				if (val == "l") { //look left
					look_left();
				}
				else if (val == "r") { //look right
					look_right();
				}
				else if (val == "s") { //look straight
					look_straight();
				}
			}
			else if (key == "look"){				
				int deg = convertToInt(val);
				if (DEBUG) {
					Serial.println(deg);
				}
				look(deg);
			}
			else if (key == "lt"){				
				int milli = convertToInt(val);
				if (DEBUG) {
					Serial.println(milli);
				}
				left();
				delay(milli);
				motor_stop();
			}
			else if (key == "rt"){				
				int milli = convertToInt(val);
				if (DEBUG) {
					Serial.println(milli);
				}
				right();
				delay(milli);
				motor_stop();
			}
			else if (key == "led" && val.length() == 2){				
				uint8_t bit0 = val[0] == '0' ? LOW : HIGH;
				uint8_t bit1 = val[1] == '0' ? LOW : HIGH;					
				digitalWrite(LED_LEFT_PIN, bit0);
				digitalWrite(LED_RIGHT_PIN, bit1);																					
			}
			else if (key == "mode" && val=="auto"){
				mode = 2;
			}
		}
		else{
			if (DEBUG){
				Serial.println("invalid request - parse error");
			}			
		}
	}
	
	switch (mode) {
	case 0:
		if (currState != 0){
			motor_stop();
			currState = 0;
		}		
		break;
	case 1:
		if (rqstState >= 0 && currState != rqstState) {
			switch (rqstState) {
			case 0:
				motor_stop();
				currState = 0;
				break;
			case 1:
				if (!obstacle) {
					up();
					currState = 1;
				}
				break;
			case 2:
				down();
				currState = 2;
				break;
			case 3:
				left();
				currState = 3;
				break;
			case 4:
				right();
				currState = 4;
				break;
			}
		}
		break;
	case 2:
		driveAuto(avgDist);
		break;
	}	
	delay(5);
}


int convertToInt(String str) {
	String inString;
	for (int i = 0; i <= str.length(); i++) {
		if (isDigit(str[i])) {
			inString += (char)str[i];
		}
		else {
			continue;
		}
	}
	return inString.toInt();
}

void driveAuto(int avgDist) {
	if (avgDist <= threshold) {
		// obstacle ahead - must react
		flashLED();
		currState = 0;
		motor_stop();
		delay(100);		
		down();
		delay(400);
		motor_stop();

		if (DEBUG) {
			Serial.println("looking left");
		}
		int ld = look_left();
		delay(500);

		if (DEBUG) {
			Serial.println("looking right");
		}
		int rd = look_right();
		delay(500);
		look_straight();

		if (ld >= rd) {
			if (DEBUG) {
				Serial.println("turning left");
			}
			left();
		}
		else {
			if (DEBUG) {
				Serial.println("turning right");
			}
			right();
		}
		delay(800);
	}
	else {
		if (currState != 1){
			currState = 1;
			up();
		}		
	}
}
void flashLED(){
	for (int i = 0; i <= 3; i++){
		digitalWrite(LED_LEFT_PIN, HIGH);
		digitalWrite(LED_RIGHT_PIN, HIGH);
		delay(50);
		digitalWrite(LED_LEFT_PIN, LOW);
		digitalWrite(LED_RIGHT_PIN, LOW);
		delay(50);
	}	
}
void look_straight() {
	myservo.write(90);
	delay(250);
}

void look(int degree) {
	myservo.write(degree);
	delay(250);
}

int look_left() {
	myservo.write(160);
	delay(250);
	return getAvgDistance();
}

int look_right() {
	myservo.write(20);
	delay(250);
	return getAvgDistance();
}

int getAvgDistance() {
	int sum = 0;
	for (int i = 0; i < 3; i++) {
		int inches = getDistanceInInches();
		//    Serial.print(inches);
		//    Serial.print(",");
		sum += inches;
	}
	int avg = sum / 3;
	//  Serial.print("avg = ");
	//  Serial.println(avg);
	return avg;
}

int getDistanceInInches() {
	// establish variables for duration of the ping,
	// and the distance result in inches and centimeters:
	long duration, inches, cm;

	// The sensor is triggered by a HIGH pulse of 10 or more microseconds.
	// Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
	pinMode(DIST_TRG_PIN, OUTPUT);
	digitalWrite(DIST_TRG_PIN, LOW);
	delayMicroseconds(2);
	digitalWrite(DIST_TRG_PIN, HIGH);
	delayMicroseconds(10);
	digitalWrite(DIST_TRG_PIN, LOW);

	// Read the signal from the sensor: a HIGH pulse whose
	// duration is the time (in microseconds) from the sending
	// of the ping to the reception of its echo off of an object.
	pinMode(DIST_ECH_PIN, INPUT);
	duration = pulseIn(DIST_ECH_PIN, HIGH);

	// convert the time into a distance
	inches = microsecondsToInches(duration);
	//  Serial.println(inches);
	return inches;
}

// --------------------------------------------------------------------------- Drive

void motor_stop() {
	digitalWrite(motor_left[0], LOW);
	digitalWrite(motor_left[1], LOW);

	digitalWrite(motor_right[0], LOW);
	digitalWrite(motor_right[1], LOW);
	if (DEBUG) {
		Serial.println("STOP");
	}
}

void left() {
	digitalWrite(motor_left[0], HIGH);
	digitalWrite(motor_left[1], LOW);

	digitalWrite(motor_right[0], HIGH);
	digitalWrite(motor_right[1], LOW);
	if (DEBUG) {
		Serial.println("LEFT");
	}
}

void right() {
	digitalWrite(motor_left[0], LOW);
	digitalWrite(motor_left[1], HIGH);

	digitalWrite(motor_right[0], LOW);
	digitalWrite(motor_right[1], HIGH);
	if (DEBUG) {
		Serial.println("RIGHT");
	}
}

void up() {
	digitalWrite(motor_left[0], LOW);
	digitalWrite(motor_left[1], HIGH);

	digitalWrite(motor_right[0], HIGH);
	digitalWrite(motor_right[1], LOW);
	if (DEBUG) {
		Serial.println("UP");
	}
}

void down() {
	digitalWrite(motor_left[0], HIGH);
	digitalWrite(motor_left[1], LOW);

	digitalWrite(motor_right[0], LOW);
	digitalWrite(motor_right[1], HIGH);
	if (DEBUG) {
		Serial.println("DOWN");
	}
}
long microsecondsToInches(long microseconds)
{
	// According to Parallax's datasheet for the PING))), there are
	// 73.746 microseconds per inch (i.e. sound travels at 1130 feet per
	// second).  This gives the distance travelled by the ping, outbound
	// and return, so we divide by 2 to get the distance of the obstacle.
	// See: http://www.parallax.com/dl/docs/prod/acc/28015-PING-v1.3.pdf
	return microseconds / 74 / 2;
}


