#include <Arduino.h>
#include "FlameFlickerSimulator.h"

#define DATA_PIN A0
#define NUM_LEDS 8

FlameFlickerSimulator pumpkin(DATA_PIN, NUM_LEDS);

void setup()
{ 
    pumpkin.begin();
}

void loop()
{
    pumpkin.run();

}
