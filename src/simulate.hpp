#include <vector>
#include <random>
#ifndef SIMULATE
#define SIMULATE

namespace simulate {
    #ifndef particle
    typedef struct particle {
        std::vector<double> pos;
        std::vector<double> speed;
        std::vector<char> rgb;
    } particle;
    #endif
    class Simulator
    {
    public:
        Simulator();
        void setupParticles(int particleCount, std::vector<int> limit);
        void simulateStepRandom();
        void simulateStepSimple();
        std::vector<particle> getParticles();
    private:
        std::default_random_engine e1;
        int particleCount;
        std::vector<particle> particles;
        std::vector<int> bounds;
    };

    int sign(int x);
}
#endif