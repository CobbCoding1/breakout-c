#include <stdio.h>
#include "raylib.h"

#define WIDTH 800
#define HEIGHT 600 

#define SPEED 0.2f
#define BLOCK_COUNT 20

typedef struct {
    Vector2 pos;
    Vector2 size;
} Entity;

Entity block_stack[BLOCK_COUNT];

Entity init_entity(float x, float y, float width, float height) {
    Entity entity = {
        .pos = {
            .x = x,
            .y = y,
        },
        .size = {
            .x = width,
            .y = height,
        }
    };
    return entity;
}

bool check_collision(Entity *e1, Entity *e2) {
    if((e1->pos.x) <= (e2->pos.x + e2->size.x) && (e1->pos.x + e1->size.x) >= e2->pos.x && (e1->pos.y) <= (e2->pos.y + e2->size.y) && ((e1->pos.y + e1->size.y) >= e2->pos.y)) {
        return true;
    }
    return false;
}

int main() {
    Vector2 ball_speed = {
        .x = SPEED - 0.15f,
        .y = SPEED - 0.1f,
    };
    
    InitWindow(WIDTH, HEIGHT, "raylib");
    Entity player = init_entity(WIDTH/2, HEIGHT - (HEIGHT/10), 100, 20);
    Entity ball = init_entity(WIDTH/2, HEIGHT/2, 20, 20);
    Entity window = init_entity(0, 0, WIDTH, HEIGHT);

    for(size_t i = 0; i < BLOCK_COUNT; i++) {
        Entity block = init_entity((35 * i + 30), 20, 30, 20);
        block_stack[i] = block;
    }

    while(!WindowShouldClose()) {
        for(size_t i = 0; i < BLOCK_COUNT; i++) {
            Entity *enemy = &block_stack[i];
            DrawRectangleV(enemy->pos, enemy->size, BLACK);
            if(check_collision(enemy, &ball)) {
                block_stack[i] = init_entity(0, 0, 0, 0);
                ball_speed.y = -ball_speed.y;
            }
        }

        // Key presses
        if(IsKeyDown(KEY_A) && player.pos.x > 0) player.pos.x -= (1 * SPEED);
        if(IsKeyDown(KEY_D) && player.pos.x < (WIDTH - player.size.x)) player.pos.x += (1 * SPEED);

        // ball speed
        ball.pos.y += (1 * ball_speed.y);
        ball.pos.x += (1 * ball_speed.x);

        // player ball collision
        if(check_collision(&player, &ball)) ball_speed.y = -ball_speed.y;

        // ball window collision
        if(ball.pos.y > HEIGHT) {
            return -1;
        }
        if(!check_collision(&ball, &window)) {
            ball_speed.y *= -1;
            ball_speed.x *= -1;
        }

        // init things
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangleV(player.pos, player.size, BLACK);
        DrawRectangleV(ball.pos, ball.size, BLACK);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
