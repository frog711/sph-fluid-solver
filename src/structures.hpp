#include <vector>
#include <iostream>
#include <fstream>
#include <string>
#ifndef STRUCT
#define STRUCT
namespace structures {
    typedef struct particle {
        double size;
        std::vector<double> pos;
        std::vector<double> speed;
        std::vector<char> rgb;
        std::vector<int> neighbors;
    } particle;

    double getSquaredDistance(std::vector<double> a, std::vector<double> b, int dim);

    double getDistance(std::vector<double> a, std::vector<double> b, int dim);
}

#endif