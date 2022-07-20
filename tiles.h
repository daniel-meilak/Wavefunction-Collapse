#pragma once

#include<vector>

#include"raylib.h"

void analizeTiles(Image tileset, int tileSize){

   // map of tile indeces. E.g. connections[0] contains {top,right,bottom,left} connection index of first tile 
   std::vector<std::vector<int>> connections(14, std::vector<int>(4));

   // map of border to id

   // loop through each tile in the tileset
   for (int i=0; i<tileSize; i++){

      Image image;
      Color color;
   }

}