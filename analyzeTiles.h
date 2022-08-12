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

// map of tile index to bitset e.g. {0,1}->[0001]
std::map<tileState, Bitset> getBitset;

// map of bitset to tile index
std::unordered_map<Bitset,tileState> getTile;

// map of tile to left-right connection
std::unordered_map<Bitset,Bitset> connectsTo;

// map of rotations (by 90 degrees)
std::unordered_map<Bitset,Bitset> rightRotation;
std::unordered_map<Bitset,Bitset> leftRotation;

// vector of weights for each tile
std::vector<int> weights;

// number of unique tiles
std::size_t uniqueTiles;

// read information on tileset from file
// tile properties are represented in braket notation int the file {a,b}. a=tile index, b=orientation
// for fast calculations, this will be converted into a unique bitset for each tile e.g. {0,0}->0001
void analyzeTiles(std::string filename){

   std::ifstream dataFile(filename);
   if (!dataFile.is_open()){
      std::cerr << "Could not open tile connections file " << filename << std::endl;
      std::exit(EXIT_FAILURE);
   }

   // regex matching "{a,b}", returning a,b as submatches
   std::regex tileIndices("\\{(\\d+)\\,(\\d+)\\}");

   // create list of tiles in braket {a,b} and bitset (00010) form 
   std::string line;
   int id{0};
   std::size_t index{0};
   std::getline(dataFile,line);

   // use regex to get each {a,b} a=tile symmetry, b=tile weight
   auto begin = std::sregex_iterator(line.begin(), line.end(), tileIndices);
   auto end   = std::sregex_iterator();

   for (std::sregex_iterator i=begin; i!=end; ++i){
      
      std::vector<tileState> brackets{{id,0},{id,1},{id,2},{id,3}};
      std::vector<Bitset> bits{(1ull<<index),(1ull<<(index+1)),(1ull<<(index+2)),(1ull<<(index+3))};
      
      int symmetry = std::stoi(i->str(1));

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
   }
   std::getline(dataFile, line);

   // set number of unique tiles
   uniqueTiles = getBitset.size();

   // keep track of connection names for next part
   std::unordered_map<std::string, Bitset> connectionBitset; 

   // create bitsets for named connections
   while (std::getline(dataFile,line)){

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

