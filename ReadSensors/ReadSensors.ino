#define REGISTRATION A0
#define D7           A1
#define D6           A2
#define D5           A3
#define D4           A6
#define D3           A7
#define D2           A8
#define D1           A9
#define D0           A10

#define REG_STATE_OPEN '.'
#define REG_STATE_WHITE 'W'
#define REG_STATE_BLACK 'B'

#define DATA_STATE_PAPER '.'
#define DATA_STATE_HOLE  'X'

void setup()
{
  Serial.begin(115200);  
}

void loop()
{
  delay(250);
  
  unsigned int reg = analogRead(A0);
  unsigned int d7  = analogRead(D7);
  unsigned int d6  = analogRead(D6);
  unsigned int d5  = analogRead(D5);
  unsigned int d4  = analogRead(D4);
  unsigned int d3  = analogRead(D3);
  unsigned int d2  = analogRead(D2);
  unsigned int d1  = analogRead(D1);
  unsigned int d0  = analogRead(D0);
  
  byte freg = getReg(reg);
  byte fd7  = getData(d7);
  byte fd6  = getData(d6);
  byte fd5  = getData(d5);
  byte fd4  = getData(d4);
  byte fd3  = getData(d3);
  byte fd2  = getData(d2);
  byte fd1  = getData(d1);
  byte fd0  = getData(d0);
  
  
  Serial.write(freg);
  Serial.write(" ");
  Serial.print(reg);
  Serial.write(" ");
  Serial.write(fd7);
  Serial.write(fd6);
  Serial.write(fd5);
  Serial.write(fd4);
  Serial.write(fd3);
  Serial.write(fd2);
  Serial.write(fd1);
  Serial.write(fd0);
  Serial.println();
}

byte getReg(unsigned int adc)
{
  if (adc < 200) return REG_STATE_OPEN;
  if (adc < 994) return REG_STATE_WHITE;
  return REG_STATE_BLACK;
}

byte getData(unsigned int adc)
{
  if (adc < 200) return DATA_STATE_HOLE;
  return DATA_STATE_PAPER;
}
