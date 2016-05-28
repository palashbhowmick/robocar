int motor_left[] = { 8, 9 };
int motor_right[] = { 10, 11 };

const int trigPin = 2;
const int echoPin = 3;

const int threshold = 8; // 8 inches

byte prevStep = 0; // 0-stop,1-frd,2-bck,3-lt,4-rt


void setup() {
	Serial.begin(9600);
	// Setup motors
	randomSeed(analogRead(0));
	int i;
	for (i = 0; i < 2; i++) {
		pinMode(motor_left[i], OUTPUT);
		pinMode(motor_right[i], OUTPUT);
	}
}

void loop() {
	if (getAvgDistance() <= threshold) {
		// obstacle ahead - must react
		motor_stop();
		delay(100);
		down();
		delay(600);
		int rnd = random(2);
		int turn = random(800, 1500);
		if (rnd == 0) {
			Serial.print("turning right, ");
			Serial.println(turn);
			right();
		}
		else {
			Serial.print("turning left, ");
			Serial.println(turn);
			left();
		}

		delay(turn);
	}
	else {
		up();
	}
	delay(10);
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

