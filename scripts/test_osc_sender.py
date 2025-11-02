#!/usr/bin/env python3
"""
Test OSC sender - simulates guitar/mic input for testing OSC threading

Usage: uv run scripts/test_osc_sender.py [--load light|medium|heavy]

# /// script
# dependencies = ["python-osc==1.8.3"]
# ///
"""

import argparse
import time
import random
from pythonosc import udp_client

def send_midi_guitar_messages(client, message_id):
    """Simulate MIDI guitar messages with multiple pitch/velocity pairs"""
    # Send 2-6 notes at once (like a guitar chord)
    num_notes = random.randint(2, 6)
    args = []
    for _ in range(num_notes):
        pitch = random.randint(44, 92)  # GUITAR_MIDI_MIN to GUITAR_MIDI_MAX
        velocity = random.randint(0, 127)
        args.extend([pitch, velocity])
    args.append(message_id)  # Last element is message ID

    client.send_message("/midi-guitar", args)

def send_mic_pitch_messages(client, message_id):
    """Simulate microphone pitch detection messages"""
    # Format: timetag, pitch/amplitude pairs, message_id
    args = [0]  # Timetag (simplified)

    # Send 1-3 detected pitches
    num_pitches = random.randint(1, 3)
    for _ in range(num_pitches):
        pitch = float(random.randint(40, 100))
        amplitude = random.uniform(0.0, 1.0)
        args.extend([pitch, amplitude])

    args.append(message_id)
    client.send_message("/pitch/mic0", args)

def send_midi_keyboard_messages(client):
    """Simulate MIDI keyboard note on/off"""
    msg_type = random.choice(["note_on", "note_off"])
    channel = 1
    note = random.randint(21, 108)  # Piano range
    velocity = random.randint(0, 127)

    client.send_message("/midi-keyboard", [msg_type, channel, note, velocity])

def main():
    parser = argparse.ArgumentParser(description='Send test OSC messages')
    parser.add_argument('--host', default='127.0.0.1', help='OSC host (default: 127.0.0.1)')
    parser.add_argument('--port', type=int, default=19811, help='OSC port (default: 19811)')
    parser.add_argument('--rate', type=int, default=100, help='Messages per second (default: 100)')
    parser.add_argument('--duration', type=int, default=30, help='Duration in seconds (default: 30)')
    parser.add_argument('--load', choices=['light', 'medium', 'heavy'], default='medium',
                       help='Load level: light (50/s), medium (100/s), heavy (300/s)')

    args = parser.parse_args()

    # Override rate based on load preset
    if args.load == 'light':
        args.rate = 50
    elif args.load == 'medium':
        args.rate = 100
    elif args.load == 'heavy':
        args.rate = 300

    client = udp_client.SimpleUDPClient(args.host, args.port)

    print(f"Sending OSC messages to {args.host}:{args.port}")
    print(f"Rate: {args.rate} messages/second")
    print(f"Duration: {args.duration} seconds")
    print(f"Total messages: {args.rate * args.duration}")
    print("Press Ctrl+C to stop\n")

    message_id = 0
    interval = 1.0 / args.rate
    start_time = time.time()

    try:
        while time.time() - start_time < args.duration:
            # Mix of message types (weighted toward guitar/mic)
            msg_type = random.choices(
                ['guitar', 'mic', 'keyboard'],
                weights=[0.5, 0.4, 0.1]
            )[0]

            if msg_type == 'guitar':
                send_midi_guitar_messages(client, message_id)
            elif msg_type == 'mic':
                send_mic_pitch_messages(client, message_id)
            else:
                send_midi_keyboard_messages(client)

            message_id += 1

            if message_id % 100 == 0:
                elapsed = time.time() - start_time
                print(f"Sent {message_id} messages ({message_id/elapsed:.1f} msg/s)")

            time.sleep(interval)

    except KeyboardInterrupt:
        print("\nStopped by user")

    elapsed = time.time() - start_time
    print(f"\nSent {message_id} messages in {elapsed:.2f}s ({message_id/elapsed:.1f} msg/s)")

if __name__ == '__main__':
    main()
