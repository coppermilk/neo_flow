
#include <Arduino.h>
#include <unity.h>

#include "Pixel_test.hpp"

void foo(){
    TEST_ASSERT_EQUAL(true, true);
}

void loop(){

}
void setup(){
        // NOTE!!! Wait for >2 secs
    // if board doesn't support software reset via Serial.DTR/RTS
    delay(2000);
  UNITY_BEGIN();
RUN_TEST(foo);
RUN_TEST(foo2);
    UNITY_END();
}