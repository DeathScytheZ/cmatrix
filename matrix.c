#include <raylib.h>
#include <raymath.h>
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
#define COLOR_HEAD_BLACK (Color) {0, 0, 0, 255}
#define COLOR_BODY_BLACK (Color) {96, 96, 96, 255}
#define COLOR_HEAD_WHITE (Color) {255, 255, 255, 255}
#define COLOR_BODY_WHITE (Color) {160, 160, 160, 255}
#define COLOR_HEAD_RED (Color) {255, 0, 0, 255}
#define COLOR_BODY_RED (Color) {255, 83, 83, 255}
#define COLOR_HEAD_BLUE (Color) {0, 0, 255, 255}
#define COLOR_BODY_BLUE (Color) {180, 180, 255, 255}
#define COLOR_HEAD_YELLOW (Color) {255, 255, 0, 255}
#define COLOR_BODY_YELLOW (Color) {255, 255, 72, 255}
#define COLOR_HEAD_ORANGE (Color) {255, 128, 0, 255}
#define COLOR_BODY_ORANGE (Color) {255, 153, 51, 255}
#define COLOR_HEAD_PINK (Color) {255, 0, 127, 255}
#define COLOR_BODY_PINK (Color) {255, 153, 204, 255}
#define COLOR_HEAD_PURPLE (Color) {127, 0, 255, 255}
#define COLOR_BODY_PURPLE (Color) {153, 51, 255, 255}
#define COLOR_HEAD_CYAN (Color) {0, 204, 204, 255}
#define COLOR_BODY_CYAN (Color) {153, 255, 255, 255}

#define MY_WHITE (Color) {225, 225, 225, 255}

typedef struct Column{
    int posY;
    char chars[MAX_TRAIL_LEN];
    int len;
    int speed;
    int intervals[MAX_TRAIL_LEN];
} Column;

typedef struct ColorTheme{
    const char* name;
    Color head;
    Color body;
} ColorTheme;

const ColorTheme themes[] = {
    {"white", COLOR_HEAD_WHITE, COLOR_BODY_WHITE},
    {"black", COLOR_HEAD_BLACK, COLOR_BODY_BLACK},
    {"red", COLOR_HEAD_RED, COLOR_BODY_RED},
    {"green", COLOR_HEAD_DEFAULT, COLOR_BODY_DEFAULT},
    {"blue", COLOR_HEAD_BLUE, COLOR_BODY_BLUE},
    {"yellow", COLOR_HEAD_YELLOW, COLOR_BODY_YELLOW},
    {"orange", COLOR_HEAD_ORANGE, COLOR_BODY_ORANGE},
    {"pink", COLOR_HEAD_PINK, COLOR_BODY_PINK},
    {"purple", COLOR_HEAD_PURPLE, COLOR_BODY_PURPLE},
    {"cyan", COLOR_HEAD_CYAN, COLOR_BODY_CYAN}
};

Column grid[MAX_GRID_SIZE];
int numThemes = sizeof(themes) / sizeof(themes[0]);
int themeIndex;

int screenWidth;
int screenHeight;
int columnCount = 0;

Color colorHead;
Color colorBody;
Color backgroundColor = BLACK;

bool isRainbowMode = false;

bool isCustomFontUsed = true;

Vector2 oldMousePosition;
Vector2 newMousePosition;
int framesCounter = 0;

char GetRandomChar(){ 
    return (char) (rand() % (126 - 33) + 33);
}

void SetThemeScheme(int argc, char* argv[]){
    if(argc > 1){
        if(argc < 3){
            fprintf(stderr, "Usage: %s -c <red|blue>\n", argv[0]);
            exit(1);
        }
        if(strcmp(argv[1], "-c") == 0){
            if(strcmp(argv[2], "rainbow") == 0){
                isRainbowMode = true;          
                return;
            }
            if(strcmp(argv[2], "random") == 0){
                int random = rand() % numThemes;
                colorHead = themes[random].head;
                colorBody = themes[random].body;
                if(strcmp(themes[random].name, "black") == 0) backgroundColor = MY_WHITE;
                return;
            }
            for(int i = 0; i < numThemes; i++){
                if(strcmp(argv[2], themes[i].name) == 0){
                    colorHead = themes[i].head;
                    colorBody = themes[i].body;
                    if(strcmp(argv[2], "black") == 0) backgroundColor = MY_WHITE;
                    return;
                }     
            }
            fprintf(stderr, "invalid color: %s\n", argv[2]);
            exit(1);
        }else{
            fprintf(stderr, "invalid argument: %s\n", argv[1]);
            exit(1);
        }
    }else{
        colorHead = COLOR_HEAD_DEFAULT;
        colorBody = COLOR_BODY_DEFAULT;
    }
}

void ResetColumn(int col){
    grid[col].posY = rand() % screenHeight * -1;
    grid[col].len = rand() % (MAX_TRAIL_LEN - MIN_TRAIL_LEN) + MIN_TRAIL_LEN;
    grid[col].speed = rand() % (MAX_SPEED - MIN_SPPED) + MIN_SPPED;
    for(int j = 0; j < grid[col].len; j++){
        grid[col].chars[j] = GetRandomChar();
        grid[col].intervals[j] = rand() % INTERVAL;
    }
}

void InitGrid(){
    columnCount = (screenWidth / FONT_SIZE) > MAX_GRID_SIZE ? MAX_GRID_SIZE : (screenWidth / FONT_SIZE );
    for(int i = 0; i < columnCount; i++){
        ResetColumn(i); 
    }
}

int main(int argc, char* argv[]){
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(0, 0, "cmatrix");
    SetTargetFPS(60);
    HideCursor();

    srand(time(NULL));

    int monitor = GetCurrentMonitor();
    screenWidth = GetMonitorWidth(monitor);
    screenHeight = GetMonitorHeight(monitor);
    SetWindowSize(screenWidth, screenHeight);
    ToggleFullscreen();

    Font font = LoadFont("/home/deathscythe/my_repos/cmatrix/resources/MesloLGS NF Regular.ttf");

    if(!IsFontValid(font)){
        font = GetFontDefault();
        isCustomFontUsed = false;
    } 

    SetThemeScheme(argc, argv);

    InitGrid();

    oldMousePosition = GetMousePosition();

    while(!WindowShouldClose()){
        newMousePosition = GetMousePosition();
        if(framesCounter > 10){
            if((Vector2Distance(newMousePosition, oldMousePosition) > 10.0f)) break;
        }else{
            framesCounter++;
            oldMousePosition = newMousePosition;
        }
        if(GetKeyPressed()) break;
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT) || IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) break;

        for(int i = 0; i < columnCount; i++){
            grid[i].posY += grid[i].speed;
            for(int j = 0; j < grid[i].len; j++){
                grid[i].intervals[j]++;
                if(grid[i].intervals[j] > INTERVAL && (rand() % 10 == 0)){
                    grid[i].chars[j] = GetRandomChar();
                    grid[i].intervals[j] = rand() % INTERVAL;
                }
            }
            if((grid[i].posY - (grid[i].len * FONT_SIZE)) > screenHeight){
                ResetColumn(i); 
            }
        }

        BeginDrawing();
        ClearBackground(backgroundColor);
        for(int i = 0; i < columnCount; i++){
            int posX = i * FONT_SIZE;
            for(int j = 0; j < grid[i].len; j++){
                int posY = grid[i].posY - (j * FONT_SIZE);
                Color trailColor;
                if(isRainbowMode) themeIndex = rand() % numThemes;
                if(j == 0){
                    trailColor = isRainbowMode == true ? themes[themeIndex].head : colorHead;
                } else if(j <= 3){
                    trailColor = isRainbowMode == true ? themes[themeIndex].body : colorBody;
                } else{
                    int len = (255 - 25) / grid[i].len; 
                    float alpha = (255 - (len * j)) / 255.0f;
                    trailColor = isRainbowMode == true ? themes[themeIndex].body : colorBody;
                    trailColor = ColorAlpha(trailColor, (float) alpha);
                }
                DrawTextCodepoint(font, grid[i].chars[j], (Vector2) {posX, posY}, FONT_SIZE, trailColor);
            }
        }
        EndDrawing();
    }
    if(isCustomFontUsed) UnloadFont(font);
    CloseWindow();
    return 0;
}
