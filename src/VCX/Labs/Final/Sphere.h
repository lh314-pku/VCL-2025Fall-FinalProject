#pragma once

#include <cmath>
#include <vector>
#include <algorithm>
#include <stdexcept>
namespace VCX::Labs::Final {
struct Vec {
    double x, y, z;
    Vec(double x_=0, double y_=0, double z_=0){ 
        x = x_; y = y_; z = z_;
    } 
    Vec operator+(const Vec &b) const { return Vec(x + b.x, y + b.y, z + b.z); }
    Vec operator-(const Vec &b) const { return Vec(x - b.x, y - b.y, z - b.z); }
    Vec operator*(double b) const { return Vec(x * b, y * b, z * b); }
    Vec mult(const Vec &b) const { return Vec(x * b.x, y * b.y, z * b.z); }
    double dot(const Vec &b) const { return x * b.x + y * b.y + z * b.z; }
    Vec operator%(Vec & b) {return Vec(y * b.z - z * b.y , z * b.x - x * b.z, x * b.y - y * b.x);}
    // 四种乘法：向量与数*、对应位置相乘mult、点乘dot、叉乘%
    Vec& norm(){ return *this = *this * (1 / sqrt(x * x + y * y + z * z)); }

    // 添加索引
    double operator[](int index) const {
        switch(index) {
            case 0 : return x;
            case 1 : return y;
            case 2 : return z;
        }
        throw std::out_of_range("Index out of range for Vec");
    }
}; 

struct Ray {
    Vec origin, direction;
    Ray (Vec o_, Vec d_) : origin(o_), direction(d_) {}
};

enum Reflect_type {
    DIFFUSE,   // 漫反射
    SPECULAR,  // 镜面反射
    REFRACTION // 折射
};

// 有个槽点：这里相当于用半径极大的球面的一部分模拟平面
// 导致这个模型不方便引入其他形状
struct Sphere {
    double radius;
    Vec position, emission, color;
    Reflect_type ref;
    Sphere (double r_, Vec pos_, Vec emiss_, Vec color_, Reflect_type refl_t) :
        radius(r_), position(pos_), emission(emiss_), color(color_), ref(refl_t) {}
    // 求交（参数化会非常方便） return distance
    double intersect(const Ray &r) const {
        Vec op = position - r.origin;
        double t;
        double eps = 1e-4;
        double b = op.dot(r.direction);
        double det = b * b - op.dot(op) + radius * radius;
        if (det < 0) return 0;
        else det = sqrt(det);
        return (t = b - det) > eps ? t : ( (t = b + det) > eps ? t : 0 );
    }
};
}