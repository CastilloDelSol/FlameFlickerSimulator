#include "FlameFlickerSimulator.h"

FlameFlickerSimulator* FlameFlickerSimulator::_instance = nullptr;

FlameFlickerSimulator::FlameFlickerSimulator(uint8_t pin, uint8_t numLeds)
: _strip(numLeds, pin, NEO_GRBW + NEO_KHZ800),
  _globalBrightness(90),
  _hFlame(IScheduler::INVALID_TASK_ID),
  _hSparkle(IScheduler::INVALID_TASK_ID),
  _hWind(IScheduler::INVALID_TASK_ID){}

void FlameFlickerSimulator::begin()
{
    _instance = this;

    _strip.begin();
    _strip.setBrightness(_globalBrightness);
    _strip.show();
    randomSeed(analogRead(0));

    _hFlame   = _tm.xTaskCreate(flameFlickerTaskWrapper, 0, 1);
    _hSparkle = _tm.xTaskCreate(candleSparkleTaskWrapper, 0, 2);
    _hWind    = _tm.xTaskCreate(windFlickerTaskWrapper,   0, 3);
}

void FlameFlickerSimulator::run() { _tm.run(); }

void FlameFlickerSimulator::flameFlickerTaskWrapper() { _instance->flameFlickerTask(); }
void FlameFlickerSimulator::candleSparkleTaskWrapper() { _instance->candleSparkleTask(); }
void FlameFlickerSimulator::windFlickerTaskWrapper() { _instance->windFlickerTask(); }


void FlameFlickerSimulator::flameFlickerTask()
{
  // === Effect parameters ===
  static const uint16_t FLICKER_INTERVAL_MS = 40;   // update rate (~25 Hz)
  static const int16_t  FLICKER_VARIATION   = 25;   // brightness variation ± range
  static const uint8_t  BASE_MIN_BRIGHTNESS = 30;   // darkest point
  static const uint8_t  BASE_MAX_BRIGHTNESS = 200;  // brightest point
  static const uint8_t  HUE_RANGE           = 18;   // hue span from red → orange
  static const uint8_t  WHITE_ACCENT_DIV    = 20;   // divide brightness by this for white accent

  static uint8_t prevBrightness = _globalBrightness;

  // Process each LED independently for a natural uneven look
  for (uint8_t i = 0; i < _strip.numPixels(); i++)
  {
    // Random warm hue between red and orange
    uint8_t hue = random(0, HUE_RANGE);

    // Smooth flicker brightness variation
    int16_t targetBrightness = _globalBrightness + random(-FLICKER_VARIATION, FLICKER_VARIATION);
    targetBrightness = constrain(targetBrightness, BASE_MIN_BRIGHTNESS, BASE_MAX_BRIGHTNESS);
    uint8_t brightness = (prevBrightness * 3 + targetBrightness) / 4; // low-pass filter (smooth transitions)
    prevBrightness = brightness;

    // Approximate HSV → RGB for red/orange range
    uint8_t red, green, blue;

    if (hue < 10)
    {
        red   = brightness;
        green = ((uint16_t)hue * brightness) / 20;
        blue  = 0;
    }
    else
    {
        red   = brightness;
        green = ((uint16_t)(hue - 10) * brightness) / 15;
        blue  = 0;
    }

    // Small white accent adds subtle warmth
    uint8_t white = brightness / WHITE_ACCENT_DIV;

    // Apply gamma correction for smoother perceived brightness
    red   = _strip.gamma8(red);
    green = _strip.gamma8(green);
    blue  = _strip.gamma8(blue);
    white = _strip.gamma8(white);

    // Write color to LED buffer
    _strip.setPixelColor(i, _strip.Color(red, green, blue, white));
  }

  // Update all LEDs at once
  _strip.show();

  // Wait before next flicker update
  _tm.vTaskDelayUntil(FLICKER_INTERVAL_MS);
}

void FlameFlickerSimulator::candleSparkleTask()
{
  // === Effect parameters ===
  const uint8_t  SPARKLE_PROBABILITY = 50;     // % chance each frame to trigger a sparkle (higher = more frequent)
  const uint16_t SPARKLE_DELAY_MS    = 180;   // how often to check (ms)
  const uint8_t  SPARKLE_COLOR_R     = 255;   // red component (0–255)
  const uint8_t  SPARKLE_COLOR_G     = 120;   // green component — controls warmth
  const uint8_t  SPARKLE_COLOR_B     = 0;     // blue component (usually 0 for warm tones)
  const uint8_t  SPARKLE_COLOR_W     = 50;    // white LED accent (0–255)

  // Only trigger a sparkle randomly with the given probability.
  if (random(0, 100) < SPARKLE_PROBABILITY)
  {
    // Pick a random LED index
    int idx = random(_strip.numPixels());

    // Gamma-corrected sparkle color (so brightness looks natural)
    uint8_t r = _strip.gamma8(SPARKLE_COLOR_R);
    uint8_t g = _strip.gamma8(SPARKLE_COLOR_G);
    uint8_t b = _strip.gamma8(SPARKLE_COLOR_B);
    uint8_t w = _strip.gamma8(SPARKLE_COLOR_W);

    // Apply the sparkle to that LED
    _strip.setPixelColor(idx, _strip.Color(r, g, b, w));

    // Show the new frame (sparkle will fade naturally as flicker task overwrites it)
    _strip.show();
  }

  // Wait before the next sparkle check
  _tm.vTaskDelayUntil(SPARKLE_DELAY_MS);
}

void FlameFlickerSimulator::windFlickerTask()
{
  // === Effect parameters ===
  static const uint8_t  BRIGHTNESS_MIN     = 110;   // lowest safe glow
  static const uint8_t  BRIGHTNESS_MAX     = 240;   // upper limit to prevent washout
  static const uint8_t  WIND_OFFSET        = 10;    // margin from min/max for wind target range
  static const uint16_t UPDATE_INTERVAL_MS = 3;     // fast update rate (~200 Hz)

  // Internal states
  static int16_t windTarget  = 90;   // next brightness target
  static int8_t  dimSpeed    = 1;    // speed factor for transitions
  static uint8_t counter     = 0;    // counter for random timing
  static uint8_t calmCounter = 0;    // prevents it from staying too dim

  // Compute effective wind target range
  const uint8_t windLow  = BRIGHTNESS_MIN + WIND_OFFSET;
  const uint8_t windHigh = BRIGHTNESS_MAX - WIND_OFFSET;

  // Occasionally pick a new target (gust or calm)
  if (++counter > random(40, 120))
  {
    counter = 0;
    // Wind gust can brighten or dim randomly within safe range
    windTarget = random(windLow, windHigh);
    dimSpeed   = random(1, 4);  // 1 = calm, 3 = gusty
  }

  // Smoothly approach the target with inertia and speed influence
  int16_t delta = windTarget - _globalBrightness;
  _globalBrightness += delta / (10 - (dimSpeed * 2)); // smaller divisor = faster adjustment

  // Add subtle noise for organic life
  _globalBrightness = constrain(_globalBrightness + random(-1, 2), BRIGHTNESS_MIN, BRIGHTNESS_MAX);

  // If it's been dim for too long, gently bias upward
  if (_globalBrightness < (BRIGHTNESS_MIN + 20))
  {
      if (++calmCounter > 80)
      {
          windTarget = random((BRIGHTNESS_MAX - 60), BRIGHTNESS_MAX);
          calmCounter = 0;
      }
  }
  else if (calmCounter > 0)
  {
      calmCounter--; // slowly relax
  }

  // Apply new brightness directly (no gamma — preserves natural movement)
  _strip.setBrightness(_globalBrightness);

  // Wait before next update
  _tm.vTaskDelayUntil(UPDATE_INTERVAL_MS);
}
