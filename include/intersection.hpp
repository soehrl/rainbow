#pragma once

#include <glm/vec3.hpp>
#include <optional>

namespace rainbow {

struct Ray {
  glm::vec3 origin;
  glm::vec3 direction;
};

struct Triangle {
  glm::vec3 vertices[3];
};

struct AxisAlignedBoundingBox {
  glm::vec3 min;
  glm::vec3 max;

  inline glm::vec3 CalculateExtend() const { return max - min; }
  inline glm::vec3 CalculateCenter() const { return (min + max) * 0.5f; }
};

struct RayTriangleIntersection {
  glm::vec3 intersection_point;
  float distance;
  glm::vec3 barycentric_coordinates;
};

std::optional<RayTriangleIntersection> ComputeRayTriangleIntersection(
    const Ray& ray, const Triangle& triangle);

bool CheckForTriangleAxisAlignedBoundingBoxIntersection(
    const Triangle& triangle, const AxisAlignedBoundingBox& aabb);

}  // namespace rainbow
