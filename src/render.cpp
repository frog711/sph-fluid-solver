#include "render.hpp"
#include <ctime> 
#include <vector>
#include <iostream>
#include <fstream>
#include <random>
namespace render {
    std::vector<std::vector<int>> shape = {{-2,0},{-1,-1},{-1,0},{-1,1},{0,-2},{0,-1},{0,0},{0,1},{0,2},
                {1,-1},{1,0},{1,1},{2,0}};

    Renderer::Renderer(sf::RenderWindow* window, std::vector<int> res) {
        this->window = window;
        this->res = res;
    }

    void Renderer::setupParticles(int particleCount, std::vector<simulate::particle> particles) {
        for (int i = 0; i < particleCount; i++) {
            sf::Vertex vertex1;
            sf::Vertex vertex2;
            sf::Vertex vertex3;
            vertex1.color = sf::Color(particles[i].rgb[0], particles[i].rgb[1], particles[i].rgb[2]);
            vertex2.color = sf::Color(particles[i].rgb[0], particles[i].rgb[1], particles[i].rgb[2]);
            vertex3.color = sf::Color(particles[i].rgb[0], particles[i].rgb[1], particles[i].rgb[2]);
            vertices.push_back(vertex1);
            vertices.push_back(vertex2);
            vertices.push_back(vertex3);
        }
    }

    void Renderer::renderParticles(int particleCount, std::vector<simulate::particle> particles) {
        for (int i = 0; i < particleCount; i++) {
            vertices[3 * i].position = sf::Vector2f(particles[i].pos[0] - 3, particles[i].pos[1]);
            vertices[3 * i + 1].position = sf::Vector2f(particles[i].pos[0] + 3, particles[i].pos[1]);
            vertices[3 * i + 2].position = sf::Vector2f(particles[i].pos[0], particles[i].pos[1] + 5);
        }
        window->draw(vertices.data(), vertices.size(), sf::PrimitiveType::Triangles);
    }
}