#pragma once

#include<cstddef>
#include<string>

#include"globals.h"

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