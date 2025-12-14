# Integration Tests - Requires GL context
# These tests verify rendering, shaders, and GL-dependent functionality

cmake_minimum_required(VERSION 3.14)

# Test files
set(INTEGRATION_TEST_FILES
    test_forms_rendering.cpp
    test_shader_compilation.cpp
    test_postprocessing.cpp
    fixtures/GLTestFixture.cpp
)

# Source files being tested (includes GL-dependent components)
set(INTEGRATION_SRC_FILES
    # Core systems
    ../../src/Utilities.cpp
    ../../src/Press.cpp
    ../../src/ColorProvider.cpp
    ../../src/KeyState.cpp
    ../../src/DrawManager.cpp
    ../../src/DrawContext.cpp
    ../../src/ColorUtilities.cpp

    # Shader pipeline
    ../../src/ShaderPipeline.cpp
    ../../src/ShaderEffect.cpp

    # ofxGui addon sources
    ${OF_ROOT}/addons/ofxGui/src/ofxBaseGui.cpp
    ${OF_ROOT}/addons/ofxGui/src/ofxButton.cpp
    ${OF_ROOT}/addons/ofxGui/src/ofxColorPicker.cpp
    ${OF_ROOT}/addons/ofxGui/src/ofxGuiGroup.cpp
    ${OF_ROOT}/addons/ofxGui/src/ofxInputField.cpp
    ${OF_ROOT}/addons/ofxGui/src/ofxLabel.cpp
    ${OF_ROOT}/addons/ofxGui/src/ofxPanel.cpp
    ${OF_ROOT}/addons/ofxGui/src/ofxSlider.cpp
    ${OF_ROOT}/addons/ofxGui/src/ofxSliderGroup.cpp
    ${OF_ROOT}/addons/ofxGui/src/ofxToggle.cpp

    # Effects
    ../../src/Effects/FilmGrainEffect.cpp
    ../../src/Effects/ScanlinesEffect.cpp
    ../../src/Effects/ChromaticAberrationEffect.cpp
    ../../src/Effects/DisplacementEffect.cpp
    ../../src/Effects/VHSGlitchEffect.cpp
    ../../src/Effects/FeedbackEffect.cpp

    # Forms - Base
    ../../src/Forms/VisualForm.cpp
    ../../src/Forms/BaseParticle.cpp
    ../../src/Forms/Particle.cpp
    ../../src/Forms/SizedSprite.cpp
    ../../src/Forms/Flock.cpp

    # Forms - Particles
    ../../src/Forms/Particles/BaseParticles.cpp
    ../../src/Forms/Particles/RadialParticles.cpp
    ../../src/Forms/Particles/EdgeParticles.cpp
    ../../src/Forms/Particles/GravityParticles.cpp
    ../../src/Forms/Particles/RandomParticles.cpp

    # Forms - Shapes
    ../../src/Forms/Shapes/Shape.cpp
    ../../src/Forms/Shapes/GlowShape.cpp
    ../../src/Forms/Shapes/FatGlowShape.cpp
    ../../src/Forms/Shapes/NoiseGrid.cpp
    ../../src/Forms/Shapes/MeshGrid.cpp

    # Forms - Waves
    ../../src/Forms/Waves/BaseWaves.cpp
    ../../src/Forms/Waves/EdgeLasers.cpp
    ../../src/Forms/Waves/LaserWaves.cpp
    ../../src/Forms/Waves/PointWaves.cpp

    # Forms - Other
    ../../src/Forms/Field.cpp
    ../../src/Forms/ImageSprocket.cpp
    ../../src/Forms/Lotus.cpp
    ../../src/ImageWrapper.cpp

    # Forms - Lightning
    ../../src/Forms/Lightning/Thunder.cpp
    ../../src/Forms/Lightning/RapidThunder.cpp
    ../../src/Forms/Lightning/LightningBolt.cpp

    # Forms - Glow
    ../../src/Forms/Glow/GlowLinePlayground.cpp
    ../../src/Forms/Glow/Orbit.cpp
)

# Create integration test executable
add_executable(integration-tests
    ${INTEGRATION_TEST_FILES}
    ${INTEGRATION_SRC_FILES}
)

# Include directories
target_include_directories(integration-tests PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Forms
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Forms/Particles
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Forms/Waves
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Forms/Shapes
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Forms/Lightning
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Forms/Glow
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/Effects
    ${CMAKE_CURRENT_SOURCE_DIR}/../../src/core
)

# Link libraries (full OF stack with GL)
target_link_libraries(integration-tests
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

# Copy shader files to build directory for tests
add_custom_command(TARGET integration-tests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory
        $<TARGET_FILE_DIR:integration-tests>/data/shaders
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/../../bin/data/shaders
        $<TARGET_FILE_DIR:integration-tests>/data/shaders
    COMMENT "Copying shader files for integration tests"
)

# Discover integration tests
include(GoogleTest)
gtest_discover_tests(integration-tests
    PROPERTIES
    LABELS "integration"
    ENVIRONMENT "DISPLAY=:0"  # For macOS/Linux with display
)
