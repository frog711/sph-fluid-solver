#include "structures.hpp"
#include "kernel.hpp"
namespace fluid {
    using namespace structures;
    class Accelerator {
        public:
            Accelerator();
            void initialize(int particleCount, particle* particles, kernel::Kernel* kernel);
            void accelerate();
        private:
            int h;
            int particleCount;
            particle* particles;
            kernel::Kernel* kernel;
            void acceleratePressure();
            void accelerateViscos();
    };
}