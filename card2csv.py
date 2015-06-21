# card2csv.py  21/06/2015  D.J.Whale
#
# (c) 2015 D.J.Whale
#
# Read a card and write it to a csv file compatible with the maze builder
# program in Adventures in Minecraft.

import time

# Use this to force test harness that returns a test card every second
#from cardreader import tester as cardreader

# Use this to force using the arduino card reader
#from cardreader import arduino as cardreader

# Use this to use whatever cardreader/__init__.py sets as default
import cardreader


CARD_FILENAME = "card.csv"


def printCard(card):
  for row in card:
    print(row)


def writeCardToFile(card, filename):
  f = open(filename, "w")
  rowcount = 0

  for row in card:
    line = ""
    for col in row:
      if len(line) > 0:
        line += ','
      if col == 'X':
        line += '1'
      else:
        line += '0'
    f.write(line)
    f.write("\n")
    rowcount += 1
    if rowcount > 7:
      break # ignore mirror test data at end of card buffer
  f.close()


while True:
  time.sleep(1)

  if cardreader.isReady():
    card = cardreader.read()
    printCard(card)
    writeCardToFile(card, CARD_FILENAME)


# END
