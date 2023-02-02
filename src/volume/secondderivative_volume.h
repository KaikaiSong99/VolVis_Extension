#pragma once
#include "volume.h"
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <string>
#include <vector>

namespace volume {
struct SecondDerivativeVoxel {
    float magnitude;
};

class SecondDerivativeVolume {
public:
    // DO NOT REMOVE
    InterpolationMode interpolationMode { InterpolationMode::NearestNeighbour };

public:
    SecondDerivativeVolume(const Volume& volume);

    SecondDerivativeVoxel getSecondDerivativeInterpolate(const glm::vec3& coord) const;
    SecondDerivativeVoxel getSecondDerivative(int x, int y, int z) const;

    float minMagnitude() const;
    float maxMagnitude() const;
    glm::ivec3 dims() const;

protected:
    SecondDerivativeVoxel getSecondDerivativeNearestNeighbor(const glm::vec3& coord) const;
    SecondDerivativeVoxel getSecondDerivativeLinearInterpolate(const glm::vec3& coord) const;
    static SecondDerivativeVoxel linearInterpolate(const SecondDerivativeVoxel& g0, const SecondDerivativeVoxel& g1, float factor);

protected:
    const glm::ivec3 m_dim;
    const std::vector<SecondDerivativeVoxel> m_data;
    const float m_minMagnitude, m_maxMagnitude;
};
}