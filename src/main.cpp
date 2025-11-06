#include <SFML/Graphics.hpp>
#include "simulate.hpp"
#include "render.hpp"
#include <ctime> 
#include <vector>
#include <iostream>
#include <fstream>
#include <random>
#include "kernel.hpp"


std::random_device r;
std::default_random_engine e1(r());
static int xRes = 500u;
static int yRes = 500u;




void writeMeasurements(char* path, std::string particleCount, int simulationSteps, int seconds, time_t duration) {
    std::ofstream myfile;
    myfile.open (path, std::ios::app);
    myfile << particleCount << "," << simulationSteps << "," << seconds << "," << duration << "\n";
    myfile.close();
}

void runSimulation(std::string input, int simulationSteps) {
    auto window = sf::RenderWindow(sf::VideoMode({xRes, yRes}), "CMake SFML Project");
    window.setSize({xRes, yRes});
    window.setPosition({100, 100});
    window.setFramerateLimit(144);
    std::cout << "0\n";
    auto simulator = simulate::Simulator();
    auto renderer = render::Renderer(&window, {xRes, yRes});
    simulator.parseFile(input);
    simulator.completeNeighborSearch();
    auto neigh = simulator.specificNeighborSearch({200, 150});
    kernel::Kernel kernel;
    kernel.initialize(simulator.getParticles(), simulator.getParticles().size(), 2, 20);
    kernel.calculateKernel();
    kernel.calculateKernelDerivative();
    for (int i = 0; i < simulator.getParticles().size(); i++) {
        double sum = 0;
        for (int j = 0; j < simulator.getParticles().size(); j++) {
            sum += kernel.getKernelEntry(i, j);
        }
        std::cout << "Kernel: " << sum << ", Derivative: " << kernel.getDerivativeEntry(i, i)[0] << ", " << kernel.getDerivativeEntry(i, i)[1] << "\n";
    }
    std::cout << "Tmp: " << kernel.getDerivativeEntry(3, 4)[0] << ", " << kernel.getDerivativeEntry(4, 3)[0] << " \n";
    renderer.setupParticles(100, simulator.getParticles());
    std::cout << "Particle set up\n";
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
        auto neigh = simulator.specificNeighborSearch({80, 80});
        //simulator.simulateStepSimple();
        sf::CircleShape shape(20 * 1.9);
        shape.setRadius(19 * 2);
        // set a 10-pixel wide orange outline
        shape.setOutlineThickness(2.f);
        shape.setPosition({80 - 2 * 19, 80 - 2 * 19});
        shape.setOutlineColor(sf::Color(0, 150, 0));
        window.draw(shape);
        renderer.renderCircles(simulator.getParticles().size(), simulator.getParticles());

        window.display();
    }
    window.close();
}

int main(int argc, char** argv) {
    std::string input = argv[1];
    int simulationSteps = std::stoi(argv[2]);
    char* file = argv[3];
    time_t start = clock();
    time_t startTime = time(NULL);
    runSimulation(input, simulationSteps);
    time_t end = clock();
    time_t endTime = time(NULL);
    writeMeasurements(file, input, simulationSteps, difftime(endTime, startTime), end - start);
}
