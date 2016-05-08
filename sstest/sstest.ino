#include <SoftwareSerial.h>

#define LED_RED 2    // PB2, pin 7
#define LED_GREEN 0  // PB0, pin 5
#define TX 1         // PB1, pin 6
#define RX 3         // testing, pin 2

SoftwareSerial ser(RX, TX);

void setup()
{
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  ser.begin(9600);
  digitalWrite(LED_RED, HIGH);
}

void loop()
{
  digitalWrite(LED_GREEN, HIGH);
  ser.print("\xAA");
  digitalWrite(LED_GREEN, LOW);
  delay(100);
}

