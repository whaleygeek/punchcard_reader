#define REGISTRATION A0
#define D7           A1

void setup()
{
  Serial.begin(115200);  
}

void loop()
{
  delay(250);
  
  unsigned int reg = analogRead(A0);
  unsigned int d7  = analogRead(D7);
  
  Serial.print(reg);
  Serial.print(",");
  Serial.println(d7);
}
