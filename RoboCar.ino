#include <Servo.h>

// distance sensor pin config
#define DIST_TRG_PIN 2
#define DIST_ECH_PIN 3


// motor pin config
byte motor_left[] = { 8, 9 };
byte motor_right[] = { 10, 11 };

//servo pin config
#define SERVO_PIN 6

const int threshold = 8; // 8 inches
#define MAX_DIST 50

int currState = 0; // 0-stop,1-frd,2-bck,3-lt,4-rt

int leftDist = 0, rightDist = 0;

long lastStateTime = 0;
int avgDist = MAX_DIST;
Servo myservo;
//AltSoftSerial wifiSerial;

int mode = 0;//0=stop,1=wifi rc, 2=auto


void setup() {
	Serial.begin(9600);

	// random seed
	randomSeed(analogRead(0));

	// Setup motors
	for (int i = 0; i < 2; i++) {
		pinMode(motor_left[i], OUTPUT);
		pinMode(motor_right[i], OUTPUT);
	}

	//setup servo;
	myservo.attach(SERVO_PIN);
	look_straight();
	delay(200);
}


void loop() {
	unsigned long t = millis();

	if ((t - lastStateTime) > 250) {
		avgDist = getAvgDistance();
		lastStateTime = t;
		//    Serial.println(avgDist);
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
		motor_stop();
	}

	// get remote command/request (if any)
	if (Serial.available()) {
		String request = Serial.readStringUntil('\r');
		if (request.indexOf("?a=u") != -1) {
			rqstState = 1;
			mode = 1; //wifi rc
		}
		else if (request.indexOf("?a=d") != -1) {
			rqstState = 2;
			mode = 1; //wifi rc
		}
		else if (request.indexOf("?a=l") != -1) {
			mode = 1; //wifi rc
			rqstState = 3;
		}
		else if (request.indexOf("?a=r") != -1) {
			mode = 1; //wifi rc
			rqstState = 4;
		}
		else if (request.indexOf("?a=s") != -1) {
			mode = 0;
			rqstState = 0;
		}
		else if (request.indexOf("?l=l") != -1) { //look left
			look_left();
		}
		else if (request.indexOf("?l=r") != -1) { //look right
			look_right();
		}
		else if (request.indexOf("?l=s") != -1) { //look straight
			look_straight();
		}
		else if (request.indexOf("/dist") != -1) {
			//      Serial.println(getAvgDistance());
		}
		else if (request.indexOf("?mode=auto") != -1) {
			mode = 2; //auto
		}
	}

	switch (mode) {
	case 0:
		motor_stop();
		currState = 0;
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
}


void driveAuto(int avgDist) {
	if (avgDist <= threshold) {
		// obstacle ahead - must react
		motor_stop();
		delay(100);
		down();
		delay(600);
		motor_stop();

		int turn = random(800, 1500);

		Serial.println("looking left");
		int ld = look_left();
		delay(500);

		Serial.println("looking right");
		int rd = look_right();
		delay(500);
		look_straight();

		if (ld >= rd) {
			Serial.println("turning right");
			right();
		}
		else {
			Serial.println("turning left");
			left();
		}
		delay(turn);
	}
	else {
		up();
	}
}

void look_straight() {
	myservo.write(90);
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
	//  Serial.println("STOP");
}

void left() {
	digitalWrite(motor_left[0], HIGH);
	digitalWrite(motor_left[1], LOW);

	digitalWrite(motor_right[0], HIGH);
	digitalWrite(motor_right[1], LOW);
	//  Serial.println("LEFT");
}

void right() {
	digitalWrite(motor_left[0], LOW);
	digitalWrite(motor_left[1], HIGH);

	digitalWrite(motor_right[0], LOW);
	digitalWrite(motor_right[1], HIGH);
	//  Serial.println("RIGHT");
}

void up() {
	digitalWrite(motor_left[0], LOW);
	digitalWrite(motor_left[1], HIGH);

	digitalWrite(motor_right[0], HIGH);
	digitalWrite(motor_right[1], LOW);
	//  Serial.println("UP");
}

void down() {
	digitalWrite(motor_left[0], HIGH);
	digitalWrite(motor_left[1], LOW);

	digitalWrite(motor_right[0], LOW);
	digitalWrite(motor_right[1], HIGH);
	//  Serial.println("DOWN");
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


