#include <SFML/Graphics.hpp>
#include <string>
#include <iostream>
#include <filesystem>
#include "simulate.hpp"
#include "structures.hpp"
#include "render.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>  
#include <sstream>
#include <string> 

using namespace structures;
namespace fs = std::filesystem;
config conf;

static int xRes = 600u;
static int yRes = 600u;

void saveScreen(sf::RenderTexture* texture, fs::path path) {
    auto modified = path.replace_extension(".png");
    texture->getTexture().copyToImage().saveToFile(modified);
}

int main(int argc, char** argv)
{
    char* path = argv[1];
    
    sf::RenderTexture texture;
    texture.resize({xRes, yRes});

    auto simulate = simulate::Simulator(conf);
    auto renderer = render::Renderer(&texture, {xRes, yRes}, conf);

    for (const auto & entry : fs::directory_iterator(path)) {
        std::cout << entry.path() << std::endl;
        conf = simulate.parseFile(entry.path().u8string());
        renderer.initialize(simulate.getParticleData());
        renderer.updateConf(conf);
        renderer.renderCircles();
        texture.display();
        saveScreen(&texture, entry.path());
    }
}