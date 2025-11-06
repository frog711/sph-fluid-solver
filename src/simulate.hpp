#include <vector>
#include <random>
#include "structures.hpp"
#include "parser.hpp"
#ifndef SIMULATE
#define SIMULATE

namespace simulate {
    using namespace structures;
    class Simulator
    {
    public:
        Simulator();
        void parseFile(std::string path);
        void setupParticles(std::vector<int> particleCount, std::vector<int> limit);
        void simulateStepRandom();
        void simulateStepSimple();
        void completeNeighborSearch();
        std::vector<int> specificNeighborSearch(std::vector<double> pos);
        std::vector<particle> getParticles();
    private:
        std::default_random_engine e1;
        int dimensions;
        int particleCount;
        double kernelSupport = 1.9;
        std::vector<particle> particles;
        std::vector<int> bounds;
        void quadraticNeighborSearch();
    };

    int sign(int x);
}
#endif