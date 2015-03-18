const byte btnReset = 0;
const byte btnDisplay = 5;

boolean reading = false;
boolean displaying = false;
boolean ledOn = false;
unsigned int maxAltitude = 0;
unsigned long millisCounter = millis();
unsigned int blinkDelay = 250;

unsigned int displayPositionDelay = 750;
unsigned int displayDigitOnDelay = 300;
unsigned int displayDigitOffDelay = 200;


void setup() {
  pinMode(btnReset, INPUT_PULLUP);
  pinMode(btnDisplay, INPUT_PULLUP);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);
  
  Serial.begin(9600);
}

void loop() {
  // watch for reset button, only if not already reading
  if (reading == false && digitalRead(btnReset) == LOW) {
    startRead();
  }
   
  // watch for display button
  if (digitalRead(btnDisplay) == LOW) {
    stopRead();
    displayAltitude();
  }
  
  if (reading) {
    doRead();
  }
  
  delay(50);
}


// Reading

void startRead() {
  reading = true;
  resetMaxAltitude();
  doRead();
}

void stopRead() {
  reading = false;
  turnOffLed();
}

void doRead() {
  blinkLed(255, 0, 0, blinkDelay);
  recordAltitude();
}

void blinkLed(int red, int green, int blue, int ms) {
  if (millis() - millisCounter > ms) {
    if (ledOn) {
      turnOffLed();
    } else {
      turnOnLed(red, green, blue);
    }
    resetCounter();
  }
}



// Bean stuff

void resetCounter() {
  millisCounter = millis();
}

void turnOnLed(int red, int green, int blue) {
  Bean.setLed(red, green, blue);
  ledOn = true;
}

void turnOffLed() {
  Bean.setLed(0,0,0);
  ledOn = false;
}



// Altitude stuff

int readAltitude() {
  return 123;
}

void displayAltitude() {
  int hundreds = maxAltitude / 100;
  int tens = (maxAltitude - (hundreds * 100)) / 10;
  int ones = maxAltitude - (hundreds * 100) - (tens * 10);
  
//  Serial.print("maxAltitude: ");
//  Serial.print(maxAltitude);
//  Serial.print("hundreds: ");
//  Serial.print(hundreds);
//  Serial.print("tens: ");
//  Serial.print(tens);
//  Serial.print("ones: ");
//  Serial.print(ones);
  
  blinkDisplay(hundreds);
  delay(displayPositionDelay);
  blinkDisplay(tens);
  delay(displayPositionDelay);
  blinkDisplay(ones);
  delay(displayPositionDelay);
  
  // show green for a second 
  turnOnLed(0, 255, 0);
  delay(1000);
  turnOffLed();
}

void blinkDisplay(int num) {
  int blinks = 0;
  
  // blink 10 times for 0
  if (num == 0) {
   num = 10;
  }
 
  while (blinks < num) {
    if (ledOn) {
      turnOnLed(255, 255, 255);
      // turn led off
      if (millis() - millisCounter > displayDigitOnDelay) {
        ledOn = false;
        blinks++;
        turnOffLed();
        resetCounter(); 
      }
    } else {
      turnOffLed();
      // turn led on
      if (millis() - millisCounter > displayDigitOffDelay) {
        turnOnLed(255, 255, 255);
        resetCounter(); 
      }
    }
  }
  
}

void resetMaxAltitude() {
  maxAltitude = readAltitude();
}

void recordAltitude() {
  int currentAltitude = readAltitude();
  if (currentAltitude > maxAltitude) {
    maxAltitude = currentAltitude;
  }
}

