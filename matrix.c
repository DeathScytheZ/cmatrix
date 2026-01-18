#include <raylib.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#define FONT_SIZE 20
#define MAX_GRID_SIZE 200
#define MIN_TRAIL_LEN 10
#define MAX_TRAIL_LEN 30
#define MIN_SPPED 4
#define MAX_SPEED 15
#define INTERVAL 10
#define COLOR_HEAD_DEFAULT (Color) {180, 255, 180, 255}
#define COLOR_BODY_DEFAULT (Color) {0, 255, 70, 255}

int screenWidth;
int screenHeight;
int columnCount = 0;

typedef struct Column{
    int posY;
    char chars[MAX_TRAIL_LEN];
    int len;
    int speed;
    int intervals[MAX_TRAIL_LEN];
} Column;

Column grid[MAX_GRID_SIZE];

char GetRandomChar(){ 
    return (char) (rand() % (126 - 33) + 33);
}

void InitGrid(){
    columnCount = (screenWidth / FONT_SIZE) > MAX_GRID_SIZE ? MAX_GRID_SIZE : (screenWidth / FONT_SIZE );
    for(int i = 0; i < columnCount; i++){
        grid[i].posY = rand() % screenHeight * -1;
        grid[i].len = rand() % (MAX_TRAIL_LEN - MIN_TRAIL_LEN) + MIN_TRAIL_LEN;
        grid[i].speed = rand() % (MAX_SPEED - MIN_SPPED) + MIN_SPPED;
        for(int j = 0; j < grid[i].len; j++){
            grid[i].chars[j] = GetRandomChar();
            grid[i].intervals[j] = rand() % INTERVAL;
        }
    }
}

int main(int argc, char* argv[]){
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(0, 0, "cmatrix");
    SetTargetFPS(60);

    srand(time(NULL));

    int monitor = GetCurrentMonitor();
    screenWidth = GetMonitorWidth(monitor);
    screenHeight = GetMonitorHeight(monitor);
    SetWindowSize(screenWidth, screenHeight);
    ToggleFullscreen();

    Color colorHead;
    Color colorBody;

    Font font = LoadFont("resources/MesloLGS NF Regular.ttf");

    if(argc > 1){
        if(argc < 3){
            fprintf(stderr, "Usage: %s -c <red|blue>\n", argv[0]);
            exit(1);
        }
        if(strcmp(argv[1], "-c") == 0){
            if (strcmp(argv[2], "red") == 0) {
                colorHead = (Color) {255, 0, 0, 255};
                colorBody = (Color) {255, 180, 180, 255}; 
            }else if(strcmp(argv[2], "blue") == 0){
                colorHead = (Color) {0, 0, 255, 255};
                colorBody = (Color) {180, 180, 255, 255}; 
            }else{
                fprintf(stderr, "invalid color: %s\n", argv[2]);
                exit(1);
            }
        }else{
            fprintf(stderr, "invalid argument: %s\n", argv[1]);
            exit(1);
        }
    }else{
        colorHead = COLOR_HEAD_DEFAULT;
        colorBody = COLOR_BODY_DEFAULT;
    }

    InitGrid();

    while(!WindowShouldClose()){
        for(int i = 0; i < columnCount; i++){
            grid[i].posY += grid[i].speed;
            for(int j = 0; j < grid[i].len; j++){
                grid[i].intervals[j]++;
                if(grid[i].intervals[j] > INTERVAL && (rand() % 10 == 0)){
                    grid[i].chars[j] = GetRandomChar();
                    grid[i].intervals[j] = rand() % 20;
                }
            }
            if((grid[i].posY - (grid[i].len * FONT_SIZE)) > screenHeight){
                grid[i].posY = rand() % screenHeight * -1;
                grid[i].speed = rand() % (MAX_SPEED - MIN_SPPED) + MIN_SPPED;
                grid[i].len = rand() % (MAX_TRAIL_LEN - MIN_TRAIL_LEN) + MIN_TRAIL_LEN; 
                for(int j = 0; j < grid[i].len; j++){
                    grid[i].chars[j] = GetRandomChar();
                    grid[i].intervals[j] = rand() % 20;
                }
            }
        }

        BeginDrawing();
        ClearBackground(BLACK);
        for(int i = 0; i < columnCount; i++){
            int posX = i * FONT_SIZE;
            for(int j = 0; j < grid[i].len; j++){
                int posY = grid[i].posY - (j * FONT_SIZE);
                Color trailColor;
                if(j == 0){
                    trailColor = colorHead;
                } else if(j <= 3){
                    trailColor = colorBody;
                } else{
                    int len = (255 - 25) / grid[i].len; 
                    float alpha = (255 - (len * j)) / 255.0f;
                    trailColor = ColorAlpha(colorBody, (float) alpha);
                }
                DrawTextCodepoint(font, grid[i].chars[j], (Vector2) {posX, posY}, FONT_SIZE, trailColor);
            }
        }
        EndDrawing();
    }

    UnloadFont(font);
    CloseWindow();
    return 0;
}
