#include <IRremote.h>

int RECV_PIN = 3;

const String left = "3cc3728d";
const String right = "3cc3b24d";
const String up = "3cc3926d";
const String down = "3cc352ad";
const String stp = "3cc342bd";
const String cont = "ffffffff";

IRrecv irrecv(RECV_PIN);

decode_results results;

// --------------------------------------------------------------------------- Motors
int motor_left[] = { 8, 9 };
int motor_right[] = { 10, 11 };

String prev;
// --------------------------------------------------------------------------- Setup
void setup() {
	Serial.begin(9600);
	irrecv.enableIRIn(); // Start the receiver
	// Setup motors
	int i;
	for (i = 0; i < 2; i++) {
		pinMode(motor_left[i], OUTPUT);
		pinMode(motor_right[i], OUTPUT);
	}
	motor_stop();
}

// --------------------------------------------------------------------------- Loop
void loop() {
	if (irrecv.decode(&results)) {
		String c = String(results.value, HEX);
		Serial.print(">");
		Serial.print(c);
		Serial.println("<");
		if (c.equals(cont)) {
			c = prev;
		}
		if (c.equals(stp)) {
			Serial.println("stop");
			motor_stop();
			prev = stp;
		}
		else if (c.equals(left)) {
			Serial.println("left");
			drive_forward();
			prev = left;
		}
		else if (c.equals(right)) {
			Serial.println("right");
			drive_backward();
			prev = right;
		}
		else if (c.equals(up)) {
			Serial.println("up");
			turn_left();
			prev = up;
		}
		else if (c.equals(down)) {
			Serial.println("down");
			turn_right();
			prev = down;
		}
		irrecv.resume(); // Receive the next value
	}
	delay(100);
	motor_stop();
}

// --------------------------------------------------------------------------- Drive

void motor_stop() {
	digitalWrite(motor_left[0], LOW);
	digitalWrite(motor_left[1], LOW);

	digitalWrite(motor_right[0], LOW);
	digitalWrite(motor_right[1], LOW);
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

