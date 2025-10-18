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

## testing

got 130+ unit tests covering the core stuff:

```bash
just test
```

see [TESTING.md](TESTING.md) for details.

## tech

- C++17
- openFrameworks 0.12.1
- MQTT (mosquitto)
- OSC
- NDI video output
- tested on macOS + raspberry pi

## license

MIT - do whatever you want

made by [@kevinkatz.io](https://kevinkatz.io)
