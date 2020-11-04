// ---------------------------------------------------------------- //
// Arduino Ultrasoninc Sensor HC-SR04
// Re-writed by Arbi Abdul Jabbaar
// Using Arduino IDE 1.8.7
// Using HC-SR04 Module
// Tested on 17 September 2019
// ---------------------------------------------------------------- //

#include <M5Stack.h>
#include <NewPing.h>
#include <Arduino.h>

// setup the TDC7200
#include <TDC7200.h>
#define PIN_TDC7200_ENABLE    2
#define PIN_TDC7200_SPI_CS    5
#define PIN_TDC7200_INT       17
#define TDC7200_CLOCK_FREQ_HZ 8000000
#define NUM_STOPS (1)

static TDC7200 tof(PIN_TDC7200_ENABLE, PIN_TDC7200_SPI_CS, TDC7200_CLOCK_FREQ_HZ);

#define echoPin 21 // attach Arduino to pin Echo of HC-SR04
#define trigPin 22 //attach Arduino to pin Trig of HC-SR04

#define MAX_DISTANCE 150 // Maximum distance we want to ping for (in centimeters). Maximum sensor distance is rated at 400-500cm.
NewPing sonar(trigPin, echoPin, MAX_DISTANCE); // NewPing setup of pins and maximum distance.
float duration, distance;

/* Used in TOF calculation*/
static void ui64toa(uint64_t v, char * buf, uint8_t base)
{
  int idx = 0;
  uint64_t w = 0;
  while (v > 0)
  {
    w = v / base;
    buf[idx++] = (v - w * base) + '0';
    v = w;
  }
  buf[idx] = 0;
  // reverse char array
  for (int i = 0, j = idx - 1; i < idx / 2; i++, j--)
  {
    char c = buf[i];
    buf[i] = buf[j];
    buf[j] = c;
  }
}


void setup() {

  Serial.begin(115200);
  M5.begin();

  M5.Lcd.fillScreen(RED);
  M5.Lcd.setCursor(100, 100);
  M5.Lcd.print("Hello");

  Serial.println(F("-- Starting TDC7200 test --"));
  while (not tof.begin())
  {
    Serial.println(F("Failed to init TDC7200"));
    delay(1000);
  }

  pinMode(PIN_TDC7200_INT, INPUT_PULLUP);     // active low (open drain)


  pinMode(trigPin, OUTPUT); // Sets the trigPin as an OUTPUT
  pinMode(echoPin, INPUT); // Sets the echoPin as an INPUT

  Serial.println(F("Ultrasonic Sensor HC-SR04 Test")); // print some text in Serial Monitor
  Serial.println(F("with m5stack"));


  if (not tof.setupMeasurement( 10,         // cal2Periods
                                1,          // avgCycles
                                NUM_STOPS,  // numStops
                                2 ))        // mode
  {
    M5.Lcd.print("FAIL");
    Serial.println(F("Fail"));
    while (1);
  } else {
    M5.Lcd.print("Setup OK");
    Serial.println(F("Setup OK"));
      Serial.print(F("\tCalibA\t")); Serial.print(tof.readCalibA());
  Serial.print(F("\tCalibB\t")); Serial.print(tof.readCalibB());
    delay(1000);
  }

  // Setup stop mask to suppress stops during the initial timing period.
  //tof.setupStopMask(121000000ull);

  // Setup overflow to timeout if tof takes longer than timeout.
  //tof.setupOverflow(130000000ull);

  delay(1000);
}




void loop() {
  // Clears the trigPin condition
  // Wait 100ms between pings (about 20 pings/sec). 29ms should be the shortest delay between pings.

  tof.startMeasurement();


  duration = sonar.ping();
  distance = (duration / 2) * 0.0343;

  delay(50);
  M5.Lcd.fillScreen(RED);
  M5.Lcd.setCursor(100, 100);
  M5.Lcd.print(distance);
  Serial.print(F("\tCalibA\t")); Serial.print(tof.readCalibA());
  Serial.print(F("\tCalibB\t")); Serial.print(tof.readCalibB());
  Serial.print("\n\n");
  if (tof.readCalibB()) {
    if (distance > 0.001) {
      M5.Lcd.setCursor(100, 130);
      M5.Lcd.print("Starting");
      // Wait for interrupt to indicate finished or overflow
      while (digitalRead(PIN_TDC7200_INT) == HIGH);

      for (uint8_t stop = 1; stop <= NUM_STOPS; ++stop)
      {
        M5.Lcd.setCursor(100, 170);

        Serial.print(F("\n\nStop ")); Serial.print(F(stop));

        uint64_t time;
        if (tof.readMeasurement(stop, time)) // 2**23 for T : 8388608
        {
          char buff[40];
          ui64toa(time, buff, 10);
          //Serial.print(F("\ttof")); Serial.print(stop); Serial.print('='); Serial.print(buff);
          M5.Lcd.setCursor(100, 150);
          M5.Lcd.print(buff);
        }

      }
    }
  }
  delay(500);


}
