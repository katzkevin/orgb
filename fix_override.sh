#!/bin/bash
# Replace "virtual ~ClassName()" with "~ClassName() override"

sed -i '' 's/virtual ~KeyState() {}/~KeyState() override {}/' src/KeyState.hpp
sed -i '' 's/virtual ~RapidThunder() {}/~RapidThunder() override {}/' src/Forms/Lightning/RapidThunder.hpp
sed -i '' 's/virtual ~Thunder()/~Thunder() override/' src/Forms/Lightning/Thunder.hpp
sed -i '' 's/virtual ~Flock() {}/~Flock() override {}/' src/Forms/Flock.hpp
sed -i '' 's/virtual ~NoiseGrid() {}/~NoiseGrid() override {}/' src/Forms/Shapes/NoiseGrid.hpp
sed -i '' 's/virtual ~Shape() {}/~Shape() override {}/' src/Forms/Shapes/Shape.hpp
sed -i '' 's/virtual ~FatGlowShape() {}/~FatGlowShape() override {}/' src/Forms/Shapes/FatGlowShape.hpp
sed -i '' 's/virtual ~MeshGrid() {}/~MeshGrid() override {}/' src/Forms/Shapes/MeshGrid.hpp
sed -i '' 's/virtual ~EdgeParticles() {}/~EdgeParticles() override {}/' src/Forms/Particles/EdgeParticles.hpp
sed -i '' 's/virtual ~RandomParticles() {}/~RandomParticles() override {}/' src/Forms/Particles/RandomParticles.hpp
sed -i '' 's/virtual ~RadialParticles() {}/~RadialParticles() override {}/' src/Forms/Particles/RadialParticles.hpp
sed -i '' 's/virtual ~Lotus() {}/~Lotus() override {}/' src/Forms/Lotus.hpp
sed -i '' 's/virtual ~LaserWaves() {}/~LaserWaves() override {}/' src/Forms/Waves/LaserWaves.hpp
sed -i '' 's/virtual ~BaseWaves() {}/~BaseWaves() override {}/' src/Forms/Waves/BaseWaves.hpp
sed -i '' 's/virtual ~PointWaves() {}/~PointWaves() override {}/' src/Forms/Waves/PointWaves.hpp
sed -i '' 's/virtual ~EdgeLasers() {}/~EdgeLasers() override {}/' src/Forms/Waves/EdgeLasers.hpp
sed -i '' 's/virtual ~Orbit() {}/~Orbit() override {}/' src/Forms/Glow/Orbit.hpp
sed -i '' 's/virtual ~GlowLinePlayground() {}/~GlowLinePlayground() override {}/' src/Forms/Glow/GlowLinePlayground.hpp
sed -i '' 's/virtual ~BaseParticle() {}/~BaseParticle() override {}/' src/Forms/BaseParticle.hpp
sed -i '' 's/virtual ~ColorProvider() {}/~ColorProvider() override {}/' src/ColorProvider.hpp
sed -i '' 's/virtual ~ImageWrapper() {}/~ImageWrapper() override {}/' src/ImageWrapper.hpp
sed -i '' 's/virtual ~DrawManager() {}/~DrawManager() override {}/' src/DrawManager.hpp

echo "Fixed override keywords"

# Fix Thunder.hpp
sed -i '' 's/void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm);/void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) override;/' src/Forms/Lightning/Thunder.hpp
sed -i '' 's/void update(KeyState & ks, ColorProvider & clr);/void update(KeyState & ks, ColorProvider & clr) override;/' src/Forms/Lightning/Thunder.hpp

# Fix Lotus.hpp  
sed -i '' 's/void update(KeyState & ks, ColorProvider & clr);/void update(KeyState & ks, ColorProvider & clr) override;/' src/Forms/Lotus.hpp
sed -i '' 's/void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm);/void draw(KeyState & ks, ColorProvider & clr, DrawManager & dm) override;/' src/Forms/Lotus.hpp

echo "Fixed additional override keywords for update/draw methods"
