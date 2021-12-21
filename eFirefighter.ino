#include <Servo.h>

#define trigPin 28
#define echoPin 29

#define PINA 8
#define PINB 9
#define INTERRUPT 0  // that is, pin 2

#define DIRECTIONA 4
#define MOTORA 5
#define DIRECTIONB 7
#define MOTORB 6

#define TIME_FORWARDS 10000
#define TIME_BACKWARDS 10000
#define TIME_TURN 1200

Servo servoLeft;          // Define left servo
Servo servoRight;         // Define right servo
int pos = 90;

volatile int rotaryCount = 0;

int relej = 8;
int buttonpin = 12; // define the flame sensor interface
int analoog = A5; // define the flame sensor interface

int val; // define numeric variables val
float sensor; //read analoog value

byte byteRead;

// Interrupt Service Routine for a change to encoder pin A
void isr () {
  boolean up;

  if (digitalRead (PINA))
    up = digitalRead (PINB);
  else
    up = !digitalRead (PINB);

  if (up)
    rotaryCount++;
  else
    rotaryCount--;
}  // end of isr


void setup () {
  servoLeft.attach(10);  // Set left servo to digital pin 10
  servoRight.attach(9);
  Serial.begin(9600);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  attachInterrupt (INTERRUPT, isr, CHANGE);   // interrupt 0 is pin 2, interrupt 1 is pin 3

  //Rover 5
  pinMode (MOTORA, OUTPUT);
  pinMode (DIRECTIONA, OUTPUT);
  pinMode (MOTORB, OUTPUT);
  pinMode (DIRECTIONB, OUTPUT);

  //relej
  pinMode(relej, OUTPUT);
  pinMode (buttonpin, INPUT) ;
  pinMode (analoog, INPUT) ;
  Serial.begin (9600);

}

byte phase;
unsigned long start;
int time_to_go;

void loop () {

  long duration, distance;
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance = (duration / 2) / 29.1;

  if (distance <= 40 || distance <= 0) {
    digitalWrite(relej, HIGH);
    delay(2000);
  } else {
    digitalWrite(relej, LOW);
  }
  delay(500);

  if (Serial.available() > 0) {
    int inByte = Serial.read();
    switch (inByte) {
        analogWrite (MOTORA, 50);
        digitalWrite (DIRECTIONA, 0);
        start = millis ();
        // check current drain
        while (millis () - start < time_to_go) {
          if (analogRead (0) > 325)  // > 1.46 amps
            break;
        }
      case '0':// naprijed desno
        analogWrite (MOTORA, 255);
        digitalWrite (DIRECTIONA, 0);
        break;

      case '1':// naprijed lijevo
        analogWrite (MOTORB, 255);
        digitalWrite (DIRECTIONB, 0);
        break;

      case '2':// nazad desno
        analogWrite (MOTORA, 255);
        digitalWrite (DIRECTIONA, 1);
        break;

      case '3':// nazad lijevo
        analogWrite (MOTORB, 255);
        digitalWrite (DIRECTIONB, 1);
        break;

      case '5':// stop sve
        analogWrite (MOTORA, 0);
        analogWrite (MOTORB, 0);
        break;

      case '6': // relay satrt
        digitalWrite(relej, HIGH);   // turn the relay on (HIGH is the voltage level)
        break;

      case '7': // relay stop
        digitalWrite(relej, LOW);   // turn the relay on (LOW is the voltage level)
        break;

      case 'a': // 1 - left
        servoLeft.write(pos += 20);
        break;

      case 'b': // 2 - right
        servoLeft.write(pos -= 20);
        break;

      case 'c': // 3 - up
        servoRight.write(pos -= 20);
        break;

      case 'd': // 4 - down
        servoRight.write(pos += 20);
        break;

      case 'e': // 5 - reset
        servoLeft.write(90);
        servoRight.write(90);
    }
  }
}
