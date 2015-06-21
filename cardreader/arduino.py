# arduino.py  20/06/2015  D.J.Whale
#
# (c) 2015 D.J.Whale
#
# Read card records from an arduino card reader.

#----- CONFIGURATION -----------------------------------------------------------

DEBUG                 = False
USE_EMBEDDED_PYSERIAL = True
BAUD                  = 115200
ENABLE_ERROR_CORRECT  = False

if USE_EMBEDDED_PYSERIAL:
  from os import sys, path
  thisdir = path.dirname(path.abspath(__file__))
  sys.path.append(thisdir)
  
import serial


#----- PORTSCAN ----------------------------------------------------------------

name = portscan.getName()
if name != None:
  if DEBUG:
    print("Using port:" + name)
  PORT = name
else:
  name = portscan.find()
  if name == None:
    raise ValueError("No port selected, giving in")
  PORT = name
  print("Your cardreader board has been detected")
  print("Now running your program...")


#----- CONFIGURE SERIAL PORT ---------------------------------------------------

s = serial.Serial(PORT)
s.baudrate = BAUD
s.parity   = serial.PARITY_NONE
s.databits = serial.EIGHTBITS
s.stopbits = serial.STOPBITS_ONE

s.close()
s.port = PORT
s.open()


TEST_DATA1 = [
  "..XXX...",
  ".XX.XX..",
  "XXXXXXX.",
  "....XXX.",
  "XXXXXXX.",
  ".XXXXX..",
  "..XXX...",
  "........"
]

buffer = None


def isReady():
  if buffer != None:
    return True
  processSerial()
  if buffer != None:
    return True
  
  return False


def read():
  global buffer
  
  if not isReady():
    return None

  rec = buffer
  buffer = None
  return buffer


REPORT_OK_BOOT       0
REPORT_OK_CARD       1
REPORT_OK_STATE      2
REPORT_OK_ROW        3
REPORT_ERR_LENGTH    129


def processSerial():
  global buffer
  buffer = TEST_DATA1 # Testing

  # Poll serial to see if there is a line of data waiting
  line = None
  #line = s.read(term='\n', timeout=0)
  if line == None:
    return

  result = getRec(line)
  if result != None:
    # There is a rec, process it
    rectype, databuf = result
    if rectype == REPORT_OK_CARD:
      buffer = decodeDataBuf(databuf)
    else:
      # Just display other rec types on diagnostics
      print(str(rectype) + " " + str(databuf))



def getRec(line):
  # :ttnnnnnn\n
  # i.e. start char is :
  # first two chars are hexascii type
  # next pairs of chars are data
  # line terminated by a newline
  if len(line) < 3:
    return None # Too short, no start/type
  if line[0] != ':'"
    return None # Not a start char

  # read the type as hexascii, error if not hexascii
  # if boot record, read ascii data and exit

  if (len(line)-1) % 2 != 0:
    return None # non-even number of data chars

  # read in runs of hexascii and decode into a byte buffer until EOL
  # error if any are not hexascii

  # return (recType, databuffer)

  return None # TODO


# END
