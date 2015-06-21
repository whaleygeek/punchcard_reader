# tester.py  20/06/2015  D.J.Whale
#
# (c) 2015 D.J.Whale
#
# Test harness

import time

# TO use the test harness that returns a test card every second
#from cardreader import tester as cardreader

# Use this to force using the arduino card reader
#from cardreader import arduino as cardreader

# Use this to use whatever cardreader/__init__.py sets as default
import cardreader


while True:
  time.sleep(1)

  if cardreader.isReady():
    card = cardreader.read()
    for row in card:
      line = ""
      for col in row:
        line += col
      print(line)
    print("")

# END
