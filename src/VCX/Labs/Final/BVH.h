#pragma once

#include <cmath>
#include <vector>
#include <algorithm>

#include "Labs/Final/Sphere.h"
// BVH, Bounding Volume Hierarchy 加速
namespace VCX::Labs::Final {
// AABB: 轴对齐包围盒
struct AABB {
    Vec max, min;
    AABB();
    AABB(const Vec &min_, const Vec &max_);
    // 检查光线与包围盒是否相交
    bool intersect(const Ray &r, double &tmin, double &tmax) const;
    // 合并两个包围盒
    static AABB merge(const AABB &a, const AABB &b);
};

struct BVHNode {
    AABB box;
    BVHNode *left;
    BVHNode *right; // 左右叶子节点
    int start, end;
    BVHNode() : start(0),end(0), left(nullptr), right(nullptr) {}
};

// 建树
BVHNode *build(std::vector<int> &indices, const std::vector<Sphere> &spheres, int start, int end);
// BVH 求交
bool intersect_BVH(const Ray &r, const BVHNode *node, const std::vector<Sphere> &spheres, const std::vector<int> &indices, double &t, int &id);
}