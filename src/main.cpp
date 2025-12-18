#include <SFML/Graphics.hpp>
#include "simulate.hpp"
#include "render.hpp"
#include <ctime> 
#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include "structures.hpp"
#include "kernel.hpp"
#include <format>



std::random_device r;
std::default_random_engine e1(r());
static int xRes = 750u;
static int yRes = 750u;
double maxSpeed = 0;
std::vector<double> lost;

structures::config conf;

void writeAnalysis(simulate::particle* p, std::ofstream* file) {
    double speed = std::sqrt(p->speed[0] * p->speed[0] + p->speed[1] * p->speed[1]);
    if (speed > maxSpeed && p->rgb[0] == 0) {
        maxSpeed = speed;
        *file << "S: " << maxSpeed << "\n";
    }
    if ((p->pos[0] > 85 || p->pos[1]) > 85 && p->rgb[0] == 0) {
        *file << "P: " << p->pos[0] << "," << p->pos[1] << "\n";
        p->rgb = {char(200), 0, 0};
    }
}

void saveScreen(sf::RenderWindow* window, int step, char* path) {
    sf::Texture texture({xRes, yRes});
    texture.update(*window);
    std::stringstream files;
    files << path << "/step" << step << ".png";
    std::string file = files.str();
    texture.copyToImage().saveToFile(file);
    //    {
    //std::cout << "screenshot saved to " << filename << std::endl;
}

void writeMeasurements(char* path, std::string particleCount, int simulationSteps, int seconds, time_t duration) {
    std::ofstream myfile;
    myfile.open (path, std::ios::app);
    myfile << particleCount << "," << simulationSteps << "," << seconds << "," << duration << "\n";
    myfile.close();
}

void writeEnergy(char* path, simulate::Simulator* sim) {
    std::ofstream myfile;
    myfile.open (path, std::ios::app);
    myfile << sim->getTotalEnergy() << ",";
    myfile.close();
}

void runSimulation(std::string input, int simulationSteps, char* path) {
    auto window = sf::RenderWindow(sf::VideoMode({xRes, yRes}), "CMake SFML Project");
    window.setSize({xRes, yRes});
    window.setPosition({100, 100});
    window.setFramerateLimit(144);
    auto simulator = simulate::Simulator(conf);
    std::cout << "Parse\n";
    conf = simulator.parseFile(input);
    std::cout << "Parsed\n";
    auto renderer = render::Renderer(&window, {xRes, yRes}, conf);
    kernel::Kernel kernel = kernel::Kernel(conf);
    kernel.initialize(simulator.getParticleData());
    simulator.addKernel(&kernel);
    //std::cout << "Tmp: " << kernel.getDerivativeEntry(3, 4)[0] << ", " << kernel.getDerivativeEntry(4, 3)[0] << " \n";
    renderer.initialize(simulator.getParticleData());

    std::stringstream dumpS;
    dumpS << path << "/info.txt";
    std::string dump = dumpS.str();

    for (int step = 0; step < simulationSteps; step++)
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }
        window.clear();
        for (int i = 0; i < 100; i++) {
            simulator.simulateStep();
        }
        std::cout << "Step: " << step << "\n";
        renderer.renderCircles();

        window.display();
        //std::cout << "Step: " << step << ": " << step % 10 << "\n";
        //if (step % 10 == 0) {
        //    saveScreen(&window, step, path);
        //}
    }
    std::cout << "Time: " << simulator.kernelTime1 << ", " << simulator.kernelTime2 << ", " << simulator.kernelTime3 << ", " << simulator.forceTime1 << ", " << simulator.forceTime2 << ", " << simulator.forceTime3 << "\n";
    window.close();
}

int main(int argc, char** argv) {
    std::string input = argv[1];
    int simulationSteps = std::stoi(argv[2]);
    char* file = argv[3];
    time_t start = clock();
    time_t startTime = time(NULL);
    runSimulation(input, simulationSteps, file);
    time_t end = clock();
    time_t endTime = time(NULL);
    //writeMeasurements(file, input, simulationSteps, difftime(endTime, startTime), end - start);
}
