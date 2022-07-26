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

// map of tile index to bitset e.g. {0,1}->[0001]
std::map<std::pair<int,int>, std::bitset<64>> getBitset;

// map of tile to left-right connection
std::unordered_map<std::bitset<64>,std::bitset<64>> leftRightCon;

// map of rotations (by 90 degrees clockwise)
std::unordered_map<std::bitset<64>,std::bitset<64>> rotate;

// read information on tileset from file
// tile properties are represented in braket notation int the file {a,b}. a=tile index, b=orientation
// for fast calculations, this will be converted into a unique bitset for each tile e.g. {0,0}->0001
void analyzeTiles(std::string filename){

   std::ifstream dataFile(filename);
   if (!dataFile.is_open()){
      std::cerr << "Could not open tile connections file " << filename << std::endl;
      std::exit(EXIT_FAILURE);
   }

   // create list of tiles in braket {a,b} and bitset (00010) form 
   std::string line;
   std::size_t id{0}, index{0};
   std::getline(dataFile,line);
   for (char c : line){

      std::vector<std::pair<int,int>> brackets{{id,0},{id,1},{id,2},{id,3}};
      std::vector<std::bitset<64>> bits{(1ull<<index),(1ull<<(index+1)),(1ull<<(index+2)),(1ull<<(index+3))};

      // skip ',' 
      if (c==','){ continue; }

      int symmetry=c-'0';

      for (int i=0; i<symmetry; i++){
         getBitset[brackets[i]] = bits[i];
         rotate[bits[i]] = bits[(i+1)%symmetry];
      }

      id++;
      index += symmetry;
   }
   std::getline(dataFile, line);

   // regex matching "{a,b}", returning a,b as submatches
   std::regex tileIndices("\\{(\\d+)\\,(\\d+)\\}");

   // keep track of connection names for next part
   std::unordered_map<std::string, std::bitset<64>> connectionBitset; 

   // create bitsets for named connections
   while (std::getline(dataFile,line)){

      // stop at empty line
      if (line.empty()){ break; }

      // get name of connection
      std::size_t pos = line.find('-');
      std::string name  = line.substr(0,pos-1);

      // read connections in bracket notation
      std::vector<std::bitset<64>> connection, rotation;

      // use regex to get each unique tile
      // get matches from beginning to end of line 
      auto begin = std::sregex_iterator(line.begin(), line.end(), tileIndices);
      auto end   = std::sregex_iterator();

      for (std::sregex_iterator i=begin; i!=end; ++i){
         connection.push_back(getBitset[{std::stoi(i->str(1)),std::stoi(i->str(2))}]);
      }

      // create bitset for connection
      std::bitset<64> bits;
      for (const auto& tile : connection){ bits |= tile; }
      connectionBitset[name] = bits;

      // calculate rotations
      rotation = connection;
      for (int i=0; i<4; i++){
        
         std::bitset<64> copyBits, rotationBits;
         for (auto& tile : rotation){
            copyBits |= tile;
            tile = rotate[tile];
            rotationBits |= tile;
         }

         rotate[copyBits] = rotationBits;

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
         
         std::bitset<64> bits = getBitset[{std::stoi(i->str(1)),std::stoi(i->str(2))}];
         leftRightCon[bits] = connectionBitset[name];
      }
   }

   for (auto [key,value] : leftRightCon){
      std::cout << key.to_string() << "   " << value.to_string() << std::endl;
   }
}