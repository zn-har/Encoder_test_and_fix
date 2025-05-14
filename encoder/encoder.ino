// Motor control pins
const int motorPWM[4] = {2, 4, 6, 8};
const int motorDir[4] = {3, 5, 7, 9};

// Encoder pins [A,B]
const int encoderA[4] = {22, 24, 26, 28};
const int encoderB[4] = {23, 25, 27, 29};

volatile int encoderCount[4] = {0, 0, 0, 0};

// ISR for each encoder channel A
void encoder0ISR() { readEncoder(0); }
void encoder1ISR() { readEncoder(1); }
void encoder2ISR() { readEncoder(2); }
void encoder3ISR() { readEncoder(3); }

void setup() {
  Serial.begin(115200);

  // Setup motors
  for (int i = 0; i < 4; i++) {
    pinMode(motorPWM[i], OUTPUT);
    pinMode(motorDir[i], OUTPUT);
  }

  // Setup encoders
  for (int i = 0; i < 4; i++) {
    pinMode(encoderA[i], INPUT);
    pinMode(encoderB[i], INPUT);
  }

  attachInterrupt(digitalPinToInterrupt(encoderA[0]), encoder0ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderA[1]), encoder1ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderA[2]), encoder2ISR, CHANGE);
  attachInterrupt(digitalPinToInterrupt(encoderA[3]), encoder3ISR, CHANGE);

  delay(2000);
  Serial.println("Starting Steering Encoder Test...");
}

void loop() {
  for (int i = 0; i < 4; i++) {
    Serial.print("\nTesting Encoder ");
    Serial.println(i + 1);
    encoderCount[i] = 0;

    // Rotate right
    moveSteering(i, true, 90);
    Serial.println("Rotated Right to 90");

    // Back to center
    moveSteering(i, false, 90);
    Serial.print("Returned to center: ");
    Serial.println(encoderCount[i]);

    // Check zero
    if (abs(encoderCount[i]) > 3) {
      Serial.println("❌ Error: Encoder didn't return to zero.");
    } else {
      Serial.println("✅ Encoder OK after right test.");
    }

    // Rotate left
    moveSteering(i, false, 90);
    Serial.println("Rotated Left to -90");

    // Back to center
    moveSteering(i, true, 90);
    Serial.print("Returned to center: ");
    Serial.println(encoderCount[i]);

    if (abs(encoderCount[i]) > 3) {
      Serial.println(" Error: Encoder didn't return to zero.");
    } else {
      Serial.println(" Encoder OK after left test.");
    }

    delay(3000); // Pause between each encoder test
  }

  Serial.println("\n All Encoders Tested. Stopping.");
  while (1); // stop loop
}

// Function to move the wheel by encoder counts
void moveSteering(int index, bool dir, int target) {
  digitalWrite(motorDir[index], dir);
  analogWrite(motorPWM[index], 100); // Adjust speed if needed

  int startCount = encoderCount[index];
  int goal = dir ? (startCount + target) : (startCount - target);

  if (dir) {
    while (encoderCount[index] < goal);
  } else {
    while (encoderCount[index] > goal);
  }

  analogWrite(motorPWM[index], 0); // stop motor
}

// Encoder reading logic
void readEncoder(int index) {
  bool A = digitalRead(encoderA[index]);
  bool B = digitalRead(encoderB[index]);

  if (A == B) encoderCount[index]++;
  else encoderCount[index]--;
}
