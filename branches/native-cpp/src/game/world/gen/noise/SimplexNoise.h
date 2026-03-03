#pragma once

#include <PrismaCraft/PrismaCraft.h>
#include <vector>
#include <array>
#include <random>
#include <cmath>

namespace PrismaCraft {

/**
 * Simplex noise generator (faster than Perlin noise)
 * Corresponds to: net.minecraft.world.level.levelgen.synth.SimplexNoise
 */
class PRISMACRAFT_API SimplexNoise {
public:
    SimplexNoise();
    explicit SimplexNoise(int64_t seed);

    // 2D noise
    double noise2D(double x, double y) const;
    double octaveNoise2D(double x, double y, int octaves, double persistence) const;

    // 3D noise
    double noise3D(double x, double y, double z) const;

    // Set seed
    void setSeed(int64_t seed);

private:
    static constexpr double F2 = 0.5 * (std::sqrt(3.0) - 1.0);
    static constexpr double G2 = (3.0 - std::sqrt(3.0)) / 6.0;
    static constexpr double F3 = 1.0 / 3.0;
    static constexpr double G3 = 1.0 / 6.0;

    void initPermutation();

    double grad(int hash, double x, double y) const;
    double grad(int hash, double x, double y, double z) const;

    int64_t seed_;
    std::array<int, 512> permutation_;
    std::mt19937_64 random_;
};

} // namespace PrismaCraft
