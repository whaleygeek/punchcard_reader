// ReadSensors.ino  20/06/2015  D.J.Whale
//
// (c) 2015 D.J.Whale

#define REGISTRATION A0

#define D7           A1
#define D6           A2
#define D5           A3
#define D4           A6
#define D3           A7
#define D2           A8
#define D1           A9
#define D0           A10

#define LED_REG      1 // TX
#define LED_D7       0 // RX
#define LED_D6       2
#define LED_D5       3
#define LED_D4       5
#define LED_D3       7
#define LED_D2       16
#define LED_D1       14
#define LED_D0       15


#define REG_STATE_OPEN   '.'
#define REG_STATE_WHITE  'W'
#define REG_STATE_BLACK  'B'

#define DATA_STATE_PAPER '.'
#define DATA_STATE_HOLE  'X'

void setup()
{
  Serial.begin(115200);  
  pinMode(LED_REG, OUTPUT);
  pinMode(LED_D7,  OUTPUT);
  pinMode(LED_D6,  OUTPUT);
  pinMode(LED_D5,  OUTPUT);
  pinMode(LED_D4,  OUTPUT);
  pinMode(LED_D3,  OUTPUT);
  pinMode(LED_D2,  OUTPUT);
  pinMode(LED_D1,  OUTPUT);
  pinMode(LED_D0,  OUTPUT);
  
}

void loop()
{
  //delay(250);
  
  // Read all 9 inputs (roughly) at same time
  unsigned int reg = analogRead(A0);
  unsigned int d7  = analogRead(D7);
  unsigned int d6  = analogRead(D6);
  unsigned int d5  = analogRead(D5);
  unsigned int d4  = analogRead(D4);
  unsigned int d3  = analogRead(D3);
  unsigned int d2  = analogRead(D2);
  unsigned int d1  = analogRead(D1);
  unsigned int d0  = analogRead(D0);
  
  // Generate filtered versions (character states)
  byte freg = getReg(reg);
  byte fd7  = getData(d7);
  byte fd6  = getData(d6);
  byte fd5  = getData(d5);
  byte fd4  = getData(d4);
  byte fd3  = getData(d3);
  byte fd2  = getData(d2);
  byte fd1  = getData(d1);
  byte fd0  = getData(d0);
  
  
  // Show card data on feedback LEDs
  digitalWrite(LED_REG, (freg == REG_STATE_BLACK));
  digitalWrite(LED_D7,  (fd7  == DATA_STATE_HOLE));
  digitalWrite(LED_D6,  (fd6  == DATA_STATE_HOLE));
  digitalWrite(LED_D5,  (fd5  == DATA_STATE_HOLE));
  digitalWrite(LED_D4,  (fd4  == DATA_STATE_HOLE));
  digitalWrite(LED_D3,  (fd3  == DATA_STATE_HOLE));
  digitalWrite(LED_D2,  (fd2  == DATA_STATE_HOLE));
  digitalWrite(LED_D1,  (fd1  == DATA_STATE_HOLE));
  digitalWrite(LED_D0,  (fd0  == DATA_STATE_HOLE));
  
  
  // Display card data on serial port
  #if 0
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
  #endif
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
