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

    Kernel::Kernel(config conf) {
        this->conf = conf;
    }

    void Kernel::updateConf(config conf) {
        this->conf = conf;
    }

    void Kernel::initialize(particle* particles) {
        this->particles = particles;
        this->kernel.resize(conf.particleCount);
        this->derivative.resize(conf.particleCount);
    }

    void Kernel::completeNeighborSearch() {
        quadraticNeighborSearch();
    }

    void Kernel::quadraticNeighborSearch() {
        for (int i = 0; i < conf.activeParticles; i++) {
            particles[i].neighbors.clear();
        }
        for (int i = 0; i < conf.activeParticles; i++) {
            particles[i].neighbors.push_back(i);
            for (int j = i + 1; j < conf.particleCount; j++) {
                double dist2 = getSquaredDistance(particles[i].pos, particles[j].pos, conf.dim);
                if (dist2 < std::pow(conf.kernelSupport * conf.h, 2)) {
                    particles[i].neighbors.push_back(j);
                }
                if (dist2 < std::pow(conf.kernelSupport * conf.h, 2)) {
                    particles[j].neighbors.push_back(i);
                }
            }
        }
        /*
        for (int i = 0; i < conf.activeParticles; i++) {
            std::cout << i << ": " << particles[i].neighbors.size() << "\n";
        }
            */
    }

    std::vector<int> Kernel::specificNeighborSearch(std::vector<double> pos) {
        std::vector<int> neighbors;
        for (int i = 0; i < conf.particleCount; i++) {
            double dist2 = getSquaredDistance(particles[i].pos, pos, conf.dim);
            if (dist2 < std::pow(conf.kernelSupport * conf.h, 2)) {
                neighbors.push_back(i);
                particles[i].rgb = {char(200), 0, 0};
            } else {
                particles[i].rgb = {char(200), char(200), char(200)};
            }
        }
        return neighbors;
    }

    void Kernel::calculateKernel() {
        double alpha = getFactor();
        for (int i = 0; i < conf.activeParticles; i++) {
            kernel[i].clear();
            for (int j : particles[i].neighbors) {
                if (j < i) continue;
                double q = getDistance(particles[i].pos, particles[j].pos, conf.dim) / conf.h;
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
        for (int i = 0; i < conf.activeParticles; i++) {
            derivative[i].clear();
            for (int j : particles[i].neighbors) {
                if (j < i) continue;
                derivative[i][j].resize(conf.dim);
                double dist = getDistance(particles[i].pos, particles[j].pos, conf.dim);
                double inner = 0;
                if (dist < conf.h) {
                    inner = -3 * pow(2 - dist / conf.h, 2) + 12 * pow(1 - dist / conf.h, 2);
                } else if (dist < 2 * conf.h) {
                    inner = -3 * pow(2 - dist / conf.h, 2);
                }
                double common = 0;
                //Ignore elements with really low distance to avoid underflow
                if (dist > 0.001 * conf.h) {
                    common = alpha / (dist * conf.h) * inner;
                }
                for (int k = 0; k < conf.dim; k++) {
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
        if (conf.dim == 1) return 1 / (6 * conf.h);
        else if (conf.dim == 2) return 5 / (14 * M_PI * pow(conf.h, 2));
        else if (conf.dim == 2) return 1 / (4 * M_PI * pow(conf.h, 3));
        else {
            throw "Maximum number of dimensions is 3";
        };
    }

    double Kernel::getKernelEntry(int i, int j) {
        //Kernel is symmetric
        if (i <= j) return kernel[i][j];
        else return kernel[j][i];
    }

    std::vector<double> Kernel::getDerivativeEntry(int i, int j) {
        std::vector<double> basic;
        basic.resize(conf.dim);
        //Derivative is antisymmetric
        if (i <= j && derivative[i].count(j) > 0) return derivative[i][j];
        else if (derivative[j].count(i) > 0) {
            for (int k = 0; k < conf.dim; k++) {
                basic[k] = derivative[j][i][k] * -1;
            }
        }
        return basic;
    }
}