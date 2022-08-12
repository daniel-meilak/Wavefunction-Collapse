#include<bitset>
#include<iostream>
#include<vector>

#include"raylib.h"

#include"analyzeTiles.h"
#include"grid.h"
#include"menu.h"
#include"storage.h"

int main(){

    InitWindow(screenWidth, screenHeight, "Wavefunction Collapse"); 

    // create grid with tileset and data sheet
    Grid grid(tilesetDir);

    // create controlls and tile select menu
    MenuControl menus(
        createControlsMenu(screenWidth-700.0f, screenHeight-105.0f, grid, Grid::reset, grid.running, grid.updateSpeed),
        createTilesMenu(10.0f, 10.0f, tilesetDir, weights)
    );

    SetTargetFPS(fps);
    while(!WindowShouldClose()){

        // add elapsed time since last update
        sinceLastUpdate += GetFrameTime();

        // update mousepos
        mousePos = GetMousePosition();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        // draw grid and menu
        grid.draw();
        menus.display();

        // collapse next grid position
        grid.update();

        // reset on completion
        if (grid.collapsed){ grid.reset(); }

        EndDrawing();
    }

    // unload all fonts and textures
    textureStore.unloadAll();
    fontStore.unloadAll();

    CloseWindow();

    return 0;
}