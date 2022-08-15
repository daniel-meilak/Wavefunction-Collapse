#pragma once

#include<bitset>
#include<cstddef>
#include<cstdlib>
#include<fstream>
#include<iostream>
#include<string>
#include<map>
#include<regex>
#include<unordered_map>
#include<utility>
#include<vector>

#include"globals.h"
#include"point.h"

// rotatability of current tileset
bool rotatable{true};

// index lookup for non rotating tilesets
std::map<int,int> nonRotatingIndex;

// vector of symmetries
std::vector<int> symmetryIndex;

// map of tile index <-> bitset e.g. {0,1}<->[0001]
std::map<tileState, Bitset> getBitset;
std::unordered_map<Bitset,tileState> getTile;

// map of tile to left-right connection
std::unordered_map<Bitset,Bitset> connectsTo;

// map of rotations (by 90 degrees)
std::unordered_map<Bitset,Bitset> rightRotation;
std::unordered_map<Bitset,Bitset> leftRotation;

// vector of weights for each tile 
std::vector<int> weights;              // original
std::vector<int> currentWeights;       // used in current simulation
std::vector<int> savedWeights;      // weights affected by range buttons
auto weightSwitch = std::move(Bitset{}.set());  // turn tile on/off (use char to avoid bool specialization);
auto nextWeightSwitch = std::move(Bitset{}.set());

// number of unique tiles with/without rotation
std::size_t uniqueTiles;

// read information on tileset from file
// tile properties are represented in braket notation int the file {a,b}. a=tile index, b=orientation
// for fast calculations, this will be converted into a unique bitset for each tile e.g. {0,0}->0001
void analyzeTiles(const std::string& filename){

   std::ifstream dataFile(filename);
   if (!dataFile.is_open()){
      std::cerr << "Could not open \"" << filename << "\". Exiting.\n";
      std::exit(EXIT_FAILURE);
   }

   // regex matching "{a,b}", returning a,b as submatches
   std::regex tileIndices("\\{(\\d+)\\,(\\d+)\\}");

   std::string line;

   // check for tileset rotatability
   std::getline(dataFile,line);
   if (line.back() == '\r'){ line.pop_back(); }

   if (line=="no rotation"){ rotatable = false; }
   else if (line=="rotate"){ rotatable = true;  }
   else {
      std::cerr << "Rotation type could not be found in \"" << filename << "\".\n";
      std::exit(EXIT_FAILURE);
   }
   std::getline(dataFile,line);
   if (line.back() == '\r'){ line.pop_back(); }

   // create list of tiles in braket {a,b} and bitset (00010) form 
   int id{0};
   std::size_t index{0};
   std::getline(dataFile,line);
   if (line.back() == '\r'){ line.pop_back(); }

   // use regex to get each {a,b} a=tile symmetry, b=tile weight
   auto begin = std::sregex_iterator(line.begin(), line.end(), tileIndices);
   auto end   = std::sregex_iterator();

   for (std::sregex_iterator i=begin; i!=end; ++i){
      
      std::vector<tileState> brackets{{id,0},{id,1},{id,2},{id,3}};
      std::vector<Bitset> bits{(1ull<<index),(1ull<<(index+1)),(1ull<<(index+2)),(1ull<<(index+3))};
      
      // get symmetry {Sym,_}
      int symmetry = std::stoi(i->str(1));

      // save index of unique tiles ignoring rotations
      nonRotatingIndex[id] = index;

      for (int j=0; j<symmetry; j++){
         
         // create maps from tileState<->bitset
         getBitset[brackets[j]] = bits[j];
         getTile[bits[j]] = brackets[j];

         // create maps for right and left rotations
         rightRotation[bits[j]] = bits[(j+1)%symmetry];
         leftRotation[bits[(j+1)%symmetry]] = bits[j];

         // fill weights for each unique tile
         weights.push_back(std::stof(i->str(2)));
      }

      id++;
      index += symmetry;
      symmetryIndex.push_back(symmetry);
   }
   std::getline(dataFile, line);
   if (line.back() == '\r'){ line.pop_back(); }

   // copy weights over
   currentWeights = weights;
   savedWeights   = weights;

   // set number of unique tiles including rotations
   uniqueTiles = getBitset.size();

   // keep track of connection names for next part
   std::unordered_map<std::string, Bitset> connectionBitset; 

   // create bitsets for named connections
   while (std::getline(dataFile,line)){
      if (line.back() == '\r'){ line.pop_back(); }

      // stop at empty line
      if (line.empty()){ break; }

      // get name of connection
      std::size_t pos = line.find('-');
      std::string name  = line.substr(0,pos-1);

      // read connections in bracket notation
      std::vector<Bitset> connection, rotation;

      // use regex to get each unique tile
      // get matches from beginning to end of line 
      auto begin = std::sregex_iterator(line.begin(), line.end(), tileIndices);
      auto end   = std::sregex_iterator();

      for (std::sregex_iterator i=begin; i!=end; ++i){
         connection.push_back(getBitset[{std::stoi(i->str(1)),std::stoi(i->str(2))}]);
      }

      // create bitset for connection
      Bitset bits;
      for (const auto& tile : connection){ bits |= tile; }
      connectionBitset[name] = bits;

      // calculate rotations
      rotation = connection;
      for (int i=0; i<4; i++){
        
         Bitset copyBits, rotationBits;
         for (auto& tile : rotation){
            copyBits |= tile;
            tile = rightRotation[tile];
            rotationBits |= tile;
         }

         rightRotation[copyBits] = rotationBits;
         leftRotation[rotationBits] = copyBits;

         // stop early for connections with symmetry < 4
         if (bits==rotationBits){ break; }
      }
   }

   // finally get leftright connections for each tile
   while (getline(dataFile,line)){
      if (line.back() == '\r'){ line.pop_back(); }

      // stop at empty line
      if (line.empty()){ break; }

      // read connection name on right
      std::size_t pos = line.find('-');
      std::string name  = line.substr(pos+2);

      // use regex to iterate through lines with multiple unique tiles on left
      auto begin = std::sregex_iterator(line.begin(), line.end(), tileIndices);
      auto end   = std::sregex_iterator();

      for (std::sregex_iterator i=begin; i!=end; ++i){
         if (!connectionBitset.contains(name)){
            std::cerr << "Name problem\n";
            std::exit(EXIT_FAILURE);
         }
         
         Bitset bits = getBitset[{std::stoi(i->str(1)),std::stoi(i->str(2))}];
         connectsTo[bits] = connectionBitset[name];
      }
   }
}

// void analyzaWangTile(const std::string& filename){

//    // load image
//    Image tilesetImage = LoadImage(filename.c_str());

//    // load colour array from image
//    Color* tilesetColors = LoadImageColors(tilesetImage);

//    // get number of tiles
//    int nTiles = tilesetImage.width/tileSize;

//    // create 3D vector to hold all border data. nTiles x 4 borders x tileSize
//    std::vector<std::vector<std::vector<Color>>> tileBorderData(nTiles,std::vector<std::vector<Color>>(4,std::vector<Color>(tileSize)));

//    // fill tileBorderData
//    for (std::size_t i=0; i<tilesetImage.width*tilesetImage.height; i+=tileArea){

//       std::size_t j=i/(tileSize*tileSize);
      
//       // fill all borders (bottom and left are reversed for consistency)
//       for (std::size_t k=0, rightOffset=tileSize-1; k<tileSize; k++){
//          tileBorderData[j][0][k] = tilesetColors[i + k];
//          tileBorderData[j][1][k] = tilesetColors[i + rightOffset + k*tileSize];
//          tileBorderData[j][2][k] = tilesetColors[i + (tileArea-1) - (rightOffset + k*tileSize)];
//          tileBorderData[j][3][k] = tilesetColors[i + (tileArea-1) - k];
//       }
//    }



//    // unload image and colours
//    UnloadImage(tilesetImage);
//    UnloadImageColors(tilesetColors);
// }

// rotate a unique tile clockwise. n: 0-0deg, 1-90deg, 2-180deg, 3-270deg
void rotate(Bitset& tile, int n, bool clockwise){
   switch (n){
   case 0: break;
   case 1: tile = clockwise ? rightRotation[tile] : leftRotation[tile]; break;
   case 2: tile =  leftRotation[leftRotation[tile]]; break;
   case 3: tile = clockwise ? leftRotation[tile] : rightRotation[tile]; break;
   default:
      std::cerr << "Invalid rotation\n";
      std::exit(EXIT_FAILURE);
   }
}

