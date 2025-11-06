#include <SFML/Graphics.hpp>
#include "simulate.hpp"
#include "structures.hpp"
#include <vector>
#ifndef RENDER
#define RENDER

namespace render {
    using namespace structures;
    class Renderer {
    public:
        Renderer(sf::RenderWindow* window, std::vector<int> res);
        void setupParticles(int particleCount, std::vector<simulate::particle> particles);
        void renderParticles(int particleCount, std::vector<simulate::particle> particles);
        void renderCircles(int particleCount, std::vector<simulate::particle> particles);
    private:
        std::vector<int> res;
        sf::RenderWindow* window;
        std::vector<sf::Vertex> vertices;
        void addParticleRender(simulate::particle element, std::vector<sf::Vertex>* render);
    };
}

#endif