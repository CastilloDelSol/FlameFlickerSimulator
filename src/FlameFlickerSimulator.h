#ifndef FIRE_SIMULATOR_H
#define FIRE_SIMULATOR_H

#include <Adafruit_NeoPixel.h>
#include <TaskScheduler.h>

/**
 * @class FlameFlickerSimulator
 * @brief A modular fire and flame simulator for RGBW NeoPixel LEDs.
 *
 * The FlameFlickerSimulator class emulates the organic glow of a flickering candle
 * using three parallel tasks:
 *  - flameFlickerTask(): fine per-pixel color and brightness variation (flame core)
 *  - candleSparkleTask(): rare bright sparks simulating glowing embers
 *  - windFlickerTask(): slow global brightness modulation (airflow / breathing)
 *
 * Designed for small LED clusters (e.g. 8–16 pixels) in pumpkin decorations
 * or ambient light installations.
 */
class FlameFlickerSimulator {
public:
    /**
     * @brief Construct a new FlameFlickerSimulator object.
     *
     * @param pin      GPIO pin connected to the LED data line.
     * @param numLeds  Number of LEDs in the strip or cluster.
     *
     * Initializes the Adafruit_NeoPixel strip and scheduler structure.
     * The strip is not started automatically; call begin() to start effects.
     */
    FlameFlickerSimulator(uint8_t pin, uint8_t numLeds);

    /**
     * @brief Initializes the LED strip and starts all flicker tasks.
     *
     * Must be called once in setup(). Configures the NeoPixel driver,
     * seeds the random generator, and registers all behavior tasks
     * (flame flicker, sparkle, and wind modulation).
     */
    void begin();

    /**
     * @brief Runs the scheduler loop.
     *
     * Call this method from your main loop() to continuously execute
     * the flicker, sparkle, and wind modulation tasks.
     */
    void run();

private:

    static void flameFlickerTaskWrapper();
    static void candleSparkleTaskWrapper();
    static void windFlickerTaskWrapper();

    /**
     * @brief Flame Flicker Task — simulates the natural, uneven glow of a flame.
     *
     * Continuously varies each LED’s color and brightness in a warm red–orange range,
     * mimicking the subtle, organic flicker of a real candle.
     * This is the main flame effect that gives the pumpkin its life.
     *
     * Tunable parameters:
     *  - FLICKER_INTERVAL_MS:  update rate (lower = faster flicker)
     *  - FLICKER_VARIATION:    brightness fluctuation range
     *  - BASE_MIN/MAX_BRIGHTNESS: limits for brightness
     *  - HUE_RANGE:            color tone range (red → orange)
     *  - WHITE_ACCENT_DIV:     white accent strength for warmth
     */
    void flameFlickerTask();

    /**
     * @brief Sparkle Task — adds rare bright “ember” flashes to random LEDs.
     *
     * Simulates occasional bursts of brightness (like glowing embers in a candle flame).
     * Runs periodically and randomly picks a pixel to flash in a warm orange tone.
     *
     * Tunable parameters:
     *  - SPARKLE_PROBABILITY:  % chance of a sparkle each frame (higher = more frequent)
     *  - SPARKLE_DELAY_MS:     task update interval in milliseconds
     *  - SPARKLE_COLOR_R/G/B/W: base color for the sparkle (gamma-corrected)
     *
     * Recommended values:
     *  - SPARKLE_PROBABILITY: 5–10 for candle, 15–25 for torch
     *  - SPARKLE_DELAY_MS:    150–300 ms for natural pacing
     */
    void candleSparkleTask();

    /**
     * @brief Wind Flicker Task — simulates airflow and breathing brightness changes.
     *
     * Creates slow, organic fluctuations in the overall candle brightness, 
     * mimicking the effect of gentle air currents or small gusts of wind.
     * This modulation gives the flame its "breathing" life and prevents static intensity.
     *
     * Tunable parameters:
     *  - BRIGHTNESS_MIN/MAX:  overall brightness limits
     *  - WIND_OFFSET:         how close to the limits gust targets can get
     *  - dimSpeed:            how fast brightness approaches the new target
     *  - calmCounter:         how long it can stay dim before a bright correction
     *  - UPDATE_INTERVAL_MS:  how frequently this task runs (higher = smoother)
     */
    void windFlickerTask();


    static FlameFlickerSimulator* _instance;   // static self-pointer used by static task wrappers
    TaskScheduler<3> _tm;             // cooperative scheduler for managing flicker tasks
    Adafruit_NeoPixel _strip;         // NeoPixel LED driver instance
    uint8_t _globalBrightness;        // current global brightness level
    uint8_t _hFlame;                   // task handle: flame flicker effect
    uint8_t _hSparkle;                // task handle: ember sparkle effect
    uint8_t _hWind;               // task handle: wind flicker (breathing) effect
};

#endif // PUMPKIN_LIGHT_H
