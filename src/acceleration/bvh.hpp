/// @file bvh.hpp
/// Implementing a BVH following Jacco's blog (jacco.ompf2.com).
#pragma once

#include "hittable.hpp"
#include "hittableList.hpp"
#include "rtweekend.hpp"
#include "shape/triangle.hpp"

#include <iostream>
#include <sstream>
#include <vector>

struct Aabb {
    Point min;
    Point max;

    bool intersect(const Ray& ray, const double tMin, const double tMax) const {
        aabbIntersections++; // Counting total no. of intersections.
        double tx1  = (min.x - ray.origin.x) / ray.direction.x;
        double tx2  = (max.x - ray.origin.x) / ray.direction.x;
        double tmin = std::min(tx1, tx2);
        double tmax = std::max(tx1, tx2);
        double ty1  = (min.y - ray.origin.y) / ray.direction.y;
        double ty2  = (max.y - ray.origin.y) / ray.direction.y;
        tmin        = std::max(tmin, std::min(ty1, ty2));
        tmax        = std::min(tmax, std::max(ty1, ty2));
        double tz1  = (min.z - ray.origin.z) / ray.direction.z;
        double tz2  = (max.z - ray.origin.z) / ray.direction.z;
        tmin        = std::max(tmin, std::min(tz1, tz2));
        tmax        = std::min(tmax, std::max(tz1, tz2));
        // std::cerr << ray << " AABB.min=" << min << " AABB.max=" << max
        //           << " tmin=" << tmin << " tmax=" << tmax
        //           //<< " r.at(tmin)=" << ray.at(tmin)
        //           << " r.at(tmax)=" << ray.at(tmax) << " tMax=" << tMax
        //           << " return "
        //           << (tmax >= tmin && tmax > 0 && tmin < tMax)
        //           << "\n";
        return tmax >= tmin && tmax > 0 && tmin < tMax; // && tmin < ray->t (?)
    }
};

struct BVHNode {
    Aabb aabb;
    size_t mLeftChildIdx; ///< Index
    size_t firstPrimIdx;  ///< Index
    size_t primCount;

    bool isLeaf() const { return primCount > 0; }
    size_t left() const { return mLeftChildIdx; }
    size_t right() const { return mLeftChildIdx + 1; }
};

class BVH : public Hittable {
public:
    /// @brief Build Bounding Volume Hierarchy. In the reference implementation,
    /// this is the buildBVH() function.
    /// @param primitives List of primitive pointers, making the structure only
    /// for handling triangles in the first place.
    BVH(const std::vector<shared_ptr<Triangle>>& primitives)
        : primitives(primitives)
        , N(primitives.size()) {
        // Upper limit of tree size.
        nodes.resize(N * 2 - 1);
        // Populate index list
        primIndices.resize(N);
        for (size_t i = 0; i < N; i++) primIndices[i] = i;

        BVHNode& root      = nodes[rootNodeIdx];
        root.mLeftChildIdx = 0;
        root.firstPrimIdx  = 0;
        root.primCount     = N;
        updateNodeBounds(rootNodeIdx);
        std::cerr << "Start recursive subdivide()\n";
        subdivide(rootNodeIdx);
    }

    // Hittable
    bool
    hit(const Ray& r, double tMin, double tMax, HitRecord& rec) const override {
        return intersect(rootNodeIdx, r, tMin, tMax, rec);
    }

    std::string tree(size_t nodeIdx, int depth = 0) const {
        const BVHNode& node = nodes[nodeIdx];
        int indent          = depth;
        std::stringstream os;
        // os << std::string(indent, ' ') << nodeIdx << "("<<depth<<")";
        os << std::string(indent, ' ') << nodeIdx
           << ": aabb.min=" << node.aabb.min << " aabb.max=" << node.aabb.max
           << " prims: " << node.primCount << "\n";
        if (!node.isLeaf()) {
            // os << ": childLeft=" << node.left() << " childRight=" <<
            // node.right() << "\n";
            os << tree(node.left(), depth + 1);
            os << tree(node.right(), depth + 1);
        } else {
            // os << ": leaf\n";
        }
        return os.str();
    }

private:
    /// @brief Update AABB bounds of root node.
    /// @param nodeIdx
    void updateNodeBounds(const size_t nodeIdx) {
        BVHNode& node = nodes[nodeIdx];
        node.aabb.min = Point(1e30f);
        node.aabb.max = Point(-1e30f);
        size_t first  = node.firstPrimIdx;
        for (size_t i = 0; i < node.primCount; i++) {
            auto leafPrimIdx = primIndices[first + i];
            std::cerr << "updateNodeBounds(" << nodeIdx
                      << "): primCount=" << node.primCount
                      << " firstPrimIdx=" << node.firstPrimIdx << " i=" << i
                      << " leafPrimIdx=" << leafPrimIdx
                      << " primitives.size()=" << primitives.size() << "\n";
            auto leafPrim = primitives[leafPrimIdx];

            node.aabb.min = glm::min(node.aabb.min, leafPrim->vertices[0]);
            node.aabb.min = glm::min(node.aabb.min, leafPrim->vertices[1]);
            node.aabb.min = glm::min(node.aabb.min, leafPrim->vertices[2]);
            node.aabb.max = glm::max(node.aabb.max, leafPrim->vertices[0]);
            node.aabb.max = glm::max(node.aabb.max, leafPrim->vertices[1]);
            node.aabb.max = glm::max(node.aabb.max, leafPrim->vertices[2]);
        }
    }

    /// @brief Recursive BVH building. Determine split axis and position, split
    /// and partition primitives, create child nodes and recurse.
    /// @param nodeIdx
    void subdivide(const size_t nodeIdx) {
        BVHNode& node = nodes[nodeIdx];
        if (node.primCount <= 2) return;
        // 1. Determine the axis and position of the split plane.
        Vec3 extent = node.aabb.max - node.aabb.min;
        int axis    = 0;
        if (extent.y > extent.x) axis = 1;
        if (extent.z > extent[axis]) axis = 2;
        std::cerr
            << nodeIdx
            << "// 1. Determine the axis and position of the split plane: "
            << axis << "\n";
        double splitPos = node.aabb.min[axis] + extent[axis] * 0.5;

        // 2. Split the group of primitives in two halves using the split plane.
        std::cerr << nodeIdx
                  << "// 2. Split the group of primitives in two halves using "
                     "the split plane.\n";
        // Implementing partition
        int i = node.firstPrimIdx;
        int j = i + node.primCount - 1;
        while (i <= j) {
            if (primitives[primIndices[i]]->centroid[axis] < splitPos) {
                i++;
            } else {
                std::swap(primIndices[i], primIndices[j]);
                j--;
            }
        }

        // 3. Create child nodes for each half.
        std::cerr << nodeIdx << "// 3. Create child nodes for each half.\n";
        // 3. Create child nodes for each half.
        // Theoretically, the split in the middle can yield an empty box on the
        // left or the right side.
        int leftCount = i - node.firstPrimIdx;
        if (!leftCount || leftCount == node.primCount) return;
        // Create child nodes
        size_t leftChildIdx  = nodesUsed++;
        size_t rightChildIdx = nodesUsed++;
        // Right child index is implicit, handled by member getter functions.
        node.mLeftChildIdx   = leftChildIdx;

        nodes[leftChildIdx].firstPrimIdx  = node.firstPrimIdx;
        nodes[leftChildIdx].primCount     = leftCount;
        nodes[rightChildIdx].firstPrimIdx = i;
        nodes[rightChildIdx].primCount    = node.primCount - leftCount;
        // Clear primCount, as isLeaf() relies on it.
        node.primCount                    = 0;
        updateNodeBounds(leftChildIdx);
        updateNodeBounds(rightChildIdx);

        // 4. Recurse into each of the child nodes.
        std::cerr << nodeIdx << "// 4. Recurse into each of the child nodes: "
                  << leftChildIdx << " and " << rightChildIdx
                  << " (nodes used=" << nodesUsed << ")\n";
        subdivide(leftChildIdx);
        subdivide(rightChildIdx);
    }

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
        int depth = 0) const {
        const BVHNode& node = nodes[nodeIdx];
        // std::cerr << std::string(depth, ' ') << "Node " << nodeIdx << " ";
        if (!node.aabb.intersect(r, tMin, tMax)) return false;

        HitRecord tempRec;
        bool anyHit  = false;
        auto closest = tMax;
        if (node.isLeaf()) {
            for (size_t i = 0; i < node.primCount; i++) {
                // Intersect triangles
                auto prim = primitives[primIndices[node.firstPrimIdx + i]];
                if (prim->hit(r, tMin, closest, tempRec)) {
                    // std::cerr << std::string(depth, ' ') << "Hit: prim "
                    //           << node.firstPrimIdx + i << "\n";
                    anyHit  = true;
                    closest = tempRec.t;
                    rec     = tempRec;
                }
            }
        } else {
            // Continue traversing BVH
            if (intersect(node.left(), r, tMin, closest, tempRec, depth + 1)) {
                anyHit  = true;
                closest = tempRec.t;
                rec     = tempRec;
            }
            if (intersect(node.right(), r, tMin, closest, tempRec, depth + 1)) {
                anyHit  = true;
                closest = tempRec.t;
                rec     = tempRec;
            }
        }
        // std::cerr << "Node " << nodeIdx << ": " << anyHit;
        return anyHit;
    }

private:
    size_t rootNodeIdx = 0;
    size_t nodesUsed   = 1;
    std::vector<BVHNode> nodes;
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
