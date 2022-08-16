#pragma once

#include<algorithm>
#include<cstddef>
#include<filesystem>
#include<string>

#include"globals.h"

// chose starting tileset
std::string getRandomTileset(){

   std::vector<std::filesystem::path> tilesets;

   for (const auto& entry : std::filesystem::directory_iterator(tilesetBaseDir)){
      tilesets.push_back((*std::next(entry.path().begin())));
   }

   std::filesystem::path choice;

   std::sample(tilesets.begin(), tilesets.end(), &choice, 1, gen);

   return choice.string();
}

// get full path to tilesetFile
std::string pathToTexture(const std::string& tilesetDir){
   std::string result;
   result.append(tilesetBaseDir);
   result += '/' + tilesetDir + '/';
   result.append(tilesetFile);

   return result; 
}

// get full path to tilesetData
std::string pathToData(const std::string& tilesetDir){
   std::string result;
   result.append(tilesetBaseDir);
   result += '/' + tilesetDir + '/';
   result.append(tilesetDataFile);

   return result; 
}

// print state
void print(const tileState& state){
   std::cout << "{" << state.x << "," << state.y << "}"; 
}