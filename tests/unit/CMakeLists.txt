# Unit Tests - No GL context required
# These tests verify business logic without OpenGL dependencies

cmake_minimum_required(VERSION 3.14)

# Test files
set(UNIT_TEST_FILES
    test_utilities.cpp
    test_press.cpp
    test_keystate.cpp
    test_colorprovider.cpp
    test_flock.cpp
)

# Source files being tested (only non-GL components)
set(UNIT_SRC_FILES
    ../../src/Utilities.cpp
    ../../src/Press.cpp
    ../../src/ColorProvider.cpp
    ../../src/KeyState.cpp
    ../../src/Forms/Flock.cpp
    ../../src/Forms/BaseParticle.cpp
    ../../src/Forms/Particle.cpp
    ../../src/Forms/SizedSprite.cpp
)

# Create unit test executable
add_executable(unit-tests
    ${UNIT_TEST_FILES}
    ${UNIT_SRC_FILES}
)

# Include directories (inherited from parent + local)
target_include_directories(unit-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Forms
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Forms/Particles
)

# Link libraries (minimal - no full GL stack)
target_link_libraries(unit-tests
    PRIVATE
    GTest::gtest_main
    ${OF_ROOT}/libs/openFrameworksCompiled/lib/osx/libopenFrameworksDebug.a
    ${OF_ROOT}/libs/FreeImage/lib/macos/FreeImage.xcframework/macos-arm64_x86_64/FreeImage.a
    ${OF_ROOT}/libs/freetype/lib/macos/freetype.xcframework/macos-arm64_x86_64/libfreetype.a
    ${OF_ROOT}/libs/cairo/lib/macos/cairo.xcframework/macos-arm64_x86_64/libcairo.a
    ${OF_ROOT}/libs/pixman/lib/macos/pixman.xcframework/macos-arm64_x86_64/libpixman-1.a
    ${OF_ROOT}/libs/curl/lib/macos/curl.xcframework/macos-arm64_x86_64/curl.a
    ${OF_ROOT}/libs/uriparser/lib/macos/uriparser.xcframework/macos-arm64_x86_64/uriparser.a
    ${OF_ROOT}/libs/pugixml/lib/macos/pugixml.xcframework/macos-arm64_x86_64/libpugixml.a
    ${OF_ROOT}/libs/tess2/lib/macos/tess2.xcframework/macos-arm64_x86_64/libtess2.a
    ${OF_ROOT}/libs/glew/lib/macos/glew.xcframework/macos-arm64_x86_64/libGLEW.a
    ${OF_ROOT}/libs/libpng/lib/macos/libpng.xcframework/macos-arm64_x86_64/libpng.a
    ${OF_ROOT}/libs/zlib/lib/macos/zlib.xcframework/macos-arm64_x86_64/zlib.a
    ${OF_ROOT}/libs/brotli/lib/macos/brotli.xcframework/macos-arm64_x86_64/brotli.a
    ${OF_ROOT}/libs/fmt/lib/macos/fmt.xcframework/macos-arm64_x86_64/libfmt.a
    ${OF_ROOT}/libs/rtAudio/lib/macos/rtAudio.xcframework/macos-arm64_x86_64/librtaudio.a
    ${OF_ROOT}/libs/glfw/lib/macos/glfw.xcframework/macos-arm64_x86_64/libglfw3.a
    boost_filesystem
    "-framework Cocoa"
    "-framework IOKit"
    "-framework CoreVideo"
    "-framework CoreFoundation"
    "-framework OpenGL"
    "-framework AppKit"
    "-framework AVFoundation"
    "-framework CoreMedia"
    "-framework CoreAudio"
    "-framework AudioToolbox"
    "-framework Security"
    "-framework SystemConfiguration"
    crypto
    ssl
)

# Discover unit tests
include(GoogleTest)
gtest_discover_tests(unit-tests
    PROPERTIES
    LABELS "unit"
)
