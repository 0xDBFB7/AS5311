#include <AUnit.h>
using namespace aunit;

#include "AS5311.h"

AS5311 myAS5311(2,3,4,5); // data, clock, chip select, index


test(parity) { 
    uint32_t message;

    	
    message = 0b00000000;
    assertEqual(myAS5311.parity_check(message), true);

    message = 0b10100011;
    assertEqual(myAS5311.parity_check(message), true);

    message = 0b10100010;
    assertEqual(myAS5311.parity_check(message), false);
    
    message = 0b00110000000000000;
    assertEqual(myAS5311.parity_check(message), true);

    message = 0b00111000000000001;
    assertEqual(myAS5311.parity_check(message), true);

    message = 0b000000000000100001;
    assertEqual(myAS5311.parity_check(message), true);

    message = 0b000000000000100000;
    assertEqual(myAS5311.parity_check(message), false);
    
    message = 0b010000000000100000;
    assertEqual(myAS5311.parity_check(message), true);
     
    message = 0b100000000100100001;
    assertEqual(myAS5311.parity_check(message), true);

    message = 0b100000000000100000;
    assertEqual(myAS5311.parity_check(message), true);

}

// test(ExampleTest, example) {
//   ...
// }

// testing(looping) {
//   ...code...
//   if (...) {
//     pass();
//   } else if (...) {
//     failTestNow();
//   } else {
//     skipTestNow();
//   }
// }

// testing(LoopingTest, looping) {
//   ...
// }

// class CustomTestOnce: public TestOnce {
//   protected:
//     // optional
//     void setup() override {
//       TestOnce::setup();
//       ...setup code...
//     }

//     // optional
//     void teardown() override {
//       ...teardown code...
//       TestOnce::teardown();
//     }

//     void assertBigStuff() {
//       ...higher level assertions...
//     }
// };

// testF(CustomTestOnce, example_test) {
//   ...
//   assertBigStuff();
//   ...
// }

// class CustomTestAgain: public TestAgain {
//   protected:
//     // optional
//     void setup() override {
//       TestAgain::setup();
//       ...setup code...
//     }

//     // optional
//     void teardown() override {
//       ...teardown code...
//       TestAgain::teardown();
//     }

//     void assertBigStuff() {
//       ...various assertions...
//     }
// };

// testingF(CustomTestAgain, example_test) {
//   ...
//   assertBigStuff();
//   ...
// }

void setup() {
  Serial.begin(115200);
  while (! Serial); // Wait until Serial is ready - Leonardo/Micro

    //   TestRunner::exclude("*");
  TestRunner::include("*");
  //TestRunner::include("CustomTestAgain", "example*");
}

void loop() {
  TestRunner::run();
}