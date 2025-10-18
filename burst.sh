#!/usr/bin/env bash

set -e
set -x

mqtp vmini '{"type": "note_on", "note": 51, "time": 0, "velocity": 115, "channel":  0, "ephemeral": true}'
sleep 0.03
mqtp vmini '{"type": "note_on", "note": 51, "time": 0, "velocity": 100, "channel":  0, "ephemeral": true}'
sleep 0.03
mqtp vmini '{"type": "note_on", "note": 51, "time": 0, "velocity": 90, "channel":  0, "ephemeral": true}'

sleep 0.03
mqtp vmini '{"type": "note_on", "note": 51, "time": 0, "velocity": 70, "channel":  0, "ephemeral": true}'

sleep 0.03
mqtp vmini '{"type": "note_on", "note": 51, "time": 0, "velocity": 60, "channel":  0, "ephemeral": true}'

sleep 0.03
mqtp vmini '{"type": "note_on", "note": 51, "time": 0, "velocity": 50, "channel":  0, "ephemeral": true}'

sleep 0.03
mqtp vmini '{"type": "note_on", "note": 51, "time": 0, "velocity": 40, "channel":  0, "ephemeral": true}'

set +e
set +x
