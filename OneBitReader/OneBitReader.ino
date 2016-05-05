// OneBitReader.ino  05/05/2015  D.J.Whale
// based on ReadSensors.ino  20/06/2015  D.J.Whale
// (c) 2015, 2016 D.J.Whale

//Turn this on to turn a state change report every state change
//#define CFGEN_SEND_STATE_REPORTS

//Turn this on to send row data reports every detected row
//#define CFGEN_SEND_ROW_REPORTS

// Pinouts for Sparkfun ProMicro:
// https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/hardware-overview-pro-micro

//GPIO reads low for hole
#define HOLE 0
//GPIO reads high for paper
#define PAPER 1


//#define NUM_CHANNELS 9
#define REG          2  
#define D7           3 
//#define D6           A2  // A2
//#define D5           A3  // A3
//#define D4           A6  // 4
//#define D3           A7  // 6
//#define D2           A8  // 8
//#define D1           A9  // 9
//#define D0           A10 // 10

#define LED_REG      1 // TX
#define LED_D7       0 // RX
//#define LED_D6       2
//#define LED_D5       3
//#define LED_D4       5
//#define LED_D3       7
//#define LED_D2       16
//#define LED_D1       14
//#define LED_D0       15

//fudged for 1 bit reader (bit 7 only)
#define ALL_PAPER  (PAPER*0xFF)
#define ALL_HOLES  (HOLE *0xFF)

#define REG_PAPER PAPER 
#define REG_HOLE  HOLE

// Set to 8, will only read IN phase
// Set to 16, will read and collect OUT phase also
// Must see all CARD_ROWS rows to get a OK report
// if less rows read, will get an error, but data still included
// This allows host to compare IN and OUT phases and error correct
#define CARD_ROWS    16

#define REPORT_OK_BOOT       0x00
#define REPORT_OK_CARD       0x01
#define REPORT_OK_STATE      0x02
#define REPORT_OK_ROW        0x03
#define REPORT_OK_ADC        0x04
#define REPORT_ERR_LENGTH    0x81

typedef enum 
{
  STATE_NOCARD        = 0,
  STATE_INSERTING,    //1
  STATE_WAITING_ROW,  //2
  STATE_IN_ROW,       //3
  STATE_GAP,          //4
  STATE_REMOVING,     //5
  STATE_END           //6
} STATE;

byte sticky = 0;
boolean seenReg = false;
byte row = 0;
byte card[CARD_ROWS];
//unsigned int adc[NUM_CHANNELS];
STATE state = STATE_NOCARD;
STATE prev = STATE_NOCARD;


void setup()
{
  Serial.begin(115200);  
  pinMode(LED_REG, OUTPUT);
  pinMode(LED_D7,  OUTPUT);
  pinMode(REG, INPUT);
  pinMode(D7,  INPUT);
  
  digitalWrite(LED_REG, HIGH);
  delay(1000);
  digitalWrite(LED_REG, LOW);
  
  digitalWrite(LED_D7, HIGH);
  delay(1000);
  digitalWrite(LED_D7, LOW);
  
  //pinMode(LED_D6,  OUTPUT);
  //pinMode(LED_D5,  OUTPUT);
  //pinMode(LED_D4,  OUTPUT);
  //pinMode(LED_D3,  OUTPUT);
  //pinMode(LED_D2,  OUTPUT);
  //pinMode(LED_D1,  OUTPUT);
  //pinMode(LED_D0,  OUTPUT);
  sendCardReport(REPORT_OK_BOOT, NULL, 0);
}

void loop()
{  
  //readPins
  byte freg = (digitalRead(REG) ? REG_PAPER : REG_HOLE);
  byte now  = (digitalRead(D7)  ? ALL_PAPER : ALL_HOLES);

  // Show live diagnostics on LEDs
  writeLEDs(freg, now);


  // crank round the acquisition state machine
  switch (state)
  {
    case STATE_NOCARD:
      // stay here while freg=hole and data=all holes (card removed)
      if ((freg == REG_PAPER) || now != ALL_HOLES)
      { // at least one sensor has seen paper
        state = STATE_INSERTING;
      } 
    break;
    
    case STATE_INSERTING:
      // check for early removal (all holes)
      if ((freg == REG_HOLE) && (now == ALL_HOLES))
      {
        // bail early if all on (removed)
        state = STATE_REMOVING;
      }
      // wait for all paper
      else if ((freg == REG_PAPER) && (now == ALL_PAPER))
      {
        row = 0;
        state = STATE_WAITING_ROW;
      }
    break;
    
    case STATE_WAITING_ROW:
      // check for early card removal (all reading holes)
      if ((freg == REG_HOLE) && (now == ALL_HOLES))
      {
        state = STATE_REMOVING;
      }
      else
      { // wait for any data to start appearing (at least one hole)
        if ((freg == REG_HOLE) || (now != ALL_PAPER))
        {
          sticky  = ~now; // 0=>hole, so we want to set bits for holes
          state   = STATE_IN_ROW;
        }
      }
    break;
    
    case STATE_IN_ROW:
      // check for early card removal (all holes)
      if ((freg == REG_HOLE) && (now == ALL_HOLES))
      {
        state = STATE_REMOVING;
      }
      else
      {
        // remember if we see the registration hole while reading a row
        // this is only punched if the row is completely unpunched
        // that way we don't miss the row
        if (freg == REG_HOLE)
        {
          seenReg = true;
        }
        // keep collecting sticky bits until all go zero again
        sticky |= ~now; // 0=>hole, so we want to collect 1's for holes
        if ((freg == REG_PAPER) && (now == ALL_PAPER))
        {
          state = STATE_GAP;
        }  
      }
    break;
    
    case STATE_GAP:
#if defined(CFGEN_SEND_ROW_REPORTS)
      {
         byte report[2];
         report[0] = row;
         report[1] = sticky;
         sendCardReport(REPORT_OK_ROW, report, sizeof(report));
      }
#endif      
      // store row data in card buffer
      card[row] = sticky;
      // advance row
      row += 1;
      // more rows?
      if (row == CARD_ROWS)
      {
        state = STATE_END;
      }
      else
      {
        state = STATE_WAITING_ROW;
      }
    break;
    
    case STATE_END:
      // wait here until card removal
      // Note, later version will read on way out again, and complare in and out readings
      if ((freg == REG_HOLE) && (now == ALL_HOLES))
      {
        sendCardReport(REPORT_OK_CARD, card, CARD_ROWS);
        state = STATE_NOCARD;
      }
    break;
    
    case STATE_REMOVING:
      // only report the rows that were read
      sendCardReport(REPORT_ERR_LENGTH, card, row);
      state = STATE_NOCARD;
    break;
  }  
  
  if (state != prev)
  {
    prev = state;
#if defined(CFGEN_SEND_STATE_REPORTS)
    {
      byte report = (byte) (state & 0xFF);
      sendCardReport(REPORT_OK_STATE, &report, 1);
    }
#endif
  }
}



void writeLEDs(byte reg, byte data)
{
  // Show card data on feedback LEDs
  digitalWrite(LED_REG, reg?HIGH:LOW);
  digitalWrite(LED_D7,  (data&(1<<7))?HIGH:LOW);
  //digitalWrite(LED_D6,  data&(1<<6));
  //digitalWrite(LED_D5,  data&(1<<5));
  //digitalWrite(LED_D4,  data&(1<<4));
  //digitalWrite(LED_D3,  data&(1<<3));
  //digitalWrite(LED_D2,  data&(1<<2));
  //digitalWrite(LED_D1,  data&(1<<1));
  //digitalWrite(LED_D0,  data&(1<<0));
}


void sendCardReport(byte type, byte* pData, byte len)
{
  Serial.write(":"); // Start char
  sendHexByte(type); // mandatory type
  
  // Show optional data of any length
  for (byte i=0; i<len; i++)
  {
    sendHexByte(pData[i]);
  }
  Serial.println(); // End char
}


void sendHexByte(byte val)
{
  Serial.write(tohexch(val>>4)); // high nybble
  Serial.write(tohexch(val));    // low nybble
}


char tohexch(byte val)
{
  val = val & 0x0F;
  if (val > 9)
  {
    return 'A' + (val-10);
  }
  else
  {
    return '0' + val;
  }
}

/* END OF FILE */


