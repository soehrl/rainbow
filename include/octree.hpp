#pragma once

#include <glm/vec3.hpp>

#include "intersection.hpp"
#include "scene.hpp"

namespace rainbow {

class Octree {
 public:
  struct OctreeData {
    glm::vec3 aabb_min;
    float padding0;
    glm::vec3 aabb_max;
    float padding1;
    uint32_t triangles_begin;
    uint32_t triangles_end;
    int32_t parent_index;
    int32_t child_indices[8];
  };
  static_assert(sizeof(OctreeData) == 76);

  struct OctreeCell {
    size_t depth;
    AxisAlignedBoundingBox aabb;
    std::vector<Scene::Triangle> triangles;
    std::vector<OctreeCell> children;
  };
  Octree(const Scene::Vertex* vertices, size_t vertex_count, size_t max_depth,
         size_t triangles_per_cell);

  void InsertTriangle(const Scene::Triangle& triangle);
  void Print() const;

  inline const std::vector<OctreeData>& GetOctreeBufferData() const {
    return data_;
  }
  const std::vector<Scene::Triangle>& GetTriangleBufferData() const {
    return triangles_;
  }
  void Build();

 private:
  const Scene::Vertex* vertices_;
  size_t vertex_count_;
  size_t max_depth_;
  size_t triangles_per_cell_;
  OctreeCell root_;
  std::vector<OctreeData> data_;
  std::vector<Scene::Triangle> triangles_;

  void InsertTriangle(OctreeCell* node, const Scene::Triangle& triangle_indices,
                      const Triangle& triangle);
  void SplitCell(OctreeCell* node);
  Triangle GetTriangleFromTriangleIndices(
      const Scene::Triangle& triangle_indices);
  void PrintCell(const OctreeCell* cell) const;
  size_t GetNumberOfTrianglesInChildren(const OctreeCell* cell) const;

  template <typename F>
  inline void Traverse(F&& function) {
    static_assert(std::is_invocable_v<F, const OctreeCell&, const OctreeCell*>);
    TraverseSubTree(root_, nullptr, function);
  }

  template <typename F>
  inline void TraverseSubTree(const OctreeCell& cell, const OctreeCell* parent,
                              F&& function) {
    static_assert(std::is_invocable_v<F, const OctreeCell&, const OctreeCell*>);
    function(cell, parent);
    for (const auto& child : cell.children) {
      TraverseSubTree(child, &cell, function);
    }
  }
};

}  // namespace rainbow