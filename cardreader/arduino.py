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

import portscan

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

rec_buffer  = None
line_buffer = ""


def isReady():
  if rec_buffer != None:
    return True
  processSerial()
  if rec_buffer != None:
    return True
  
  return False


def read():
  global rec_buffer
  
  if not isReady():
    return None

  rec = rec_buffer
  rec_buffer = None
  return rec


def readline():
  global line_buffer

  while True:
    data = s.read(1)
    if len(data) == 0:
      return None # no new data has been received
    data = data[0]

    if data == '\n':
      pass # strip newline

    elif data[0] == '\r':
      line = line_buffer
      line_buffer = ""
      print(line)
      return line

    else:
      line_buffer += data


def processSerial():
  global buffer

  # Poll serial to see if there is a line of data waiting
  line = readline()
  if line == None:
    return

  result = getRec(line)
  if result != None:
    # There is a rec, process it
    databuf = result
    rectype = ord(databuf[0])
    payload = databuf[1:]
    print("rectype:" + str(rectype))

    if rectype == REPORT_OK_CARD:
      print("CARD OK")
      buffer = decodeDataBuf(databuf)
    else:
      # Just display other rec types on diagnostics
      print("Unhandled rec:" + str(rectype) + " " + str(databuf))


REPORT_OK_BOOT       = 0
REPORT_OK_CARD       = 1
REPORT_OK_STATE      = 2
REPORT_OK_ROW        = 3
REPORT_OK_ADC        = 4
REPORT_ERR_LENGTH    = 129


def getRec(line):
  # :ttnnnnnn\n
  # i.e. start char is :
  # first two chars are hexascii type
  # next pairs of chars are data
  # line terminated by a newline
  if len(line) < 3:
    return None # Too short, no start/type
  if line[0] != ':':
    return None # Not a start char

  line = line[1:] # strip start char

  # read the type as hexascii, error if not hexascii
  # if boot record, read ascii data and exit

  try:
    data = line.decode('hex') # now binary
  except:
    print("non hex data:" + line)
    return None

  return data # binary buffer


def decodeDataBuf(buf):
  return TEST_DATA1 # TODO



# END
