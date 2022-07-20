#include<iostream>
#include<vector>

#include"raylib.h"

#include"constants.h"
#include"tiles.h"

struct Borders{
    std::vector<Color> top,right,bottom,left;
};





int main(){

    InitWindow(screenWidth, screenHeight, "Wavefunction Collapse");

    Image lane  = LoadImage("tilesets/diag.png");
    Image cross = LoadImage("tilesets/cross.png");

    Color* laneColors  = LoadImageColors(lane);
    Color* crossColors = LoadImageColors(cross);

    Borders laneBorders, crossBorders;

    laneBorders.top  = std::vector<Color>(laneColors, laneColors+14);
    crossBorders.top = std::vector<Color>(crossColors, crossColors+14);

    for (int i=0; i<14*14; i+=14){
        laneBorders.left.push_back(*(laneColors+i));
        laneBorders.right.push_back(*(laneColors+13+i));

        crossBorders.left.push_back(*(crossColors+i));
        crossBorders.right.push_back(*(crossColors+13+i));
    }

    laneBorders.bottom  = std::vector<Color>(laneColors+14*13, laneColors+14*14);
    crossBorders.bottom = std::vector<Color>(crossColors+14*13, crossColors+14*14);

    // if (laneBorders.left == crossBorders.left){ std::cout << "The same\n"; }
    // else { std::cout << "Not the same\n"; }

    if (laneBorders.left == laneBorders.right){ std::cout << "True\n"; }
    else { std::cout << "False\n"; }

    // SetTargetFPS(60);
    // while (!WindowShouldClose()){


    //     BeginDrawing();
    //     ClearBackground(RAYWHITE);

    //     for (int i=0; i<14; i++){
    //         //DrawTextureRec(tiles, {i*14.0f, 0.0f, 14.0f, 14.0f}, {i*16.0f, 0.0f}, WHITE);
    //         //DrawTexturePro(tiles, {i*14.0f, 0.0f, 14.0f, 14.0f}, {i*16.0f*4, 0.0f, 14.0f*4, 14.0f*4}, {}, 0.0f, WHITE);
    //     }

    //     EndDrawing();
    // }

    UnloadImage(lane);
    UnloadImage(cross);
    UnloadImageColors(laneColors);
    UnloadImageColors(crossColors);

    CloseWindow();

    return 0;
}