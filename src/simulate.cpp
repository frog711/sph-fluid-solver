#include "simulate.hpp"
#include "parser.hpp"
#include <iostream>
#include <fstream>
#include <string>
namespace simulate {
    Simulator::Simulator() { 
        std::random_device r;
        std::default_random_engine e1(r());
        this->e1 = e1;
    }

    void Simulator::parseFile(std::string path) {
        parser::Parser parser;
        parser.open(path);
        this->particleCount = parser.getParticleCount();
        this->dimensions = parser.getDimension();
        this->bounds = parser.getBounds();
        this->particles.resize(this->particleCount);
        std::cout << "P: " << particleCount << "\n";
        std::cout << "A: " << particles.size() << "\n";
        std::uniform_int_distribution<> distRGB(0, 255);
        for (int i = 0; i < particleCount; i++) {
            particle current = this->particles[i];
            parser.parseNextParticle(this->particles.data() + i);
            particles[i].rgb = {char(200), char(200), char(200)};
        }
    }

    void Simulator::setupParticles(std::vector<int> particleCount, std::vector<int> limit) {
        this->particleCount = particleCount[0] * particleCount[1];
        this->bounds = limit;
        double size = std::min(limit[0] / particleCount[0], limit[1] / particleCount[1]) / 2;
        particles.resize(this->particleCount);
        std::uniform_int_distribution<> distX(0, bounds[0]);
        std::uniform_int_distribution<> distY(0, bounds[1]);
        std::uniform_real_distribution<> distV(-2, 2);
        std::uniform_int_distribution<> distRGB(0, 255);
        for (int x = 0; x < particleCount[0]; x++) {
            for (int y = 0; y < particleCount[1]; y++) {
                int i = y * particleCount[1] + x;
                particles[i].size = int(size);
                particles[i].pos = {std::cos(0.5) * size * x + std::sin(0.5) * size * y + size / 2, -std::sin(0.5) * size * x + std::cos(0.5) * size * y + size / 2};
                particles[i].speed = {double(distV(e1)), double(distV(e1))};
                particles[i].rgb = {char(distRGB(e1)), char(distRGB(e1)), char(distRGB(e1))};
            }
        }
    }

    /**
    Simulate one time step for all particles
    Only for 2d so far
    */
    void Simulator::simulateStepRandom() {
        std::uniform_real_distribution<> distAcc(-1, 1);
        std::uniform_int_distribution<> distX(0, bounds[0]);
        std::uniform_int_distribution<> distY(0, bounds[0]);
        for (int i = 0; i < particleCount; i++) {
            particles[i].speed[0] = particles[i].speed[0] + distAcc(e1);
            particles[i].speed[1] = particles[i].speed[1] + distAcc(e1);
            if (particles[i].speed[0] > 3 || particles[i].speed[0] < -3) {
                particles[i].speed[0] = 3 * sign(particles[i].speed[0]);
            }
            if (particles[i].speed[1] > 3 || particles[i].speed[1] < -3) {
                particles[i].speed[1] = 3 * sign(particles[i].speed[1]);
            }
            particles[i].pos[0] = particles[i].pos[0] + particles[i].speed[0];
            particles[i].pos[1] = particles[i].pos[1] + particles[i].speed[1];
            if (particles[i].pos[0] > bounds[0] || particles[i].pos[0] < 0 ||
                particles[i].pos[1] > bounds[1] || particles[i].pos[1] < 0) {
                particles[i].pos[0] = distX(e1);
                particles[i].pos[1] = distY(e1);
            }
        }
    }

    /**
    Simulate one time step for all particles
    */
    void Simulator::simulateStepSimple() {
        for (int i = 0; i < particleCount; i++) {
            for (int j = 0; j < dimensions; j++) {
                particles[i].pos[j] = particles[i].pos[j] + particles[i].speed[j];
            }
        }
    }

    void Simulator::completeNeighborSearch() {
        quadraticNeighborSearch();
    }

    void Simulator::quadraticNeighborSearch() {
        for (int i = 0; i < particleCount; i++) {
            particles[i].neighbors.clear();
        }
        for (int i = 0; i < particleCount; i++) {
            particles[i].neighbors.push_back(i);
            for (int j = i + 1; j < particleCount; j++) {
                double dist2 = getSquaredDistance(particles[i].pos, particles[j].pos, dimensions);
                if (dist2 < std::pow(kernelSupport * particles[i].size, 2)) {
                    particles[i].neighbors.push_back(j);
                }
                if (dist2 < std::pow(kernelSupport * particles[j].size, 2)) {
                    particles[j].neighbors.push_back(i);
                }
            }
        }
        for (int i = 0; i < particleCount; i++) {
            std::cout << i << ": " << particles[i].neighbors.size() << "\n";
        }
    }

    std::vector<int> Simulator::specificNeighborSearch(std::vector<double> pos) {
        std::vector<int> neighbors;
        for (int i = 0; i < particleCount; i++) {
            double dist2 = getSquaredDistance(particles[i].pos, pos, dimensions);
            if (dist2 < std::pow(kernelSupport * particles[i].size, 2)) {
                neighbors.push_back(i);
                particles[i].rgb = {char(200), 0, 0};
            } else {
                particles[i].rgb = {char(200), char(200), char(200)};
            }
        }
        return neighbors;
    }

    std::vector<particle> Simulator::getParticles() {
        return this->particles;
    }

    int sign(int x) {
        return (x > 0) - (x < 0);
    }
}