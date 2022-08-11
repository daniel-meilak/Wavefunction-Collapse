#include<bitset>
#include<iostream>
#include<vector>

#include"raylib.h"

#include"grid.h"
#include"storage.h"

int main(){

    InitWindow(screenWidth, screenHeight, "Wavefunction Collapse"); 

    // create grid with tieset and data sheet
    Grid grid(textureStore.add("tilesets/circuit.png"), "tilesets/circuit_data.txt");

    SetTargetFPS(fps);
    while(!WindowShouldClose()){

        BeginDrawing();
        ClearBackground(RAYWHITE);

        grid.draw();
        grid.update();

        if (grid.collapsed){ grid.reset(); }

        EndDrawing();
    }

    // unload all fonts and textures
    textureStore.unloadAll();
    fontStore.unloadAll();

    CloseWindow();

    return 0;
}