#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <vector>
#include <functional>
#include <algorithm>
#include <iostream>


#include "Labs/Final/Sphere.h"
#include "Labs/Final/BVH.h"
/**
 * 此处考虑过类似 RayTracing 部分的现成模型
 * 但是渲染慢（三角网格而非参数模型）、模型复杂（没有形成的模型，Sphere面数太多）
 * 故考虑完全复制 c++ 版本的 smallpt(经过了重排版)
 */

namespace VCX::Labs::Final {
    // 建立模型
    extern Sphere spheres[];
    extern int numSpheres;
    // 内联辅助函数
    inline double clamp(double x) {
        return x < 0 ? 0 : x > 1 ? 1 : x;
    } // 截断
    inline double correct(double x) {
        return pow(clamp(x), 1/2.2);
    } // 伽马矫正
    inline bool intersect(const Ray &r, double &t, int &id) {
        double d, inf = t = 1e20;
        for(int i = numSpheres ; i--;)
            if((d = spheres[i].intersect(r)) && d < t) {
                t = d;
                id = i;
            }
        return t < inf;
    }

    // 核心函数:Path Tacing
    Vec radiance(const Ray &ray, int depth, unsigned short *Xi, const BVHNode *bvhRoot, const std::vector<Sphere> &spheres,const std::vector<int> &indices, int E = 1);
    using ProgressCallback = std::function<void(float)>;
    // 入口（改编自main）
    Vec* PathTrace(int w, int h, int samps, ProgressCallback progressCallback);
}