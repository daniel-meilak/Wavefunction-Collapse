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

struct ButtonBase {

   Texture2D& texture;
   Rectangle source{};
   Rectangle bounds{};

   int state{0}; // 0 normal, 1 hover, 2 click

   float& scale;

   ButtonBase(Texture& texture, float x, float y, float& scale): texture(texture), scale(scale){};
};

//---------------------------------------
// Button which can be held
//---------------------------------------
struct ButtonHold : ButtonBase{

   // holdable
   const bool holdable;
   int heldDuration{0};

   ButtonHold(Texture& texture, float x, float y, float scale, bool holdable=false): ButtonBase(texture,x,y,scale), holdable(holdable){
      source = {0.0f, 0.0f, texture.width/3.0f, static_cast<float>(texture.height)};
      bounds = {x, y, source.width*scale, source.height*scale};
   };

   // display at current bounds
   bool display();

   // display at new coordinate
   bool display_at(float x, float y);
};

bool ButtonHold::display(){

   bool clicked{false};

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

   ButtonIcon(Texture& texture, float x, float y, float scale, bool& on): ButtonBase(texture,x,y,scale), on(on){
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