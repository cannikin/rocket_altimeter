#include "Wire.h"
#include "I2Cdev.h"
#include "BMP085.h"

BMP085 barometer;

float temperature;
float pressure;
float altitude;

const byte btnReset = 0;
const byte btnDisplay = 5;
const unsigned long maxBeanSleep = 0xFFFFFFFF;

boolean reading = false;
boolean displaying = false;
boolean ledOn = false;
boolean shouldRead = false;

float maxAltitude;
float minAltitude;
unsigned long millisCounter = millis();
unsigned long altitudeMicrosCounter;

const unsigned int blinkDelay = 250;
const unsigned int displayPositionDelay = 750;
const unsigned int displayDigitOnDelay = 300;
const unsigned int displayDigitOffDelay = 200;


void setup() {
  Wire.begin();
  Serial.begin(9600);

  barometer.initialize();

  pinMode(btnReset, INPUT_PULLUP);
  pinMode(btnDisplay, INPUT_PULLUP);
  pinMode(2, OUTPUT);
  pinMode(3, OUTPUT);

  Bean.attachChangeInterrupt(btnReset, wakeFromSleep);

  // Sleep until we want to start reading
  Bean.sleep(maxBeanSleep);
}


void loop() {
  // watch for reset button, only if not already reading
  if ((reading == false && digitalRead(btnReset) == LOW) || shouldRead) {
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
}


//////////////
// Bean stuff
//////////////

void resetCounter() {
  millisCounter = millis();
}

void turnOnLed(byte red, byte green, byte blue) {
  Bean.setLed(red, green, blue);
  ledOn = true;
}

void turnOffLed() {
  Bean.setLed(0,0,0);
  ledOn = false;
}

void wakeFromSleep() {
  shouldRead = true;
}


///////////
// Reading
///////////

void startRead() {
  reading = true;
  shouldRead = false;
  resetMaxAltitude();
}

void stopRead() {
  reading = false;
  turnOffLed();
}

void doRead() {
  blinkLed(255, 0, 0, blinkDelay);
  recordAltitude();
  delay(50);
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


//////////////////
// Altitude stuff
//////////////////

// Saves the current altitude as max if it's higher than the existing max
void recordAltitude() {
  float currentAltitude = readAltitude();

  if (currentAltitude > maxAltitude) {
    maxAltitude = currentAltitude;
    Serial.print("new max: ");
    Serial.print(maxAltitude);
    Serial.print("m\n");
  }

  if (currentAltitude < minAltitude) {
    minAltitude = currentAltitude;
    Serial.print("new min: ");
    Serial.print(minAltitude);
    Serial.print("m\n");
  }
}


// Read altitude from barometer
float readAltitude() {
  int tries = 0;
  float sumAltitude = 0;

  // Read altitude three times and average
  while (tries < 3) {
    tries++;
    altitudeMicrosCounter = micros();
    barometer.setControl(BMP085_MODE_PRESSURE_3);

    while (micros() - altitudeMicrosCounter < barometer.getMeasureDelayMicroseconds());

    sumAltitude += barometer.getAltitude(barometer.getPressure());
    delay(10);
  }

  return sumAltitude / 3;
}


// Resets max altitude back to the current altitude
void resetMaxAltitude() {
  maxAltitude = minAltitude = readAltitude();
}



/////////////////
// Display stuff
/////////////////


// Display altitude by blinking LED
void displayAltitude() {
  int displayAltitude = int((maxAltitude - minAltitude) * 3.28084);

  Serial.print("display: ");
  Serial.print(displayAltitude);
  Serial.print("ft\n");

  int hundreds = displayAltitude / 100;
  int tens = (displayAltitude - (hundreds * 100)) / 10;
  int ones = displayAltitude - (hundreds * 100) - (tens * 10);

  delay(500);
  // don't display hundreds place if 0
  if (hundreds > 0) {
    blinkDisplay(hundreds);
    delay(displayPositionDelay);
  }
  // don't display tens place if hundreds and tens are both 0
  if (hundreds > 0 || tens != 0) {
    blinkDisplay(tens);
    delay(displayPositionDelay);
  }
  // alawys display ones place
  blinkDisplay(ones);
  delay(displayPositionDelay);

  // show green for a second to say we're done
  turnOnLed(0, 255, 0);
  delay(1000);
  turnOffLed();

  // Sleep until we want to start reading again
  Bean.sleep(maxBeanSleep);
}


// Actual timing and blink function
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
