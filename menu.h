#pragma once

#include<algorithm>
#include<cstdarg>
#include<cstring>
#include<functional>
#include<memory>
#include<string>
#include<vector>

#include"raylib.h"

#include"button.h"
#include"globals.h"
#include"grid.h"
#include"section.h"
#include"storage.h"

// Menu of sections 
struct MenuBase {

   std::vector<std::unique_ptr<SectionBase>> sections;

   // menu box
   Rectangle bounds;

   // menu size multiplier
   float scale{3.0f};

   // minimize/maximize button
   bool maximized{false};

   MenuBase(float x, float y): bounds({x,y,0.0f,0.0f}){};

   // move menu position
   void move(float newX, float newY);

   // display menu (sections, buttons and min/max)
   virtual void display();
};

// display menu (sections, buttons and min/max)
void MenuBase::display(){
   if (maximized){ for (auto& section : sections){ section->display(); } }
};

//----------------------------------
// Horizonally Stacking Menu
//----------------------------------
struct MenuHorizontal : MenuBase{

   ButtonIcon button{textureStore.getRef("UI/open-close-h.png"), 0.0f, 0.0f, scale, maximized};

   MenuHorizontal(float x, float y): MenuBase(x,y){};

   void display() override;

   template<typename SectionType, typename ...Ts>
   void addSection(Ts&&... args);
};

void MenuHorizontal::display(){

   MenuBase::display();

   // min/max button
   if (button.display()){
      maximized = !maximized;

      button.bounds.y += maximized? -bounds.height : bounds.height;
   };
}

// add different types of Section
template<typename SectionType, typename ...Ts>
void MenuHorizontal::addSection(Ts&&... args){     

   auto section_ptr = std::make_unique<SectionType>(bounds.x + bounds.width, bounds.y, std::forward<Ts>(args)..., scale);
   
   // update menu bounds
   bounds.width += section_ptr->bounds.width;
   bounds.height = std::max(section_ptr->bounds.height, bounds.height);         
   
   // update minMax button bounds
   button.bounds.y = bounds.y + bounds.height*!maximized - button.bounds.height;
   button.bounds.x = (2*bounds.x + bounds.width)*0.5f - button.bounds.width*0.5f;

   sections.push_back(std::move(section_ptr));
   
}

//----------------------------------
// Vertically Stacking Menu
//----------------------------------
struct MenuVertical : MenuBase{

   ButtonIcon button{textureStore.getRef("UI/open-close-v.png"), 0.0f, 0.0f, scale, maximized};

   MenuVertical(float x, float y): MenuBase(x,y){};

   void display() override;

   // add different types of Section
   template<typename SectionType, typename ...Ts>
   void addSection(Ts&&... args);
};

void MenuVertical::display(){

   MenuBase::display();

   // min/max button
   if (button.display()){
      maximized = !maximized;

      button.bounds.x += maximized? bounds.width : -bounds.width;
   };
}

// add different types of Section
template<typename SectionType, typename ...Ts>
void MenuVertical::addSection(Ts&&... args){     

   auto section_ptr = std::make_unique<SectionType>(bounds.x, bounds.y + bounds.height, std::forward<Ts>(args)..., scale);

   // update menu bounds
   bounds.height += section_ptr->bounds.height;
   bounds.width = std::max(section_ptr->bounds.width, bounds.width);
   
   // update minMax button bounds
   button.bounds.x = bounds.x + bounds.width*maximized;
   button.bounds.y = (bounds.y + 20.0f); //- button.bounds.height*0.5f;

   sections.push_back(std::move(section_ptr));
}


//---------------------------------------------------------
// Functions to construct Contols and Tile select menus
//---------------------------------------------------------

// create a menu with a reset button, play/pause, and speed controls
std::unique_ptr<MenuHorizontal> createControlsMenu(float x, float y, Grid& grid, std::function<void(Grid&)> reset, bool& playPause, int& speed){

   auto menu = std::make_unique<MenuHorizontal>(x,y);

   menu->addSection<SectionBasicButton>("UI/rectangle-sheet.png", "Reset", grid, reset);
   menu->addSection<SectionBoolIcon>("UI/play-pause.png", playPause);
   menu->addSection<SectionRange1>("UI/rectangle-sheet.png", "Speed: ", speed, 0.0, fps);

   return menu;
}

std::unique_ptr<MenuVertical> createTilesMenu(float x, float y, std::string& filename, std::vector<int>& weights, Grid& grid){

   auto menu = std::make_unique<MenuVertical>(x,y);

   menu->addSection<SectionTiles>(filename, weights, grid);

   return menu;
}

//----------------------------------
// Container for menus
//----------------------------------
class MenuControl{
   std::vector<std::unique_ptr<MenuBase>> menus;

public:
   MenuControl(std::unique_ptr<MenuHorizontal>&& controlMenu, std::unique_ptr<MenuVertical>&& tileMenu){
      menus.push_back(std::move(controlMenu));
      menus.push_back(std::move(tileMenu));
   };

   void display(){ for (auto& menu : menus){ menu->display(); } };
};