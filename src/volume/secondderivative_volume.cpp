#include "secondderivative_volume.h"
#include <algorithm>
#include <exception>
#include <glm/geometric.hpp>
#include <glm/vector_relational.hpp>
#include <gsl/span>

namespace volume {

// Compute the maximum magnitude from all gradient voxels
static float computeMaxMagnitude(gsl::span<const SecondDerivativeVoxel> data)
{
    return std::max_element(
        std::begin(data),
        std::end(data),
        [](const SecondDerivativeVoxel& lhs, const SecondDerivativeVoxel& rhs) {
            return lhs.magnitude < rhs.magnitude;
        })
        ->magnitude;
}

// Compute the minimum magnitude from all gradient voxels
static float computeMinMagnitude(gsl::span<const SecondDerivativeVoxel> data)
{
    return std::min_element(
        std::begin(data),
        std::end(data),
        [](const SecondDerivativeVoxel& lhs, const SecondDerivativeVoxel& rhs) {
            return lhs.magnitude < rhs.magnitude;
        })
        ->magnitude;
}

// Compute a gradient volume from a volume
static std::vector<SecondDerivativeVoxel> computeSecondDerivativeVolume(const Volume& volume)
{
    const auto dim = volume.dims();

    std::vector<SecondDerivativeVoxel> out(static_cast<size_t>(dim.x * dim.y * dim.z));
    for (int z = 1; z < dim.z - 1; z++) {
        for (int y = 1; y < dim.y - 1; y++) {
            for (int x = 1; x < dim.x - 1; x++) {
                const float gx = (volume.getVoxel(x + 1, y, z) - volume.getVoxel(x - 1, y, z)) / 2.0f;
                const float gy = (volume.getVoxel(x, y + 1, z) - volume.getVoxel(x, y - 1, z)) / 2.0f;
                const float gz = (volume.getVoxel(x, y, z + 1) - volume.getVoxel(x, y, z - 1)) / 2.0f;

                const glm::vec3 v { gx, gy, gz };
                const size_t index = static_cast<size_t>(x + dim.x * (y + dim.y * z));
                out[index] = SecondDerivativeVoxel { v, glm::length(v) };
            }
        }
    }
    return out;
}

SecondDerivativeVolume::SecondDerivativeVolume(const Volume& volume)
    : m_dim(volume.dims())
    , m_data(computeSecondDerivativeVolume(volume))
    , m_minMagnitude(computeMinMagnitude(m_data))
    , m_maxMagnitude(computeMaxMagnitude(m_data))
{
}

float SecondDerivativeVolume::maxMagnitude() const
{
    return m_maxMagnitude;
}

float SecondDerivativeVolume::minMagnitude() const
{
    return m_minMagnitude;
}

glm::ivec3 SecondDerivativeVolume::dims() const
{
    return m_dim;
}

// This function returns a gradientVoxel at coord based on the current interpolation mode.
SecondDerivativeVoxel SecondDerivativeVolume::getSecondDerivativeInterpolate(const glm::vec3& coord) const
{
    switch (interpolationMode) {
    case InterpolationMode::NearestNeighbour: {
        return getSecondDerivativeNearestNeighbor(coord);
    }
    case InterpolationMode::Linear: {
        return getSecondDerivativeLinearInterpolate(coord);
    }
    case InterpolationMode::Cubic: {
        // No cubic in this case, linear is good enough for the gradient.
        return getSecondDerivativeLinearInterpolate(coord);
    }
    default: {
        throw std::exception();
    }
    };
}

// This function returns the nearest neighbour given a position in the volume given by coord.
// Notice that in this framework we assume that the distance between neighbouring voxels is 1 in all directions
SecondDerivativeVoxel SecondDerivativeVolume::getSecondDerivativeNearestNeighbor(const glm::vec3& coord) const
{
    if (glm::any(glm::lessThan(coord, glm::vec3(0))) || glm::any(glm::greaterThanEqual(coord, glm::vec3(m_dim))))
        return { glm::vec3(0.0f), 0.0f };

    auto roundToPositiveInt = [](float f) {
        return static_cast<int>(f + 0.5f);
    };

    return getSecondDerivative(roundToPositiveInt(coord.x), roundToPositiveInt(coord.y), roundToPositiveInt(coord.z));
}

// ======= TODO : IMPLEMENT ========
// Returns the trilinearly interpolated gradinet at the given coordinate.
// Use the linearInterpolate function that you implemented below.
SecondDerivativeVoxel SecondDerivativeVolume::getSecondDerivativeLinearInterpolate(const glm::vec3& coord) const
{
    if (glm::any(glm::lessThan(coord, glm::vec3(0))) || glm::any(glm::greaterThanEqual(coord, glm::vec3(m_dim - 1))))
        return { glm::vec3(0.0f), 0.0f };

    // precalculate floor points
    int floorX = floor(coord.x);
    int floorY = floor(coord.y);
    int floorZ = floor(coord.z);

    // precalculate distances
    float diffx = coord.x - floorX;
    float diffy = coord.y - floorY;
    float diffz = coord.z - floorZ;

    // Get surrounding 8 neighbors
    SecondDerivativeVoxel c000 = getSecondDerivative(floorX, floorY, floorZ);
    SecondDerivativeVoxel c001 = getSecondDerivative(floorX, floorY, floorZ + 1);

    SecondDerivativeVoxel c010 = getSecondDerivative(floorX, floorY + 1, floorZ);
    SecondDerivativeVoxel c011 = getSecondDerivative(floorX, floorY + 1, floorZ + 1);

    SecondDerivativeVoxel c100 = getSecondDerivative(floorX + 1, floorY, floorZ);
    SecondDerivativeVoxel c101 = getSecondDerivative(floorX + 1, floorY, floorZ + 1);

    SecondDerivativeVoxel c110 = getSecondDerivative(floorX + 1, floorY + 1, floorZ);
    SecondDerivativeVoxel c111 = getSecondDerivative(floorX + 1, floorY + 1, floorZ + 1);

    // Interpolate over z
    SecondDerivativeVoxel c00 = linearInterpolate(c000, c001, diffz);
    SecondDerivativeVoxel c01 = linearInterpolate(c010, c011, diffz);
    SecondDerivativeVoxel c10 = linearInterpolate(c100, c101, diffz);
    SecondDerivativeVoxel c11 = linearInterpolate(c110, c111, diffz);

    // Interpolate over y
    SecondDerivativeVoxel c0 = linearInterpolate(c00, c01, diffy);
    SecondDerivativeVoxel c1 = linearInterpolate(c10, c11, diffy);

    // Interpolate over x
    SecondDerivativeVoxel output = linearInterpolate(c0, c1, diffx);

    return output;
}

// ======= TODO : IMPLEMENT ========
// This function should linearly interpolates the value from g0 to g1 given the factor (t).
// At t=0, linearInterpolate should return g0 and at t=1 it returns g1.
SecondDerivativeVoxel SecondDerivativeVolume::linearInterpolate(const SecondDerivativeVoxel& g0, const SecondDerivativeVoxel& g1, float factor)
{
    SecondDerivativeVoxel interpolated_G = { glm::vec3(0.0f), 0.0f };
    interpolated_G.dir = g0.dir * (1 - factor) + g1.dir * factor;
    interpolated_G.magnitude = g0.magnitude * (1 - factor) + g1.magnitude * factor;
    return interpolated_G;
}

// This function returns a gradientVoxel without using interpolation
SecondDerivativeVoxel SecondDerivativeVolume::getSecondDerivative(int x, int y, int z) const
{
    const size_t i = static_cast<size_t>(x + m_dim.x * (y + m_dim.y * z));
    return m_data[i];
}
}