/// @file BVH implementation
#include "bvh3.hpp"

#include "aabb.hpp"

#include <iostream>
#include <sstream>

namespace blikker_pt3 {

BVH::BVH(const std::vector<shared_ptr<Primitive>>& primitives)
    : primitives(primitives)
    , N(primitives.size())
{
    // Upper limit of tree size.
    nodes.resize(N * 2);
    // Populate index list
    primIndices.resize(N);
    for (size_t i = 0; i < N; i++) primIndices[i] = i;

    Node& root         = nodes[rootNodeIdx];
    root.mLeftChildIdx = 0;
    root.firstPrimIdx  = 0;
    root.primCount     = N;
    updateNodeBounds(rootNodeIdx);
    // std::cerr << "Start recursive subdivide()\n";
    subdivide(rootNodeIdx);
}
std::string BVH::tree(size_t nodeIdx, int depth) const
{
    const Node& node = nodes[nodeIdx];
    int indent       = depth;
    std::stringstream os;
    // os << std::string(indent, ' ') << nodeIdx << "("<<depth<<")";
    os << std::string(indent, ' ') << nodeIdx << ": aabb.min=" << node.aabb.min
       << " aabb.max=" << node.aabb.max << " prims: " << node.primCount << "\n";
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

void BVH::updateNodeBounds(const size_t nodeIdx)
{
    Node& node    = nodes[nodeIdx];
    node.aabb.min = Vec3(1e30f);
    node.aabb.max = Vec3(-1e30f);
    size_t first  = node.firstPrimIdx;
    for (size_t i = 0; i < node.primCount; i++) {
        auto leafPrimIdx = primIndices[first + i];
        // std::cerr << "updateNodeBounds(" << nodeIdx
        //           << "): primCount=" << node.primCount
        //           << " firstPrimIdx=" << node.firstPrimIdx << " i=" << i
        //           << " leafPrimIdx=" << leafPrimIdx
        //           << " primitives.size()=" << primitives.size() << "\n";
        auto leafPrim    = primitives[leafPrimIdx];
        // std::cerr << leafPrim->vertices[0] << "\n";

        leafPrim->growAABB(node.aabb);
        // node.aabb.min = glm::min(node.aabb.min, leafPrim->vertices[0]);
        // node.aabb.min = glm::min(node.aabb.min, leafPrim->vertices[1]);
        // node.aabb.min = glm::min(node.aabb.min, leafPrim->vertices[2]);
        // node.aabb.max = glm::max(node.aabb.max, leafPrim->vertices[0]);
        // node.aabb.max = glm::max(node.aabb.max, leafPrim->vertices[1]);
        // node.aabb.max = glm::max(node.aabb.max, leafPrim->vertices[2]);
    }
}

float BVH::evaluateSAH(Node& node, int axis, float pos)
{
    Aabb leftBox { .min = Vec3(1e30), .max = Vec3(-1e30) };
    Aabb rightBox { .min = Vec3(1e30), .max = Vec3(-1e30) };
    int leftCount = 0, rightCount = 0;
    for (size_t i = 0; i < node.primCount; i++) {
        auto prim = primitives[primIndices[node.firstPrimIdx + i]];
        if (prim->centroid()[axis] < pos) {
            leftCount++;
            prim->growAABB(leftBox);
        } else {
            rightCount++;
            prim->growAABB(rightBox);
        }
    }
    float cost = leftCount * leftBox.area() + rightCount * rightBox.area();
    return cost > 0.0 ? cost : 1e30f;
}

double BVH::findBestSplitPlane(Node& node, int& axis, double& splitPos)
{
    // Uniform SAH. Instead of doing an exhaustive sweep of all primitives for a
    // O(N^2) cost, step by uniform intervals for a O(N) cost.
    float bestCost = 1e30f;
    // ? What is then the best bin count? Would maybe depend on primitive count?
    uint32_t bins  = 48;
    for (uint32_t a = 0; a < 3; a++) {
        float boundsMin = 1e30f;
        float boundsMax = -1e30f;
        for (int i = 0; i < node.primCount; i++) {
            auto prim = primitives[primIndices[node.mLeftChildIdx + i]];
            boundsMin = std::min(boundsMin, (float)prim->centroid()[a]);
            boundsMax = std::max(boundsMax, (float)prim->centroid()[a]);
        }
        float step      = (boundsMax - boundsMin) / bins;
        for (uint32_t i = 0; i < bins; i++) {
            double candidatePos = boundsMin + i * step;
            float cost          = evaluateSAH(node, a, candidatePos);
            if (cost < bestCost) {
                bestCost = cost;
                splitPos = candidatePos;
                axis     = a;
            }
        }
    }
    return bestCost;
}

void BVH::subdivide(const size_t nodeIdx)
{
    Node& node = nodes[nodeIdx];
    // 1. Determine the axis and position of the split plane, using SAH.

    // 1a. Exhaustive SAH evaluation to find best split.
    int axis;        //   = -1;
    double splitPos; // = 0;
    float splitCost = findBestSplitPlane(node, axis, splitPos);

    // 1b. Evaluate if a split is actually improving from the parent node.
    float noSplitCost = node.cost();
    if (splitCost >= noSplitCost) {
        return;
    }

    // 2. Split the group of primitives in two halves using the split plane.
    // Implementing partition
    int i = node.firstPrimIdx;
    int j = i + node.primCount - 1;
    while (i <= j) {
        if (primitives[primIndices[i]]->centroid()[axis] < splitPos) {
            i++;
        } else {
            std::swap(primIndices[i], primIndices[j]);
            j--;
        }
    }

    // 3. Create child nodes for each half.
    // Theoretically, the split in the middle can yield an empty box on the
    // left or the right side.
    int leftCount = i - node.firstPrimIdx;
    if (!leftCount || leftCount == node.primCount) {
        return;
    }
    // Create child nodes
    size_t leftChildIdx  = nodesUsed++;
    size_t rightChildIdx = nodesUsed++;
    // Right child index is implicit, handled by member getter functions.
    size_t firstPrimIdx  = node.firstPrimIdx;
    node.mLeftChildIdx   = leftChildIdx;

    nodes[leftChildIdx].firstPrimIdx  = firstPrimIdx;
    nodes[leftChildIdx].primCount     = leftCount;
    nodes[rightChildIdx].firstPrimIdx = i;
    nodes[rightChildIdx].primCount    = node.primCount - leftCount;
    // Clear primCount, as isLeaf() relies on it.
    node.primCount                    = 0;
    updateNodeBounds(leftChildIdx);
    updateNodeBounds(rightChildIdx);
    float cost =
        nodes[leftChildIdx].primCount * nodes[leftChildIdx].aabb.area()
        + nodes[rightChildIdx].primCount * nodes[rightChildIdx].aabb.area();
    // 4. Recurse into each of the child nodes.
    subdivide(leftChildIdx);
    subdivide(rightChildIdx);
}

bool BVH::intersect(
    const size_t nodeIdx,
    const Ray& r,
    const double tMin,
    const double tMax,
    HitRecord& rec,
    int depth) const
{
    const Node& node = nodes[nodeIdx];
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

bool BVH::intersectIterative(
    const size_t nodeIdx,
    const Ray& r,
    const double tMin,
    const double tMax,
    HitRecord& rec) const
{
    const Node* node = &nodes[rootNodeIdx];
    const Node* stack[64]; // Magic number stack size
    uint32_t stackPtr = 0;

    bool anyHit = false;
    HitRecord tempRec;
    auto closest = tMax;

    for (;;) {
        if (node->isLeaf()) {
            // Intersect leaf node's primitives
            for (size_t i = 0; i < node->primCount; i++) {
                auto prim = primitives[primIndices[node->firstPrimIdx + i]];
                if (prim->hit(r, tMin, closest, tempRec)) {
                    anyHit  = true;
                    closest = tempRec.t;
                    rec     = tempRec;
                    // if (!rec.mat)
                    //     std::cerr << "Returned record with nullptr material:
                    //     "<< prim <<"\n";
                }
            }
            if (!stackPtr) {
                // No more nodes to check, return current result
                return anyHit;
            } else {
                node = stack[--stackPtr];
            }

            continue;
        }

        const Node* child1 = &nodes[node->left()];
        const Node* child2 = &nodes[node->right()];
        HitRecord dist1, dist2;
        child1->aabb.hit(r, tMin, tMax, dist1);
        child2->aabb.hit(r, tMin, tMax, dist2);
        if (dist1.t > dist2.t) {
            // Sort children by nearest
            std::swap(dist1.t, dist2.t);
            std::swap(child1, child2);
        }
        if (dist1.t == 1e30) {
            if (!stackPtr) {
                // No more nodes to check, return current result
                return anyHit;
            } else
                node = stack[--stackPtr];
        } else {
            // Prepare child1
            node = child1;
            // Push child2 to stack for processing next
            if (dist2.t != 1e30) stack[stackPtr++] = child2;
        }
    }
}

}; // namespace blikker_pt3
