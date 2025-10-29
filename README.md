# FlameFlickerSimulator

**FlameFlickerSimulator** is an Arduino-compatible library that simulates a realistic candle or flame flicker effect using **Adafruit NeoPixel (SK6812 / WS2812 RGBW)** LEDs and a cooperative **TaskScheduler**.

It combines three concurrent effects:
- 🕯️ **Flame Flicker:** Warm, uneven red–orange glow for each LED
- 💨 **Wind Flicker:** Slow breathing-like brightness modulation
- ✨ **Ember Sparkle:** Rare, brief bright flashes

Perfect for pumpkins, lanterns, candles, or cozy ambient lighting.

---

## 🔧 Hardware Requirements

- LED Type: SK6812 RGBW (or compatible NeoPixel)
- Microcontroller: Arduino Nano / Uno / ESP32 / etc.
- Power Supply: 5V regulated (with enough current for your LEDs)

---

## ⚙️ Installation

### PlatformIO
Add to your `platformio.ini`:
```ini
lib_deps = 
    https://github.com/CastilloDelSol/FlameFlickerSimulator.git
    https://github.com/CastilloDelSol/TaskScheduler.git
```

### Arduino IDE
1. Download as ZIP from GitHub
2. Open Arduino IDE → Sketch → Include Library → Add .ZIP Library...
3. Install **Adafruit NeoPixel** and **TaskScheduler** manually if prompted

---

## 💡 Example

Basic example for SK6812 RGBW strip with 8 LEDs on pin A0:

```cpp
#include <Arduino.h>
#include "FlameFlickerSimulator.h"

#define DATA_PIN A0
#define NUM_LEDS 8

FlameFlickerSimulator pumpkin(DATA_PIN, NUM_LEDS);

void setup() { pumpkin.begin(); }
void loop()  { pumpkin.run();   }
```

This creates a warm, dynamic, candle-like flicker effect.

---

## 🧩 Dependencies
- [Adafruit NeoPixel](https://github.com/adafruit/Adafruit_NeoPixel)
- [TaskScheduler (CastilloDelSol)](https://github.com/CastilloDelSol/TaskScheduler)

---

## 🧠 Author
Created with 🎃 and ❤️ by **CastilloDelSol**
