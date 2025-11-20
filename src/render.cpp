#include "render.hpp"
#include <ctime> 
#include <vector>
#include <iostream>
#include <fstream>
#include <random>
namespace render {
    std::vector<std::vector<int>> shape = {{-2,0},{-1,-1},{-1,0},{-1,1},{0,-2},{0,-1},{0,0},{0,1},{0,2},
                {1,-1},{1,0},{1,1},{2,0}};

    Renderer::Renderer(sf::RenderTarget* window, std::vector<int> res, config conf) {
        this->window = window;
        this->res = res;
        this->conf = conf;
    }

    void Renderer::initialize(particle* particles) {
        this->particles = particles;
        for (int i = 0; i < conf.particleCount; i++) {
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

    void Renderer::updateConf(config conf) {
        this->conf = conf;
    }

    int Renderer::transformSize(double initial) {
        if (conf.dim != 2) throw "Rendering is only supported for 2D particles";
        double maxRes = std::min(res[0] / conf.area[0], res[1] / conf.area[1]);
        return initial * maxRes;
    }

    std::vector<int> Renderer::transformPoint(std::vector<double> initial) {
        if (conf.dim != 2) throw "Rendering is only supported for 2D particles";
        double resX = res[0] / conf.area[0];
        double resY = res[1] / conf.area[1];
        std::vector<int> result;
        result.resize(2);
        if (resX > resY) {
            double renderX = conf.area[0] * resY;
            result[1] = int(initial[1] * resY);
            result[0] = int(initial[0] * resY + (res[0] - renderX) / 2);
        } else {
            double renderY = conf.area[1] * resX;
            result[0] = int(initial[0] * resX);
            result[1] = int(initial[1] * resX + (res[1] - renderY) / 2);
        }
        return result;
    }

    void Renderer::renderParticles() {
        for (int i = 0; i < conf.particleCount; i++) {
            vertices[3 * i].position = sf::Vector2f(particles[i].pos[0] - 3, particles[i].pos[1]);
            vertices[3 * i + 1].position = sf::Vector2f(particles[i].pos[0] + 3, particles[i].pos[1]);
            vertices[3 * i + 2].position = sf::Vector2f(particles[i].pos[0], particles[i].pos[1] + 5);
        }
        window->draw(vertices.data(), vertices.size(), sf::PrimitiveType::Triangles);
    }

    void Renderer::renderCircles() {
        for (int i = 0; i < conf.particleCount; i++) {
            int size = transformSize(conf.h);
            std::vector<int> pos = transformPoint(particles[i].pos);
            sf::CircleShape shape;
            shape.setRadius(size / 2);
            shape.setFillColor(sf::Color(particles[i].rgb[0], particles[i].rgb[1], particles[i].rgb[2]));
            shape.setOutlineThickness(0);
            shape.setPosition({pos[0] - size / 2, pos[1] - size / 2});
            window->draw(shape);
        }
    }
}