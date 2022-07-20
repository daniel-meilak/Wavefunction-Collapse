#pragma once

#include<algorithm>
#include<map>
#include<vector>

#include"raylib.h"

// operators to compare Colors (for std::map)
bool operator==(const Color& rhs, const Color& lhs){
    return rhs.r==lhs.r && rhs.g==lhs.g && rhs.b==lhs.b && rhs.a==lhs.a;
}

bool operator<(const Color& rhs, const Color& lhs){
   if (rhs.r != lhs.r){ return rhs.r<lhs.r; }
   else if (rhs.g != lhs.g){ return rhs.g<lhs.r; }
   else if (rhs.b != lhs.b){ return rhs.b<lhs.b; }
   else { return rhs.a<lhs.a; }
}

// checks if a vector of Colors (a border of pixels) is symmetrical
bool symmetrical(const std::vector<Color>& border){
    
    auto it1 = border.begin();
    auto it2 = border.end()-1;

    while (it1 < it2){
        if (*it1 != *it2){ return false; }
        it1++;
        it2--;
    }

    return true;
}

void analizeTiles(const Image& tileset){

   const int tileArea = tileset.height*tileset.height;
   const int n = tileset.width/tileset.height;

   // map of tile indeces. E.g. connections[0] contains {top,right,bottom,left} connection index of first tile 
   std::vector<std::vector<int>> connections(n, std::vector<int>(4));

   // map of border to id
   std::map<std::vector<Color>, int> borderMap;
   int indexID{0};

   // convert image to array of Colors
   Color* colors =  LoadImageColors(tileset);

   // loop through each tile in the tileset
   for (int i=0; i<tileArea*n; i+=tileArea){

      // top and bottom borders
      std::vector<Color> top{colors+i, colors+i+tileset.height};
      std::vector<Color> bottom{colors+i+tileArea-tileset.height, colors+i+tileArea};
      
      // left and right borders
      std::vector<Color> left,right;
      for (int j=0; j<tileArea; j+=tileset.height){
         left.push_back(*(colors+i+j));
         right.push_back(*(colors+i+j+tileset.height-1));
      }

      // keep borders together to loop through
      std::vector<std::vector<Color>> borders{std::move(top), std::move(right), std::move(bottom), std::move(left)};

      for (int j=0; j<4; j++){

         std::vector<Color>& border = borders[j];

         // check if antisymmetric. Antisymmetric borders must link to mirrored copy
         if (!symmetrical(border)){ std::reverse(border.begin(), border.end()); }

         if (!borderMap.contains(border)){
            borderMap[border] = indexID;
            indexID++;
         }

         connections[i/tileArea][j] = borderMap[border];
      }

   }

}