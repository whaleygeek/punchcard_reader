Arduino/Raspberry Pi Punchcard reader
====

Here are the code files for the Minecraft Punchcard Reader.

This is a small box that you feed hand-punched pieces of card into
with patterns punched on them, and the program will build those patterns
inside Minecraft for you!

The Python code runs on Raspberry Pi, PC or Mac.

For a good write-up of what this project does and some really nice
photos of it in use, please see Gemma Latham's blog post here:

[Gemma's blog] (http://gemmamaylatham.co.uk/post/123551650183/patterncraft-launched-at-liverpool-makefest-june)

For those of you following along with this project live, you can build
your own PunchCard reader from the materials on this website and by
looking at the photos that Gemma has posted on her blog. We hope to write
the build instructions up in detail over summer.

Inside this github repo, you will find the arduino source code for an
Arduino Pro Micro. I buy these from here:

[Sparkfun ProMicro] (http://skpang.co.uk/catalog/pro-micro-33v8mhz-with-headers-and-anyio-firmware-p-1327.html)

Inside the box I used two pieces of veroboard cut and bolted back to back
with the copper facing outwards, drilled 9 holes to line up with the
holes in the punchcard, and soldered IR transmitter LEDs on one side of
the board, and IR filtered photo transistors on the other. The LED's
and photo transistors point through the holes in the board. The Arduino
Pro-Micro then uses it's 9 analog inputs to read the voltage from each
of the Photo Transistors, and work out where the holes are in the card.

I bought these parts, 9 of each, and used 1K resistors in line with
each of them to limit the current that flows through them to an acceptable
value.

IR Transmitter LED:

[OSRAM SFH484-2] (http://uk.farnell.com/webapp/wcs/stores/servlet/ProductDisplay?catalogId=15001&langId=44&urlRequestType=Base&partNumber=1212737&storeId=10151)

IR receiver Phototransistor:

[Fairchild QSD123] (http://uk.farnell.com/webapp/wcs/stores/servlet/ProductDisplay?catalogId=15001&langId=44&urlRequestType=Base&partNumber=2453252&storeId=10151)

When the card is fed in and removed, a small string of hex numbers
is sent via a serial port using PySerial at the Python end to read
these numbers. They are then decoded into CSV data and written to a text
file.

Gemma then modified Adventure 6 from the book Adventures in Minecraft
to read that file and build shapes facing upwards inside Minecraft.

I based most of the python code on the anyio library that I wrote for
the electronics chapter in the book: Adventures in Minecraft. The firmware
in the ProMicro is loaded with the firmware of this project, but it
basically works using the same techniques.

David Whale
@whaleygeek
June 2015

