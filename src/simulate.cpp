#include "simulate.hpp"
#include "parser.hpp"
#include "kernel.hpp"
#include <iostream>
#include <algorithm>
#include <fstream>
#include <string>
namespace simulate {
    Simulator::Simulator(config conf) { 
        std::random_device r;
        std::default_random_engine e1(r());
        this->e1 = e1;
        this->conf = conf;
    }

    void Simulator::updateConf(config conf) {
        this->kernel->updateConf(conf);
        this->conf = conf;
    }

    config Simulator::parseFile(std::string path) {
        auto parser = parser::Parser();
        bool parsed = parser.open(path);
        std::cout << "Paths: " << path << "\n";
        if (!parsed) throw "File error";
        std::cout << "Opened file\n";
        this->conf = parser.getParsedConfig();
        this->conf.activeParticles = 0;
        this->particles.resize(conf.particleCount);
        std::uniform_int_distribution<> distRGB(0, 255);
        for (int i = 0; i < conf.particleCount; i++) {
            particle current = this->particles[i];
            parser.parseNextParticle(this->particles.data() + i);
            if (particles[i].isStationary) particles[i].rgb = {char(200), char(200), char(200)};
            else particles[i].rgb = {char(100), char(100), char(255)};
            particles[i].restDensity = particles[i].mass / std::pow(conf.h, 2);
            particles[i].acc.resize(conf.dim);
            if (!particles[i].isStationary) this->conf.activeParticles++;
        }
        parser.close();
        int uncheckedBoundary = conf.activeParticles;
        for (int i = 0; i < conf.activeParticles; i++) {
            //Invariant: Particles < i are active
            //Particles >= conf.activeParticles & < uncheckedBoundary are boundary
            if (particles[i].isStationary) {
                //Search for fluid particle after boundary
                while (!particles[uncheckedBoundary++].isStationary) {
                    if (uncheckedBoundary == conf.particleCount) {
                        throw "Out of bounds";
                    }
                }
                if (particles[uncheckedBoundary].isStationary) {
                    throw "Invalid fluid particle count";
                }
                particle tmp = particles[i];
                particles[i] = particles[uncheckedBoundary];
                particles[uncheckedBoundary] = tmp;
            }
        }
        return this->conf;
    }

    void Simulator::addKernel(kernel::Kernel* kernel) {
        this->kernel = kernel;
    }

    /**
    Simulate one time step for all particles
    */
    void Simulator::simulateStep() {
        computeAcceleration();
        performUpdateStep();
        checkBoundingBoxes();
    }

    void Simulator::computeAcceleration() {
        for (int i = 0; i < conf.activeParticles; i++) {
            particles[i].acc[0] = 0.0;
            particles[i].acc[1] = 0.0;
        }
        //std::cout << "Init: " << particles[0].pos[1] << "," << particles[0].speed[1] << ","<< particles[0].acc[1] << "\n";
        //auto t1 = clock();
        kernel->completeNeighborSearch();
        //auto t2 = clock();
        kernel->calculateKernel();
        //auto t3 = clock();
        kernel->calculateKernelDerivative();    
        //auto t4 = clock();
        computeExternalForces();
        //auto t5 = clock();
        //std::cout << "Ext: " << particles[0].pos[1] << "," << particles[0].speed[1] << ","<< particles[0].acc[1] << "\n";
        computePressureForces();
        //auto t6 = clock();
        //std::cout << "Pre: " << particles[0].pos[1] << "," << particles[0].speed[1] << ","<< particles[0].acc[1] << "\n";
        computeViscousForces();
        //std::cout << "Vic: " << particles[0].pos[1] << "," << particles[0].speed[1] << ","<< particles[0].acc[1] << "\n";
        //auto t7 = clock();
        /*
        kernelTime1 += (t2 - t1);
        kernelTime2 += (t3 - t2);
        kernelTime3 += (t4 - t3);
        forceTime1 += (t5 - t4);
        forceTime2 += (t6 - t5);
        forceTime3 += (t7 - t6);
        */
    }

    void Simulator::computeViscousForces() {
        for (int i = 0; i < conf.activeParticles; i++) {
            for (int k = 0; k < particles[i].neighbors.size(); k++) {
                int j = particles[i].neighbors[k];
                double dWX = particles[i].kernelDerivX[k];
                double dWY = particles[i].kernelDerivY[k];
                double coef = 2 * conf.nu * particles[j].mass / particles[j].density;
                if (particles[j].isStationary) {
                    coef = 2 * conf.nu * particles[j].mass / particles[i].density;
                }
                double vij0 = particles[i].speed[0] - particles[j].speed[0];
                double xij0 = particles[i].pos[0] - particles[j].pos[0];
                particles[i].acc[0] += coef * (vij0 * xij0) / (xij0 * xij0 + 0.01 * conf.h * conf.h) * dWX;

                double vij1 = particles[i].speed[1] - particles[j].speed[1];
                double xij1 = particles[i].pos[1] - particles[j].pos[1];
                particles[i].acc[1] += coef * (vij1 * xij1) / (xij1 * xij1 + 0.01 * conf.h * conf.h) * dWY;
            }
            //std::cout << "Acc: " << particles[i].acc[0] << ", " << particles[i].acc[1] << "\n";
        }
    }

    void Simulator::computePressureForces() {
        //std::cout << "Start pressure computation\n";
        double avgDensity = 0;
        double avgNeighbors = 0;
        for (int i = 0; i < conf.activeParticles; i++) {
            particles[i].density = 0.0;
            for (int k = 0; k < particles[i].neighbors.size(); k++) {
                particles[i].density += particles[particles[i].neighbors[k]].mass * particles[i].kernel[k];
            }
            avgDensity += particles[i].density;
            //double densityRatio = particles[i].density / particles[i].restDensity;
            particles[i].pressure = std::max(conf.k * ((particles[i].density / particles[i].restDensity) - 1), 0.0);
        }
        for (int i = 0; i < conf.activeParticles; i++) {
            for (int k = 0; k < particles[i].neighbors.size(); k++) {
                int j = particles[i].neighbors[k];
                double coef = particles[j].mass * (particles[i].pressure / (particles[i].density * particles[i].density) + 
                    particles[j].pressure / (particles[j].density * particles[j].density));
                if (particles[j].isStationary) {
                    coef = particles[j].mass * 2 * (particles[i].pressure / (particles[i].density * particles[i].density));
                }
                particles[i].acc[0] -= coef * particles[i].kernelDerivX[k];
                particles[i].acc[1] -= coef * particles[i].kernelDerivY[k];
            }
        }
    }

    double Simulator::getTotalEnergy() {
        double energy = 0;
        for (int i = 0; i < conf.activeParticles; i++) {
            energy += particles[i].pressure * conf.h * conf.h / conf.k;
            for (int d = 0; d < conf.dim; d++) {
                energy += 0.5 * particles[i].mass * particles[i].speed[d] * particles[i].speed[d];
                energy += conf.area[d] - particles[i].mass * conf.g[d] * particles[i].pos[d];
            }
        }
        return energy;
    }

    void Simulator::computeExternalForces() {
        for (int i = 0; i < conf.activeParticles; i++) {
            particles[i].acc[0] += conf.g[0];
            particles[i].acc[1] += conf.g[1];
        }
    }

    void Simulator::checkBoundingBoxes() {
        for (int i = 0; i < conf.activeParticles; i++) {
            if (particles[i].pos[0] > conf.area[0] || particles[i].pos[1] > conf.area[1] || particles[i].pos[0] < 0 || particles[i].pos[1] < 0) {
                particles[i] = particles[conf.activeParticles - 1];
                particles[conf.activeParticles] = particles[conf.particleCount - 1];
                particles.resize(conf.particleCount - 1);
                conf.activeParticles--;
                conf.particleCount--;
                kernel->updateConf(conf);
            }
        }
    }

    void Simulator::performUpdateStep() {
        for (int i = 0; i < conf.activeParticles; i++) {
            particles[i].speed[0] += particles[i].acc[0] * conf.timestep;
            particles[i].speed[1] += particles[i].acc[1] * conf.timestep;
            particles[i].pos[0] += particles[i].speed[0] * conf.timestep;
            particles[i].pos[1] += particles[i].speed[1] * conf.timestep;
        }
    }

    std::vector<particle> Simulator::getParticles() {
        return this->particles;
    }

    particle* Simulator::getParticleData() {
        return this->particles.data();
    }

    int sign(int x) {
        return (x > 0) - (x < 0);
    }
}