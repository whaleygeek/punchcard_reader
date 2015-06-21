// ReadSensors.ino  20/06/2015  D.J.Whale
//
// (c) 2015 D.J.Whale

// punching
// punch data in the 8 columns
// if you want an empty row, punch the REG hole
// don't punch all 8+REG else it will see a card removal


//TODO: add a record type that shows min/max values for zero's and ones on the last read
//this will be helpful to be able to diagnose problems in the field.
//min/max 1/0 for all 9 channels, in case of sensor misallignment.

// The ADC is 10 bit (0..1023)
// The photo transistors conduct when they detect IR
// So a HOLE will make the transistor conduct, current will flow, and
// the voltage at the ADC will drop, giving a value lower than this reading.
// When no card is present, all photo transistors will conduct and draw current.
// <  200 = HOLE  = 1
// >= 200 = PAPER = 0

#define CFG_SENSOR_THRESHOLD 512

//Turn on to send a raw ADC value report every time it changes
//#define CFGEN_SEND_ADC_REPORTS

//Turn this on to turn a state change report every state change
//#define CFGEN_SEND_STATE_REPORTS

//Turn this on to send row data reports every detected row
//#define CFGEN_SEND_ROW_REPORTS

// Pinouts for Sparkfun ProMicro:
// https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/hardware-overview-pro-micro

#define NUM_CHANNELS 9
#define REG          A0  // A0
#define D7           A1  // A1
#define D6           A2  // A2
#define D5           A3  // A3
#define D4           A6  // 4
#define D3           A7  // 6
#define D2           A8  // 8
#define D1           A9  // 9
#define D0           A10 // 10

#define LED_REG      1 // TX
#define LED_D7       0 // RX
#define LED_D6       2
#define LED_D5       3
#define LED_D4       5
#define LED_D3       7
#define LED_D2       16
#define LED_D1       14
#define LED_D0       15

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
  STATE_NOCARD = 0,
  STATE_INSERTING,
  STATE_WAITING_ROW,
  STATE_IN_ROW,
  STATE_GAP,
  STATE_REMOVING,
  STATE_END
} STATE;

byte sticky = 0;
boolean seenReg = false;
byte row = 0;
byte card[CARD_ROWS];
unsigned int adc[NUM_CHANNELS];
STATE state = STATE_NOCARD;
STATE prev = STATE_NOCARD;


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
  sendCardReport(REPORT_OK_BOOT, NULL, 0);
}


void loop()
{  
  readADCs();
  
  // Generate filtered versions of registration and data
  byte freg = getData(adc[8], 0);  
  byte now  = getData(adc[7], 7) | getData(adc[6], 6) | getData(adc[5], 5) | getData(adc[4], 4)
            | getData(adc[3], 3) | getData(adc[2], 2) | getData(adc[1], 1) | getData(adc[0], 0);

  // Show live diagnostics on LEDs
  writeLEDs(freg, now);

#if defined(CFGEN_SEND_ADC_REPORTS)
  if (now != sticky)
  {
    Serial.write(':');
    Serial.write(REPORT_OK_ADC);
    for (int i=8; i>=0; i--)
    {
      //in decimal ADC values are 10 bits (3 nybbles)
      Serial.print(adc[i]);
      Serial.print(" ");
    }
    Serial.println();
  }
#endif

  
  // crank round the acquisition state machine
  switch (state)
  {
    case STATE_NOCARD:
      // stay here while freg=1 and data=0xFF, all holes (card removed)
      if ((freg != 1) || now != 0xFF)
      { // at least one sensor has seen paper
        state = STATE_INSERTING;
        //Serial.println(state);
      } 
    break;
    
    case STATE_INSERTING:
      // check for early removal (all holes)
      if ((freg == 1) && (now == 0xFF))
      {
        // bail early if all on (removed)
        state = STATE_REMOVING;
        //Serial.println(state);
      }
      // wait for all paper
      else if ((freg == 0) && (now == 0x00))
      {
        row = 0;
        state = STATE_WAITING_ROW;
        //Serial.println(state);
      }
    break;
    
    case STATE_WAITING_ROW:
      // check for early card removal (all reading holes)
      if ((freg == 1) && (now == 0xFF))
      {
        state = STATE_REMOVING;
        //Serial.println(state);
      }
      else
      { // wait for any data to start appearing (at least one hole)
        if ((freg == 1) || (now != 0x00))
        {
          sticky  = now;
          state   = STATE_IN_ROW;
          //Serial.println(state);
        }
      }
    break;
    
    case STATE_IN_ROW:
      // check for early card removal (all holes)
      if ((freg == 1) && (now == 0xFF))
      {
        state = STATE_REMOVING;
        //Serial.println(state);
      }
      else
      {
        // remember if we see the registration hole while reading a row
        // this is only punched if the row is completely unpunched
        // that way we don't miss the row
        if (freg == 1)
        {
          seenReg = true;
        }
        // keep collecting sticky bits until all go zero again
        sticky |= now;
        if ((freg == 0) && (now == 0x00))
        {
          state = STATE_GAP;
          //Serial.println(state);
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
        //Serial.println(state);
      }
      else
      {
        state = STATE_WAITING_ROW;
        //Serial.println(state);
      }
    break;
    
    case STATE_END:
      // wait here until card removal
      // Note, later version will read on way out again, and complare in and out readings
      if ((freg == 1) && (now == 0xFF))
      {
        sendCardReport(REPORT_OK_CARD, card, CARD_ROWS);
        state = STATE_NOCARD;
        //Serial.println(state);
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


byte readADCs(void)
{
  // Read all 9 inputs (roughly) at same time
  adc[8] = analogRead(REG);
  adc[7] = analogRead(D7);
  adc[6] = analogRead(D6);
  adc[5] = analogRead(D5);
  adc[4] = analogRead(D4);
  adc[3] = analogRead(D3);
  adc[2] = analogRead(D2);
  adc[1] = analogRead(D1);
  adc[0] = analogRead(D0);
}


byte getData(unsigned int adc, byte bitno)
{
  if (adc < CFG_SENSOR_THRESHOLD) return (1<<bitno);
  return 0;
}


void writeLEDs(byte reg, byte data)
{
  // Show card data on feedback LEDs
  digitalWrite(LED_REG, reg);
  digitalWrite(LED_D7,  data&(1<<7));
  digitalWrite(LED_D6,  data&(1<<6));
  digitalWrite(LED_D5,  data&(1<<5));
  digitalWrite(LED_D4,  data&(1<<4));
  digitalWrite(LED_D3,  data&(1<<3));
  digitalWrite(LED_D2,  data&(1<<2));
  digitalWrite(LED_D1,  data&(1<<1));
  digitalWrite(LED_D0,  data&(1<<0));
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


