#include "parser.hpp"

namespace parser {
    Parser::Parser() {}

    bool Parser::open(std::string path) {
        file.open(path, std::ios::in);
        if (file.is_open()) {
            std::string sa;
            getline(file, sa);
            auto tokens = split(sa, ' ');
            if (tokens.size() < 2) {
                throw "Invalid line";
            }
            particleCount = std::stoi(tokens[0]);
            dim = std::stoi(tokens[1]);
            if (tokens.size() != dim + 2) {
                throw "Invalid line";
            }
            for (int i = 0; i < dim; i++) {
                bounds.push_back(std::stoi(tokens[2 + i]));
            }
        }
        return file.is_open();
    }

    void Parser::close() {
        file.close();
    }

    int Parser::getParticleCount() {
        return particleCount;
    }

    int Parser::getDimension() {
        return dim;
    }

    std::vector<int> Parser::getBounds() {
        return bounds;
    }

    void Parser::parseNextParticle(particle* target) {
        std::string sa;
        getline(file, sa);
        auto tokens = split(sa, ',');
        if (tokens.size() != 2 * dim + 1) {
            std::cout << "Particle: " << sa << ", " << tokens.size() << "\n";
            throw "Invalid line";
        }
        target->size = std::stoi(tokens[0]);
        for (int i = 0; i < dim; i++) {
            target->pos.push_back(std::stod(tokens[1 + i]));
            target->speed.push_back(std::stof(tokens[1 + dim + i]));
        }
    }

    std::vector<std::string> Parser::split(std::string line, char delim) {
        std::vector<std::string> tokens;
        auto start = 0U;
        auto end = line.find(delim);
        while (end != std::string::npos) {
            tokens.push_back(line.substr(start, end - start));
            start = end + 1;
            end = line.find(delim, start);
        }
        tokens.push_back(line.substr(start, end - start));
        return tokens;
    }
}