// Connections:


#include "TDC7200.h"
#include <M5Stack.h>

#define PIN_TDC7200_INT       17
#define PIN_TDC7200_ENABLE    2
#define PIN_TDC7200_STOP      22
#define PIN_TDC7200_START     21

#define PIN_TDC7200_SPI_CS    5
#define TDC7200_CLOCK_FREQ_HZ 8000000

#define oePin 3 //

static TDC7200 tof(PIN_TDC7200_ENABLE, PIN_TDC7200_SPI_CS, TDC7200_CLOCK_FREQ_HZ);

int NbLines;
#define NUM_STOPS (2)

void setup()
{
  NbLines = 0;
  delay(50);
  Serial.begin(115200);
  Serial.println(F("-- Starting TDC7200 test --"));
  while (not tof.begin())
  {
    Serial.println(F("Failed to init TDC7200"));
    delay(1000);
  }
  delay(1000);
  pinMode(oePin, OUTPUT);
  digitalWrite(oePin, LOW);

  pinMode(PIN_TDC7200_INT, INPUT_PULLUP);     // active low (open drain)

  digitalWrite(PIN_TDC7200_START, LOW);
  pinMode(PIN_TDC7200_START, OUTPUT);

  digitalWrite(PIN_TDC7200_STOP, LOW);
  pinMode(PIN_TDC7200_STOP, OUTPUT);

  if (not tof.setupMeasurement( 10,         // cal2Periods
                                1,          // avgCycles
                                NUM_STOPS,  // numStops
                                2 ))        // mode
  {
    Serial.println(F("Failed to setup measurement"));
    while (1);
  }

  // Setup stop mask to suppress stops during the initial timing period.
  //    tof.setupStopMask(121000000ull);

  // Setup overflow to timeout if tof takes longer than timeout.
  //    tof.setupOverflow(130000000ull);

}

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

static void genPulse(const uint32_t usec, const uint8_t numStops)
{
  noInterrupts();
  digitalWrite(PIN_TDC7200_START, HIGH);
  digitalWrite(PIN_TDC7200_START, LOW);

  for (uint8_t i = 0; i < numStops; ++i)
  {
    delayMicroseconds(usec);

    digitalWrite(PIN_TDC7200_STOP, HIGH);
    digitalWrite(PIN_TDC7200_STOP, LOW);
  }
  interrupts();
}

void printCalib(String placeholder) {
  Serial.println("\n-- ");Serial.print(NbLines); Serial.print(" ");
  Serial.print("Testing: "); Serial.print(placeholder); Serial.print(" --\n");
  Serial.print(F("-- CalibA\t")); Serial.print(tof.readCalibA());
  Serial.print(F("\tCalibB\t")); Serial.print(tof.readCalibB());
  Serial.println("\n-- --");
}



void loop()
{
  static uint16_t pulseUs = 100;
  NbLines++;
  Serial.print(F("delay=")); Serial.print(pulseUs);
  printCalib("Avant measurement");
  tof.startMeasurement();
  printCalib("Apres measurement");
  genPulse(pulseUs, NUM_STOPS);

  // Wait for interrupt to indicate finished or overflow
  while (digitalRead(PIN_TDC7200_INT) == HIGH);
  printCalib("Apres INT");
  for (uint8_t stop = 1; stop <= NUM_STOPS; ++stop)
  {
    uint64_t time;
    if (tof.readMeasurement(stop, time))
    {
      char buff[40];
      ui64toa(time, buff, 10);

      Serial.print(F("\ttof")); Serial.print(stop); Serial.print('='); Serial.print(buff);
    }
  }

  Serial.println();
  delay(1000);

  /*
      pulseUs += 100;
      if (pulseUs > 2000)
      {
          pulseUs = 0;
      }

  */
}
