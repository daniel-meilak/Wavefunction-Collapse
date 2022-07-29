#pragma once

#include<array>
#include<cstddef>
#include<utility>

#include"point.h"

// tile size multiplier
constexpr int scale{3};

// tile width/height original size (pixels)
constexpr int tileSize{14};
constexpr int tileScaled{scale*tileSize};

// grid dimensions
constexpr int gridWidth{20};
constexpr int gridHeight{15};

// window size
constexpr int screenWidth{gridWidth*tileSize*scale};
constexpr int screenHeight{gridHeight*tileSize*scale};

// bitset max and alias
constexpr std::size_t N=64;
using Bitset = std::bitset<N>;

// cardinal directions (right, bottom, left, up)
constexpr std::array<Point,4> cardinals{Point(1,0),Point(0,1),Point(-1,0),Point(0,-1)};

// clearer names for rotate() 
enum dir{clockwise=true, anticlockwise=false};