#pragma once

#include "PerlinNoise.h"
#include "SimplexNoise.h"
#include <PrismaCraft/PrismaCraft.h>
#include <memory>
#include <vector>

namespace PrismaCraft {

/**
 * Octave noise (multiple octaves of noise combined)
 * Corresponds to: net.minecraft.world.level.levelgen.synth.ImprovedNoise
 */
class PRISMACRAFT_API OctaveNoise {
public:
    enum class NoiseType {
        PERLIN,
        SIMPLEX
    };

    OctaveNoise(NoiseType type, int64_t seed);

    // Add octave
    void addOctave(double frequency, double amplitude);

    // 2D noise
    double noise2D(double x, double y) const;
    double octaveNoise2D(double x, double y, int octaves, double persistence) const;

    // 3D noise
    double noise3D(double x, double y, double z) const;

    // Set seed
    void setSeed(int64_t seed);

private:
    NoiseType type_;
    std::unique_ptr<PerlinNoise> perlin_;
    std::unique_ptr<SimplexNoise> simplex_;
    int64_t seed_;

    struct Octave {
        double frequency;
        double amplitude;
    };
    std::vector<Octave> octaves_;
};

} // namespace PrismaCraft
