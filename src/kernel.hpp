#include "structures.hpp"
#include <map>

namespace kernel {
    using namespace structures;

    class Kernel {
        public: 
            Kernel();
            //The particles should not change during the simulation
            void initialize(std::vector<particle> particles, int particleCount, int dim, int h);
            void calculateKernel();
            double getKernelEntry(int i, int j);
            void calculateKernelDerivative();
            std::vector<double> getDerivativeEntry(int i, int j);

        private:
            std::vector<particle> particles;
            std::vector<std::map<int, double>> kernel;
            std::vector<std::map<int, std::vector<double>>> derivative; 
            int particleCount;
            int dim;
            int h;
            double getFactor();
    };
}