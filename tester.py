# tester.py  20/06/2015  D.J.Whale
#
# (c) 2015 D.J.Whale
#
# Test harness

import time
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
