#include"constants.h"

#include"raylib.h"

int main(){

    InitWindow(screenWidth, screenHeight, "Wavefunction Collapse");

    Texture2D tiles = LoadTexture("tilesets/circuit-tileset-182x14.png");


    SetTargetFPS(60);
    while (!WindowShouldClose()){


        BeginDrawing();
        ClearBackground(RAYWHITE);

        for (int i=0; i<14; i++){
            //DrawTextureRec(tiles, {i*14.0f, 0.0f, 14.0f, 14.0f}, {i*16.0f, 0.0f}, WHITE);
            DrawTexturePro(tiles, {i*14.0f, 0.0f, 14.0f, 14.0f}, {i*16.0f*4, 0.0f, 14.0f*4, 14.0f*4}, {}, 0.0f, WHITE);
        }

        EndDrawing();
    }

    UnloadTexture(tiles);

    CloseWindow();

    return 0;
}