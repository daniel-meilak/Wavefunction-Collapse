#pragma once

#include<array>
#include<bitset>
#include<cstddef>
#include<random>
#include<string_view>
#include<utility>

#include"point.h"
#include"raylib.h"

//-----------------------------------
// Constants
//-----------------------------------

// simulation fps
constexpr int fps{60};

// tile size multiplier
constexpr int scaling{2};

// number of simulated steps per display
constexpr int nCalcs{2};

// tile width/height original size (pixels)
constexpr int tileSize{32};
constexpr int tileArea{tileSize*tileSize};
constexpr int tileScaled{scaling*tileSize};

// grid dimensions
constexpr int gridWidth{24};
constexpr int gridHeight{12};

// window size
constexpr int screenWidth{gridWidth*tileSize*scaling};
constexpr int screenHeight{gridHeight*tileSize*scaling};

// bitset max and alias
constexpr std::size_t N=128;
using Bitset = std::bitset<N>;

// cardinal directions (right, bottom, left, up)
constexpr std::array<Point,4> cardinals{Point(1,0),Point(0,1),Point(-1,0),Point(0,-1)};

// Textures
constexpr int maxTextures{64};

// tileset directory and file names
constexpr const char* rootPath{R"(./)"};
constexpr const char* tilesetBaseDir{"tilesets/"};
constexpr const char* tilesetFile{"/tileset.png"};
constexpr const char* tilesetDataFile{"/data.txt"};

// wait time after a grid collapse
constexpr float waitTime{5.0f};

// debug
constexpr bool debug{false};

//-----------------------------------
// Variables
//-----------------------------------

// clearer names for rotate() 
enum dir{clockwise=true, anticlockwise=false};

// mouse position on window
Vector2 mousePos{};

// elapsed time since last update
float sinceLastUpdate{};

// selected tileset directory
std::string tilesetDir{};

// random numbers
std::random_device rd;
std::mt19937 gen(rd());

