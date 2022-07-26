#include<iostream>
#include<vector>

#include"raylib.h"

#include"constants.h"
#include"analyzeTiles.h"

int main(){

    InitWindow(screenWidth, screenHeight, "Wavefunction Collapse");

    analyzeTiles("tilesets/uniques.txt");

    CloseWindow();

    return 0;
}