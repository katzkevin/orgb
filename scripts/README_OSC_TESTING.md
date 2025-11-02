# OSC Threading Test Setup

## Quick Start

1. Start your orgb application

2. In another terminal, run the test sender:
```bash
# Light load (50 msg/s)
uv run scripts/test_osc_sender.py --load light

# Medium load (100 msg/s) - default
uv run scripts/test_osc_sender.py --load medium

# Heavy load (300 msg/s) - stress test
uv run scripts/test_osc_sender.py --load heavy

# Custom rate
uv run scripts/test_osc_sender.py --rate 200 --duration 60
```

## What to Look For

**Before Threading:**
- Watch for `[warning] warnOnSlow: OSC IO running slowly` messages
- Note the actual time vs target time (should see 20-30ms vs 4.16ms)
- Frame rate may drop under heavy load

**After Threading:**
- Warnings should disappear or be much less frequent
- Actual processing time should be well under 4.16ms
- Frame rate should remain stable at 60fps

## Alternative: Use oscsend (if you have liblo)

```bash
# Install on macOS
brew install liblo

# Send test messages
while true; do
  oscsend localhost 19811 /midi-guitar iii 60 80 1
  sleep 0.01
done
```

## Port Configuration

Default port: 19811 (matches OSC_PORT in src/ofApp.h:66)
