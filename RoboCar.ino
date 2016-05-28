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

byte prevStep = 0; // 0-stop,1-frd,2-bck,3-lt,4-rt

int leftDist = 0, rightDist = 0;
Servo myservo;
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
	int avgDist = getAvgDistance();
	if (avgDist == 0) {
		motor_stop();
		delay(500);
	}
	else if (avgDist <= threshold) {
		// obstacle ahead - must react
		motor_stop();
		delay(100);
		down();
		delay(600);
		motor_stop();

		int turn = random(800, 1500);

		Serial.println("looking left");
		int ld = look_left();
		delay(1000);

		Serial.println("looking right");
		int rd = look_right();
		delay(1000);
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
	delay(10);
}

void look_straight() {
	myservo.write(90);
	delay(250);
}

int look_right() {
	myservo.write(160);
	delay(250);
	return getAvgDistance();
}

int look_left() {
	myservo.write(20);
	delay(250);
	return getAvgDistance();
}

int getAvgDistance() {
	int sum = 0;
	for (int i = 0; i < 3; i++) {
		int inches = getDistanceInInches();
		Serial.print(inches);
		Serial.print(",");
		sum += inches;
	}
	int avg = sum / 3;
	Serial.print("avg = ");
	Serial.println(avg);
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
	Serial.println(inches);
	return inches;
}

// --------------------------------------------------------------------------- Drive

void motor_stop() {
	digitalWrite(motor_left[0], LOW);
	digitalWrite(motor_left[1], LOW);

	digitalWrite(motor_right[0], LOW);
	digitalWrite(motor_right[1], LOW);
	delay(25);
}

void left() {
	digitalWrite(motor_left[0], HIGH);
	digitalWrite(motor_left[1], LOW);

	digitalWrite(motor_right[0], HIGH);
	digitalWrite(motor_right[1], LOW);
}

void right() {
	digitalWrite(motor_left[0], LOW);
	digitalWrite(motor_left[1], HIGH);

	digitalWrite(motor_right[0], LOW);
	digitalWrite(motor_right[1], HIGH);
}

void up() {
	digitalWrite(motor_left[0], LOW);
	digitalWrite(motor_left[1], HIGH);

	digitalWrite(motor_right[0], HIGH);
	digitalWrite(motor_right[1], LOW);
}

void down() {
	digitalWrite(motor_left[0], HIGH);
	digitalWrite(motor_left[1], LOW);

	digitalWrite(motor_right[0], LOW);
	digitalWrite(motor_right[1], HIGH);
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


