#pragma once

#include<bitset>
#include<climits>
#include<cstddef>
#include<queue>
#include<random>
#include<unordered_set>
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

   Tile(Texture2D& texture): texture(texture){};
};

struct Grid{

   // tileset
   Texture2D texture;

   // bitset grid
   std::vector<std::vector<Bitset>> bitsetGrid;

   // texture grid
   std::vector<std::vector<Tile>> tileGrid;

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
      tileGrid = std::vector<std::vector<Tile>>(gridHeight, std::vector<Tile>(gridWidth,Tile(texture)));

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

   // if all tiles are collapsed, continue
   if (collapsible.empty()){ return; }
   
   // grid position to collapse
   Point currentPos;

   // check if theres is only one possible tile to collapse
   if (collapsible.size()==1){ currentPos = collapsible[0]; }
   // otherwise, choose a tile randomly
   else {
      std::uniform_int_distribution<std::size_t> chooseTile(0,collapsible.size()-1);
      currentPos = collapsible[chooseTile(gen)];
   }

   // aliases for convenience
   Bitset& currentBitset = bitsetGrid[currentPos.y][currentPos.x];
   Tile& currentTile = tileGrid[currentPos.x][currentPos.y];

   // choose a tile and orientation to collapse to
   std::vector<int> possibilities;
   for (std::size_t i=0; i<uniqueTiles; i++){
      if (currentBitset[i]){ possibilities.push_back(i); }
   }
   std::uniform_int_distribution<std::size_t> chooseState(0,possibilities.size()-1);
   
   // get bitset of new tile and orientation
   currentBitset = Bitset{1ull<<possibilities[chooseState(gen)]};
   
   // get tile configuration 
   auto [tileID, orientation] = getTile[currentBitset];
   currentTile.id = tileID;
   currentTile.orientation = orientation;

   // reset choices
   collapsible.clear();

   // propagate collapse
   
   // keep track of tiles with lowest entropy (lowest num of possible tiles)
   std::size_t lowestEntropy{ULLONG_MAX};

   // keep track of tiles that have been resolved and those already in queue
   std::unordered_set<Point> resolvedTiles{currentPos}, inQueue;

   // queue of tiles to resolve (need queue as FIFO, want to resolve newly added tiles last)
   std::queue<Point> toResolve{{currentPos}};

   // Breadth-first seach. Resolve nearest neighbours, then next nearest etc. 
   while (!toResolve.empty()){

      // get the top of the queue
      Point& resolvingPos = toResolve.front();
      Bitset& resolvingBitset = bitsetGrid[resolvingPos.y][resolvingPos.x];

      // propagate possibilities for neighbours
      for (int i=0; i<4; i++){

         // get position of neighbour
         Point nearPos = resolvingPos + cardinals[i];

         // check that we aren't out of bounds
         if (nearPos.x<0 || nearPos.y<0 || nearPos.x>=gridWidth || nearPos.y>=gridHeight){ continue; }

         // get bitset of neighbour
         Bitset& nearBitset = bitsetGrid[nearPos.y][nearPos.x];

         // if neighbour is collapsed, ignore and continue
         if (nearBitset.count()==1){ continue; }

         // for each tile resolvingPos can be, find all possible connections to neighbour
         Bitset newPossibilities;
         for (std::size_t j=0; j<uniqueTiles; j++){

            // ignore tiles resolvingPos can't be
            if (!resolvingBitset[j]){ continue; }

            // left is current possibility, right is bitset of all tiles that connect to left
            Bitset left{1ull<<j}, right;

            // rotate current tile so that we can look up left<->right connections
            rotate(left, i, dir::anticlockwise);

            // get possible connections
            right = connectsTo[left];

            // rotate connections back to original orientation
            rotate(right, i, dir::clockwise);

            // find all possibilites from union (bitwise |=) of all individual possibilities
            newPossibilities |= right;

            if (newPossibilities.count()==0){
               std::cout << "Problem Here?" << std::endl;
            }
         }         

         // take all previous possible states in nearBitset and remove those not in newPossibilities
         nearBitset &= newPossibilities;

         // count possibilities
         std::size_t numPossibilities = nearBitset.count();

         // if there is no possible tile to collapse to, the simulation ends
         if (numPossibilities == 0){
            collapsible.clear();
            std::cout << "Tile {" << resolvingPos.x << "," << resolvingPos.y << "} cannot be collapsed. Simulation halted." <<std::endl;
            return;
         }

         // if equal to lowestEntropy (lowest number of possibilities), add neighbour to collapsible
         if (numPossibilities == lowestEntropy){ collapsible.push_back(nearPos); }
         // if less, reset collapsible and add
         else if (numPossibilities < lowestEntropy){
            collapsible.clear();
            collapsible.push_back(nearPos);
            lowestEntropy = numPossibilities;
         }

         // add resolved pos to resolved list
         resolvedTiles.insert(resolvingPos);

         // add neighbours to resolving queue
         for (const auto& neighbour : cardinals){
            Point next = nearPos + neighbour;

            // don't add neighbours that are out of bounds
            if (next.x<0 || next.y<0 || next.x>=gridWidth || next.y>=gridHeight){ continue; }

            // if neighbour hasn't already been resolved, and isn't already in the queue, add it
            if (!resolvedTiles.contains(next) && !inQueue.contains(next)){

               if (next==currentPos){
                  std::cout << "Problem here?" << std::endl;
               }

               toResolve.push(next);
               inQueue.insert(next);
            } 
            
         }

         // add this neighbour to resolving queue
      }

      // pop tile from resolving queue
      toResolve.pop();
   }      
}

void Grid::draw(){

   // draw grid
   for (int j=0; j<gridHeight; j++){
      for (int i=0; i<gridWidth; i++){

         const Tile& tile = tileGrid[j][i];

         DrawTexturePro(texture,
                        {static_cast<float>(tile.id*tileSize), 0.0f, tileSize, tileSize},
                        {static_cast<float>(i*tileScaled+tileScaled/2.0f), static_cast<float>(j*tileScaled+tileScaled/2.0f), tileScaled, tileScaled},
                        {tileScaled/2.0f, tileScaled/2.0f},
                        tile.orientation*90.0f, WHITE);
      }
   }

   for (int i=0; i<gridHeight; i++){
      DrawLine(0.0f, static_cast<float>(i*tileScaled), gridWidth*tileScaled, static_cast<float>(i*tileScaled), RED);
      DrawLine(static_cast<float>(i*tileScaled), 0.0f, static_cast<float>(i*tileScaled), gridWidth*tileScaled, RED);
   }   
   
}