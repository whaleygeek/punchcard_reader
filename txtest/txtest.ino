// txtest.ino  08/05/2016  D.J.Whale
//
// Test serial communications on ATTiny85 using USI

#define LED_RED 2    // PB2, pin 7
#define LED_GREEN 0  // PB0, pin 5
#define TX 1         // PB1, pin 6

void setup()
{
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(TX, OUTPUT);
  //Serial.begin(9600);
  digitalWrite(LED_RED, HIGH);
}

void loop()
{
  digitalWrite(LED_GREEN, HIGH);
  //Serial.println('hello');
  digitalWrite(TX, HIGH);
  delay(10);
  digitalWrite(TX, LOW);
  digitalWrite(LED_GREEN, LOW);
  delay(100);
}
