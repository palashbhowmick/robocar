// --------------------------------------------------------------------------- Motors
int motor_left[] = { 8, 9 };
int motor_right[] = { 10, 11 };

// --------------------------------------------------------------------------- Setup
void setup() {
	Serial.begin(9600);
	// Setup motors
	int i;
	for (i = 0; i < 2; i++) {
		pinMode(motor_left[i], OUTPUT);
		pinMode(motor_right[i], OUTPUT);
	}
	motor_stop();
	while (!Serial.available());
	Serial.println("--- PALASH's ROBO CAR");
	Serial.println("1) FORWARD");
	Serial.println("2) BACKWARD");
	Serial.println("3) TURN LEFT");
	Serial.println("4) TURN WRITE");
	Serial.println("0) STOP");
}

const int trigPin = 2;
const int echoPin = 4;
// --------------------------------------------------------------------------- Loop
void loop() {
	int inches = getDistanceInInches();
	if (inches > 8) {
		drive_forward();
		delay(200);
	}
	else {
		motor_stop();
		delay(100);
		drive_backward();
		delay(250);
		turn_left();
		delay(2000);
		inches = getDistanceInInches();
		if (inches <= 8) {
			turn_right();
			delay(4000);
			inches = getDistanceInInches();
			if (inches <= 8) {
				turn_left();
				delay(2000);
			}
		}
	}
	delay(100);
}

int getDistanceInInches() {
	// establish variables for duration of the ping,
	// and the distance result in inches and centimeters:
	long duration, inches, cm;

	// The sensor is triggered by a HIGH pulse of 10 or more microseconds.
	// Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
	pinMode(trigPin, OUTPUT);
	digitalWrite(trigPin, LOW);
	delayMicroseconds(2);
	digitalWrite(trigPin, HIGH);
	delayMicroseconds(10);
	digitalWrite(trigPin, LOW);

	// Read the signal from the sensor: a HIGH pulse whose
	// duration is the time (in microseconds) from the sending
	// of the ping to the reception of its echo off of an object.
	pinMode(echoPin, INPUT);
	duration = pulseIn(echoPin, HIGH);

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

void drive_forward() {
	digitalWrite(motor_left[0], HIGH);
	digitalWrite(motor_left[1], LOW);

	digitalWrite(motor_right[0], HIGH);
	digitalWrite(motor_right[1], LOW);
}

void drive_backward() {
	digitalWrite(motor_left[0], LOW);
	digitalWrite(motor_left[1], HIGH);

	digitalWrite(motor_right[0], LOW);
	digitalWrite(motor_right[1], HIGH);
}

void turn_left() {
	digitalWrite(motor_left[0], LOW);
	digitalWrite(motor_left[1], HIGH);

	digitalWrite(motor_right[0], HIGH);
	digitalWrite(motor_right[1], LOW);
}

void turn_right() {
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

