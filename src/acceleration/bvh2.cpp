/// @file BVH implementation
#include "bvh2.hpp"

#include "aabb.hpp"

#include <iostream>
#include <sstream>

blikker_pt2::BVH::BVH(const std::vector<shared_ptr<Primitive>>& primitives)
    : primitives(primitives)
    , N(primitives.size()) {
    // Upper limit of tree size.
    nodes.resize(N * 2);
    // Populate index list
    primIndices.resize(N);
    for (size_t i = 0; i < N; i++) primIndices[i] = i;

    Node& root      = nodes[rootNodeIdx];
    root.mLeftChildIdx = 0;
    root.firstPrimIdx  = 0;
    root.primCount     = N;
    updateNodeBounds(rootNodeIdx);
    // std::cerr << "Start recursive subdivide()\n";
    subdivide(rootNodeIdx);
}

std::string blikker_pt2::BVH::tree(size_t nodeIdx, int depth) const {
    const Node& node = nodes[nodeIdx];
    int indent          = depth;
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

void blikker_pt2::BVH::updateNodeBounds(const size_t nodeIdx) {
    Node& node = nodes[nodeIdx];
    node.aabb.min = Vec3(infinity);
    node.aabb.max = Vec3(-infinity);
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

void blikker_pt2::BVH::subdivide(const size_t nodeIdx) {
    Node& node = nodes[nodeIdx];
    // 1. Determine the axis and position of the split plane, using SAH.

    // 1a. Exhaustive SAH evaluation to find best split.
    int bestAxis   = -1;
    float bestPos = 0;
    float bestCost = infinity;

    for (int axis = 0; axis < 3; axis++)
        for (int i = 0; i < node.primCount; i++) {
            auto tri = primitives[primIndices[node.firstPrimIdx + i]];
            float candidatePos = tri->centroid()[axis];
            float cost          = evaluateSAH(node, axis, candidatePos);
            if (cost < bestCost) {
                bestCost = cost;
                bestPos  = candidatePos;
                bestAxis = axis;
            }
        }

    // 1b. Evaluate if a split is actually improving from the parent node.
    float parentArea = node.aabb.area();
    float parentCost = node.primCount * parentArea;

    if (bestCost >= parentCost) {
        return;
    }

    int axis       = bestAxis;
    float splitPos = bestPos;

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

bool blikker_pt2::BVH::intersect(
    const size_t nodeIdx,
    const Ray& r,
    const float tMin,
    const float tMax,
    HitRecord& rec,
    int depth) const {
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

bool blikker_pt2::BVH::intersectIterative(
    const size_t nodeIdx,
    const Ray& r,
    const float tMin,
    const float tMax,
    HitRecord& rec) const {
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
                    anyHit = true;
                    closest = tempRec.t;
                    rec = tempRec;
                    // if (!rec.mat)
                    //     std::cerr << "Returned record with nullptr material: "<< prim <<"\n";
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
        if (dist1.t == infinity) {
            if (!stackPtr) {
                // No more nodes to check, return current result
                return anyHit;
            }
            else node = stack[--stackPtr];
        } else {
            // Prepare child1
            node = child1;
            // Push child2 to stack for processing next
            if (dist2.t != infinity) stack[stackPtr++] = child2;
        }
    }
}
