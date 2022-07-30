#include<bitset>
#include<iostream>
#include<vector>

#include"raylib.h"

#include"grid.h"

int main(){

    InitWindow(screenWidth, screenHeight, "Wavefunction Collapse"); 

    // create grid with tieset and data sheet
    Grid grid("tilesets/circuit.png", "tilesets/circuit_data.txt");

    SetTargetFPS(fps);
    while(!WindowShouldClose()){

        BeginDrawing();
        ClearBackground(RAYWHITE);

        //if (IsKeyDown(KEY_SPACE)){ grid.update(); }
        grid.update();
        grid.draw();

        EndDrawing();
    }

    // unload grid texture
    grid.unloadTexture();

    CloseWindow();

    return 0;
}