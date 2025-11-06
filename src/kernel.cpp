#include "kernel.hpp"
#include <cmath>
#include <iomanip>
#include <iostream>
#include <limits>
#include <numbers>
#include <string_view>
#include "structures.hpp"
#define _USE_MATH_DEFINES
#include <math.h>


namespace kernel {
    using namespace structures;

    Kernel::Kernel() {}
    void Kernel::initialize(std::vector<particle> particles, int particleCount, int dim, int h) {
        this->particles = particles;
        this->particleCount = particleCount;
        this->dim = dim;
        this->h = h;
        this->kernel.resize(particleCount);
        this->derivative.resize(particleCount);
    }

    void Kernel::calculateKernel() {
        double alpha = getFactor();
        for (int i = 0; i < particleCount; i++) {
            kernel[i].clear();
            for (int j : particles[i].neighbors) {
                if (j < i) continue;
                double q = getDistance(particles[i].pos, particles[j].pos, dim) / h;
                if (q < 1) {
                    kernel[i][j] = alpha * (pow(2 - q, 3) - 4 * pow(1 - q, 3));
                } else if (q < 2) {
                    kernel[i][j] = alpha * pow(2 - q, 3);
                } else {
                    kernel[i][j] = 0;
                }
            }
        }
    }

    void Kernel::calculateKernelDerivative() {
        double alpha = getFactor();
        for (int i = 0; i < particleCount; i++) {
            derivative[i].clear();
            for (int j : particles[i].neighbors) {
                if (j < i) continue;
                derivative[i][j].resize(dim);
                double dist = getDistance(particles[i].pos, particles[j].pos, dim);
                double inner = 0;
                if (dist < h) {
                    inner = -3 * pow(2 - dist / h, 2) + 12 * pow(1 - dist / h, 2);
                } else if (dist < 2 * h) {
                    inner = -3 * pow(2 - dist / h, 2);
                }
                double common = alpha / (dist * h) * inner;
                for (int k = 0; k < dim; k++) {
                    if (i == j) {
                        derivative[i][j][k] = 0;
                    } else {
                        derivative[i][j][k] = (particles[i].pos[k] - particles[j].pos[k]) * common;
                    }
                }
            }
        } 
    }

    double Kernel::getFactor() {
        if (dim == 1) return 1 / (6 * h);
        else if (dim == 2) return 5 / (14 * M_PI * pow(h, 2));
        else if (dim == 2) return 1 / (4 * M_PI * pow(h, 3));
        else throw "Maximum number of dimensions is 3";
    }

    double Kernel::getKernelEntry(int i, int j) {
        //Kernel is symmetric
        if (i <= j) return kernel[i][j];
        else return kernel[j][i];
    }

    std::vector<double> Kernel::getDerivativeEntry(int i, int j) {
        std::vector<double> basic;
        basic.resize(dim);
        //Derivative is antisymmetric
        if (i <= j && derivative[i].count(j) > 0) return derivative[i][j];
        else if (derivative[j].count(i) > 0) {
            for (int k = 0; k < dim; k++) {
                basic[k] = derivative[j][i][k] * -1;
            }
        }
        return basic;
    }
}