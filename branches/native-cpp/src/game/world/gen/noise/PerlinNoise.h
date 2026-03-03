#pragma once

#include <PrismaCraft/PrismaCraft.h>
#include <vector>
#include <array>
#include <random>
#include <cmath>

namespace PrismaCraft {

/**
 * Perlin noise generator
 * Corresponds to: net.minecraft.world.level.levelgen.synth.PerlinNoise
 */
class PRISMACRAFT_API PerlinNoise {
public:
    PerlinNoise();
    explicit PerlinNoise(int64_t seed);

    // 2D noise
    double noise2D(double x, double y) const;
    double octaveNoise2D(double x, double y, int octaves, double persistence) const;

    // 3D noise
    double noise3D(double x, double y, double z) const;
    double octaveNoise3D(double x, double y, double z, int octaves, double persistence) const;

    // Set seed
    void setSeed(int64_t seed);

private:
    void initPermutation();

    double fade(double t) const {
        return t * t * t * (t * (t * 6 - 15) + 10);
    }

    double lerp(double t, double a, double b) const {
        return a + t * (b - a);
    }

    double grad(int hash, double x, double y, double z) const;

    int64_t seed_;
    std::array<int, 512> permutation_;
    std::mt19937_64 random_;
};

} // namespace PrismaCraft
