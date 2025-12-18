#include <vector>
#include <random>
#include "structures.hpp"
#include "parser.hpp"
#include "kernel.hpp"
#ifndef SIMULATE
#define SIMULATE

namespace simulate {
    using namespace structures;
    class Simulator
    {
    public:
        int kernelTime1;
        int kernelTime2;
        int kernelTime3;
        int forceTime1;
        int forceTime2;
        int forceTime3;
        Simulator(config conf);
        void updateConf(config conf);
        config parseFile(std::string path);
        void addKernel(kernel::Kernel* kernel);
        void simulateStep();
        double getTotalEnergy();
        std::vector<particle> getParticles();
        particle* getParticleData();
    private:
        config conf;
        kernel::Kernel *kernel;
        std::default_random_engine e1;
        std::vector<particle> particles;
        void checkBoundingBoxes();
        void performUpdateStep();
        void computeAcceleration();
        void computePressureForces();
        void computeViscousForces();
        void computeExternalForces();
    };

    int sign(int x);
}
#endif