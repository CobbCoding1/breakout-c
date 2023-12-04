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

typedef struct {
    size_t score;
    size_t destroyed_count; 
    Vector2 ball_initial_pos;
    Vector2 ball_speed;
} Game;

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

Game init_game() {
    Game game = {
        .score = 0,
        .destroyed_count = 0,
        .ball_speed = {
            .x = SPEED - 0.15f,
            .y = SPEED - 0.05f,
        },
        .ball_initial_pos = {
            .x = WIDTH/2,
            .y = HEIGHT/2,
        },
    };
    return game;
}

int main() {
    Game game = init_game();
    
    InitWindow(WIDTH, HEIGHT, "breakout");
    Entity player = init_entity(WIDTH/2, HEIGHT - (HEIGHT/10), 100, 20);
    Entity ball = init_entity(game.ball_initial_pos.x, game.ball_initial_pos.y, 20, 20);
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
        // crash if all blocks are destroyed
        if(game.destroyed_count >= BLOCK_COUNT) return 1;

        // block rendering and collision handling
        for(size_t i = 0; i < BLOCK_COUNT; i++) {
            Block *enemy = &block_stack[i];
            if(!enemy->is_destroyed) {
                DrawRectangleV(enemy->entity.pos, enemy->entity.size, BLACK);
                if(check_collision(&enemy->entity, &ball)) {
                    game.destroyed_count++;
                    block_stack[i].is_destroyed = true; 
                    game.score += 1;
                    game.ball_speed.y = -game.ball_speed.y;
                }
            }
        }

        // Key presses
        if(IsKeyDown(KEY_A) && player.pos.x > 0) player.pos.x -= (1 * SPEED);
        if(IsKeyDown(KEY_D) && player.pos.x < (WIDTH - player.size.x)) player.pos.x += (1 * SPEED);

        // ball speed
        ball.pos.y += (1 * game.ball_speed.y);
        ball.pos.x += (1 * game.ball_speed.x);

        // player ball collision
        if(check_collision(&player, &ball)) game.ball_speed.y = -game.ball_speed.y;

        // ball window collision
        if(ball.pos.y > HEIGHT) {
            game.score -= 1; 
            ball.pos.x = game.ball_initial_pos.x;
            ball.pos.y = game.ball_initial_pos.y;
        }
        if(ball.pos.y < 0) game.ball_speed.y *= -1; 
        if(ball.pos.x > WIDTH) game.ball_speed.x *= -1; 
        if(ball.pos.x < 0) game.ball_speed.x *= -1;

        // init things
        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        DrawRectangleV(player.pos, player.size, BLACK);
        DrawRectangleV(ball.pos, ball.size, BLACK);
        char *score_str = malloc(11);
        sprintf(score_str, "Score: %zu", game.score);
        DrawText(score_str, 5, 0, 20, BLUE);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
