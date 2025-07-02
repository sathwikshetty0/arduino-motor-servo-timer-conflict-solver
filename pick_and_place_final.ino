#include <AFMotor.h>
#include <Servo.h>

// Motor Objects
AF_DCMotor MotorFR(1);  // Front right motor
AF_DCMotor MotorFL(3);  // Front left motor
AF_DCMotor MotorBL(4);  // Back left motor
AF_DCMotor MotorBR(2);  // Back right motor

// Servo Object
Servo myServo;          // Servo on HW-130 Shield (Servo 1 = D10)

// Pins and Variables
const int ledPin = 9;  // LED indicator pin
int valSpeed = 180;     // Default motor speed
String inputString = "";

// Mode control variable
// true = Motor Mode is active
// false = Servo Mode is active
bool isMotorModeActive = true; 

void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT);

  // --- Initial Setup ---
  // Start in MOTOR MODE by default for safety.
  isMotorModeActive = true;
  digitalWrite(ledPin, HIGH); // Turn LED ON to indicate Motor Mode
  myServo.detach();           // Ensure servo is detached at the start

  // Set initial motor speed
  SetSpeed(valSpeed);
  
  // Stop all motors at the beginning
  stopAllMotors();
}

void loop() {
  while (Serial.available() > 0) {
    char inChar = Serial.read();

    if (inChar == '\n') {
      processCommand(inputString);
      inputString = "";
    } else {
      inputString += inChar;
    }
  }
}

void processCommand(String command) {
  if (command.length() == 0) return;

  char cmd = command.charAt(0);

  // The new 'X' command to toggle modes
  if (cmd == 'X') {
    isMotorModeActive = !isMotorModeActive; // Flip the mode

    if (isMotorModeActive) {
      // --- SWITCHING TO MOTOR MODE ---
      myServo.detach();            // Release the timer for the motors
      digitalWrite(ledPin, HIGH);  // LED ON for Motor Mode
      Serial.println("Motor Mode Activated. Servo detached.");
    } else {
      // --- SWITCHING TO SERVO MODE ---
      stopAllMotors();             // Stop motors before switching
      myServo.attach(10);          // Take control of the timer for the servo
      myServo.write(0);           // Center the servo
      digitalWrite(ledPin, LOW);   // LED OFF for Servo Mode
       Serial.println("Servo Mode Activated. Motors disabled.");
    }
    return; // Exit after processing the mode switch
  }

  // Only run motor or servo commands if in the correct mode
  if (isMotorModeActive) {
    // --- MOTOR COMMANDS ---
    switch (cmd) {
      case 'F': MotorFL.run(FORWARD); MotorFR.run(FORWARD); MotorBL.run(FORWARD); MotorBR.run(FORWARD); break;
      case 'B': MotorFL.run(BACKWARD); MotorFR.run(BACKWARD); MotorBL.run(BACKWARD); MotorBR.run(BACKWARD); break;
      case 'R': MotorFL.run(BACKWARD); MotorFR.run(FORWARD); MotorBL.run(BACKWARD); MotorBR.run(FORWARD); break;
      case 'L': MotorFL.run(FORWARD); MotorFR.run(BACKWARD); MotorBL.run(FORWARD); MotorBR.run(BACKWARD); break;
      case 'S': stopAllMotors(); break;
      case '1': SetSpeed(65); break;
      case '2': SetSpeed(130); break;
      case '3': SetSpeed(195); break;
      case '4': SetSpeed(255); break;
      // You can add G, H, I, J back here if needed
      default: Serial.println("Unknown command or command not available in Motor Mode."); break;
    }
  } else {
    // --- SERVO COMMANDS ---
    switch (cmd) {
      case 'Z':
        if (command.length() > 1) {
          int angle = command.substring(1).toInt();
          angle = constrain(angle, 0, 180);
          myServo.write(angle);
          Serial.print("Servo Angle Set to: ");
          Serial.println(angle);
        }
        break;
      default: Serial.println("Command not available in Servo Mode. Press 'X' to switch to Motor Mode."); break;
    }
  }
}

void SetSpeed(int val) {
  valSpeed = val;
  MotorFL.setSpeed(val);
  MotorFR.setSpeed(val);
  MotorBL.setSpeed(val);
  MotorBR.setSpeed(val);
}

void stopAllMotors() {
  MotorFL.run(RELEASE);
  MotorFR.run(RELEASE);
  MotorBL.run(RELEASE);
  MotorBR.run(RELEASE);
}