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

    // create grid with tieset and data sheet
    Grid grid(textureStore.add(tilesetFile), tilesetDataFile);

    // create controlls and tile select menu
    MenuControl menus(
        createControlsMenu(screenWidth*0.5f, screenHeight*0.87f, grid, Grid::reset, grid.running, grid.updateSpeed),
        createTilesMenu(10.0f, 10.0f, tilesetFile, weights)
    );

    SetTargetFPS(fps);
    while(!WindowShouldClose()){

        // add elapsed time since last update
        sinceLastUpdate += GetFrameTime();

        // update mousepos
        mousePos = GetMousePosition();

        BeginDrawing();
        ClearBackground(RAYWHITE);

        grid.draw();
        menus.display();

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