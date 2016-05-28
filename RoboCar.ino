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

// --------------------------------------------------------------------------- Loop
void loop() {

	if (Serial.available()) {
		Serial.print(">");
		char c = Serial.read();
		if (c == '0') {
			Serial.println("stop");
			motor_stop();
		}
		else if (c == '3') {
			Serial.println("go");
			drive_forward();
		}
		else if (c == '4') {
			Serial.println("back");
			drive_backward();
		}
		else if (c == '1') {
			Serial.println("left");
			turn_left();
		}
		else if (c == '2') {
			Serial.println("right");
			turn_right();
		}
	}
	delay(10);
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
