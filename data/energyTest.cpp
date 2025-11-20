#include "../src/simulate.hpp"
#include "../src/kernel.hpp"
#include "../src/structures.hpp"

using namespace structures;

config conf1;
auto kernel1 = kernel::Kernel(conf1);
simulate::Simulator simulator1 = simulate::Simulator(conf1);

config conf2;
auto kernel2 = kernel::Kernel(conf2);
simulate::Simulator simulator2 = simulate::Simulator(conf2);

double upperError = 1.005;

void setup() {
    simulator1 = simulate::Simulator(conf1);
    conf1 = simulator1.parseFile("./data/plane");
    kernel1 = kernel::Kernel(conf1);
    kernel1.initialize(simulator1.getParticleData());
    simulator1.addKernel(&kernel1);

    simulator2 = simulate::Simulator(conf2);
    conf2 = simulator2.parseFile("./data/void");
    kernel2 = kernel::Kernel(conf2);
    kernel2.initialize(simulator2.getParticleData());
    simulator2.addKernel(&kernel2);

}

void printHeader(char* file) {
    std::ofstream myfile;
    myfile.open (file, std::ios::app);
    myfile << "k,m,v,a,b,e,\n";
    myfile.close();
}

void printResult(char* file, double k, double mass, double collisionEnergy) {
    //Particle should be deflected upwards
    bool isValid = simulator1.getParticles()[0].pos[1] < 100;
    double error = (simulator1.getTotalEnergy() - simulator2.getTotalEnergy()) / collisionEnergy;
    std::ofstream myfile;
    myfile.open (file, std::ios::app);
    myfile << k << "," << mass << "," << isValid << "," << simulator1.getTotalEnergy() << "," << simulator2.getTotalEnergy()<< "," << error << ",\n";
    myfile.close();
}

void varyStiffness(char* file) {
    for (int k = 1; k <= 100; k++) {
        for (int init = 0; init < 50; init++) {
            setup();
            conf1.k = 0.1 * k;
            conf2.k = 0.1 * k;
            simulator1.updateConf(conf1);
            simulator2.updateConf(conf2);
            simulator1.getParticleData()[0].pos[1] = 10 + init;
            simulator2.getParticleData()[0].pos[1] = 10 + init;
            double collisionEnergy = 0;
            for (int i = 0; i < 150; i++) {
                simulator1.simulateStep();
                simulator2.simulateStep();
                if (simulator2.getParticles()[0].pos[1] > 100 && collisionEnergy == 0) {
                    collisionEnergy = simulator2.getTotalEnergy();
                }
            }
            printResult(file, conf1.k, simulator1.getParticles()[0].mass, collisionEnergy);
        }
    }
}

void varyMass(char* file) {
    for (int k = 1; k <= 1000; k++) {
        setup();
        double mass = 0.1 * k;
        for (int p = 0; p < 1; p++) {
            particle* p1 = simulator1.getParticleData() + p;
            p1->mass = mass;
            p1-> restDensity = mass / std::pow(conf1.h, 2);
            particle* p2 = simulator2.getParticleData() + p;
            p2->mass = mass;
            p2-> restDensity = mass / std::pow(conf1.h, 2);
        }
        std::cout << simulator1.getParticles()[0].mass << "\n";
        for (int i = 0; i < 150; i++) {
            simulator1.simulateStep();
            simulator2.simulateStep();
        }
        std::cout << simulator1.getParticles()[0].pos[1] << "\n";
        printResult(file, mass, mass, 0);
    }
}

void resetScenario(double speed, int k, double timestep) {
    double posx = simulator1.getParticleData()[0].pos[0];
    simulator1.getParticleData()[0].pos = {2.5, 4.5};
    simulator1.getParticleData()[0].speed = {0, speed};
    simulator1.getParticleData()[0].acc = {0, 0};
    conf1.k = k;
    conf1.timestep = timestep;
    simulator1.updateConf(conf1);
}

bool checkValidBounce(int expectedBounces, int upperLimit, double tolerance) {
    int direction = 1;
    int bounces = 0;
    double maxHeight = 1.0 - simulator1.getParticles()[0].pos[1];
    for (int i = 0; i < upperLimit; i++) {
        simulator1.simulateStep();
        double height = 1.0 - simulator1.getParticles()[0].pos[1];

        //Particle went through the boundary
        if (height < -1.0) {
            std::cout << "Bounds: " << conf1.timestep << ", " << simulator1.getParticles()[0].pos[1] << ": " << height << "\n";
            return false;
        }
        if (direction == -1 && simulator1.getParticles()[0].speed[1] >= 0) {
            if (height > maxHeight * upperError) {
                std::cout << "Explode: " << conf1.timestep << ": " << height << "/" << maxHeight << "\n";
                return false;
            }
            else if (height < maxHeight * tolerance) {
                //std::cout << "Collapse: " << conf1.timestep << ": " << height << "\n";
                //return false;
            }
            else {
                bounces++;
                maxHeight = height; 
            }
        } 
        if (simulator1.getParticles()[0].speed[1] >= 0) direction = 1;
        else direction = -1;
        if (bounces >= expectedBounces) return true;
    }
    std::cout << "Timeout!\n";
    return false;
}

double getEnergyLoss(int timeout) {
    int direction = 1;
    double initialHeight = 1.0 - simulator1.getParticles()[0].pos[1];
    for (int i = 0; i < timeout; i++) {
        simulator1.simulateStep();
        double height = 1.0 - simulator1.getParticles()[0].pos[1];

        //Particle went through the boundary
        if (height < -1.0) {
            std::cout << "Bounds: " << conf1.timestep << ", " << simulator1.getParticles()[0].pos[1] << ": " << height << "\n";
            return -1;
        }
        if (direction == -1 && simulator1.getParticles()[0].speed[1] >= 0) {
            return height / initialHeight;
        } 
        if (simulator1.getParticles()[0].speed[1] >= 0) direction = 1;
        else direction = -1;
    }
    std::cout << "Timeout!\n";
    return -1;
}

double findTimestepBound(double initPos, double startingPoint, double step, double timeout) {
    for (int i = 0; i < 10; i++) {
        double timestep = startingPoint + (10 - i) * step;
        resetScenario(initPos, conf1.k, timestep);
        bool valid = checkValidBounce(5, timeout / step, 0.5);
        if (valid) return timestep;
    }
    return -1;
}

void findMaximumTimestep(double step, double timeout) {
    double initPos = simulator1.getParticles()[0].pos[1];
    double currentGuess = 0;
    //std::cout << "Initial step: " << step << "\n";
    while (step >= 0.0001) {
        //std::cout << "Searching range from " << currentGuess << " to " << currentGuess + 10 * step << "\n";
        double result = findTimestepBound(initPos, currentGuess, step, timeout);
        //std::cout << "Found: " << result << "\n";
        if (result > 0) {
            //std::cout << "Improved guess: " << result << "\n";
            currentGuess = result;
        }
        step = step / 10;
        //std::cout << "Best guess: " << currentGuess << "\n";
    }   
    std::cout << conf1.k << ":" << currentGuess << ",\n";
}

//Search the space of (startingPoint, startingPoint + 10 * step] with precision step
//for the lowest stiffness that stops the particle
int findStiffnessBound(double initSpeed, int startingPoint, int step) {
    for (int i = 1; i <= 10; i++) {
        resetScenario(initSpeed, startingPoint + i * step, conf1.timestep);
        //std::cout << "Testing k=" << startingPoint + i * step << "\n";
        bool valid = true;
        for (int s = 0; s < 5000; s++) {
            simulator1.simulateStep();
            //The particle phased through the barrier
            if (simulator1.getParticles()[0].pos[1] > 5.5) {
                //printf("Out of bounds %i\n", startingPoint + i * step);
                s = 15000;
                valid = false;
            }
            //The particle bounced
            if (simulator1.getParticles()[0].speed[1] < -0.1) {
                return startingPoint + i * step;
            }
        }
        //The particle did not bounce but also did not phase through the barrier
        if (valid) {
            return startingPoint + i * step;
        }
    }
    return -1;
}

int findMinimumStiffness(double initPos, int guess) {
    int step = std::pow(10, std::floor(std::log10(guess)));
    int currentGuess = 0;
    while (step >= 10) {
        std::cout << "Searching range from " << currentGuess << " to " << currentGuess + 10 * step << "\n";
        int result = findStiffnessBound(initPos, currentGuess, step);
        if (result < 0) {
            std::cout << "Could not find k between " << currentGuess << " and " << currentGuess + 10 * step << "\n";
            return -1;
        }
        else {
            //Guess should always be slightly to low
            currentGuess = result - step;
            step = step / 10;
        }
        std::cout << "Best guess: " << currentGuess << "\n";
    }  
    return currentGuess; 
}

void plotEnergyLoss() {
    for (int i = 0; i <= 400; i++) {
        setup();
        conf1.nu = i * 0.0025;
        simulator1.updateConf(conf1);
        std::cout << conf1.nu << ":" << getEnergyLoss(1500) << ",\n";
    }
}

int main(int argc, char** argv) {
    if (argc < 3) throw "Fail\n";
    setup();
    int result = findMinimumStiffness(std::stod(argv[1]), std::stoi(argv[2]));
    if (argc > 3) {
        std::ofstream myfile;
        myfile.open (argv[3], std::ios::app);
        myfile << conf1.timestep << "," << argv[1] << "," << result << "\n";
    myfile.close();
    }
    //conf1.k = std::stoi(argv[3]);
    //findMaximumTimestep(std::stod(argv[1]), std::stod(argv[2]));
    //if (argc > 4) {
    //    upperError = std::stoi(argv[4]);
    //}
    //plotEnergyLoss();
}