#pragma once

#include<filesystem>
#include<iterator>
#include<memory.h>
#include<string>
#include<unordered_set>
#include<vector>

#include"button.h"
#include"globals.h"
#include"grid.h"
#include"storage.h"
#include"utils.h"

//------------------------
// Base class of section
//------------------------
struct SectionBase{

   // scaling for everything in the secion
   float scale;

   // Each section has a bounds set at initialization
   Rectangle bounds{};

   // display each seaction
   virtual void display() = 0;

   SectionBase(float scale): scale(scale){};

   virtual ~SectionBase(){};
};

//--------------------------------------------------
// Section with incr+decr arrow buttons
//--------------------------------------------------
struct SectionRangeBase : SectionBase {

   // alpha_beta font by Brian Kent (AEnigma)
   Font& font{fontStore.getRef("fonts/alpha_beta.png")};

   // font for displaying value
   float fontSize{font.baseSize*scale};
   float spacing{1.0f*scale};
   Vector2 messagePos;

   // associated buttons
   ButtonHold leftButton{textureStore.getRef("UI/left-arrow.png"), 0.0f, 0.0f, scale, true};
   ButtonHold rightButton{textureStore.getRef("UI/right-arrow.png"), 0.0f, 0.0f, scale, true};

   // min max values for buttons
   int min;
   int max;

   SectionRangeBase(int min, int max, float scale):
      SectionBase(scale), min(min), max(max){};

   void moveButtons();

   virtual void display() = 0;

   virtual ~SectionRangeBase(){};   
};

void SectionRangeBase::moveButtons(){

   // buttons are vertically centred and 5% away from border
   leftButton.bounds.x  = bounds.x + 0.05f*bounds.width;
   rightButton.bounds.x = bounds.x + 0.95f*bounds.width - rightButton.bounds.width;

   leftButton.bounds.y = bounds.y + 0.5f*bounds.height - 0.5f*leftButton.bounds.height;
   rightButton.bounds.y = leftButton.bounds.y;
}

//---------------------------------------------------------------------
// Section with background, message+value and incr+decr arrow buttons
//---------------------------------------------------------------------
struct SectionRange1 : SectionRangeBase {

   // Texture
   Texture2D& texture;
   Rectangle source{0.0f, 0.0f, texture.width/3.0f, static_cast<float>(texture.height)};

   int& variable;

   // message properties
   std::string message;

   SectionRange1(float x, float y, std::string filename, std::string message, int& variable, int min, int max, float scale);

   void display() override;
};

SectionRange1::SectionRange1(float x, float y, std::string filename, std::string message, int& variable, int min, int max, float scale):
   SectionRangeBase(min,max,scale), texture(textureStore.getRef(filename.c_str())), variable(variable), message(message){

      bounds = {x,y,source.width*scale, source.height*scale};

      moveButtons();
}

void SectionRange1::display(){

   // display background
   DrawTexturePro(texture, source, bounds, {}, 0.0f, WHITE);

   // display left and right buttons and get state
   if (rightButton.display() && variable<max){ variable++; };
   if (leftButton.display()  && variable>min){ variable--; };

   std::string text = message + std::to_string(variable);

   // align message
   Vector2 textSize = MeasureTextEx(font, text.c_str(), fontSize, spacing);
   messagePos = {bounds.x + 0.5f*bounds.width - 0.5f*textSize.x, bounds.y + 0.5f*bounds.height - 0.5f*textSize.y};

   // display message
   DrawTextEx(font, text.c_str(), messagePos, fontSize, spacing, WHITE);
}

//------------------------------------------------------
// Section with multiple values incr+decr arrow buttons
//------------------------------------------------------
struct SectionRange2 : SectionRangeBase {

   std::vector<int*> controlledWeights;

   int tileIndex;

   SectionRange2(float x, float y, int tileIndex, int min, int max, float scale);

   void display() override;
};

SectionRange2::SectionRange2(float x, float y, int tileIndex, int min, int max, float scale): SectionRangeBase(min,max,scale), tileIndex(tileIndex){

   // bounds
   bounds = {x,y - leftButton.bounds.height*0.5f,leftButton.bounds.width*4.0f,leftButton.bounds.height};

   // fill controlledWeights
   for (int i=0; i<symmetryIndex[tileIndex]; i++){
      controlledWeights.push_back(&currentWeights[nonRotatingIndex[tileIndex] + i]);
   }

   // reduce font size
   fontSize -= 2.0f;

   // move buttons
   moveButtons();
}

void SectionRange2::display(){

   // get first weight
   int weight = *controlledWeights[0];

   // align weight text
   std::string text = std::to_string(weight);
   Vector2 textSize = MeasureTextEx(font, text.c_str(), fontSize, spacing);
   messagePos = {bounds.x + 0.5f*bounds.width - 0.5f*textSize.x, bounds.y + 0.5f*bounds.height - 0.5f*textSize.y};

   // If tile is currently turned off, grey out controlls
   if ( weightSwitch[nonRotatingIndex[tileIndex]] == 0){
      rightButton.display_grayed();
      leftButton.display_grayed();
   }
   // else display normally and with usable buttons
   else {
      if (rightButton.display() && weight<max){ for (auto ptr : controlledWeights){ (*ptr)++; } };
      if (leftButton.display()  && weight>min){ for (auto ptr : controlledWeights){ (*ptr)--; } };
   }   

   // display message
   DrawTextEx(font, text.c_str(), messagePos, fontSize, spacing, WHITE);
}

//---------------------------------------------------------------------------
// Section that is fully covered by a button. Static message. Runs Grid::function
//---------------------------------------------------------------------------
struct SectionBasicButton : SectionBase {

   // alpha_beta font by Brian Kent (AEnigma)
   Font& font{fontStore.getRef("fonts/alpha_beta.png")};

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

   SectionBasicButton(float x, float y, std::string filename, std::string message, Grid& grid, std::function<void(Grid&)> func, float scale);

   void display() override;

   void move(float x, float y);
};

SectionBasicButton::SectionBasicButton(float x, float y, std::string filename, std::string message, Grid& grid, std::function<void(Grid&)> func, float scale):
   SectionBase(scale), texture(textureStore.getRef(filename.c_str())), message(message), grid(grid), func(func){
      
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

   SectionBoolIcon(float x, float y, std::string filename, bool& variable, float scale);

   void display() override;

   void move(float x, float y);
};

SectionBoolIcon::SectionBoolIcon(float x, float y, std::string filename, bool& variable, float scale):
   SectionBase(scale), texture(textureStore.getRef(filename.c_str())), variable(variable){

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
   Font& font{fontStore.getRef("fonts/alpha_beta.png")};

   // message properties
   std::string& tilesetName;
   Vector2 messagePos;
   float fontSize{font.baseSize*1.4f};
   float spacing{1.0f};

   // tilset selection background
   Texture2D& topTexture{textureStore.getRef("UI/tile-menu-top.png")};
   Rectangle  topSource{0.0f,0.0f,static_cast<float>(topTexture.width),static_cast<float>(topTexture.height)};
   Rectangle  topBounds{0.0f,0.0f,topSource.width*scale,topSource.height*scale};

   // individual tile selection background
   Texture2D& botTexture{textureStore.getRef("UI/tile-menu-bot.png")};
   Rectangle  botSource{0.0f,0.0f,static_cast<float>(botTexture.width),static_cast<float>(botTexture.height)};
   Rectangle  botBounds{0.0f,topBounds.height,botSource.width*scale,botSource.height*scale};

   // drop down background
   Texture2D& dropTexture{textureStore.getRef("UI/tile-menu-drop.png")};
   Rectangle  dropSource{0.0f,0.0f,static_cast<float>(dropTexture.width),static_cast<float>(dropTexture.height)};
   Rectangle  dropBounds{0.0f,0.0f,dropSource.width*scale,dropSource.height*scale};

   // drop down end
   Texture2D& endTexture{textureStore.getRef("UI/tile-menu-end.png")};
   Rectangle  endSource{0.0f,0.0f,static_cast<float>(endTexture.width),static_cast<float>(endTexture.height)};
   Rectangle  endBounds{0.0f,0.0f,endSource.width*scale,endSource.height*scale};

   // tileset select drop down button
   ButtonHold dropDown{textureStore.getRef("UI/tile-menu-arrow.png"), 106.0f*scale, 9.0f*scale, scale};
   
   // tileset select drop down options
   std::vector<ButtonHold> dropDownOptions;
   ButtonHold dropDownBack{dropTexture,0.0f,0.0f,scale};

   // list of tileset files
   std::unordered_set<std::string> tilesets;

   // grid to control
   Grid& grid;

   // vector of tile buttons
   std::vector<ButtonTile> tileButtons;
   std::vector<SectionRange2> weightControls;

   // bool for showing drop down menu
   bool dropDownEnabled{false};

   SectionTiles(float x, float y, std::string& tilesetName, Grid& grid, float scale);

   void display() override;

   void move(float x, float y);

   void showDropDown();

   void createTileButtons();
};

SectionTiles::SectionTiles(float x, float y, std::string& tilesetName, Grid& grid, float scale):
   SectionBase(scale), tilesetName(tilesetName), grid(grid){

   // move all components
   move(x,y);

   // set bounds of all components
   bounds = {topBounds.x, topBounds.y, topBounds.width, topBounds.height+botBounds.height};

   // set starting tileset filename and position
   Vector2 textSize = MeasureTextEx(font, tilesetName.c_str(), fontSize, spacing);
   messagePos = {topBounds.x + 0.1f*topBounds.width, topBounds.y + 0.5f*topBounds.height - textSize.y*0.5f};

   // create tileset buttons in drop down
   for (const auto& entry : std::filesystem::directory_iterator(tilesetBaseDir)){
      tilesets.insert((*std::next(entry.path().begin())).string());
   }

   // create tile buttons for lower menu
   createTileButtons();
}

void SectionTiles::display(){

   // display top and bottom textures
   DrawTexturePro(topTexture, topSource, topBounds, {}, 0.0f, WHITE);
   DrawTexturePro(botTexture, botSource, botBounds, {}, 0.0f, WHITE);

   // display drop down button
   if (dropDown.display()){ dropDownEnabled = !dropDownEnabled; }

   // display tileset file
   DrawTextEx(font, tilesetName.c_str(), messagePos, fontSize, spacing, WHITE);

   // display tiles 
   for (auto& button : tileButtons){ button.display(); }

   // display weight controls
   for (auto& controls : weightControls){ controls.display(); }

   // show drop down if enabled
   if (dropDownEnabled){ showDropDown(); }
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

void SectionTiles::showDropDown(){

   // check for change in tileset
   std::string newTileset;

   float startX{bounds.x + topBounds.width*8.0f/128};
   float startY{bounds.y + topBounds.height*23.0f/32};
   Vector2 pathPos{messagePos};
   pathPos.y += dropBounds.height;

   for (const auto& entry : tilesets){

      // skip current tileset
      if (entry == tilesetDir){ continue; }

      // draw background button and check for click
      if (dropDownBack.display_at(startX,startY)){ newTileset = entry; };
      startY += dropBounds.height;

      // draw text
      DrawTextEx(font, entry.c_str(), pathPos, fontSize, spacing, WHITE);
      pathPos.y += dropBounds.height;
   }

   // draw line at bottom of drop down
   DrawTexturePro(endTexture, endSource, {startX,startY,endBounds.width,endBounds.height}, {}, 0.0f, WHITE);

   // on click, swap tileset and reset grid
   if (!newTileset.empty()){
      dropDownEnabled = false;
      tilesetName = newTileset;

      changeTileset(tilesetName, grid);

      // reset tile buttons
      tileButtons.clear();
      weightControls.clear();
      createTileButtons();
   }
}

void SectionTiles::createTileButtons(){

   // set up for button positions
   float startX{botBounds.x + botBounds.width*0.17f};
   float startY{botBounds.y + tileSize*scale*0.1f};

   float startX2{botBounds.x+1.0f*scale};
   float startY2{startY + tileSize*scale*0.25f};

   float addX{botBounds.width/3.0f};
   float addY{tileSize*scale*0.7f};

   int i{0};

   // loop through tile ids
   for (const auto& [rotatingId,nonRotatingId] : nonRotatingIndex ){
      
      // create tile button
      tileButtons.push_back(ButtonTile(*grid.texture, startX, startY, scale, rotatable ? rotatingId : nonRotatingId, rotatingId));

      // create tile weight controls
      weightControls.push_back(SectionRange2(startX2, startY2, rotatingId, 1, 200, scale/2.0f));

      // set position for next button
      if ( ++i%3 == 0 ){
         startX  = botBounds.x + botBounds.width*0.17f;
         startX2 = botBounds.x+1.0f*scale;
         startY += addY;
         startY2+= addY;
         i=0;
      }
      else {
         startX += addX;
         startX2 += addX;
      }
   }   

   // resize botBounds to contain all tiles
   botBounds.height = tileButtons.back().bounds.y + addY - botBounds.y;
}