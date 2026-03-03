#include "PerlinNoise.h"

namespace PrismaCraft {

PerlinNoise::PerlinNoise() : seed_(0) {
    random_.seed(seed_);
    initPermutation();
}

PerlinNoise::PerlinNoise(int64_t seed) : seed_(seed) {
    random_.seed(seed);
    initPermutation();
}

void PerlinNoise::setSeed(int64_t seed) {
    seed_ = seed;
    random_.seed(seed);
    initPermutation();
}

void PerlinNoise::initPermutation() {
    // Initialize permutation table
    std::array<int, 256> p;
    for (int i = 0; i < 256; ++i) {
        p[i] = i;
    }

    // Shuffle
    std::uniform_int_distribution<int> dist(0, 255);
    for (int i = 255; i > 0; --i) {
        int j = dist(random_);
        std::swap(p[i], p[j]);
    }

    // Duplicate for overflow
    for (int i = 0; i < 512; ++i) {
        permutation_[i] = p[i & 255];
    }
}

double PerlinNoise::grad(int hash, double x, double y, double z) const {
    int h = hash & 15;
    double u = (h < 8) ? x : y;
    double v = (h < 4) ? y : ((h == 12 || h == 14) ? x : z);
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}

double PerlinNoise::noise2D(double x, double y) const {
    return noise3D(x, y, 0.0);
}

double PerlinNoise::noise3D(double x, double y, double z) const {
    // Find unit cube containing point
    int X = static_cast<int>(std::floor(x)) & 255;
    int Y = static_cast<int>(std::floor(y)) & 255;
    int Z = static_cast<int>(std::floor(z)) & 255;

    // Find relative x, y, z of point in cube
    x -= std::floor(x);
    y -= std::floor(y);
    z -= std::floor(z);

    // Compute fade curves
    double u = fade(x);
    double v = fade(y);
    double w = fade(z);

    // Hash coordinates of the 8 cube corners
    int A = permutation_[X] + Y;
    int AA = permutation_[A] + Z;
    int AB = permutation_[A + 1] + Z;
    int B = permutation_[X + 1] + Y;
    int BA = permutation_[B] + Z;
    int BB = permutation_[B + 1] + Z;

    // Blend results
    double res = lerp(w,
        lerp(v,
            lerp(u, grad(permutation_[AA], x, y, z), grad(permutation_[BA], x - 1, y, z)),
            lerp(u, grad(permutation_[AB], x, y - 1, z), grad(permutation_[BB], x - 1, y - 1, z))),
        lerp(v,
            lerp(u, grad(permutation_[AA + 1], x, y, z - 1), grad(permutation_[BA + 1], x - 1, y, z - 1)),
            lerp(u, grad(permutation_[AB + 1], x, y - 1, z - 1), grad(permutation_[BB + 1], x - 1, y - 1, z - 1))));

    return res;
}

double PerlinNoise::octaveNoise2D(double x, double y, int octaves, double persistence) const {
    double total = 0.0;
    double frequency = 1.0;
    double amplitude = 1.0;
    double maxValue = 0.0;

    for (int i = 0; i < octaves; ++i) {
        total += noise2D(x * frequency, y * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0;
    }

    return total / maxValue;
}

double PerlinNoise::octaveNoise3D(double x, double y, double z, int octaves, double persistence) const {
    double total = 0.0;
    double frequency = 1.0;
    double amplitude = 1.0;
    double maxValue = 0.0;

    for (int i = 0; i < octaves; ++i) {
        total += noise3D(x * frequency, y * frequency, z * frequency) * amplitude;
        maxValue += amplitude;
        amplitude *= persistence;
        frequency *= 2.0;
    }

    return total / maxValue;
}

} // namespace PrismaCraft
