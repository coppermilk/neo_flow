#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <unity.h>

void foo(){
    TEST_ASSERT_EQUAL(true, true);
}



void setup() {
   delay(2000);
    UNITY_BEGIN();

    RUN_TEST(foo);
}

void loop() {
   
}