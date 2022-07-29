#pragma once

#include<algorithm>
#include<bitset>
#include<climits>
#include<cstddef>
#include<iterator>
#include<map>
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
   tileState state{1,0};

   Tile(Texture2D& texture): texture(texture){};
};

struct Grid{

   // tileset
   Texture2D texture;

   // bitset grid
   std::vector<std::vector<Bitset>> bitsetGrid;

   // texture grid
   std::vector<std::vector<Tile>> tileGrid;

   // map of number of connections for each tile. Only keeps track of uncollapsed tiles
   std::map<std::size_t,std::unordered_set<Point>> entropyList;

   // random numbers
   std::mt19937 gen;

   // flag for full collapse
   bool collapsed{false};

   // construct grid. Add texture ref to tiles, set up random number gen, choose initial starting point
   Grid(std::string tileset, std::string dataSheet): texture(LoadTexture(tileset.c_str())){

      // analyze tileset data
      analyzeTiles(dataSheet);

      bitsetGrid = std::vector<std::vector<Bitset>>(gridHeight,std::vector<Bitset>(gridWidth,Bitset(std::string(uniqueTiles,'1'))));
      tileGrid = std::vector<std::vector<Tile>>(gridHeight, std::vector<Tile>(gridWidth,Tile(texture)));

      std::random_device rd; // slow, high quality, random number to seed fast generator
      
      gen.seed(rd());

      // fill entropyList
      for (int i=0; i<gridWidth; i++){
         for (int j=0; j<gridHeight; j++){
            entropyList[uniqueTiles].insert({i,j});
         }
      }
   }

   // debugging tileset analysis. Shows left<->right connections for each unique tile
   void debugTileset();

   // Update grid
   void update();

   // Draw grid
   void draw();

   // reset grid to default state
   void reset();

   // unload texture
   void unloadTexture(){ UnloadTexture(texture); }
};

void Grid::reset(){
   bitsetGrid = std::vector<std::vector<Bitset>>(gridHeight,std::vector<Bitset>(gridWidth,Bitset(std::string(uniqueTiles,'1'))));
   tileGrid = std::vector<std::vector<Tile>>(gridHeight, std::vector<Tile>(gridWidth,Tile(texture)));
}

void Grid::update(){

   // if all tiles are collapsed, continue
   if (collapsed){ return; }

   //------------------------------
   // collapse a tile
   //------------------------------

   // get list of lowest entropies
   auto it = entropyList.begin();
   auto& [entropy, tiles] = *it;
   
   // grid position to collapse
   Point currentPos;

   // check if theres is only one possible tile to collapse
   if (tiles.size()==1){ currentPos = *tiles.begin(); }
   // otherwise, choose a tile randomly
   else { std::sample(tiles.begin(), tiles.end(), &currentPos, 1, gen); }

   // aliases for convenience
   Bitset& currentBitset = bitsetGrid[currentPos.y][currentPos.x];
   Tile& currentTile = tileGrid[currentPos.y][currentPos.x];

   // if there are multiple possibilities
   if (entropy!=1){
      
      // get all possible unique tiles to collapse to
      std::vector<int> possibilities;
      for (std::size_t i=0; i<uniqueTiles; i++){
         if (currentBitset[i]){ possibilities.push_back(i); }
      }
      
      // choose one of the possibilites
      int bitPos;
      std::sample(possibilities.begin(), possibilities.end(), &bitPos, 1,gen);

      // get bitset of new tile and orientation
      currentBitset = Bitset{1ull<<bitPos};
   }

   // get tile configuration 
   currentTile.state = getTile[currentBitset];

   // remove from entropyList (only keep uncollapsed tiles)
   tiles.erase(currentPos);
   if (tiles.empty()){ entropyList.erase(it); }

   // check if wavefunction is fully collapsed
   if (entropyList.empty()){
      collapsed = true;
      std::cout << "Grid is fully Collapsed!\n";
      return;
   }

   //------------------------------
   // propagate collapse
   //------------------------------

   // keep track of tiles that have been resolved and those already in queue
   std::unordered_set<Point> resolvedTiles, inQueue;

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

         // if neighbour is collapsed or resolved, ignore and continue
         if (nearBitset.count()==1 || resolvedTiles.contains(nearPos)){ continue; }

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

            // if (newPossibilities.count()==0){
            //    std::cout << "Problem Here?" << std::endl;
            // }
         }         

         std::size_t oldCount=nearBitset.count(), newCount;

         // take all previous possible states in nearBitset and remove those not in newPossibilities
         nearBitset &= newPossibilities;

         // count possibilities
         newCount = nearBitset.count();

         // if there is no possible tile to collapse to, the simulation ends
         if (newCount == 0){
            std::cout << "Tile {" << resolvingPos.x << "," << resolvingPos.y << "} cannot be collapsed. Simulation halted." <<std::endl;
            return;
         }

         // if number of possibilities has changed, update entropyList
         if (newCount != oldCount){
            auto it = entropyList.find(oldCount);
            it->second.erase(nearPos);
            if (it->second.empty()){ entropyList.erase(it); }
            entropyList[newCount].insert(nearPos);
         }

         // add neighbour to resolving queue, if not added already
         if (!inQueue.contains(nearPos)){
            toResolve.push(nearPos);
            inQueue.insert(nearPos);
         }
      }

      // add tile to resolved list
      resolvedTiles.insert(resolvingPos);

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
                        {static_cast<float>(tile.state.x*tileSize), 0.0f, tileSize, tileSize},
                        {static_cast<float>(i*tileScaled+tileScaled/2.0f), static_cast<float>(j*tileScaled+tileScaled/2.0f), tileScaled, tileScaled},
                        {tileScaled/2.0f, tileScaled/2.0f},
                        tile.state.y*90.0f, WHITE);
      }
   }

   // for (int i=0; i<gridHeight; i++){
   //    DrawLine(0.0f, static_cast<float>(i*tileScaled), gridWidth*tileScaled, static_cast<float>(i*tileScaled), RED);
   // }   

   // for (int i=0; i<gridWidth; i++){
   //    DrawLine(static_cast<float>(i*tileScaled), 0.0f, static_cast<float>(i*tileScaled), gridWidth*tileScaled, RED);
   // }
   
}

void Grid::debugTileset(){

   static auto it = getBitset.begin();

   // stop whe it==last unique tile
   if (it==getBitset.end()){ 
      reset();
      return;
   }

   auto [state,bitset] = *it;

   // clear grid
   reset();

   // set {0,0} to a unique tile
   tileGrid[0][0].state = state;

   // display all left<->right connections to current state
   Bitset connections = connectsTo[bitset];
   std::size_t j{0}, k{2};
   for (std::size_t i=0; i<uniqueTiles; i++){

      // ignore impossible connections
      if (!connections[i]){ continue; }

      // move to newline if there are many connections
      if (j==gridWidth){
         j=0;
         k++;
      }

      // set a grid tiles to show connections
      tileGrid[j++][k].state = getTile[1ull<<i];
   }

   it++;
}