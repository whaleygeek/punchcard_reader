// txtest.ino  08/05/2016  D.J.Whale
//
// Test serial communications on ATTiny85 using USI

#define LED_RED 2    // PB2, pin 7
#define LED_GREEN 0  // PB0, pin 5
#define TX 1         // PB1, pin 6

#define HALF_BIT_TIME 5 //ms

void setup()
{
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(TX, OUTPUT);
  digitalWrite(TX, HIGH); // default state=marking
  digitalWrite(LED_RED, HIGH);
  
  USICR = B00010000;
  // start condition interrupt: off
  // counter overflow interrupt: off
  // wire mode: 01 (three wire)
  // clock source: USICLK generated
  // USICLK: 0 
  // toggle clock: no
}

void loop()
{
  byte i;
  digitalWrite(LED_GREEN, HIGH);
  
  // Transmit a single byte using USI
  // Transmit start bit (low)
  USIDR = 0x00; // high bit zero
  // clock out start bit
  delay(HALF_BIT_TIME*2);
  
  // Clock out 8 data bits
  USIDR = 0xAA;
  // use counter, so that later we can use timer counter hardware for accurate timing
  USISR = B00001000; // overflow interrupt disabled, clock preloaded with 8 (+8=overflow)
  //do
  for (i=0; i<8; i++)
  {  
    delay(HALF_BIT_TIME);
    USICR |= B00000010; //USICLK;
    delay(HALF_BIT_TIME);
    USICR &= ~ B00000010; //USICLK;
  }
  //while ((USICR & B01000000) == 0);
  
  // Clock out stop bit (high)
  USIDR = 0x80;
  delay(HALF_BIT_TIME*2);
  
  digitalWrite(LED_GREEN, LOW);
  delay(100);
  
}
