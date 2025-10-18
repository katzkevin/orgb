# Raspberry Pi
// At advice of https://github.com/openframeworks/openFrameworks/issues/5056#issue-148234615
    settings.glesVersion = 2;

    if ofAppRunnner.cpp

# ofxNDI

ofxNDI depends on some custom project settings.

https://github.com/2bbb/ofxNDI/blob/48616bf6a51dd1abfe2a30608390b95dc97e5132/readme.md

Build Phases > Copy Files
select "Destination" to Executables
click +
add /Users/katz/workspace/of_v0.11.2_osx_release/addons/ofxNDI/libs/NDI/lib/osx/x64/libndi.4.dylib

Build Settings > Linking > Runpath Search Paths
Delete the line that's there and replace it with set value to @executable_path

# ofxMQTT

Get addon, rename to ofxMQTT
Reclone eclipse/mosquitto
brew install openssl

Add the following when it errors
#if defined(__APPLE__)
#include <mach/mach_time.h>
#endif

cmake . -DOPENSSL_ROOT_DIR=/opt/homebrew/Cellar/openssl@1.1/1.1.1k -DOPENSSL_LIBRARIES=/opt/homebrew/Cellar/openssl@1.1/1.1.1k/lib
cmake .


# How to profile
Add `-pg` to the flags in config.cmake. It doesn't get added to the build piex/orgb step, so manually run exactly what Make prints to screen, with pg added

Invoke like this, itll generate a gmon.out
USER=pi HOME=/home/pi MOSQUITTO_HOST=magenta MOSQUITTO_USER=gomezchamberlain MOSQUITTO_PASSWORD="blah" MOSQUITTO_CLIENT_ID="abcdef-orgb" OFW_WIDTH=32 OFW_HEIGHT=32 HARDWARE_MAPPING="adafruit-hat" LED_COLUMNS=32 LED_ROWS=32 LED_CHAIN=1 LED_PARALLEL=1 LED_BRIGHTNESS=100 IDLE_LED_BRIGHTNESS=10 GPIO_SLOWDOWN=2 PIXEL_MAPPER_CONFIG="Rotate:270" LED_RGB_SEQUENCE=RGB OFW_LOG_LEVEL=OF_LOG_VERBOSE EXIT_AFTER_FRAMES=600 sudo -E xvfb-run -a -s "-screen 0 32x32x24" /home/pi/openFrameworks/apps/myApps/piex/bin/piex_debug

Then call `gprof bin/piex gmon.out > profile.monday.12-27_21_18`
