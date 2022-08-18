#pragma once

#include<algorithm>
#include<cstddef>
#include<filesystem>
#include<string>

#include"globals.h"

// chose starting tileset, also change directory to project root
std::string setUpTileset(){

   // set current directory
   std::filesystem::current_path(rootPath);

   std::vector<std::filesystem::path> tilesets;

   for (const auto& entry : std::filesystem::directory_iterator(tilesetBaseDir)){
      tilesets.push_back((*std::next(entry.path().begin())));
   }

   std::filesystem::path choice;

   std::sample(tilesets.begin(), tilesets.end(), &choice, 1, gen);

   return choice.string();
}

// get full path to tilesetFile
std::string pathToTexture(){
   return std::string{tilesetBaseDir + tilesetDir + tilesetFile}; 
}

// get full path to tilesetData
std::string pathToData(){
   return std::string{tilesetBaseDir + tilesetDir + tilesetDataFile}; 
}

// print state
void print(const tileState& state){
   std::cout << "{" << state.x << "," << state.y << "}"; 
}