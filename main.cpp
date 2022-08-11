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

        grid.draw();
        grid.update();

        if (grid.collapsed){ grid.reset(); }

        EndDrawing();
    }

    // unload grid texture
    grid.unloadTexture();

    CloseWindow();

    return 0;
}