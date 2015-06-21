# arduino.py  20/06/2015  D.J.Whale
#
# (c) 2015 D.J.Whale
#
# Read card records from an arduino card reader.

#----- CONFIGURATION -----------------------------------------------------------

DEBUG                 = False
USE_EMBEDDED_PYSERIAL = True
BAUD                  = 115200

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

def isReady():
  #TODO if there is a full record in the buffer, return True
  #else see if any serial data waiting to be processed
  #if there is, process it until none left, or something to return
  #if there is a ful record in the buffer, return True
  #else return False
  return True


def read():
  #if not ready, return None
  #else
  #  retrieve one record from the internal buffer, delete it
  #  return that record
  return TEST_DATA1



# END
