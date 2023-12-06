#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include "raylib.h"

#define WIDTH 800
#define HEIGHT 600 

#define SPEED 500.0f
#define PLAYER_SPEED 1.5 * SPEED
#define BALL_SPEED SPEED * 0.5
#define BLOCK_COUNT 60 
#define BLOCK_WIDTH 40 
#define BLOCK_HEIGHT 20
#define ITERATIONS 4
#define BOUNCE_WAIT 300

typedef struct {
    Vector2 pos;
    Vector2 size;
} Entity;

typedef struct {
    Entity entity;
    bool is_destroyed;
} Block;

typedef struct {
    int score;
    int high_score;
    size_t destroyed_count; 
    size_t bounce_wait;
    Vector2 ball_initial_pos;
    Vector2 ball_speed;
    float delta_time;
    Entity player;
    Entity ball;
} Game;

Block block_stack[BLOCK_COUNT];

char *read_from_file(char *file_name) {
    FILE *file = fopen(file_name, "a+");
    if(file == NULL) {
        fprintf(stderr, "error: could not open file %s\n", file_name);
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *current_score = malloc(sizeof(char) * length);
    fread(current_score, 1, length, file);
    current_score[length] = '\0';
    fclose(file);
    return current_score;
}

void write_to_file(char *file_name, int score) {
    FILE *file = fopen(file_name, "a+");
    if(file == NULL) {
        fprintf(stderr, "error: could not open file %s\n", file_name);
        exit(1);
    }
    fseek(file, 0, SEEK_END);
    int length = ftell(file);
    fseek(file, 0, SEEK_SET);
    char *current_score = malloc(sizeof(char) * length);
    fread(current_score, 1, length, file);
    current_score[length] = '\0';
    fclose(file);
    if(atoi(current_score) < score) {
        file = fopen(file_name, "w+");
        fprintf(file, "%d", score);
        fclose(file);
    }
    free(current_score);
}

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
    if((e1->pos.x) <= (e2->pos.x + e2->size.x) && 
        (e1->pos.x + e1->size.x) >= e2->pos.x && 
        (e1->pos.y) <= (e2->pos.y + e2->size.y) && 
        ((e1->pos.y + e1->size.y) >= e2->pos.y)) {
        return true;
    }
    return false;
}

void reset_blocks() {
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
}

void reset_ball(Entity *ball, const Game *game) {
    ball->pos.x = game->ball_initial_pos.x;
    ball->pos.y = game->ball_initial_pos.y;
}

float random_x() {
    srand(time(NULL));
    return SPEED - (float)rand()/(float)(RAND_MAX/0.19f);
}

void init_game(Game *game) {
    char *high_score = read_from_file("highscore.txt");
    printf("%s\n", high_score);

    
    game->score = 0;
    game->destroyed_count = 0;
    Vector2 ball_speed = {
        .x = random_x(),
        .y = BALL_SPEED,
    };
    game->ball_speed = ball_speed;
    Vector2 ball_initial_pos = {
        .x = WIDTH/2,
        .y = HEIGHT/2,
    };
    game->ball_initial_pos = ball_initial_pos;
    game->bounce_wait = 0;
    game->high_score = atoi(high_score);
    free(high_score);


    reset_blocks();
}



int main() {
    Game game = {0};
    init_game(&game);
    game.ball = init_entity(game.ball_initial_pos.x, game.ball_initial_pos.y, 20, 20);
    game.player = init_entity(WIDTH/2, HEIGHT - (HEIGHT/10), 100, 20);
    
    InitWindow(WIDTH, HEIGHT, "breakout");

    while(!WindowShouldClose()) {
        game.delta_time = GetFrameTime();
        // crash if all blocks are destroyed
        if(game.destroyed_count >= BLOCK_COUNT) {
            Vector2 font_size = MeasureTextEx(GetFontDefault(), "GAME OVER", 100, 10.0f);
            DrawText("GAME OVER", WIDTH/2 - font_size.x/2, HEIGHT/2 - font_size.y/2, 100, BLUE);
            EndDrawing();
            write_to_file("highscore.txt", game.score);
            reset_ball(&game.ball, &game);
            init_game(&game);
            game.score += 1;
            sleep(2);
        }

        // block rendering and collision handling
        for(size_t i = 0; i < BLOCK_COUNT; i++) {
            Block *enemy = &block_stack[i];
            if(!enemy->is_destroyed) {
                DrawRectangleV(enemy->entity.pos, enemy->entity.size, BLACK);
                if(check_collision(&enemy->entity, &game.ball)) {
                    game.destroyed_count++;
                    block_stack[i].is_destroyed = true; 
                    game.score += 1;
                    game.ball_speed.y = BALL_SPEED;
                }
            }
        }

        // Key presses
        if(game.player.pos.x < 0) game.player.pos.x = 0;
        if(game.player.pos.x > (WIDTH - game.player.size.x)) game.player.pos.x = (WIDTH - game.player.size.x);
        if(IsKeyDown(KEY_A)) game.player.pos.x -= PLAYER_SPEED * game.delta_time;
        if(IsKeyDown(KEY_D)) game.player.pos.x += PLAYER_SPEED * game.delta_time;

        // game.ball speed
        game.ball.pos.y += (1 * game.ball_speed.y) * game.delta_time;
        game.ball.pos.x += (1 * game.ball_speed.x) * game.delta_time;

        // game.player game.ball collision
        if(game.bounce_wait == 0) {
            if((game.player.pos.x) <= (game.ball.pos.x + game.ball.size.x) && 
               (game.player.pos.x + game.player.size.x) >= game.ball.pos.x && 
               (game.player.pos.y) <= (game.ball.pos.y + game.ball.size.y) && 
               (game.player.pos.y + 5) >= (game.ball.pos.y + game.ball.size.y) && 
               (game.player.pos.y + game.player.size.y) >= (game.ball.pos.y)
            ) {
                game.ball_speed.y *= -1;
                game.bounce_wait = BOUNCE_WAIT;
            } else if((game.player.pos.x) <= (game.ball.pos.x + game.ball.size.x) && (game.player.pos.x + game.ball.size.x * 0.25) >= (game.ball.pos.x + game.ball.size.x) && 
               (game.player.pos.x + game.player.size.x) >= game.ball.pos.x && 
               (game.player.pos.y) <= (game.ball.pos.y + game.ball.size.y) && 
               (game.player.pos.y + game.player.size.y) >= (game.ball.pos.y)
            ) {
                game.ball_speed.y *= -1;
                game.ball_speed.x = -random_x();
                game.bounce_wait = BOUNCE_WAIT;
            } else if((game.player.pos.x) <= (game.ball.pos.x + game.ball.size.x) &&
               (game.player.pos.x + game.player.size.x) >= game.ball.pos.x && (game.player.pos.x + game.player.size.x) <= (game.ball.pos.x + game.ball.size.x * 0.25) &&
               (game.player.pos.y) <= (game.ball.pos.y + game.ball.size.y) && 
               (game.player.pos.y + game.player.size.y) >= (game.ball.pos.y)
            ) {
                game.ball_speed.y *= -1;
                game.ball_speed.x = random_x();
                game.bounce_wait = BOUNCE_WAIT;
            }
        }

        // game.ball window collision
        if(game.ball.pos.y > HEIGHT) {
            game.score -= 1; 
            reset_ball(&game.ball, &game);
        }
        if(game.ball.pos.y < 0) game.ball_speed.y *= -1; 
        if(game.ball.pos.x > WIDTH) game.ball_speed.x *= -1; 
        if(game.ball.pos.x < 0) game.ball_speed.x *= -1;

        // init things
        BeginDrawing();
        ClearBackground(LIGHTGRAY);
        DrawRectangleV(game.player.pos, game.player.size, BLACK);
        DrawRectangleV(game.ball.pos, game.ball.size, BLACK);
        char *score_str = malloc(45);
        sprintf(score_str, "Score: %d   High Score: %d", game.score, game.high_score);
        DrawText(score_str, 5, 0, 20, BLUE);
        free(score_str);
        EndDrawing();
        if(game.bounce_wait > 0) game.bounce_wait--;
    }
    CloseWindow();
    return 0;
}
