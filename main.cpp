#include<bitset>
#include<iostream>
#include<vector>

#include"raylib.h"

#include"grid.h"

int main(){

    InitWindow(screenWidth, screenHeight, "Wavefunction Collapse"); 

    // create grid with tieset and data sheet
    Grid grid("tilesets/circuit.png", "tilesets/circuit_data.txt");

    SetTargetFPS(60);
    while(!WindowShouldClose()){

        BeginDrawing();
        ClearBackground(RAYWHITE);

        if (IsKeyReleased(KEY_SPACE)){ grid.update(); }
        grid.draw();

        EndDrawing();
    }

    // unload grid texture
    grid.unloadTexture();

    CloseWindow();

    return 0;
}