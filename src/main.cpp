#include<bitset>
#include<iostream>
#include<vector>

#include"raylib.h"

#include"analyzeTiles.h"
#include"grid.h"
#include"menu.h"
#include"storage.h"
#include"utils.h"

#ifdef PLATFORM_WEB
    #include<emscripten/emscripten.h>
#endif

// set up global pointers
Grid* gridPtr;
MenuControl* menusPtr;

void UpdateDrawFrame();

int main(){

    InitWindow(screenWidth, screenHeight, "Wavefunction Collapse");

    // chose random tileset
    tilesetDir = setUpTileset();

    // create grid with tileset and data sheet
    Grid grid;
    gridPtr = &grid;

    // create controlls and tile select menu
    MenuControl menus(
        createControlsMenu(screenWidth-700.0f, screenHeight-105.0f, grid, &Grid::reset, grid.running, grid.updateSpeed),
        createTilesMenu(10.0f, 10.0f, tilesetDir,grid)
    );
    menusPtr = &menus;

    // Web version uses emscripten function
    #ifdef PLATFORM_WEB
        emscripten_set_main_loop(UpdateDrawFrame, 60, 1);
    #else
        SetTargetFPS(fps);
        while(!WindowShouldClose()){ UpdateDrawFrame(); }
    #endif

    // unload all fonts and textures
    textureStore.unloadAll();
    fontStore.unloadAll();

    CloseWindow();

    return 0;
}

// main loop funciton
void UpdateDrawFrame(){

    // add elapsed time since last update
    sinceLastUpdate += GetFrameTime();

    // update mousepos
    mousePos = GetMousePosition();

    BeginDrawing();
    ClearBackground(RAYWHITE);

    // reset on completion
    if (gridPtr->collapsed){ gridPtr->reset(); }

    // draw grid and menu
    gridPtr->draw();
    menusPtr->display();

    // collapse next grid position
    gridPtr->update();

    EndDrawing();
}