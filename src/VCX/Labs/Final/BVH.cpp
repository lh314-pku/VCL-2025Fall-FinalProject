#include "Labs/Final/BVH.h"

namespace VCX::Labs::Final {
    AABB::AABB() :
        min(Vec(1e20, 1e20, 1e20)), max(Vec(-1e20, -1e20, -1e20))
    {}
    AABB::AABB(const Vec &min_, const Vec &max_) :
        min(min_), max(max_)
    {}
    bool AABB::intersect(const Ray &r, double &tmin, double &tmax) const {
        // 遍历 3 个维度（x, y, z）
        for (int i = 0; i < 3; i++) {
            double invD = 1.0 / r.direction[i];//光线与平面x=min[i]相交的t=（min[i]-r.o[i])/r.d[i]
            double t0 = (min[i] - r.origin[i]) * invD;//ray=r.o+t*r.d,进入包围盒的t
            double t1 = (max[i] - r.origin[i]) * invD;//离开包围盒的t
            if (invD < 0.0) std::swap(t0, t1); //若光线方向为负，交换t0，t1
            tmin = t0 > tmin ? t0 : tmin;//光线进入包围盒
            tmax = t1 < tmax ? t1 : tmax;//光线离开包围盒
            if (tmax <= tmin) return false;
        }
        return true; 
    }
    AABB AABB::merge(const AABB &a, const AABB &b) {
        // 合并两个包围盒的最小点
        Vec merge_min;
        merge_min.x = std::min(a.min.x, b.min.x);
        merge_min.y = std::min(a.min.y, b.min.y);
        merge_min.z = std::min(a.min.z, b.min.z);
        // 合并两个包围盒的最大点
        Vec merge_max;
        merge_max.x = std::max(a.max.x, b.max.x);
        merge_max.y = std::max(a.max.y, b.max.y);
        merge_max.z = std::max(a.max.z, b.max.z);
        // 返回新的包围盒
        return AABB(merge_min, merge_max);
    }

    BVHNode *build(std::vector<int> &indices, const std::vector<Sphere> &spheres, int start, int end) {
        BVHNode *node = new BVHNode();
        // 计算当前节点的包围盒
        AABB box;
        for (int i = start; i < end; i++) {
            const Sphere &s = spheres[indices[i]];
            Vec sphereMin = s.position - Vec(s.radius, s.radius, s.radius);
            Vec sphereMax = s.position + Vec(s.radius, s.radius, s.radius);
            box = AABB::merge(box, AABB(sphereMin, sphereMax));
        }
        node->box = box;

        // 如果是叶节点：不再划分，返回node
        if (end - start <= 1) {
            node->start = start;
            node->end = end;
            return node;
        }

        // 划分空间，按坐标轴排序（x轴）
        int axis = 0;
        //依据中心点对indices排序，范围为start～end
        //！！indice才是真正记录了排序后的球，不是sphere下标！！
        std::sort(indices.begin() + start, indices.begin() + end, [&](int a, int b) {
            return spheres[a].position[axis] < spheres[b].position[axis];
        });

        // 二分递归构建左右子树
        int mid = (start + end) / 2;
        node->left = build(indices, spheres, start, mid);
        node->right = build(indices, spheres, mid, end);
        return node;
    }

    bool intersect_BVH(const Ray &r, const BVHNode *node, const std::vector<Sphere> &spheres, const std::vector<int> &indices, double &t, int &id) {
        double tmin = 0, tmax = 1e20;
        if (!node->box.intersect(r, tmin, tmax)) return false; // 光线未与包围盒相交

        if (node->left == nullptr && node->right == nullptr) {
            // 没有子节点：自己是叶节点，测试光线与球体相交
            bool hit = false;
            for (int i = node->start; i < node->end; i++) {
                int sphereIdx = indices[i]; 
                double d = spheres[sphereIdx].intersect(r);
                if (d && d < t) {
                    t = d;
                    id = sphereIdx;
                    hit = true;
                }
            }
            return hit;
        }
        // 检查左右子树
        bool hitLeft = node->left && intersect_BVH(r, node->left, spheres, indices,t, id);
        bool hitRight = node->right && intersect_BVH(r, node->right, spheres, indices, t, id);
        return hitLeft || hitRight;//光线与左右子树中的任意一个几何体相交
    }
}