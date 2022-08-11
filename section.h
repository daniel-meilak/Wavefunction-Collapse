#pragma once

#include<filesystem>
#include<string>
#include<vector>

#include"button.h"
#include"globals.h"
#include"grid.h"
#include"storage.h"

//------------------------
// Base class of section
//------------------------
struct SectionBase{

   // scaling for everything in the secion
   float& scale;

   // Each section has a bounds set at initialization
   Rectangle bounds{};

   // display each seaction
   virtual void display() = 0;

   SectionBase(float& scale):scale(scale){};

   virtual ~SectionBase(){};
};

//---------------------------------------------------------
// Section with centre message and incr+decr arrow buttons
//---------------------------------------------------------
struct SectionRange : SectionBase {

   // alpha_beta font by Brian Kent (AEnigma)
   Font& font{fontStore.add("fonts/alpha_beta.png")};

   // Texture
   Texture2D& texture;
   Rectangle source{0.0f, 0.0f, texture.width/3.0f, static_cast<float>(texture.height)};

   // message properties
   std::string message;
   Vector2 messagePos;
   float fontSize{static_cast<float>(font.baseSize)};
   float spacing{1.0f};
   
   // int value to change
   int& variable;

   // associated buttons
   ButtonHold leftButton{textureStore.add("UI/left-arrow.png"), 0.0f, 0.0f, scale, true};
   ButtonHold rightButton{textureStore.add("UI/right-arrow.png"), 0.0f, 0.0f, scale, true};

   SectionRange(float x, float y, std::string filename, std::string message, int& variable, float& scale);

   void display() override;

   void move(float x, float y);
};

SectionRange::SectionRange(float x, float y, std::string filename, std::string message, int& variable, float& scale):
   SectionBase(scale), texture(textureStore.add(filename.c_str())), message(message), variable(variable){
      
      // set scaling
      bounds.width = source.width*scale;
      bounds.height = source.height*scale;
      fontSize *= scale;
      spacing *= scale;

      // move to correct position in menu
      move(x,y);
};

void SectionRange::display(){

   // display background
   DrawTexturePro(texture, source, bounds, {}, 0.0f, WHITE);

   // display left and right buttons and get state
   if (rightButton.display() && variable<fps){ variable++; };
   if (leftButton.display()  && variable!=0 ){ variable--; };

   std::string text = message + std::to_string(variable);

   // align message
   Vector2 textSize = MeasureTextEx(font, text.c_str(), fontSize, spacing);
   messagePos = {bounds.x + 0.5f*bounds.width - 0.5f*textSize.x, bounds.y + 0.5f*bounds.height - 0.5f*textSize.y};

   // display message
   DrawTextEx(font, text.c_str(), messagePos, fontSize, spacing, WHITE);
}

void SectionRange::move(float x, float y){

   // move section
   bounds.x = x;
   bounds.y = y;

   // buttons are vertically centred and 5% away from border
   leftButton.bounds.x  = bounds.x + 0.05f*bounds.width;
   rightButton.bounds.x = bounds.x + 0.95f*bounds.width - rightButton.bounds.width;

   leftButton.bounds.y = bounds.y + 0.5f*bounds.height - 0.5f*leftButton.bounds.height;
   rightButton.bounds.y = leftButton.bounds.y;
}

//---------------------------------------------------------------------------
// Section that is fully covered by a button. Static message. Runs function
//---------------------------------------------------------------------------
struct SectionBasicButton : SectionBase {

   // alpha_beta font by Brian Kent (AEnigma)
   Font& font{fontStore.add("fonts/alpha_beta.png")};

   Texture2D& texture;

   // message properties
   std::string message;
   Vector2 messagePos;
   float fontSize{static_cast<float>(font.baseSize)};
   float spacing{1.0f};

   // grid to control
   Grid& grid;

   // function to run
   std::function<void(Grid&)> func;

   // button
   ButtonHold button{texture, 0.0f, 0.0f, scale};

   SectionBasicButton(float x, float y, std::string filename, std::string message, Grid& grid, std::function<void(Grid&)> func, float& scale);

   void display() override;

   void move(float x, float y);
};

SectionBasicButton::SectionBasicButton(float x, float y, std::string filename, std::string message, Grid& grid, std::function<void(Grid&)> func, float& scale):
   SectionBase(scale), texture(textureStore.add(filename.c_str())), message(message), grid(grid), func(func){
      
      bounds = {x, y,texture.width*scale/3.0f, texture.height*scale};

      // set scaling
      fontSize *= scale;
      spacing *= scale;

      // move to correct position in menu
      button.bounds = bounds;

      // calculate message size to position correctly
      Vector2 textSize = MeasureTextEx(font, message.c_str(), fontSize, spacing);
      messagePos = {bounds.x + 0.5f*bounds.width - 0.5f*textSize.x, bounds.y + 0.5f*bounds.height - 0.5f*textSize.y};
};

void SectionBasicButton::display(){

   // display button and get state
   if (button.display()){ func(grid); };

   // display message
   DrawTextEx(font, message.c_str(), messagePos, fontSize, spacing, WHITE);
}

void SectionBasicButton::move(float x, float y){

   // move section (for text position)
   bounds.x = x;
   bounds.y = y;

   // move button
   button.bounds = bounds;
}

//--------------------------------------------------------------------------------------------
// Section that is fully covered by a button. Icon which changes on actication. Changes bool
//--------------------------------------------------------------------------------------------
struct SectionBoolIcon : SectionBase {

   Texture2D& texture;

   // bool to change
   bool& variable;

   // button
   ButtonIcon button{texture, 0.0f, 0.0f, scale, variable};

   SectionBoolIcon(float x, float y, std::string filename, bool& variable, float& scale);

   void display() override;

   void move(float x, float y);
};

SectionBoolIcon::SectionBoolIcon(float x, float y, std::string filename, bool& variable, float& scale):
   SectionBase(scale), texture(textureStore.add(filename.c_str())), variable(variable){

      bounds = {x,y,texture.width*scale/3.0f, texture.height*scale*0.5f};

      // move to correct position in menu
      button.bounds = bounds;
};

void SectionBoolIcon::display(){

   // display button and get state
   if (button.display()){ variable = !variable; };
}

void SectionBoolIcon::move(float x, float y){

   // move section (for text position)
   bounds.x = x;
   bounds.y = y;

   // move button
   button.bounds = bounds;
}

//--------------------------------------------------------------------------------------------
// Section for changing tiles
//--------------------------------------------------------------------------------------------
struct SectionTiles : SectionBase {

   // alpha_beta font by Brian Kent (AEnigma)
   Font& font{fontStore.add("fonts/alpha_beta.png")};

   // message properties
   std::string tilesetFile;
   Vector2 messagePos;
   float fontSize{font.baseSize*1.4f};
   float spacing{1.0f};

   // tilset selection background
   Texture2D& topTexture{textureStore.add("UI/tile-menu-top.png")};
   Rectangle topSource{0.0f,0.0f,static_cast<float>(topTexture.width),static_cast<float>(topTexture.height)};
   Rectangle topBounds{0.0f,0.0f,topSource.width*scale,topSource.height*scale};

   // individual tile selection background
   Texture2D& botTexture{textureStore.add("UI/tile-menu-bot.png")};
   Rectangle botSource{0.0f,0.0f,static_cast<float>(botTexture.width),static_cast<float>(botTexture.height)};
   Rectangle botBounds{0.0f,topBounds.height,botSource.width*scale,botSource.height*scale};

   // tileset select drop down button
   ButtonHold dropDown{textureStore.add("UI/tile-menu-arrow.png"), 106.0f*scale, 9.0f*scale, scale};
   
   // weights
   std::vector<int>& weights;

   SectionTiles(float x, float y, std::string filename, std::vector<int>& weights, float& scale);

   void display() override;

   void move(float x, float y);
};

SectionTiles::SectionTiles(float x, float y, std::string filename, std::vector<int>& weights, float& scale):
   SectionBase(scale), weights(weights){

   // move all components
   move(x,y);

   // set bounds of all components
   bounds = {topBounds.x, topBounds.y, topBounds.width, topBounds.height+botBounds.height};

   // set starting tileset filename and position
   std::filesystem::path p{filename};
   tilesetFile = p.filename().string();
   Vector2 textSize = MeasureTextEx(font, tilesetFile.c_str(), fontSize, spacing);
   messagePos = {topBounds.x + 0.1f*topBounds.width, topBounds.y + 0.5f*topBounds.height - textSize.y*0.5f};

}

void SectionTiles::display(){

   // display top and bottom textures
   DrawTexturePro(topTexture, topSource, topBounds, {}, 0.0f, WHITE);
   DrawTexturePro(botTexture, botSource, botBounds, {}, 0.0f, WHITE);

   // display drop down button
   if (dropDown.display()){};

   // display tileset file
   DrawTextEx(font, tilesetFile.c_str(), messagePos, fontSize, spacing, WHITE);
}

void SectionTiles::move(float x, float y){

   // work out translation vector
   Vector2 trans{x - topBounds.x, y - topBounds.y};

   // move each component
   topBounds.x += trans.x;
   topBounds.y += trans.y;
   botBounds.x += trans.x;
   botBounds.y += trans.y;
   dropDown.bounds.x += trans.x;
   dropDown.bounds.y += trans.y;
}