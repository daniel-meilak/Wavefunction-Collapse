#pragma once

#include<array>
#include<bitset>
#include<cstddef>
#include<utility>

#include"point.h"
#include"raylib.h"

//-----------------------------------
// Constants
//-----------------------------------

// simulation fps
constexpr int fps{60};

// tile size multiplier
constexpr int scale{3};

// tile width/height original size (pixels)
constexpr int tileSize{14};
constexpr int tileScaled{scale*tileSize};

// grid dimensions
constexpr int gridWidth{32};
constexpr int gridHeight{18};

// window size
constexpr int screenWidth{gridWidth*tileSize*scale};
constexpr int screenHeight{gridHeight*tileSize*scale};

// bitset max and alias
constexpr std::size_t N=64;
using Bitset = std::bitset<N>;

// cardinal directions (right, bottom, left, up)
constexpr std::array<Point,4> cardinals{Point(1,0),Point(0,1),Point(-1,0),Point(0,-1)};

// Textures
constexpr int maxTextures{50};

// debug
constexpr bool debug{false};

//-----------------------------------
// Variable
//-----------------------------------

// clearer names for rotate() 
enum dir{clockwise=true, anticlockwise=false};

// mouse position on window
Vector2 mousePos{};

// elapsed time since last update
float sinceLastUpdate{};

