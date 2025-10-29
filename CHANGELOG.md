## [v1.1.0] - 2025-10-28
### Added
- Configurable gamma correction:
  - `FlameFlickerSimulator` constructor now accepts an optional `gamma` value.
  - Generates an internal 256-entry gamma LUT at runtime for per-instance color correction.
  - Default gamma remains `2.2`, matching previous visual output.

### Changed
- Replaced static Adafruit `gamma8()` calls with internal table lookup.
- Enables user control over LED brightness curve to suit different LED models.

### Notes
- Slight increase in flash and RAM footprint due to `powf()` usage and LUT storage.
- Backward compatible with prior API.

## [v1.0.0] - 2025-10-28
### Notes
- Initial Release
