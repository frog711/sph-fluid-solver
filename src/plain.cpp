#include "simulate.hpp"
#include <ctime> 
#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include "structures.hpp"
#include "kernel.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <sstream>
#include <string> 

std::random_device r;
std::default_random_engine e1(r());
static int xRes = 600u;
static int yRes = 600u;
double maxSpeed = 0;
std::vector<double> lost;

structures::config conf;

void saveState(int step, simulate::Simulator* sim, char* path, double densityAvg, double cfl) {
    std::stringstream files;
    files << path << "/step" << step << ".txt";
    std::string file = files.str();
    std::ofstream myfile;
    conf = sim->getConf();
    myfile.open (file, std::ios::app);
    myfile << conf.particleCount << " " << conf.dim << " " << conf.h << " " << conf.k << " " << conf.nu << " ";
    myfile << conf.kernelSupport << " " << conf.fastRender << " " << conf.timestep << " ";
    myfile << conf.g[0] << " " << conf.g[1] << " " << conf.area[0] << " " << conf.area[1] << "\n"; 

    for (auto particle : sim->getParticles()) {
        myfile << particle.isStationary << "," << particle.mass << "," << particle.pos[0] << "," << particle.pos[1];
        myfile << "," << particle.speed[0] << "," <<particle.speed[1] << "\n";
    }
    myfile.close();

    std::stringstream stats;
    stats << path << "/density.txt";
    std::string statFile = stats.str();
    std::ofstream densityFile;
    densityFile.open (statFile, std::ios::app);
    densityFile << step << "," << densityAvg << "\n";
    densityFile.close();

    std::stringstream stats2;
    stats2 << path << "/cfl.txt";
    std::string statFile2 = stats2.str();
    std::ofstream cflFile;
    cflFile.open (statFile2, std::ios::app);
    cflFile << step << "," << cfl << "\n";
    cflFile.close();
}

void runSimulation(std::string input, double seconds, int saveInterval, char* path) {
    auto simulator = simulate::Simulator(conf);
    conf = simulator.parseFile(input);
    kernel::Kernel kernel = kernel::Kernel(conf);
    kernel.initialize(simulator.getParticleData());
    simulator.addKernel(&kernel);
    //saveState(0, &simulator, path, computeAverageDensity(simulator));
    int initial = 0;
    for (int step = initial; step < seconds / conf.timestep; step++) {
        simulator.simulateStep();
        if ((step + 1) % saveInterval == 0) {
            std::cout << "Step: " << step << "\n";
            saveState(step + 1, &simulator, path, simulator.getAverageDensity(), simulator.getCFL());
        }
    }
}

int main(int argc, char** argv) {
    std::string input = argv[1];
    double seconds = std::stod(argv[2]);
    int saveInterval = std::stoi(argv[3]);
    char* file = argv[4];
    time_t start = clock();
    time_t startTime = time(NULL);
    runSimulation(input, seconds, saveInterval, file);
    time_t end = clock();
    time_t endTime = time(NULL);
    //writeMeasurements(file, input, simulationSteps, difftime(endTime, startTime), end - start);
}
