cmake_minimum_required(VERSION 3.14)
project(orgb_tests)

# Set C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find openFrameworks
set(OF_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../of_v0.12.1_osx_release" CACHE PATH "openFrameworks root directory")

# Include GoogleTest
include(FetchContent)
FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG release-1.12.1
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

enable_testing()

# Include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Forms
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Forms/Particles
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Forms/Waves
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Forms/Shapes
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Forms/Lightning
    ${CMAKE_CURRENT_SOURCE_DIR}/../src/Forms/Glow
    ${OF_ROOT}/libs/openFrameworks
    ${OF_ROOT}/libs/openFrameworks/graphics
    ${OF_ROOT}/libs/openFrameworks/app
    ${OF_ROOT}/libs/openFrameworks/sound
    ${OF_ROOT}/libs/openFrameworks/video
    ${OF_ROOT}/libs/openFrameworks/events
    ${OF_ROOT}/libs/openFrameworks/communication
    ${OF_ROOT}/libs/openFrameworks/math
    ${OF_ROOT}/libs/openFrameworks/types
    ${OF_ROOT}/libs/openFrameworks/utils
    ${OF_ROOT}/libs/openFrameworks/gl
    ${OF_ROOT}/libs/openFrameworks/3d
    ${OF_ROOT}/libs/glew/include
    ${OF_ROOT}/libs/glm/include
    ${OF_ROOT}/libs/utf8/include
    ${OF_ROOT}/libs/json/include
    ${OF_ROOT}/libs/pugixml/include
    ${OF_ROOT}/libs/freeimage/include
    ${OF_ROOT}/libs/freetype/include
    ${OF_ROOT}/libs/freetype/include/freetype2
    ${OF_ROOT}/libs/glfw/include
    ${OF_ROOT}/libs/tess2/include
    ${OF_ROOT}/libs/poco/include
    ${OF_ROOT}/libs/fmod/include
    ${OF_ROOT}/libs/rtAudio/include
    ${OF_ROOT}/libs/curl/include
    ${OF_ROOT}/libs/cairo/include
    ${OF_ROOT}/libs/cairo/include/cairo
    ${OF_ROOT}/libs/uriparser/include
    /opt/homebrew/opt/boost/include
)

# Link directories
link_directories(
    ${OF_ROOT}/libs/openFrameworksCompiled/lib/osx
    ${OF_ROOT}/libs/freeimage/lib/macos/FreeImage.xcframework/macos-arm64_x86_64
    ${OF_ROOT}/libs/freetype/lib/macos/freetype.xcframework/macos-arm64_x86_64
    ${OF_ROOT}/libs/cairo/lib/macos/cairo.xcframework/macos-arm64_x86_64
    ${OF_ROOT}/libs/pixman/lib/macos/pixman.xcframework/macos-arm64_x86_64
    ${OF_ROOT}/libs/curl/lib/macos/curl.xcframework/macos-arm64_x86_64
    ${OF_ROOT}/libs/uriparser/lib/macos/uriparser.xcframework/macos-arm64_x86_64
    ${OF_ROOT}/libs/pugixml/lib/macos/pugixml.xcframework/macos-arm64_x86_64
    ${OF_ROOT}/libs/tess2/lib/macos/tess2.xcframework/macos-arm64_x86_64
    ${OF_ROOT}/libs/glew/lib/macos/glew.xcframework/macos-arm64_x86_64
    ${OF_ROOT}/libs/libpng/lib/macos/libpng.xcframework/macos-arm64_x86_64
    ${OF_ROOT}/libs/zlib/lib/macos/zlib.xcframework/macos-arm64_x86_64
    ${OF_ROOT}/libs/brotli/lib/macos/brotli.xcframework/macos-arm64_x86_64
    ${OF_ROOT}/libs/fmt/lib/macos/fmt.xcframework/macos-arm64_x86_64
    ${OF_ROOT}/libs/rtAudio/lib/macos/rtAudio.xcframework/macos-arm64_x86_64
    /opt/homebrew/opt/boost/lib
    /opt/homebrew/opt/openssl/lib
)

# Source files to test
set(SRC_FILES
    ../src/Utilities.cpp
    ../src/Press.cpp
    ../src/ColorProvider.cpp
    ../src/KeyState.cpp
)

# Test files
set(TEST_FILES
    test_utilities.cpp
    test_press.cpp
    test_keystate.cpp
    test_colorprovider.cpp
)

# Create test executable
add_executable(
    orgb_tests
    ${TEST_FILES}
    ${SRC_FILES}
)

# Link libraries
target_link_libraries(
    orgb_tests
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

# Discover tests
include(GoogleTest)
gtest_discover_tests(orgb_tests)
