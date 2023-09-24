AS5311 SSI Interface Arduino Library
=============================

Library for accessing SSI data and error codes from Austrian Microsystems AS5311 12 bit linear position encoder


The AS5311 says it uses "SSI", but this isn't yet quite the same as 
the standard that encoders typically used https://en.wikipedia.org/wiki/Synchronous_Serial_Interface 


https://electronics.stackexchange.com/questions/348885/use-rs232-rs422-converter-with-encoder

https://www.sra-measurement.com/high-accuracy-magnetic-linear-tape

Pole pitch for this tape is 2mm

The metal backing on the tape is very hard to cut.

the width of the tape interferes with the mounting screws. The screws need to be flat head 2 mm 

---- 

The unit tests run on the AVR hardware. This isn't best-practice, but it works surprisingly well for the small and short-compile time. 
In fact the test failure asserts are really powerful in AUnit - gives you a very
nice indication of where things went wrong.


I was tempted to eschew TDD for this - but it's lucky I did, since I caught a bug, namely that <util/parity.h> only computes the parity on 8 bits.



Forked from https://github.com/smellsofbikes/AS5045_arduino_library