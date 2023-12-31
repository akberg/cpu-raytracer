/// @file bvh2.hpp
/// Implementing a BVH following Jacco's blog (jacco.ompf2.com).
///
/// Part 2 introduces the SAH for a better partitioning result.
///
/// BVH build: 6-7ms, render: 25632ms
#pragma once

#include "aabb.hpp"
#include "hittable.hpp"
// #include "hittableList.hpp"
#include "hittable.hpp"
#include "rtweekend.hpp"
#include "shape/triangle.hpp"

#include <vector>

namespace blikker_pt3 {
class BVH : public Hittable {
public:
    /// @brief Build Bounding Volume Hierarchy. In the reference implementation,
    /// this is the buildBVH() function.
    /// @param primitives List of primitive pointers, making the structure only
    /// for handling triangles in the first place.
    BVH(const std::vector<shared_ptr<Primitive>>& primitives);

    // Hittable
    bool
    hit(const Ray& r, float tMin, float tMax, HitRecord& rec) const override
    {
        return intersectIterative(rootNodeIdx, r, tMin, tMax, rec);
    }

    std::string tree(uint32_t nodeIdx, int depth = 0) const;

    uint32_t getNodesUsed() const { return nodesUsed - 1; }

    struct Node {
        Aabb aabb;
        uint32_t primCount; ///< For a leaf: primitive count, for an inner node: 0
        union {
            uint32_t mLeftChildIdx; ///< Index of left child node, for inner nodes
            uint32_t firstPrimIdx;  ///< Index of first primitive, for leaf nodes
        };

        bool isLeaf() const { return primCount > 0; }
        uint32_t left() const { return mLeftChildIdx; }
        uint32_t right() const { return mLeftChildIdx + 1; }
        float cost() const { return primCount * aabb.area(); }
    };

    struct Bin {
        Aabb bounds;
        int primCount = 0;
    };

private:
    /// @brief Update AABB bounds of root node.
    /// @param nodeIdx
    void updateNodeBounds(const uint32_t nodeIdx);

    float findBestSplitPlane(Node& node, int& axis, float& splitPos);

    /// @brief Recursive BVH building. Determine split axis and position, split
    /// and partition primitives, create child nodes and recurse.
    /// @param nodeIdx
    void subdivide(const uint32_t nodeIdx);

    /// @brief Recursive intersection function called by hit(). First check if
    /// AABB intersects ray, in which case either; For leaf nodes, compute hit
    /// similarly to HittableList (exactly what this is); For inner nodes,
    /// recurse and handle results.
    bool intersect(
        const uint32_t nodeIdx,
        const Ray& r,
        const float tMin,
        const float tMax,
        HitRecord& rec,
        int depth = 0) const;

    /// @brief Recursive intersection function called by hit(). First check if
    /// AABB intersects ray, in which case either; For leaf nodes, compute hit
    /// similarly to HittableList (exactly what this is); For inner nodes,
    /// recurse and handle results.
    bool intersectIterative(
        const uint32_t nodeIdx,
        const Ray& r,
        const float tMin,
        const float tMax,
        HitRecord& rec) const;

    /// @brief Evaluate SAH for the node on the given split axis and position.
    /// @param axis Split axis
    /// @param pos Split position
    /// @return
    float evaluateSAH(Node& node, int axis, float pos);

private:
    uint32_t rootNodeIdx = 0;
    uint32_t nodesUsed   = 2;
    std::vector<Node> nodes;
    /// @brief Reference to list of primitives. Assume that this one can be
    /// shared among subsystems, as so should not be modified.
    const std::vector<shared_ptr<Primitive>>& primitives;
    /// @brief Order of indices into the primitives list. This also allows for
    /// the element size to be decreased (surely won't need 2^64 primitives, or
    /// even 2^32).
    std::vector<uint32_t> primIndices;
    /// Primitives size
    uint32_t N;
};
} // namespace blikker_basic
