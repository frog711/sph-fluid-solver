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
        if (!parsed) throw "File error";
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
    }

    void Simulator::computeAcceleration() {
        for (int i = 0; i < conf.particleCount; i++) {
            for (int d = 0; d < conf.dim; d++) particles[i].acc[d] = 0.0;
        }
        //std::cout << "Init: " << particles[0].pos[1] << "," << particles[0].speed[1] << ","<< particles[0].acc[1] << "\n";
        kernel->completeNeighborSearch();
        kernel->calculateKernel();
        kernel->calculateKernelDerivative();
        computeExternalForces();
        //std::cout << "Ext: " << particles[0].pos[1] << "," << particles[0].speed[1] << ","<< particles[0].acc[1] << "\n";
        computePressureForces();
        //std::cout << "Pre: " << particles[0].pos[1] << "," << particles[0].speed[1] << ","<< particles[0].acc[1] << "\n";
        computeViscousForces();
        //std::cout << "Vic: " << particles[0].pos[1] << "," << particles[0].speed[1] << ","<< particles[0].acc[1] << "\n";
    }

    void Simulator::computeViscousForces() {
        for (int i = 0; i < conf.activeParticles; i++) {
            for (int k = 0; k < particles[i].neighbors.size(); k++) {
                int j = particles[i].neighbors[k];
                std::vector<double> dW = kernel->getDerivativeEntry(i, j);
                double coef = 2 * conf.nu * particles[j].mass / particles[j].density;
                if (particles[j].isStationary) {
                    coef = 2 * conf.nu * particles[j].mass / particles[i].density;
                }
                double vij0 = particles[i].speed[0] - particles[j].speed[0];
                double xij0 = particles[i].pos[0] - particles[j].pos[0];
                particles[i].acc[0] += coef * (vij0 * xij0) / (xij0 * xij0 + 0.01 * conf.h * conf.h) * dW[0];

                double vij1 = particles[i].speed[1] - particles[j].speed[1];
                double xij1 = particles[i].pos[1] - particles[j].pos[1];
                particles[i].acc[1] += coef * (vij1 * xij1) / (xij1 * xij1 + 0.01 * conf.h * conf.h) * dW[1];
            }
            //std::cout << "Acc: " << particles[i].acc[0] << ", " << particles[i].acc[1] << "\n";
        }
    }

    void Simulator::computePressureForces() {
        //std::cout << "Start pressure computation\n";
        for (int i = 0; i < conf.activeParticles; i++) {
            particles[i].density = 0.0;
            for (int k = 0; k < particles[i].neighbors.size(); k++) {
                int j = particles[i].neighbors[k];
                particles[i].density += particles[j].mass * kernel->getKernelEntry(i, j);
            }
            particles[i].pressure = std::max(conf.k * (particles[i].density / particles[i].restDensity - 1), 0.0);
            //std::cout << "Neighbor count: " << particles[i].neighbors.size() << "\n";
            //std::cout << "Density: " << particles[i].density << " Pressure: " << particles[i].pressure << "\n";
        }
        for (int i = 0; i < conf.activeParticles; i++) {
            for (int k = 0; k < particles[i].neighbors.size(); k++) {
                int j = particles[i].neighbors[k];
                std::vector<double> dW = kernel->getDerivativeEntry(i, j);
                double coef = particles[j].mass * (particles[i].pressure / std::pow(particles[i].density, 2) + 
                    particles[j].pressure / std::pow(particles[j].density, 2));
                if (particles[j].isStationary) {
                    coef = particles[j].mass * 2 * (particles[i].pressure / std::pow(particles[i].density, 2));
                }
                particles[i].acc[0] -= coef * dW[0];
                particles[i].acc[1] -= coef * dW[1];
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

    void Simulator::performUpdateStep() {
        for (int i = 0; i < conf.activeParticles; i++) {
            for (int d = 0; d < conf.dim; d++) {
                particles[i].speed[d] += particles[i].acc[d] * conf.timestep;
                particles[i].pos[d] += particles[i].speed[d] * conf.timestep;
            }
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