AS5311 SSI Interface Arduino Library
=============================

Library for accessing SSI data and error codes from Austrian Microsystems AS5311 12 bit linear position encoder


The AS5311 says it uses "SSI", but this isn't yet quite
https://en.wikipedia.org/wiki/Synchronous_Serial_Interface yet.

---- 

The unit tests run on the AVR hardware. This isn't best-practice, but it works surprisingly well for the small and short-compile time. 
In fact the test failure asserts are really powerful in AUnit - gives you a very
nice indication of where things went wrong.


I was tempted to eschew TDD for this - but it's lucky I did, since I caught a bug, namely that <util/parity.h> only computes the parity on 8 bits.



Forked from https://github.com/smellsofbikes/AS5045_arduino_library