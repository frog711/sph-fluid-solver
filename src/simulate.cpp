#include "simulate.hpp"
namespace simulate {
    Simulator::Simulator() { 
        std::random_device r;
        std::default_random_engine e1(r());
        this->e1 = e1;
    }

    void Simulator::setupParticles(int particleCount, std::vector<int> limit) {
        this->particleCount = particleCount;
        this->bounds = limit;
        particles.resize(particleCount);
        std::uniform_int_distribution<> distX(0, bounds[0]);
        std::uniform_int_distribution<> distY(0, bounds[1]);
        std::uniform_real_distribution<> distV(-2, 2);
        std::uniform_int_distribution<> distRGB(0, 256);
        for (int i = 0; i < particleCount; i++) {
            particles[i].pos = {distX(e1), distY(e1)};
            particles[i].speed = {distV(e1), distV(e1)};
            particles[i].rgb = {distRGB(e1), distRGB(e1), distRGB(e1)};
        }
    }

    /**
    Simulate one time step for all particles
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
            particles[i].pos[0] = particles[i].pos[0] + particles[i].speed[0];
            particles[i].pos[1] = particles[i].pos[1] + particles[i].speed[1];
            if (particles[i].pos[0] > bounds[0] || particles[i].pos[0] < 0 ||
                particles[i].pos[1] > bounds[1] || particles[i].pos[1] < 0) {
                particles[i].pos[0] = bounds[0] / 2;
                particles[i].pos[1] = bounds[1] / 2;
            }
        }
    }

    std::vector<particle> Simulator::getParticles() {
        return this->particles;
    }

    int sign(int x) {
        return (x > 0) - (x < 0);
    }
}