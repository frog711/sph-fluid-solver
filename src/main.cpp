#include <SFML/Graphics.hpp>
#include "simulate.hpp"
#include "render.hpp"
#include <ctime> 
#include <vector>
#include <iostream>
#include <fstream>
#include <random>


std::random_device r;
std::default_random_engine e1(r());
static int xRes = 1920u;
static int yRes = 1080u;




void writeMeasurements(char* path, int particleCount, int simulationSteps, int seconds, time_t duration) {
    std::ofstream myfile;
    myfile.open (path, std::ios::app);
    myfile << particleCount << "," << simulationSteps << "," << seconds << "," << duration << "\n";
    myfile.close();
}

void runSimulation(int particleCount, int simulationSteps) {
    auto window = sf::RenderWindow(sf::VideoMode({xRes, yRes}), "CMake SFML Project");
    window.setFramerateLimit(144);

    auto simulator = simulate::Simulator();
    auto renderer = render::Renderer(&window, {xRes, yRes});
    simulator.setupParticles(particleCount, {xRes, yRes});
    renderer.setupParticles(particleCount, simulator.getParticles());
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
        simulator.simulateStepRandom();
        renderer.renderParticles(particleCount, simulator.getParticles());

        window.display();
    }
    window.close();
}

int main(int argc, char** argv) {
    int particleCount = std::stoi(argv[1]);
    int simulationSteps = std::stoi(argv[2]);
    char* file = argv[3];
    time_t start = clock();
    time_t startTime = time(NULL);
    runSimulation(particleCount, simulationSteps);
    time_t end = clock();
    time_t endTime = time(NULL);
    writeMeasurements(file, particleCount, simulationSteps, difftime(endTime, startTime), end - start);
}
