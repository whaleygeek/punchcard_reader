
// Using ATTiny85

#define LED_RED    2 // PB2, pin 7
#define LED_GREEN  0 // PB0, pin 5

void setup()
{
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
}

void loop()
{
  digitalWrite(LED_RED, HIGH);
  delay(250);
  digitalWrite(LED_RED, LOW);

  digitalWrite(LED_GREEN, HIGH);
  delay(250);
  digitalWrite(LED_GREEN, LOW);  
}


