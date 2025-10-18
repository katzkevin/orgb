#!/bin/bash
# Simple run script for orgb

# Ensure NDI library is in place
if [ ! -f bin/orgb.app/Contents/MacOS/libndi.4.dylib ]; then
    echo "Copying NDI library to app bundle..."
    cp libndi.4.dylib bin/orgb.app/Contents/MacOS/
fi

# Run the application
echo "Starting orgb..."
./bin/orgb.app/Contents/MacOS/orgb
