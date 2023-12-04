#include <stdio.h>
#include <stdlib.h>
#include "raylib.h"

#define WIDTH 800
#define HEIGHT 600 

#define SPEED 0.2f
#define BLOCK_COUNT 60 
#define BLOCK_WIDTH 40 
#define BLOCK_HEIGHT 20
#define ITERATIONS 4

typedef struct {
    Vector2 pos;
    Vector2 size;
} Entity;

typedef struct {
    Entity entity;
    bool is_destroyed;
} Block;

Block block_stack[BLOCK_COUNT];

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
        .y = SPEED - 0.05f,
    };

    Vector2 ball_initial_pos = {
        .x = WIDTH/2,
        .y = HEIGHT/2,
    };
    
    InitWindow(WIDTH, HEIGHT, "breakout");
    Entity player = init_entity(WIDTH/2, HEIGHT - (HEIGHT/10), 100, 20);
    Entity ball = init_entity(ball_initial_pos.x, ball_initial_pos.y, 20, 20);
    int score = 0;
    size_t destroyed_count = 0;
    size_t block_stack_pos = 0;

    for(size_t layers = 0; layers <= ITERATIONS; layers++) {
        for(size_t i = 0; i < BLOCK_COUNT/ITERATIONS; i++) {
            Block block = {
                .entity = init_entity(((BLOCK_WIDTH + 10) * i + (BLOCK_WIDTH - 5)), ((BLOCK_HEIGHT + 5) * layers) + 20, BLOCK_WIDTH, BLOCK_HEIGHT),
                .is_destroyed = false,
            };
            block_stack[block_stack_pos++] = block;
        }
    }

    while(!WindowShouldClose()) {
        if(destroyed_count >= BLOCK_COUNT) return 1;
        for(size_t i = 0; i < BLOCK_COUNT; i++) {
            Block *enemy = &block_stack[i];
            if(!enemy->is_destroyed) {
                DrawRectangleV(enemy->entity.pos, enemy->entity.size, BLACK);
                if(check_collision(&enemy->entity, &ball)) {
                    destroyed_count++;
                    block_stack[i].is_destroyed = true; 
                    score += 1;
                    ball_speed.y = -ball_speed.y;
                }
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
            score -= 1; 
            ball.pos.x = ball_initial_pos.x;
            ball.pos.y = ball_initial_pos.y;
        }
        if(ball.pos.y < 0) ball_speed.y *= -1; 
        if(ball.pos.x > WIDTH) ball_speed.x *= -1; 
        if(ball.pos.x < 0) ball_speed.x *= -1;

        // init things
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawRectangleV(player.pos, player.size, BLACK);
        DrawRectangleV(ball.pos, ball.size, BLACK);
        char *score_str = malloc(11);
        sprintf(score_str, "Score: %d", score);
        DrawText(score_str, 5, 0, 20, RED);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
