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
#include "rtweekend.hpp"
#include "hittable.hpp"
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
    hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
        return intersectIterative(rootNodeIdx, r, tMin, tMax, rec);
    }

    std::string tree(size_t nodeIdx, int depth = 0) const;

    size_t getNodesUsed() const { return nodesUsed - 1; }

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

    double findBestSplitPlane(Node& node, int& axis, double& splitPos);

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
        const double tMin,
        const double tMax,
        HitRecord& rec,
        int depth = 0) const;

    /// @brief Recursive intersection function called by hit(). First check if
    /// AABB intersects ray, in which case either; For leaf nodes, compute hit
    /// similarly to HittableList (exactly what this is); For inner nodes,
    /// recurse and handle results.
    bool intersectIterative(
        const size_t nodeIdx,
        const Ray& r,
        const double tMin,
        const double tMax,
        HitRecord& rec) const;

    /// @brief Evaluate SAH for the node on the given split axis and position.
    /// @param axis Split axis
    /// @param pos Split position
    /// @return
    float evaluateSAH(Node& node, int axis, float pos) {
        // debug parameters: , int *lc, int *rc, float *la, float *ra
        Aabb leftBox { .min = Vec3(1e30), .max = Vec3(-1e30) };
        Aabb rightBox { .min = Vec3(1e30), .max = Vec3(-1e30) };
        int leftCount = 0, rightCount = 0;
        for (size_t i = 0; i < node.primCount; i++) {
            auto prim = primitives[primIndices[node.firstPrimIdx + i]];
            if (prim->centroid()[axis] < pos) {
                leftCount++;
                prim->growAABB(leftBox);
                // leftBox.grow(prim->vertices[0]);
                // leftBox.grow(prim->vertices[1]);
                // leftBox.grow(prim->vertices[2]);
            } else {
                rightCount++;
                prim->growAABB(rightBox);
                // rightBox.grow(prim->vertices[0]);
                // rightBox.grow(prim->vertices[1]);
                // rightBox.grow(prim->vertices[2]);
            }
        }
        // if (lc) *lc = leftCount;
        // if (rc) *rc = rightCount;
        // if (la) *la = leftBox.area();
        // if (ra) *ra = rightBox.area();
        float cost = leftCount * leftBox.area() + rightCount * rightBox.area();
        return cost > 0.0 ? cost : 1e30f;
    }

private:
    size_t rootNodeIdx = 0;
    size_t nodesUsed   = 2;
    std::vector<Node> nodes;
    /// @brief Reference to list of primitives. Assume that this one can be
    /// shared among subsystems, as so should not be modified.
    const std::vector<shared_ptr<Primitive>>& primitives;
    /// @brief Order of indices into the primitives list. This also allows for
    /// the element size to be decreased (surely won't need 2^64 primitives, or
    /// even 2^32).
    std::vector<uint32_t> primIndices;
    /// Primitives size
    size_t N;
};
} // namespace blikker_basic
