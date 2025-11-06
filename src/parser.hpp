#include <iostream>
#include <fstream>
#include <string>
#include "structures.hpp"
#ifndef PARSER
#define PARSER

namespace parser {
    using namespace structures;
    class Parser {
        public:
            Parser();
            bool open(std::string path);
            int getParticleCount();
            int getDimension();
            std::vector<int> getBounds();
            void parseNextParticle(particle* target);
            void close();
        private:
            std::fstream file;
            int dim;
            int particleCount;
            std::vector<int> bounds;
            std::vector<std::string> split(std::string, char delim);
    };
}

#endif