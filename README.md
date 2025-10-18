# orgb

audio-reactive visualizations for piano, guitar, whatever

plays MIDI over MQTT/OSC and makes pretty things happen in real-time. made for live performances, generative art, or just messing around.

## what it does

- listens to MIDI input (piano/guitar/controllers)
- generates real-time visuals based on notes, velocity, sustain
- different visual modes: particles, waves, lightning, shapes, etc
- color palettes that map to pitch/mood
- outputs via NDI for streaming or projection

runs on macOS and raspberry pi.

## quick start

```bash
# install deps
brew install boost

# clone
git clone https://github.com/kevcmk/orgb.git
cd orgb

# build (needs openFrameworks 0.12.1)
make

# run
./bin/orgb
```

you'll need [openFrameworks](https://openframeworks.cc/) installed.

## config

copy `.env` and fill in your MQTT creds:

```bash
MOSQUITTO_HOST=your-mqtt-broker
MOSQUITTO_USER=username
MOSQUITTO_PASSWORD=password
MOSQUITTO_CLIENT_ID=orgb-client
```

works without MQTT too, just uses OSC instead.

## shaders

production-ready post-processing shader system:

```bash
# validate all shaders
just shader-validate

# run shader tests
just shader-test

# check everything (validation + tests)
just shader-check
```

includes film grain and scanlines effects. see [SHADERS.md](SHADERS.md) for usage.

## testing

got 130+ unit tests covering the core stuff:

```bash
# run C++ unit tests
just test

# run shader tests
just shader-test

# run everything
just test && just shader-check
```

see [TESTING.md](TESTING.md) for details.

## ci/cd

automated validation on every push:

- ✅ shader compilation checks
- ✅ unit tests (C++ + shaders)
- ✅ code quality checks

workflows run in parallel for speed. see [.github/workflows/](.github/workflows/) for config.

## tech

- C++17
- openFrameworks 0.12.1
- GLSL shaders (GL2/GL3/ES2)
- MQTT (mosquitto)
- OSC
- NDI video output
- tested on macOS + raspberry pi

## license

MIT - do whatever you want

made by [@kevinkatz.io](https://kevinkatz.io)
