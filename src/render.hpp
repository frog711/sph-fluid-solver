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
        Renderer(sf::RenderWindow* window, std::vector<int> res, config conf);
        void initialize(particle* particles);
        void renderParticles();
        void renderCircles();
    private:
        std::vector<int> transformPoint(std::vector<double> pos);
        int transformSize(double size);
        config conf;
        std::vector<int> res;
        sf::RenderWindow* window;
        particle* particles;
        std::vector<sf::Vertex> vertices;
        void addParticleRender(simulate::particle element, std::vector<sf::Vertex>* render);
    };
}

#endif