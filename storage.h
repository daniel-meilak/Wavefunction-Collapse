#pragma once

#include<array>
#include<cstdlib>
#include<cstddef>
#include<filesystem>
#include<iostream>
#include<string>
#include<type_traits>
#include<unordered_map>

#include"raylib.h"

template <typename T>
struct Storage{
   
   T& getRef(std::string filename);
   T* getPtr(std::string filename);

   void unloadAll();

private:

   std::size_t index{0};
   std::array<T,maxTextures> textures;

   std::unordered_map<std::string,int> textureIndex;

};

// Crate stores in global scope
Storage<Texture2D> textureStore;
Storage<Font> fontStore;

// Texture Storage
template<>
Texture2D& Storage<Texture2D>::getRef(std::string filename){

   // check if texture is already loaded
   if (textureIndex.contains(filename)){
      return textures[textureIndex.at(filename)];
   }
   // else add new texture
   else{

      // check if path exists
      if (!std::filesystem::exists(filename)){
         std::cerr << "Texture file path \"" << filename << "\" is not valid.\n";
         std::exit(EXIT_FAILURE);
      }

      // increment texture index and check if limit reached
      if (index == textures.size()){
         std::cerr << "Texture storage limit reached. Cannot load \"" << filename << "\".\n";
         std::exit(EXIT_FAILURE);
      }

      textures[index] = LoadTexture(filename.c_str());

      // add to uniqueTextures in case of future requests
      textureIndex[filename] = index;

      // return reference to texture and post increment index
      return textures[index++];
   }
}

template<>
Texture2D* Storage<Texture2D>::getPtr(std::string filename){

   // check if texture is already loaded
   if (textureIndex.contains(filename)){
      return &textures[textureIndex.at(filename)];
   }
   // else add new texture
   else{

      // check if path exists
      if (!std::filesystem::exists(filename)){
         std::cerr << "Texture file path \"" << filename << "\" is not valid.\n";
         std::exit(EXIT_FAILURE);
      }

      // increment texture index and check if limit reached
      if (index == textures.size()){
         std::cerr << "Texture storage limit reached. Cannot load \"" << filename << "\".\n";
         std::exit(EXIT_FAILURE);
      }

      textures[index] = LoadTexture(filename.c_str());

      // add to uniqueTextures in case of future requests
      textureIndex[filename] = index;

      // return pointer to texture and post increment index
      return &textures[index++];
   }
}

template<>
void Storage<Texture2D>::unloadAll(){

   for (std::size_t i=0; i<=index; i++){ 
      UnloadTexture(textures[i]); 
   }
}


// Font Storage
template<>
Font& Storage<Font>::getRef(std::string filename){

   // check if texture is already loaded
   if (textureIndex.contains(filename)){
      return textures[textureIndex.at(filename)];
   }
   // else add new texture
   else{

      // check if path exists
      if (!std::filesystem::exists(filename)){
         std::cerr << "Font file path \"" << filename << "\" is not valid.\n";
         std::exit(EXIT_FAILURE);
      }

      // increment texture index and check if limit reached
      if (index == textures.size()){
         std::cerr << "Font storage limit reached. Cannot load \"" << filename << "\".\n";
         std::exit(EXIT_FAILURE);
      }

      textures[index] = LoadFont(filename.c_str());

      // add to uniqueTextures in case of future requests
      textureIndex[filename] = index;

      // return reference to texture and post increment index
      return textures[index++];
   }
}

template<>
void Storage<Font>::unloadAll(){

   for (std::size_t i=0; i<=index; i++){ 
      UnloadFont(textures[i]); 
   }
}