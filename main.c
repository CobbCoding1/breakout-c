#include <stdio.h>
#include "raylib.h"

#define WIDTH 800
#define HEIGHT 600 

int main() {
    InitWindow(WIDTH, HEIGHT, "raylib");
    while(!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("HELLO", 100, 100, 20, BLACK);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
