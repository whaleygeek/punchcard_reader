// txtest.ino  08/05/2016  D.J.Whale
//
// Test serial communications on ATTiny85 using USI

#define LED_RED 2    // PB2, pin 7
#define LED_GREEN 0  // PB0, pin 5
#define TX 1         // PB1, pin 6

#define BIT_TIME 10 //ms

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
  //TODO could shift out two bits, for 1 bit of pad, to simplify timing/shifting
  USIDR = 0x00; // high bit zero, pin changes low immediately
  // clock out start bit
  delay(BIT_TIME);
  
  // Clock out 8 data bits
  // use counter, so that later we can use timer counter hardware for accurate timing
  USISR = B00001000; // overflow interrupt cleared, clock preloaded with 8 (+8=overflow)
  USIDR = 0xAA;

  do
  //for (i=0; i<8; i++)
  {  
    delay(BIT_TIME);
    USICR |= B00000010; //1<<USICLK; // shift happens here
    USICR &= ~ B00000010; //1<<USICLK; 
  }
  //while ((USISR & B01000000) == 0); // 1<<USIOIF
  while ((USISR & 0x0F) != 0);
  
  // Clock out stop bit (high)
  USIDR = 0x80; // high bit one, pin changes immediately
  delay(BIT_TIME);
  
  digitalWrite(LED_GREEN, LOW);
  delay(100);
  
}
