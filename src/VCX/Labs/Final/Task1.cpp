#include "Labs/Final/Task1.h"
#include <iostream>
#include <functional>
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// #include "Labs/Final/Sphere.h"
// #include "Labs/Final/BVH.h"

namespace VCX::Labs::Final {
    double erand48(unsigned short xsubi[3]){
        return (double) rand() / (double)RAND_MAX;
    }

    // 建立模型
    Sphere spheres[]={
        Sphere(1e5,  Vec(1e5+1, 40.8, 81.6),   Vec(), Vec(0.75,0.25,0.25),DIFFUSE), //left wall
        Sphere(1e5,  Vec(-1e5+99, 40.8, 81.6), Vec(), Vec(0.25,0.25,0.75),DIFFUSE), //right wall
        Sphere(1e5,  Vec(50, 40.8, 1e5),       Vec(), Vec(0.75,0.75,0.75),DIFFUSE), //back wall
        Sphere(1e5,  Vec(50, 40.8, -1e5+170),  Vec(), Vec(),DIFFUSE), //front wall
        Sphere(1e5,  Vec(50, 1e5, 81.6),       Vec(), Vec(0.75,0.75,0.75),DIFFUSE), //bottom wall
        Sphere(1e5,  Vec(50, -1e5+81.6, 81.6), Vec(), Vec(0.75,0.75,0.75),DIFFUSE), //top wall
        Sphere(16.5, Vec(27, 16.5, 47),        Vec(), Vec(1,1,1)*0.999,SPECULAR), //mirror
        Sphere(16.5, Vec(73, 16.5, 78),        Vec(), Vec(1,1,1)*0.999,REFRACTION), //glass
        Sphere(1.5,  Vec(50, 81.6-16.5, 81.6), Vec(4,4,4)*200,Vec(),DIFFUSE), //light
    };

    int numSpheres = sizeof(spheres) / sizeof(Sphere);

    Vec radiance(const Ray &ray, int depth, unsigned short *Xi, const BVHNode *bvhRoot, const std::vector<Sphere> &spheres,const std::vector<int> &indices, int E) {
        // if (depth > 0) std::cout << "Radiance depth: " << depth << std::endl;
        double t = 1e20; // ray 参数：r = o + d * t
        int id = 0;      // 物体 ID
        if (!intersect_BVH(ray, bvhRoot, spheres, indices, t, id)) return Vec();
        const Sphere &obj = spheres[id];
        if (depth > 10) return Vec(); // 避免过深
        Vec cross = ray.origin + ray.direction * t; // 交点
        Vec normal = (cross - obj.position).norm(); // 球面法向量优势区间
        Vec nl = normal.dot(ray.direction) < 0 ? normal : normal * (-1);
        Vec color = obj.color;

        // 如果是发射体，直接返回 emission
        // if (obj.emission.x > 0 || obj.emission.y > 0 || obj.emission.z > 0) return obj.emission;
        // 俄罗斯轮盘赌，depth > 5 开始
        double p = color.x>color.y && color.x>color.z ? color.x : color.y>color.z ? color.y :color.z;
        if (depth >= 5 || !p) {
            if (erand48(Xi) < p) color = color * (1 / p);
            else return  obj.emission * E; // E:Emission?
        }
        depth++;
        // 漫反射
        if (obj.ref == DIFFUSE) {
            double r1 = 2 * M_PI * erand48(Xi);
            double r2 = erand48(Xi), r2s = sqrt(r2); 
            //create orthonormal coordinate (w,u,v)
            Vec w = nl;
            Vec u = ((fabs(w.x) > 0.1 ? Vec(0,1) : Vec(1)) % w).norm();
            Vec v = w % u;
            Vec d = (u * cos(r1) * r2s + v * sin(r1) * r2s + w * sqrt(1 - r2)).norm(); //采样方向

            //loop over lights
            Vec e;
            for(int i = 0; i<numSpheres; i++){
                const Sphere &s = spheres[i];
                if(s.emission.x <= 0 && s.emission.y <= 0 && s.emission.z <= 0) continue; //skip non-lights
                //create random direction towards sphere
                Vec sw = s.position - cross, su=((fabs(sw.x) > 0.1 ? Vec(0,1) : Vec(1)) % sw).norm(), sv=sw % su; //create coordinate system for sampling: sw,su,sv
                double cos_a_max = sqrt(1 - s.radius * s.radius / (cross - s.position).dot(cross - s.position)); //determine max angle
                //calculate sample direction based on randam numbers
                double eps1 = erand48(Xi), eps2 = erand48(Xi);
                double cos_a = 1-eps1 + eps1 * cos_a_max;
                double sin_a = sqrt(1 - cos_a * cos_a);
                double phi = 2 * M_PI * eps2;
                Vec l = su * cos(phi) * sin_a + sv * sin(phi) * sin_a + sw * cos_a;
                l.norm();
                //create shadow ray
                //if (intersect(Ray(x,l),spheres,numSpheres,t,id) && id == i){
                double shadowT = 1e20;
                if (intersect_BVH(Ray(cross,l),bvhRoot, spheres,indices, shadowT,id) && id == i){
                    double omega = 2*M_PI*(1-cos_a_max); //计算立体角（solid angle）
                    e = e + color.mult(s.emission*l.dot(nl)*omega)*M_1_PI;
                }
            }//loop over lights end
            return obj.emission * E + e + color.mult(radiance(Ray(cross, d), depth, Xi, bvhRoot, spheres, indices, 0));
        }
        // 理想镜面反射
        else if (obj.ref == SPECULAR) {
            return obj.emission + color.mult(
                radiance(Ray(cross, ray.direction - normal * 2 * normal.dot(ray.direction)), depth, Xi, bvhRoot, spheres, indices)
            );
        }
        // 其他
        Ray reflRay(cross, ray.direction - normal * 2 * normal.dot(ray.direction));
        bool into = normal.dot(nl) > 0; //ray from outside going in?
        //nc：起始介质折射率 nt：目标介质折射率（glass IOR=1.5） nnt：比值 ddn：入射角余弦
        double nc =1, nt=1.5, nnt = into ? nc / nt : nt / nc, ddn = ray.direction.dot(nl), cos2t;

        //判断是否发生全反射 cos2t=光线折射角的余弦平方值，cos2t<0全反射
        if((cos2t = 1 - nnt * nnt * (1 - ddn * ddn)) < 0){
            return obj.emission + color.mult(radiance(reflRay, depth, Xi, bvhRoot, spheres, indices));
        }
        //otherwise反射或折射
        Vec tdir = (ray.direction * nnt - normal * ((into ? 1 :-1) * (ddn * nnt + sqrt(cos2t)))).norm(); //折射光线方向
        double a = nt - nc, b = nt + nc, R0 = a * a / (b * b), c = 1 - (into ? -ddn : tdir.dot(normal)); //R0:垂直入射时的菲涅耳反射系数，c=1-cos(theta)
        double Re = R0 + (1-R0) * c * c * c * c * c; // Schlick近似公式求菲涅耳反射系数
        double Tr = 1 - Re;//折射比例
        //定义概率p，调整权重
        double P=0.25 + 0.5 * Re, RP = Re / P, TP = Tr / (1 - P);
        //depth>2则轮盘赌，erand48(Xi)<P折射，否则反射
        return obj.emission + color.mult(depth > 2 ? (erand48(Xi) < P ?
            radiance(reflRay, depth, Xi,bvhRoot,spheres,indices) * RP : radiance(Ray(cross,tdir), depth, Xi, bvhRoot, spheres, indices) * TP) :
            radiance(reflRay, depth, Xi,bvhRoot,spheres,indices) * Re + radiance(Ray(cross,tdir), depth, Xi, bvhRoot, spheres, indices) * Tr);
    }

    using ProgressCallback = std::function<void(float)>;

    Vec* PathTrace(int w, int h, int samps, ProgressCallback progressCallback) {
        std::cout << "PathTrace start" << std::endl;
        Ray camera(Vec(50, 52, 295.6), Vec(0, -0.042612, -1).norm());
        Vec cx = Vec(w * 0.5135 / h);
        Vec cy = (cx % camera.direction).norm() * 0.5135;
        Vec r;
        Vec* c = new Vec[w * h]; // Resulting image
        // Initialize image buffer to zero
        for (int i = 0; i < w * h; i++) {
            c[i] = Vec(0, 0, 0);
        }
        // Build BVH tree
        std::vector<Sphere> spheresVec(spheres, spheres + numSpheres);
        std::vector<int> indices(numSpheres);
        for (int i = 0; i < numSpheres; i++) indices[i] = i;
        std::cout << "Building BVH" << std::endl;
        BVHNode* bvhRoot = build(indices, spheresVec, 0, numSpheres);
        std::cout << "BVH built" << std::endl;
        // 并行 for 循环
        #pragma omp parallel for schedule(dynamic, 1) private(r)
        for(int y = 0; y < h; y++) {
            // 更新进度
            // fprintf(stderr,"\rRendering (%d spp) %5.2f%%\n", samps*4,100.*y/(h-1)); //打印进度
            if(progressCallback){
                progressCallback(static_cast<float>(y)/h);
            }
            unsigned short Xi[3] = {0, 0, static_cast<unsigned short>(y * y * y)};
            for(int x = 0; x < w; x++) {
                int pixelIndex = (h - y - 1) * w + x;
                // 2x2 超采样
                // std::cout << "Start SS" << std::endl;
                for(int sy = 0; sy < 2; sy++) {
                    for(int sx = 0; sx < 2; sx++) {
                        r = Vec();
                        for (int s = 0; s < samps; s++) {
                            double r1 = 2 * erand48(Xi), dx = r1 < 1 ? sqrt(r1) - 1 : 1 - sqrt(2 - r1);
                            double r2 = 2 * erand48(Xi), dy = r2 < 1 ? sqrt(r2) - 1 : 1 - sqrt(2 - r2);
                            Vec d = cx * (((sx + 0.5 + dx) / 2 + x) / w - 0.5) +
                                    cy * (((sy + 0.5 + dy) / 2 + y) / h - 0.5) + camera.direction;
                            r = r + radiance(Ray(camera.origin + d * 140, d.norm()), 0, Xi, bvhRoot, spheresVec, indices) * (1. / samps);
                        }
                        // std::cout << r.x << " " << r.y << " " << r.z << std::endl;
                        c[pixelIndex] = c[pixelIndex] + Vec(clamp(r.x), clamp(r.y), clamp(r.z)) * 0.25;
                    }
                }
            }
        }
        if(progressCallback){
            progressCallback(1.0f);
        }
        std::cout << "PathTrace done" << std::endl;
        return c;
    }
}