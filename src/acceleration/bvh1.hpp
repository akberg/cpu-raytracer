/// @file bvh1.hpp
/// Implementing a BVH following Jacco's blog (jacco.ompf2.com).
///
/// Part 1 follows a very basic method, using a "midpoint split" object
/// partitioning.
#pragma once

#include "aabb.hpp"
#include "hittable.hpp"
// #include "hittableList.hpp"
#include "rtweekend.hpp"
#include "shape/triangle.hpp"

#include <vector>

namespace blikker_basic {
class BVH : public Hittable {
public:
    /// @brief Build Bounding Volume Hierarchy. In the reference implementation,
    /// this is the buildBVH() function.
    /// @param primitives List of primitive pointers, making the structure only
    /// for handling triangles in the first place.
    BVH(const std::vector<shared_ptr<Triangle>>& primitives);

    // Hittable
    bool
    hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const override {
        return intersect(rootNodeIdx, r, tMin, tMax, rec);
    }

    std::string tree(size_t nodeIdx, int depth = 0) const;

    size_t getNodesUsed() const { return nodesUsed; }

struct Node {
    Aabb aabb;
    size_t primCount; ///< For a leaf: primitive count, for an inner node: 0
    union {
        size_t mLeftChildIdx; ///< Index of left child node, for inner nodes
        size_t firstPrimIdx;  ///< Index of first primitive, for leaf nodes
    };

    bool isLeaf() const { return primCount > 0; }
    size_t left() const { return mLeftChildIdx; }
    size_t right() const { return mLeftChildIdx + 1; }
};

private:
    /// @brief Update AABB bounds of root node.
    /// @param nodeIdx
    void updateNodeBounds(const size_t nodeIdx);

    /// @brief Recursive BVH building. Determine split axis and position, split
    /// and partition primitives, create child nodes and recurse.
    /// @param nodeIdx
    void subdivide(const size_t nodeIdx);

    /// @brief Recursive intersection function called by hit(). First check if
    /// AABB intersects ray, in which case either; For leaf nodes, compute hit
    /// similarly to HittableList (exactly what this is); For inner nodes,
    /// recurse and handle results.
    bool intersect(
        const size_t nodeIdx,
        const Ray& r,
        const float tMin,
        const float tMax,
        HitRecord& rec,
        int depth = 0) const;

private:
    size_t rootNodeIdx = 0;
    size_t nodesUsed   = 2;
    std::vector<Node> nodes;
    /// @brief Reference to list of primitives. Assume that this one can be
    /// shared among subsystems, as so should not be modified.
    const std::vector<shared_ptr<Triangle>>& primitives;
    /// @brief Order of indices into the primitives list. This also allows for
    /// the element size to be decreased (surely won't need 2^64 primitives, or
    /// even 2^32).
    std::vector<uint32_t> primIndices;
    /// Primitives size
    size_t N;
};
} // namespace blikker_basic
