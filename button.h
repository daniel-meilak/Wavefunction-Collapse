#pragma once

#include<algorithm>
#include<array>
#include<cmath>
#include<functional>
#include<numeric>
#include<vector>

#include"raylib.h"

#include"globals.h"
#include"storage.h"
#include"utils.h"

struct ButtonBase {

   Texture2D& texture;
   Rectangle source{};
   Rectangle bounds{};

   int state{0}; // 0 normal, 1 hover, 2 click

   float scale;

   ButtonBase(Texture& texture, float scale): texture(texture), scale(scale){};
};

//---------------------------------------
// Button which can be held
//---------------------------------------
struct ButtonHold : ButtonBase{

   // holdable
   const bool holdable;
   int heldDuration{0};

   ButtonHold(Texture2D& texture, float x, float y, float scale, bool holdable=false): ButtonBase(texture,scale), holdable(holdable){
      source = {0.0f, 0.0f, texture.width/3.0f, static_cast<float>(texture.height)};
      bounds = {x, y, source.width*scale, source.height*scale};
   };

   // display at current bounds
   bool display();

   // display at new coordinate
   bool display_at(float x, float y);

   // display greyed out and not usable
   void display_grayed();
};

void ButtonHold::display_grayed(){
   DrawTexturePro(texture, source, bounds, {}, 0.0f, DARKGRAY);
}

bool ButtonHold::display(){

   bool clicked{false};

   // mouse over button
   if (CheckCollisionPointRec(mousePos, bounds)){

      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) ){
         state = 2;
         if (holdable){
            heldDuration++;
            if (heldDuration >= 15){ clicked = true; }
         }
      }
      else { state = 1; }

      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
         clicked = true;
         if (holdable){ heldDuration = 0; }
      }
   }
   else {
      if (holdable){ heldDuration = 0; }
      state = 0;
   }

   source.x = state*texture.width/3.0f;
   
   DrawTexturePro(texture, source, bounds, {}, 0.0f, WHITE);

   return clicked;
}

bool ButtonHold::display_at(float x, float y){

   bounds.x = x;
   bounds.y = y;

   return display();
}



//---------------------------------------
// Button which changes Icon
//---------------------------------------
struct ButtonIcon : ButtonBase{

   bool& on;

   ButtonIcon(Texture& texture, float x, float y, float scale, bool& on): ButtonBase(texture,scale), on(on){
      source = {0.0f, 0.0f, texture.width/3.0f, texture.height/2.0f};
      bounds = {x, y, source.width*scale, source.height*scale};

      // check for start state
      if (on){ source.y = texture.height*0.5f; }
   };

   bool display();

   void onClick(){ source.y = (0+!on)*texture.height*0.5f; }
};

bool ButtonIcon::display(){

   bool clicked{false};

   if (CheckCollisionPointRec(mousePos, bounds)){

      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) ){ state = 2; }
      else { state = 1; }

      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
         onClick();
         clicked = true;
      }
   }
   else { state = 0; }

   source.x = state*texture.width/3.0f;
   
   DrawTexturePro(texture, source, bounds, {}, 0.0f, WHITE);

   return clicked;
}

struct ButtonTile : ButtonBase{

   Bitset controlledTiles;

   bool on{true};

   Rectangle border{};

   ButtonTile(Texture& texture, float x, float y, float scale, float tileId, int rotatingId): ButtonBase(texture,scale){
      source = {tileId*tileSize, 0.0, static_cast<float>(tileSize), static_cast<float>(tileSize)};
      bounds = {x, y, tileSize*scale*0.5f, tileSize*scale*0.5f};
      border = {x-1.0f, y-1.0f, bounds.width+2.0f, bounds.height+2.0f};

      // create bitset of controlled tiles
      for (int i=0; i<symmetryIndex[rotatingId]; i++){
         controlledTiles.set(nonRotatingIndex[rotatingId]+i);
      }
   };

   void display();

};

void ButtonTile::display(){
   
   if (CheckCollisionPointRec(mousePos, bounds)){

      if (IsMouseButtonDown(MOUSE_BUTTON_LEFT) ){ state = 2; }
      else { state = 1; }

      // on mouse click flip all weights controlled by this button
      if (IsMouseButtonReleased(MOUSE_BUTTON_LEFT)){
         on = !on;
         nextWeightSwitch^=controlledTiles;
      }
   }
   else { state = 0; }

   Color color;
   switch (state){
      case 0: color = WHITE; break;
      case 1: color = LIGHTGRAY; break;
      case 2: color = GRAY; break;
   }
   
   DrawTexturePro(texture, source, bounds, {}, 0.0f, on ? color : DARKGRAY);
   DrawRectangleLinesEx(border, 1.0f, BLACK);
}

// struct ButtonSlider : ButtonBase{

//    int& value;

//    Texture2D& slotTexture;
//    Rectangle slotSource{0.0f,0.0f,static_cast<float>(slotTexture.width),static_cast<float>(slotTexture.height)};
//    Rectangle slotBounds{0.0f,0.0f,slotSource.width*scale,slotSource.height*scale};

//    ButtonSlider(Texture2D& texture, float x, float y, float scale, int& value, Texture2D& slotTexture):
//       ButtonBase(texture,x,y,scale), value(value), slotTexture(slotTexture){
//          source = {0.0f,0.0f,texture.width/3.0f,static_cast<float>(texture.height)};
//          bounds = {x,y,source.width*scale,source.height*scale};
//    }

//    void display();
// };

// void ButtonSlider::display(){

//    // mouse over button
//    if (CheckCollisionPointRec(mousePos,bounds)){

//       if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)){
//          state = 2;
//          bounds.y = mousePos.y;

//          // do not exit slot bounds
//          if (bounds.y > slotBounds.y){ bounds.y = slotBounds.y; }
//          else if (bounds.y < slotBounds.y+slotBounds.height){ bounds.y = slotBounds.y+slotBounds.height; }

//          // calculate new value

//       }
//       else { state = 1; }

//    }
//    else { state = 0; }
// }