#pragma once

#include<bitset>
#include<cstddef>
#include<random>
#include<utility>
#include<tuple>
#include<vector>

#include"raylib.h"

#include"analyzeTiles.h"
#include"constants.h"

struct Tile{
   Texture2D& texture;
   int id{1};
   int orientation{0};
   bool collapsed{false};

   Tile(Texture2D& texture): texture(texture){};
};

struct Grid{

   // tileset
   Texture2D texture;

   // bitset grid
   std::vector<std::vector<Bitset>> bitsetGrid;

   // texture grid
   std::vector<std::vector<Tile>> textureGrid;

   // possible tiles to collapse
   std::vector<Point> collapsible;

   // random numbers
   std::mt19937 gen;
   std::uniform_int_distribution<int> getRand{0,gridHeight-1};

   // construct grid. Add texture ref to tiles, set up random number gen, choose initial starting point
   Grid(std::string tileset, std::string dataSheet): texture(LoadTexture(tileset.c_str())){

      // analyze tileset data
      analyzeTiles(dataSheet);

      bitsetGrid = std::vector<std::vector<Bitset>>(gridHeight,std::vector<Bitset>(gridWidth,Bitset(std::string(uniqueTiles,'1'))));
      textureGrid = std::vector<std::vector<Tile>>(gridHeight, std::vector<Tile>(gridWidth,Tile(texture)));

      std::random_device rd; // slow, high quality, random number to seed fast generator
      //gen = std::mt19937(rd());  // fast random number generator
      gen.seed(rd());

      // determine initial tile to start with
      collapsible.push_back({getRand(gen), getRand(gen)});
   }

   // Update grid
   void update();

   // Draw grid
   void draw();

   // unload texture
   void unloadTexture(){ UnloadTexture(texture); }
};

void Grid::update(){
   
   // grid position to collapse
   Point pos;

   // check if theres is only one possible tile to collapse
   if (collapsible.size()==1){ pos = collapsible[0]; }
   else if (collapsible.empty()){ pos = {getRand(gen), getRand(gen)}; }
   // otherwise, choose a tile randomly
   else {
      std::uniform_int_distribution<std::size_t> chooseTile(0,collapsible.size()-1);
      pos = collapsible[chooseTile(gen)];
   }

   // choose a tile and orientation to collapse to
   std::vector<int> possibilities;
   for (std::size_t i=0; i<uniqueTiles; i++){
      if (bitsetGrid[pos.y][pos.x][i]){ possibilities.push_back(i); }
   }
   std::uniform_int_distribution<std::size_t> chooseState(0,possibilities.size()-1);
   auto [tileID, orientation] = getTile[1ull<<possibilities[chooseState(gen)]];
   textureGrid[pos.y][pos.x].id = tileID;
   textureGrid[pos.y][pos.x].orientation = orientation;

   // set collapsed
   textureGrid[pos.y][pos.x].collapsed = true;

   // propagate collapse
   // for (int i=0; i<4; i++){

   //    Bitset left, right;

   //    // rotate current tile so that we can look up left<->right connections
   //    left = rotate(bitsetGrid[pos.y][pos.x], i, dir::anticlockwise);

   // }


   // reset choices
   collapsible.clear();
}

void Grid::draw(){

   // draw grid
   for (int j=0; j<gridHeight; j++){
      for (int i=0; i<gridWidth; i++){

         const Tile& tile = textureGrid[j][i];

         DrawTexturePro(texture,
                        {static_cast<float>(tile.id*tileSize), 0.0f, tileSize, tileSize},
                        {static_cast<float>(i*tileScaled+tileScaled/2.0f), static_cast<float>(j*tileScaled+tileScaled/2.0f), tileScaled, tileScaled},
                        {tileScaled/2.0f, tileScaled/2.0f},
                        tile.orientation*90.0f, WHITE);
      }
   }
}