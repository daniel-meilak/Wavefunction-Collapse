#pragma once

#include<algorithm>
#include<bitset>
#include<climits>
#include<cstddef>
#include<filesystem>
#include<iterator>
#include<map>
#include<queue>
#include<random>
#include<unordered_set>
#include<utility>
#include<vector>

#include"raylib.h"

#include"analyzeTiles.h"
#include"globals.h"
#include"storage.h"
#include"utils.h"

struct Grid{

   // tileset
   Texture2D* texture{textureStore.getPtr(pathToTexture())};

   // bitset grid
   std::vector<std::vector<Bitset>> bitsetGrid;

   // texture grid
   std::vector<std::vector<tileState>> tileGrid;

   // map of number of connections for each tile. Only keeps track of uncollapsed tiles
   std::map<std::size_t,std::unordered_set<Point>> entropyList;

   // grid update time
   int updateSpeed{30};

   // pause grid update
   bool running{true};

   // wait timer
   float waitTimer{0.0f};

   // flag for full collapse
   bool collapsed{false};

   // construct grid.
   Grid();

   // debugging tileset analysis. Shows left<->right connections for each unique tile
   void debugTileset();
   std::map<tileState, Bitset>::iterator debugIt;

   // Update grid
   void update();

   // Draw grid
   void draw();

   // reset grid to default state
   void reset();

   // pause for duration
   bool waiting();

   // reset entropy list (used to find next tile to collapse)
   void resetEntropy();
};

// place all tiles at maximum entropy
void Grid::resetEntropy(){

   entropyList.clear();

   // fill entropyList
   for (int i=0; i<gridWidth; i++){
      for (int j=0; j<gridHeight; j++){
         entropyList[uniqueTiles].insert({i,j});
      }
   }
}

// analyze the chose tileset, create grid, fill entropies
Grid::Grid(){

   // analyze tileset data
   analyzeTiles();

   bitsetGrid = std::vector<std::vector<Bitset>>(gridHeight,std::vector<Bitset>(gridWidth,Bitset(std::string(uniqueTiles,'1'))));
   tileGrid = std::vector<std::vector<tileState>>(gridHeight, std::vector<tileState>(gridWidth));

   // fill entropyList (set all tiles to max)
   resetEntropy();

   // setup debug it
   if constexpr (debug){ debugIt = getBitset.begin(); }
}

bool Grid::waiting(){
   
   // If timer is active
   if (waitTimer > 0.0f){
      
      if (sinceLastUpdate >= waitTimer){ collapsed = true; }
      
      return true;
   }
   else { return false; }

}

void Grid::reset(){
   
   // reset Grids
   bitsetGrid = std::vector<std::vector<Bitset>>(gridHeight,std::vector<Bitset>(gridWidth,Bitset(std::string(uniqueTiles,'1'))));
   tileGrid = std::vector<std::vector<tileState>>(gridHeight, std::vector<tileState>(gridWidth));

   // reset Entropy
   resetEntropy();

   // set state to uncollapsed
   collapsed = false;

   // swap out weights
   for (std::size_t i=0; i<weights.size(); i++){
      if (!weightSwitch[i]    ){ currentWeights[i] = savedWeights[i]; }
      if (!nextWeightSwitch[i]){ savedWeights[i] = currentWeights[i]; }

      currentWeights[i] *= nextWeightSwitch[i];
   }
   weightSwitch = nextWeightSwitch;

   // set wait timer to 0
   waitTimer = 0.0f;
}

void Grid::update(){

   // if grid is paused
   // or not enough time has passed for next update
   // or there is a wait timer (grid is paused for 'waitTime' secs after completing a collapse)
   // continue
   if (!running || waiting() || sinceLastUpdate < 1.0f/updateSpeed){ return; }

   // reset time since last update
   sinceLastUpdate=0.0f;

   // debug tileset
   if constexpr (debug){
      debugTileset();

      // set grid speed back to zero
      updateSpeed = 0;

      return;
   }

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
   Bitset& currentBitset = bitsetGrid[static_cast<std::size_t>(currentPos.y)][static_cast<std::size_t>(currentPos.x)];
   tileState& currentTile = tileGrid[static_cast<std::size_t>(currentPos.y)][static_cast<std::size_t>(currentPos.x)];

   // if there are multiple possibilities
   if (entropy!=1){
      
      // get all possible unique tiles to collapse to and set up weights
      std::vector<std::size_t> possibilities;
      std::vector<int> adjustedWeights;
      for (std::size_t i=0; i<uniqueTiles; i++){
         if (currentBitset[i]){
            possibilities.push_back(i);
            adjustedWeights.push_back(currentWeights[i]);
         }
      }

      // set up a distribution
      std::discrete_distribution dist(adjustedWeights.begin(), adjustedWeights.end());

      // get bitset of new tile and orientation
      currentBitset = Bitset{}.set(possibilities[dist(gen)]);
   }

   // get tile configuration 
   currentTile = getTile[currentBitset];

   // remove from entropyList (only keep uncollapsed tiles)
   tiles.erase(currentPos);
   if (tiles.empty()){ entropyList.erase(it); }

   // check if wavefunction is fully collapsed
   if (entropyList.empty()){

      if constexpr (debug){ std::cout << "Grid is fully Collapsed!\n"; }

      // pause grid for 5 seconds
      waitTimer = waitTime;

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
      for (std::size_t i=0; i<4; i++){

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
            Bitset left{Bitset{}.set(j)}, right;

            // rotate current tile so that we can look up left<->right connections
            rotate(left, i, dir::anticlockwise);

            // get possible connections
            right = connectsTo[left];            

            // rotate connections back to original orientation
            rotate(right, i, dir::clockwise);

            // find all possibilites from union (bitwise |=) of all individual possibilities
            newPossibilities |= right;
         }         

         // remove all disabled tiles (weight = 0)
         newPossibilities &= weightSwitch;

         // error if no tile can be placed
         if constexpr (debug && newPossibilities.count()==0){
            std::cout << "Problem Here?" << std::endl;
         }

         std::size_t oldCount=nearBitset.count(), newCount;

         // take all previous possible states in nearBitset and remove those not in newPossibilities
         nearBitset &= newPossibilities;

         // count possibilities
         newCount = nearBitset.count();

         // if there is no possible tile to collapse to, the simulation ends
         if (newCount == 0){
            std::cerr << "Tile {" << resolvingPos.x << "," << resolvingPos.y << "} cannot be collapsed. Resetting grid." <<std::endl;
            reset();
            return;
         }

         // if number of possibilities has changed, update entropyList
         if (newCount != oldCount){
            auto iter = entropyList.find(oldCount);
            iter->second.erase(nearPos);
            if (iter->second.empty()){ entropyList.erase(iter); }
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

         const tileState& tile = tileGrid[j][i];

         if (rotatable){
            DrawTexturePro(*texture,
                           {static_cast<float>(tile.x*tileSize), 0.0f, tileSize, tileSize},
                           {static_cast<float>(i*tileScaled+tileScaled/2.0f), static_cast<float>(j*tileScaled+tileScaled/2.0f), tileScaled, tileScaled},
                           {tileScaled/2.0f, tileScaled/2.0f},
                           tile.y*90.0f,
                           WHITE);
         }
         else {
            DrawTexturePro(*texture,
                           {static_cast<float>((nonRotatingIndex[tile.x] + tile.y)*tileSize), 0.0f, tileSize, tileSize},
                           {static_cast<float>(i*tileScaled+tileScaled/2.0f), static_cast<float>(j*tileScaled+tileScaled/2.0f), tileScaled, tileScaled},
                           {tileScaled/2.0f, tileScaled/2.0f},
                           0.0f,
                           WHITE);
         }
      }
   }

   if constexpr (debug){
      for (int i=0; i<gridHeight; i++){
         DrawLine(0.0f, static_cast<float>(i*tileScaled), gridWidth*tileScaled, static_cast<float>(i*tileScaled), RED);
      }   

      for (int i=0; i<gridWidth; i++){
         DrawLine(static_cast<float>(i*tileScaled), 0.0f, static_cast<float>(i*tileScaled), gridWidth*tileScaled, RED);
      }
   }   
}

void Grid::debugTileset(){

   // stop when it==last unique tile
   if (debugIt==getBitset.end()){ 
      reset();
      return;
   }

   auto [state,bitset] = *debugIt;

   // clear grid
   reset();

   // set {0,0} to a unique tile
   tileGrid[3][0] = state;

   // display all left<->right connections to current state
   Bitset connections = connectsTo[bitset];
   std::size_t j{0}, k{2};
   for (std::size_t i=0; i<uniqueTiles; i++){

      // ignore impossible connections
      if (!connections[i]){ continue; }

      // move to newline if there are many connections
      if (j==gridHeight){
         j=0;
         k++;
      }

      // set a grid tiles to show connections
      tileGrid[j++][k] = getTile[Bitset{}.set(i)];
   }

   debugIt++;
}

//--------------------------------------------
// analyze new tileset, reset grid
//--------------------------------------------
void changeTileset(const std::string& newTileset, Grid& grid){

   // reset global data
   nonRotatingIndex.clear();
   symmetryIndex.clear();
   getBitset.clear();
   getTile.clear();
   connectsTo.clear();
   rightRotation.clear();
   leftRotation.clear();
   weights.clear();
   currentWeights.clear();
   savedWeights.clear();
   weightSwitch.set();
   nextWeightSwitch.set();

   // swap out tileset
   tilesetDir = newTileset;

   // analyze tileset
   analyzeTiles();

   // change grid texture pointer
   grid.texture = textureStore.getPtr(pathToTexture());

   // in debug reset grid.debugIt
   if constexpr (debug){ grid.debugIt = getBitset.begin(); }

   // reset grid
   grid.reset();
}