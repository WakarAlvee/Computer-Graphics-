#include <windows.h>
#include <mmsystem.h>
#include <GL/glut.h>
#include <stdio.h>
#include <cmath>
#include <cstdlib>
#include <ctime>
//#pragma comment(lib, "winmm.lib")  // Link multimedia library

#define M_PI 3.14159265359

// =============== GAME STATE MANAGEMENT ===============
enum GameState {
    MAIN_MENU,
    EASY_MODE,
    MEDIUM_MODE,
    HARD_MODE
};

GameState currentState = MAIN_MENU;

// =============== GLOBAL VARIABLES ===============
int SCREEN_W = 80, SCREEN_H = 40;
int windowWidth = 1000, windowHeight = 500;
int NUM_COLS = 16;
float COL_W = 5.0f;

// Main menu variables
int rotationAngle = 0;

// Easy mode variables
float speed_easy = 0.15f;
bool gameOver_easy = false;
float tileY_easy[8] = {60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 110.0f, 120.0f, 130.0f};
int playerCol_easy = 7;
float playerY_easy = 0.0f;

// Medium mode variables
float speed_medium = 0.003f;
float maxSpeed_medium = 0.015f;
float speedIncrement_medium = 0.00003f;
bool gameOver_medium = false;
bool bombCaught_medium = false;
bool showExplosion_medium = false;
int explosionTimer_medium = 0;
float tileY_medium[8] = {60.0f, 70.0f, 80.0f, 90.0f, 100.0f, 110.0f, 120.0f, 130.0f};
int playerCol_medium = 7;
float playerY_medium = 0.0f;

// Hard mode variables
float speed_hard = 0.05f;
bool gameOver_hard = false;
bool gameWon_hard = false;
int score_hard = 0;
float tileY_hard[16];
float bombY_hard[3];
int bombCols_hard[3];
bool bombExploding_hard = false;
float explodeX_hard = 0, explodeY_hard = 0;
float explodeRadius_hard = 0;
float shakeOffset_hard = 0;
int shakeFrame_hard = 0;
int playerCol_hard = 3;
float playerY_hard = 0;
int playerCol2_hard = 11;
float playerY2_hard = 0;
int speedupTimer_hard = 0;

// =============== FUNCTION PROTOTYPES ===============
void display();
void reshape(int width, int height);
void update(int value);
void keyboard(int key, int x, int y);
void normalKeys(unsigned char key, int x, int y);
void renderBitmapString(int x, int y, void* font, const char* string);

// Main menu functions
void drawAllShapes();
void drawSquareTile_menu(int x, int y, int size);
void drawTriangleTile_menu(int x, int y, int size);
void drawStarTile_menu(int x, int y, int size);
void drawDiamondTile_menu(int x, int y, int size);
void drawPentagonTile_menu(int x, int y, int size);
void drawFullCircleTile_menu(int x, int y, int size);
void drawHeartTile_menu(int x, int y, int size);
void drawCrossTile_menu(int x, int y, int size);
void displayMainMenu();

// Easy mode functions
void drawBackground_easy();
void drawTrees_easy();
void drawSquareTile_easy(int col, float y);
void drawTriangleTile_easy(int col, float y);
void drawStarTile_easy(int col, float y);
void drawDiamondTile_easy(int col, float y);
void drawPentagonTile_easy(int col, float y);
void drawFullCircleTile_easy(int col, float y);
void drawHeartTile_easy(int col, float y);
void drawWheelTile_easy(int col, float y);
void drawCrossTile_easy(int col, float y);
void drawPlayer_easy();
void drawGameOver_easy();
void displayEasyMode();
bool collision_easy(float y, int col);
void sound_easy(int col);

// Medium mode functions
void drawBackground_medium();
void drawTrees_medium();
void drawSquareTile_medium(int col, float y);
void drawTriangleTile_medium(int col, float y);
void drawStarTile_medium(int col, float y);
void drawDiamondTile_medium(int col, float y);
void drawPentagonTile_medium(int col, float y);
void drawFullCircleTile_medium(int col, float y);
void drawBombTile_medium(int col, float y);
void drawWheelTile_medium(int col, float y);
void drawCrossTile_medium(int col, float y);
void drawPlayer_medium();
void drawGameOver_medium();
void drawBombExplosion_medium();
void displayMediumMode();
bool collision_medium(float y, int col);
void sound_medium(int col);

// Hard mode functions
void drawBackground_hard();
void drawSquareTile_hard(int col, float y);
void drawTriangleTile_hard(int col, float y);
void drawCircleTile_hard(int col, float y);
void drawDiamondTile_hard(int col, float y);
void drawBomb_hard(int col, float y);
void drawExplosion_hard();
void drawPlayer_hard();
void drawGameOver_hard();
void drawScore_hard();
void drawWinMessage_hard();
void displayHardMode();
bool collision_hard(float y, int col, int pCol, float pY);
void sound_hard(int col);

// =============== UTILITY FUNCTIONS ===============
void renderBitmapString(int x, int y, void* font, const char* string) {
    const char* c;
    glRasterPos2i(x, y);
    for (c = string; *c != '\0'; c++) {
        glutBitmapCharacter(font, *c);
    }
}

void sound_easy() {
    PlaySound(NULL, NULL, SND_PURGE);  // Stop any playing sound
    PlaySound("a.wav", NULL, SND_ASYNC|SND_FILENAME);
}

void sound_medium() {
    //printf("Medium mode - Caught tile in column %d!\n", col);
    PlaySound("a.wav", NULL,SND_ASYNC|SND_FILENAME|SND_LOOP);
}

void sound_hard() {
  //  printf("Hard mode - Caught tile in column %d!\n", col);
    PlaySound("a.wav", NULL,SND_ASYNC|SND_FILENAME|SND_LOOP);
}

bool collision_easy(float y, int col) {
    return (col == playerCol_easy && y <= playerY_easy + 6.0f && y >= playerY_easy);
}

bool collision_medium(float y, int col) {
    return (col == playerCol_medium && y <= playerY_medium + 6.0f && y >= playerY_medium);
}

bool collision_hard(float y, int col, int pCol, float pY) {
    return (col == pCol && y <= pY + 6 && y >= pY);
}

// =============== MAIN MENU DRAWING FUNCTIONS ===============
void drawSquareTile_menu(int x, int y, int size) {
    glPushMatrix();
    glTranslatef(x + size/2, y + size/2, 0);
    glRotatef(rotationAngle, 0, 0, 1);
    glTranslatef(-size/2, -size/2, 0);
    glColor3f(1.0, 0.6, 0.0);
    glBegin(GL_QUADS);
        glVertex2i(0, 0);
        glVertex2i(size, 0);
        glVertex2i(size, size);
        glVertex2i(0, size);
    glEnd();
    glColor3f(0.8, 0.3, 0.0);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
        glVertex2i(0, 0);
        glVertex2i(size, 0);
        glVertex2i(size, size);
        glVertex2i(0, size);
    glEnd();
    glPopMatrix();
}

void drawTriangleTile_menu(int x, int y, int size) {
    glPushMatrix();
    glTranslatef(x + size/2, y + size/2, 0);
    glRotatef(rotationAngle, 0, 0, 1);
    glColor3f(1.0, 0.2, 0.8);
    glBegin(GL_TRIANGLES);
        glVertex2i(-size/2, -size/2);
        glVertex2i(size/2, -size/2);
        glVertex2i(0, size/2);
    glEnd();
    glColor3f(0.7, 0.0, 0.5);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
        glVertex2i(-size/2, -size/2);
        glVertex2i(size/2, -size/2);
        glVertex2i(0, size/2);
    glEnd();
    glPopMatrix();
}

void drawStarTile_menu(int x, int y, int size) {
    glPushMatrix();
    glTranslatef(x + size/2, y + size/2, 0);
    glRotatef(rotationAngle, 0, 0, 1);
    int r = size / 2;
    glColor3f(1.0, 0.9, 0.1);
    glBegin(GL_POLYGON);
        glVertex2f(0.18 * r, 0.35 * r);
        glVertex2f(0.9 * r, 0.35 * r);
        glVertex2f(0.28 * r, -0.05 * r);
        glVertex2f(0.46 * r, -0.8 * r);
        glVertex2f(0.0 * r, -0.35 * r);
        glVertex2f(-0.46 * r, -0.8 * r);
        glVertex2f(-0.28 * r, -0.05 * r);
        glVertex2f(-0.9 * r, 0.35 * r);
        glVertex2f(-0.18 * r, 0.35 * r);
        glVertex2f(0.0 * r, 0.95 * r);
    glEnd();
    glColor3f(0.8, 0.6, 0.0);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.0 * r, 0.95 * r);
        glVertex2f(0.18 * r, 0.35 * r);
        glVertex2f(0.9 * r, 0.35 * r);
        glVertex2f(0.28 * r, -0.05 * r);
        glVertex2f(0.46 * r, -0.8 * r);
        glVertex2f(0.0 * r, -0.35 * r);
        glVertex2f(-0.46 * r, -0.8 * r);
        glVertex2f(-0.28 * r, -0.05 * r);
        glVertex2f(-0.9 * r, 0.35 * r);
        glVertex2f(-0.18 * r, 0.35 * r);
    glEnd();
    glPopMatrix();
}

void drawDiamondTile_menu(int x, int y, int size) {
    glPushMatrix();
    glTranslatef(x + size/2, y + size/2, 0);
    glRotatef(rotationAngle, 0, 0, 1);
    int half = size / 2;
    glColor3f(0.8, 0.3, 1.0);
    glBegin(GL_QUADS);
        glVertex2i(0, half);
        glVertex2i(half, 0);
        glVertex2i(0, -half);
        glVertex2i(-half, 0);
    glEnd();
    glColor3f(0.5, 0.1, 0.7);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
        glVertex2i(0, half);
        glVertex2i(half, 0);
        glVertex2i(0, -half);
        glVertex2i(-half, 0);
    glEnd();
    glPopMatrix();
}

void drawPentagonTile_menu(int x, int y, int size) {
    glPushMatrix();
    glTranslatef(x + size/2, y + size/2, 0);
    glRotatef(rotationAngle, 0, 0, 1);
    int r = size / 2;
    glColor3f(0.9, 0.4, 0.9);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 5; i++) {
            double angle = -M_PI/2.0 + 2.0*M_PI*i/5.0;
            glVertex2f(r * cos(angle), r * sin(angle));
        }
    glEnd();
    glColor3f(0.6, 0.2, 0.6);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
        for(int i = 0; i < 5; i++) {
            double angle = -M_PI/2.0 + 2.0*M_PI*i/5.0;
            glVertex2f(r * cos(angle), r * sin(angle));
        }
    glEnd();
    glPopMatrix();
}

void drawFullCircleTile_menu(int x, int y, int size) {
    glPushMatrix();
    glTranslatef(x + size/2, y + size/2, 0);
    glRotatef(rotationAngle, 0, 0, 1);
    int r = size / 2;
    glColor3f(0.3, 0.8, 1.0);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 24; i++) {
            double angle = 2.0 * M_PI * i / 24.0;
            glVertex2f(r * cos(angle), r * sin(angle));
        }
    glEnd();
    glColor3f(0.1, 0.5, 0.8);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
        for(int i = 0; i < 24; i++) {
            double angle = 2.0 * M_PI * i / 24.0;
            glVertex2f(r * cos(angle), r * sin(angle));
        }
    glEnd();
    glPopMatrix();
}

void drawHeartTile_menu(int x, int y, int size) {
    glPushMatrix();
    glTranslatef(x + size/2, y + size/2, 0);
    glRotatef(rotationAngle, 0, 0, 1);
    double scale = size / 32.0;
    glColor3f(1.0, 0.2, 0.4);
    glBegin(GL_POLYGON);
        for (double t = 0.0; t <= 6.28; t += 0.12) {
            double xt = scale * 16.0 * pow(sin(t), 3);
            double yt = scale * (13.0 * cos(t) - 5.0 * cos(2.0*t) - 2.0 * cos(3.0*t) - cos(4.0*t));
            glVertex2f(xt, yt);
        }
    glEnd();
    glColor3f(0.8, 0.0, 0.2);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
        for (double t = 0.0; t <= 6.28; t += 0.12) {
            double xt = scale * 16.0 * pow(sin(t), 3);
            double yt = scale * (13.0 * cos(t) - 5.0 * cos(2.0*t) - 2.0 * cos(3.0*t) - cos(4.0*t));
            glVertex2f(xt, yt);
        }
    glEnd();
    glPopMatrix();
}

void drawCrossTile_menu(int x, int y, int size) {
    glPushMatrix();
    glTranslatef(x + size/2, y + size/2, 0);
    glRotatef(rotationAngle, 0, 0, 1);
    int half = size/2;
    int thickness = size / 4;
    glColor3f(0.4, 0.4, 0.4);
    glBegin(GL_QUADS);
        glVertex2i(-thickness/2, -half);
        glVertex2i(thickness/2, -half);
        glVertex2i(thickness/2, half);
        glVertex2i(-thickness/2, half);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2i(-half, -thickness/2);
        glVertex2i(half, -thickness/2);
        glVertex2i(half, thickness/2);
        glVertex2i(-half, thickness/2);
    glEnd();
    glColor3f(0.7, 0.7, 0.7);
    glLineWidth(3);
    glBegin(GL_LINE_LOOP);
        glVertex2i(-thickness/2, -half);
        glVertex2i(thickness/2, -half);
        glVertex2i(thickness/2, half);
        glVertex2i(-thickness/2, half);
    glEnd();
    glBegin(GL_LINE_LOOP);
        glVertex2i(-half, -thickness/2);
        glVertex2i(half, -thickness/2);
        glVertex2i(half, thickness/2);
        glVertex2i(-half, thickness/2);
    glEnd();
    glPopMatrix();
}

void drawAllShapes() {
    int shapeY = 25;
    int shapeSize = 4;
    int spacing = 9;
    int totalWidth = 7 * spacing;
    int startX = (SCREEN_W - totalWidth - shapeSize) / 2;
    drawSquareTile_menu(startX + 0*spacing, shapeY, shapeSize);
    drawTriangleTile_menu(startX + 1*spacing, shapeY, shapeSize);
    drawStarTile_menu(startX + 2*spacing, shapeY, shapeSize);
    drawDiamondTile_menu(startX + 3*spacing, shapeY, shapeSize);
    drawCrossTile_menu(startX + 4*spacing, shapeY, shapeSize);
    drawPentagonTile_menu(startX + 5*spacing, shapeY, shapeSize);
    drawHeartTile_menu(startX + 6*spacing, shapeY, shapeSize);
    drawFullCircleTile_menu(startX + 7*spacing, shapeY, shapeSize);
}

void displayMainMenu() {
    glClearColor(0.87, 0.94, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    drawAllShapes();

    glColor3f(0.1, 0.2, 0.6);
    renderBitmapString(25, 37, GLUT_BITMAP_TIMES_ROMAN_24, "American International University, Bangladesh");

    glColor3f(0.3, 0.4, 0.7);
    glLineWidth(2);
    glBegin(GL_LINES);
        glVertex2i(5, 36);
        glVertex2i(75, 36);
    glEnd();

    glColor3f(0.2, 0.5, 0.2);
    renderBitmapString(27, 34, GLUT_BITMAP_TIMES_ROMAN_24, "Submitted To: MAHFUJUR RAHMAN");

    glColor3f(0.7, 0.1, 0.1);
    renderBitmapString(27, 31, GLUT_BITMAP_TIMES_ROMAN_24, "CATCHING MAGIC SHAPES");

    glColor3f(0.3, 0.3, 0.5);
    int labelY = 22;
    int totalWidth = 7 * 9;
    int startX = (SCREEN_W - totalWidth - 4) / 2 + 1;
    int spacing = 9;
    renderBitmapString(startX + 0*spacing, labelY, GLUT_BITMAP_HELVETICA_12, "SQUARE");
    renderBitmapString(startX + 1*spacing - 1, labelY, GLUT_BITMAP_HELVETICA_12, "TRIANGLE");
    renderBitmapString(startX + 2*spacing, labelY, GLUT_BITMAP_HELVETICA_12, "STAR");
    renderBitmapString(startX + 3*spacing - 1, labelY, GLUT_BITMAP_HELVETICA_12, "DIAMOND");
    renderBitmapString(startX + 4*spacing, labelY, GLUT_BITMAP_HELVETICA_12, "CROSS");
    renderBitmapString(startX + 5*spacing - 2, labelY, GLUT_BITMAP_HELVETICA_12, "PENTAGON");
    renderBitmapString(startX + 6*spacing, labelY, GLUT_BITMAP_HELVETICA_12, "HEART");
    renderBitmapString(startX + 7*spacing, labelY, GLUT_BITMAP_HELVETICA_12, "CIRCLE");

    glColor3f(0.5, 0.3, 0.1);
    renderBitmapString(33, 19, GLUT_BITMAP_TIMES_ROMAN_24, "PROJECT MEMBERS");

    glColor3f(0.6, 0.4, 0.2);
    glLineWidth(2);
    glBegin(GL_LINES);
        glVertex2i(30, 18);
        glVertex2i(50, 18);
    glEnd();

    glColor3f(0.7, 0.2, 0.1);
    renderBitmapString(20, 16, GLUT_BITMAP_HELVETICA_18, "ID:   23-51196-1");
    renderBitmapString(20, 14, GLUT_BITMAP_HELVETICA_18, "NAME: MD.AHOSAN KABIR RONOK");
    renderBitmapString(20, 11, GLUT_BITMAP_HELVETICA_18, "ID:   22-49742-3");
    renderBitmapString(20, 9, GLUT_BITMAP_HELVETICA_18, "NAME: MD.FARHAD MASUM SHOHAG");

    renderBitmapString(45, 16, GLUT_BITMAP_HELVETICA_18, "ID:   23-50934-1");
    renderBitmapString(45, 14, GLUT_BITMAP_HELVETICA_18, "NAME: SAZZAD UL HAQUE");
    renderBitmapString(45, 11, GLUT_BITMAP_HELVETICA_18, "ID:   23-51784-2");
    renderBitmapString(45, 9, GLUT_BITMAP_HELVETICA_18, "NAME: WAKAR ALVEE");

    glColor3f(0.1, 0.6, 0.1);
    renderBitmapString(32, 6, GLUT_BITMAP_TIMES_ROMAN_24, "GAME LEVELS:");
    glColor3f(0.2, 0.7, 0.2);
    renderBitmapString(25, 4, GLUT_BITMAP_HELVETICA_18, "EASY (Press E)  =>  MEDIUM (Press M)  =>  HARD (Press H)");

    glColor3f(0.4, 0.5, 0.7);
    glLineWidth(3);
    glBegin(GL_LINES);
        glVertex2i(1, 39); glVertex2i(4, 39);
        glVertex2i(1, 39); glVertex2i(1, 36);
        glVertex2i(76, 39); glVertex2i(79, 39);
        glVertex2i(79, 39); glVertex2i(79, 36);
    glEnd();
    glBegin(GL_LINES);
        glVertex2i(1, 1); glVertex2i(4, 1);
        glVertex2i(1, 1); glVertex2i(1, 4);
        glVertex2i(76, 1); glVertex2i(79, 1);
        glVertex2i(79, 1); glVertex2i(79, 4);
    glEnd();

    glFlush();
}

// =============== EASY MODE FUNCTIONS ===============
void drawBackground_easy() {
    glBegin(GL_QUADS);
        glColor3f(0.5f, 0.7f, 1.0f);
        glVertex2f(0.0f, 40.0f);
        glVertex2f(80.0f, 40.0f);
        glColor3f(0.8f, 0.9f, 1.0f);
        glVertex2f(80.0f, 0.0f);
        glVertex2f(0.0f, 0.0f);
    glEnd();

    glColor3f(0.2f, 0.2f, 0.3f);
    glLineWidth(1.5f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(80.0f, 0.0f);
        glVertex2f(80.0f, 40.0f);
        glVertex2f(0.0f, 40.0f);
    glEnd();

    float sx = 70.0f, sy = 35.0f, sr = 3.0f;
    glColor3f(1.0f, 0.95f, 0.0f);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 16; i++) {
            double angle = 2.0 * M_PI * i / 16.0;
            glVertex2f(sx + sr * cos(angle), sy + sr * sin(angle));
        }
    glEnd();
    glColor3f(0.9f, 0.6f, 0.0f);
    glLineWidth(1.2f);
    glBegin(GL_LINE_LOOP);
        for(int i = 0; i < 16; i++) {
            double angle = 2.0 * M_PI * i / 16.0;
            glVertex2f(sx + sr * cos(angle), sy + sr * sin(angle));
        }
    glEnd();

    glColor3f(0.2f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(20.0f, 0.0f);
        glVertex2f(20.0f, 40.0f);
        glVertex2f(0.0f, 40.0f);
    glEnd();
    glColor3f(0.05f, 0.3f, 0.05f);
    glLineWidth(1.2f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(20.0f, 0.0f);
        glVertex2f(20.0f, 40.0f);
        glVertex2f(0.0f, 40.0f);
    glEnd();

    glColor3f(0.2f, 0.8f, 0.2f);
    glBegin(GL_QUADS);
        glVertex2f(60.0f, 0.0f);
        glVertex2f(80.0f, 0.0f);
        glVertex2f(80.0f, 40.0f);
        glVertex2f(60.0f, 40.0f);
    glEnd();
    glColor3f(0.05f, 0.3f, 0.05f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(60.0f, 0.0f);
        glVertex2f(80.0f, 0.0f);
        glVertex2f(80.0f, 40.0f);
        glVertex2f(60.0f, 40.0f);
    glEnd();

    for (int i = 4; i <= 11; ++i) {
        float lx = i * COL_W;
        float rx = (i + 1) * COL_W;
        glColor3f(0.6f, 0.3f, 0.1f);
        glBegin(GL_QUADS);
            glVertex2f(lx, 0.0f);
            glVertex2f(rx, 0.0f);
            glVertex2f(rx, 40.0f);
            glVertex2f(lx, 40.0f);
        glEnd();
        glColor3f(0.3f, 0.15f, 0.05f);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(lx, 0.0f);
            glVertex2f(rx, 0.0f);
            glVertex2f(rx, 40.0f);
            glVertex2f(lx, 40.0f);
        glEnd();
    }

    glColor3f(0.35f, 0.18f, 0.08f);
    glLineWidth(2.0f);
    for (int i = 4; i <= 12; ++i) {
        float x = i * COL_W;
        glBegin(GL_LINES);
            glVertex2f(x, 0.0f);
            glVertex2f(x, 40.0f);
        glEnd();
    }
}

void drawTrees_easy() {
   // LEFT TREES - tree 1
    glColor3f(0, 1, 0);
    glBegin(GL_POLYGON);
        glVertex2f(5, 10);
        glVertex2f(2, 4);
        glVertex2f(4, 5);
        glVertex2f(2, 2);
        glVertex2f(8, 2);
        glVertex2f(6, 5);
        glVertex2f(8, 4);
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
        glVertex2f(5, 10);
        glVertex2f(2, 4);
        glVertex2f(4, 5);
        glVertex2f(2, 2);
        glVertex2f(8, 2);
        glVertex2f(6, 5);
        glVertex2f(8, 4);
    glEnd();
    glColor3f(1, 0, 0);
    glLineWidth(3);
    glBegin(GL_LINES);
        glVertex2f(5, 2);
        glVertex2f(5, 0);
    glEnd();

    // tree 2
    glColor3f(0, 1, 0);
    glBegin(GL_POLYGON);
        glVertex2f(15, 20);
        glVertex2f(12, 14);
        glVertex2f(14, 15);
        glVertex2f(12, 12);
        glVertex2f(18, 12);
        glVertex2f(16, 15);
        glVertex2f(18, 14);
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
        glVertex2f(15, 20);
        glVertex2f(12, 14);
        glVertex2f(14, 15);
        glVertex2f(12, 12);
        glVertex2f(18, 12);
        glVertex2f(16, 15);
        glVertex2f(18, 14);
    glEnd();
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
        glVertex2f(15, 12);
        glVertex2f(15, 10);
    glEnd();

    // tree 3
    glColor3f(0, 1, 0);
    glBegin(GL_POLYGON);
        glVertex2f(5, 30);
        glVertex2f(2, 24);
        glVertex2f(4, 25);
        glVertex2f(2, 22);
        glVertex2f(8, 22);
        glVertex2f(6, 25);
        glVertex2f(8, 24);
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
        glVertex2f(5, 30);
        glVertex2f(2, 24);
        glVertex2f(4, 25);
        glVertex2f(2, 22);
        glVertex2f(8, 22);
        glVertex2f(6, 25);
        glVertex2f(8, 24);
    glEnd();
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
        glVertex2f(5, 22);
        glVertex2f(5, 20);
    glEnd();

    // tree 4
    glColor3f(0, 1, 0);
    glBegin(GL_POLYGON);
        glVertex2f(10, 39);
        glVertex2f(7, 34);
        glVertex2f(9, 35);
        glVertex2f(7, 32);
        glVertex2f(13, 32);
        glVertex2f(11, 35);
        glVertex2f(13, 34);
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
        glVertex2f(10, 39);
        glVertex2f(7, 34);
        glVertex2f(9, 35);
        glVertex2f(7, 32);
        glVertex2f(13, 32);
        glVertex2f(11, 35);
        glVertex2f(13, 34);
    glEnd();
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
        glVertex2f(10, 32);
        glVertex2f(10, 30);
    glEnd();

    // RIGHT TREES
    glColor3f(0, 1, 0);
    glBegin(GL_POLYGON);
        glVertex2f(75, 10);
        glVertex2f(72, 4);
        glVertex2f(74, 5);
        glVertex2f(72, 2);
        glVertex2f(78, 2);
        glVertex2f(76, 5);
        glVertex2f(78, 4);
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
        glVertex2f(75, 10);
        glVertex2f(72, 4);
        glVertex2f(74, 5);
        glVertex2f(72, 2);
        glVertex2f(78, 2);
        glVertex2f(76, 5);
        glVertex2f(78, 4);
    glEnd();
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
        glVertex2f(75, 2);
        glVertex2f(75, 0);
    glEnd();

    glColor3f(0, 1, 0);
    glBegin(GL_POLYGON);
        glVertex2f(65, 20);
        glVertex2f(62, 14);
        glVertex2f(64, 15);
        glVertex2f(62, 12);
        glVertex2f(68, 12);
        glVertex2f(66, 15);
        glVertex2f(68, 14);
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
        glVertex2f(65, 20);
        glVertex2f(62, 14);
        glVertex2f(64, 15);
        glVertex2f(62, 12);
        glVertex2f(68, 12);
        glVertex2f(66, 15);
        glVertex2f(68, 14);
    glEnd();
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
        glVertex2f(65, 12);
        glVertex2f(65, 10);
    glEnd();

    glColor3f(0, 1, 0);
    glBegin(GL_POLYGON);
        glVertex2f(75, 30);
        glVertex2f(72, 24);
        glVertex2f(74, 25);
        glVertex2f(72, 22);
        glVertex2f(78, 22);
        glVertex2f(76, 25);
        glVertex2f(78, 24);
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
        glVertex2f(75, 30);
        glVertex2f(72, 24);
        glVertex2f(74, 25);
        glVertex2f(72, 22);
        glVertex2f(78, 22);
        glVertex2f(76, 25);
        glVertex2f(78, 24);
    glEnd();
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
        glVertex2f(75, 22);
        glVertex2f(75, 20);
    glEnd();

    glColor3f(0, 1, 0);
    glBegin(GL_POLYGON);
        glVertex2f(70, 39);
        glVertex2f(67, 34);
        glVertex2f(69, 35);
        glVertex2f(67, 32);
        glVertex2f(73, 32);
        glVertex2f(71, 35);
        glVertex2f(73, 34);
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(2);
    glBegin(GL_LINE_LOOP);
        glVertex2f(70, 39);
        glVertex2f(67, 34);
        glVertex2f(69, 35);
        glVertex2f(67, 32);
        glVertex2f(73, 32);
        glVertex2f(71, 35);
        glVertex2f(73, 34);
    glEnd();
    glColor3f(1, 0, 0);
    glBegin(GL_LINES);
        glVertex2f(70, 32);
        glVertex2f(70, 30);
    glEnd();

}

void drawSquareTile_easy(int col, float y) {
    glPushMatrix();
    glTranslatef(col * COL_W + COL_W/2.0f, y + COL_W/2.0f, 0.0f);
    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(-2.5f, -2.5f);
        glVertex2f(2.5f, -2.5f);
        glVertex2f(2.5f, 2.5f);
        glVertex2f(-2.5f, 2.5f);
    glEnd();
    glColor3f(0.3f, 0.15f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-2.5f, -2.5f);
        glVertex2f(2.5f, -2.5f);
        glVertex2f(2.5f, 2.5f);
        glVertex2f(-2.5f, 2.5f);
    glEnd();
    glPopMatrix();
}

void drawTriangleTile_easy(int col, float y) {
    glPushMatrix();
    glTranslatef(col * COL_W + COL_W/2.0f, y + COL_W/2.0f, 0.0f);
    glColor3f(1.0f, 0.0f, 0.5f);
    glBegin(GL_TRIANGLES);
        glVertex2f(-2.5f, -2.5f);
        glVertex2f(2.5f, -2.5f);
        glVertex2f(0.0f, 2.5f);
    glEnd();
    glColor3f(0.4f, 0.0f, 0.2f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-2.5f, -2.5f);
        glVertex2f(2.5f, -2.5f);
        glVertex2f(0.0f, 2.5f);
    glEnd();
    glPopMatrix();
}

void drawStarTile_easy(int col, float y) {
    glPushMatrix();
    glTranslatef(col * COL_W + COL_W/2.0f, y + COL_W/2.0f, 0.0f);
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_POLYGON);

        glVertex2f(0.45f, 0.9f);
        glVertex2f(2.25f, 0.9f);
        glVertex2f(0.7f, -0.1f);
        glVertex2f(1.15f, -2.0f);
        glVertex2f(0.0f, -0.9f);
        glVertex2f(-1.15f, -2.0f);
        glVertex2f(-0.7f, -0.1f);
        glVertex2f(-2.25f, 0.9f);
        glVertex2f(-0.45f, 0.9f);
        glVertex2f(0.0f, 2.4f);
    glEnd();
    glColor3f(0.5f, 0.4f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.0f, 2.4f);
        glVertex2f(0.45f, 0.9f);
        glVertex2f(2.25f, 0.9f);
        glVertex2f(0.7f, -0.1f);
        glVertex2f(1.15f, -2.0f);
        glVertex2f(0.0f, -0.9f);
        glVertex2f(-1.15f, -2.0f);
        glVertex2f(-0.7f, -0.1f);
        glVertex2f(-2.25f, 0.9f);
        glVertex2f(-0.45f, 0.9f);
    glEnd();
    glPopMatrix();
}

void drawDiamondTile_easy(int col, float y) {
    glPushMatrix();
    glTranslatef(col * COL_W + COL_W/2.0f, y + COL_W/2.0f, 0.0f);
    glColor3f(0.5f, 0.0f, 0.8f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, 2.5f);
        glVertex2f(2.5f, 0.0f);
        glVertex2f(0.0f, -2.5f);
        glVertex2f(-2.5f, 0.0f);
    glEnd();
    glColor3f(0.2f, 0.0f, 0.3f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.0f, 2.5f);
        glVertex2f(2.5f, 0.0f);
        glVertex2f(0.0f, -2.5f);
        glVertex2f(-2.5f, 0.0f);
    glEnd();
    glPopMatrix();
}

void drawPentagonTile_easy(int col, float y) {
    glPushMatrix();
    glTranslatef(col * COL_W + COL_W/2.0f, y + COL_W/2.0f, 0.0f);
    glColor3f(0.8f, 0.0f, 0.8f);
    glBegin(GL_POLYGON);
        glVertex2f(0.0f, 2.5f);
        glVertex2f(2.38f, 0.77f);
        glVertex2f(1.47f, -2.02f);
        glVertex2f(-1.47f, -2.02f);
        glVertex2f(-2.38f, 0.77f);
    glEnd();
    glColor3f(0.3f, 0.0f, 0.3f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.0f, 2.5f);
        glVertex2f(2.38f, 0.77f);
        glVertex2f(1.47f, -2.02f);
        glVertex2f(-1.47f, -2.02f);
        glVertex2f(-2.38f, 0.77f);
    glEnd();
    glPopMatrix();
}

void drawFullCircleTile_easy(int col, float y) {
    glPushMatrix();
    glTranslatef(col * COL_W + COL_W/2.0f, y + COL_W/2.0f, 0.0f);
    glColor3f(0.0f, 0.5f, 1.0f);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 16; i++) {
            double angle = 2.0 * M_PI * i / 16.0;
            glVertex2f(2.5f * cos(angle), 2.5f * sin(angle));
        }
    glEnd();
    glColor3f(0.0f, 0.2f, 0.4f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        for(int i = 0; i < 16; i++) {
            double angle = 2.0 * M_PI * i / 16.0;
            glVertex2f(2.5f * cos(angle), 2.5f * sin(angle));
        }
    glEnd();
    glPopMatrix();
}

void drawHeartTile_easy(int col, float y) {
    glPushMatrix();
    glTranslatef(col * COL_W + COL_W/2.0f, y + COL_W/2.0f, 0.0f);

    glColor3f(1.0f, 0.0f, 0.0f);
    glBegin(GL_POLYGON);
    for (float t = 0; t <= 2*M_PI; t += 0.1f) {
        float x = 1.6f * (16 * pow(sin(t), 3)) / 16.0f;
        float y_heart = 1.6f * (13*cos(t) - 5*cos(2*t) - 2*cos(3*t) - cos(4*t)) / 16.0f;
        glVertex2f(x, y_heart);
    }
    glEnd();
    glPopMatrix();
}


void drawWheelTile_easy(int col, float y) {
    static float angle = 0.0f;
    angle += 5.0f;
    if (angle >= 360.0f) angle -= 360.0f;
    glPushMatrix();
    glTranslatef(col * COL_W + COL_W/2.0f, y + COL_W/2.0f, 0.0f);

    glColor3f(0.22f, 0.22f, 0.22f);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 12; i++) {
            double a = 2.0 * M_PI * i / 12.0;
            glVertex2f(2.25f * cos(a), 2.25f * sin(a));
        }
    glEnd();

    glColor3f(0.85f, 0.85f, 0.85f);
    glLineWidth(2.2f);
    glPushMatrix();
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
        for(int i = 0; i < 8; i++) {
            double a = 2.0 * M_PI * i / 8.0;
            glVertex2f(0.0f, 0.0f);
            glVertex2f(2.2f * cos(a), 2.2f * sin(a));
        }
    glEnd();
    glPopMatrix();

    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 12; i++) {
            double a = 2.0 * M_PI * i / 12.0;
            glVertex2f(0.73f * cos(a), 0.73f * sin(a));
        }
    glEnd();

    glColor3f(0.05f, 0.05f, 0.05f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        for(int i = 0; i < 12; i++) {
            double a = 2.0 * M_PI * i / 12.0;
            glVertex2f(2.25f * cos(a), 2.25f * sin(a));
        }
    glEnd();
    glPopMatrix();
}

void drawCrossTile_easy(int col, float y) {
    static float angle = 0.0f;
    angle += 5.0f;
    if (angle >= 360.0f) angle -= 360.0f;
    glPushMatrix();
    glTranslatef(col * COL_W + COL_W/2.0f, y + COL_W/2.0f, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(-0.5f, -2.5f);
        glVertex2f(0.5f, -2.5f);
        glVertex2f(0.5f, 2.5f);
        glVertex2f(-0.5f, 2.5f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f(-2.5f, -0.5f);
        glVertex2f(2.5f, -0.5f);
        glVertex2f(2.5f, 0.5f);
        glVertex2f(-2.5f, 0.5f);
    glEnd();

    glColor3f(0.6f, 0.6f, 0.6f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.5f, -2.5f);
        glVertex2f(0.5f, -2.5f);
        glVertex2f(0.5f, 2.5f);
        glVertex2f(-0.5f, 2.5f);
    glEnd();
    glBegin(GL_LINE_LOOP);
        glVertex2f(-2.5f, -0.5f);
        glVertex2f(2.5f, -0.5f);
        glVertex2f(2.5f, 0.5f);
        glVertex2f(-2.5f, 0.5f);
    glEnd();
    glPopMatrix();
}

void drawPlayer_easy() {
    glPushMatrix();
    glTranslatef(playerCol_easy * COL_W, playerY_easy, 0.0f);
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(5.0f, 0.0f);
        glVertex2f(5.0f, 6.0f);
        glVertex2f(0.0f, 6.0f);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.2f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0.0f, 0.0f);
        glVertex2f(5.0f, 0.0f);
        glVertex2f(5.0f, 6.0f);
        glVertex2f(0.0f, 6.0f);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glVertex2f(2.5f, 0.0f);
        glVertex2f(2.5f, 6.0f);
        glVertex2f(0.0f, 3.0f);
        glVertex2f(5.0f, 3.0f);
    glEnd();
    glPopMatrix();
}

void drawGameOver_easy() {
    glColor3f(1.0f, 0.0f, 0.0f);
    glRasterPos2f(20.0f, 20.0f);
    const char* msg = "GAME OVER - Press R to Restart| Press B to go to Home Page";
    while (*msg) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *msg++);
}

void displayEasyMode() {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    drawBackground_easy();
    drawTrees_easy();
    if (!gameOver_easy) {
        drawSquareTile_easy(4, tileY_easy[0]);
        drawTriangleTile_easy(5, tileY_easy[1]);
        drawStarTile_easy(6, tileY_easy[2]);
        drawDiamondTile_easy(7, tileY_easy[3]);
        drawCrossTile_easy(8, tileY_easy[4]);
        drawPentagonTile_easy(9, tileY_easy[5]);
        drawHeartTile_easy(10, tileY_easy[6]);
        drawWheelTile_easy(11, tileY_easy[7]);
        drawPlayer_easy();
    } else {
        drawGameOver_easy();
    }
    glFlush();
}

// =============== MEDIUM MODE FUNCTIONS ===============
void drawBackground_medium() {
    // Same as easy mode background
    drawBackground_easy();
}

void drawTrees_medium() {
    // Same as easy mode trees
    drawTrees_easy();
}

void drawSquareTile_medium(int col, float y) {
    float x = col * COL_W;
    float w = COL_W;
    glColor3f(1.0f, 0.5f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(x + 0.0f, y + 0.0f);
        glVertex2f(x + w, y + 0.0f);
        glVertex2f(x + w, y + w);
        glVertex2f(x + 0.0f, y + w);
    glEnd();
    glColor3f(0.3f, 0.15f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x + 0.0f, y + 0.0f);
        glVertex2f(x + w, y + 0.0f);
        glVertex2f(x + w, y + w);
        glVertex2f(x + 0.0f, y + w);
    glEnd();
}

void drawTriangleTile_medium(int col, float y) {
    float x = col * COL_W;
    float w = COL_W;
    glColor3f(1.0f, 0.0f, 0.5f);
    glBegin(GL_TRIANGLES);
        glVertex2f(x + 0.0f, y + 0.0f);
        glVertex2f(x + w, y + 0.0f);
        glVertex2f(x + w/2, y + w);
    glEnd();
    glColor3f(0.4f, 0.0f, 0.2f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x + 0.0f, y + 0.0f);
        glVertex2f(x + w, y + 0.0f);
        glVertex2f(x + w/2, y + w);
    glEnd();
}

void drawStarTile_medium(int col, float y) {
    float x = col * COL_W;
    float w = COL_W;
    float cx = x + w / 2.0f;
    float cy = y + w / 2.0f;
    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_POLYGON);

        glVertex2f(cx + 0.45f, cy + 0.87f);
        glVertex2f(cx + 2.25f, cy + 0.87f);
        glVertex2f(cx + 0.7f, cy - 0.13f);
        glVertex2f(cx + 1.15f, cy - 2.0f);
        glVertex2f(cx + 0.0f, cy - 0.87f);
        glVertex2f(cx - 1.15f, cy - 2.0f);
        glVertex2f(cx - 0.7f, cy - 0.13f);
        glVertex2f(cx - 2.25f, cy + 0.87f);
        glVertex2f(cx - 0.45f, cy + 0.87f);
        glVertex2f(cx + 0.0f, cy + 2.5f);
    glEnd();
    glColor3f(0.5f, 0.4f, 0.0f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx + 0.0f, cy + 2.5f);
        glVertex2f(cx + 0.45f, cy + 0.87f);
        glVertex2f(cx + 2.25f, cy + 0.87f);
        glVertex2f(cx + 0.7f, cy - 0.13f);
        glVertex2f(cx + 1.15f, cy - 2.0f);
        glVertex2f(cx + 0.0f, cy - 0.87f);
        glVertex2f(cx - 1.15f, cy - 2.0f);
        glVertex2f(cx - 0.7f, cy - 0.13f);
        glVertex2f(cx - 2.25f, cy + 0.87f);
        glVertex2f(cx - 0.45f, cy + 0.87f);
    glEnd();
}

void drawDiamondTile_medium(int col, float y) {
    float x = col * COL_W;
    float w = COL_W;
    float cx = x + w / 2.0f;
    float cy = y + w / 2.0f;
    glColor3f(0.5f, 0.0f, 0.8f);
    glBegin(GL_QUADS);
        glVertex2f(cx, cy + w / 2.0f);
        glVertex2f(cx + w / 2.0f, cy);
        glVertex2f(cx, cy - w / 2.0f);
        glVertex2f(cx - w / 2.0f, cy);
    glEnd();
    glColor3f(0.2f, 0.0f, 0.3f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx, cy + w / 2.0f);
        glVertex2f(cx + w / 2.0f, cy);
        glVertex2f(cx, cy - w / 2.0f);
        glVertex2f(cx - w / 2.0f, cy);
    glEnd();
}

void drawPentagonTile_medium(int col, float y) {
    float x = col * COL_W;
    float w = COL_W;
    float cx = x + w / 2.0f;
    float cy = y + w / 2.0f;
    glColor3f(0.8f, 0.0f, 0.8f);
    glBegin(GL_POLYGON);
        glVertex2f(cx + 0.0f, cy + 2.5f);
        glVertex2f(cx + 1.9f, cy + 0.6f);
        glVertex2f(cx + 1.2f, cy - 1.5f);
        glVertex2f(cx - 1.2f, cy - 1.5f);
        glVertex2f(cx - 1.9f, cy + 0.6f);
    glEnd();
    glColor3f(0.3f, 0.0f, 0.3f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(cx + 0.0f, cy + 2.5f);
        glVertex2f(cx + 1.9f, cy + 0.6f);
        glVertex2f(cx + 1.2f, cy - 1.5f);
        glVertex2f(cx - 1.2f, cy - 1.5f);
        glVertex2f(cx - 1.9f, cy + 0.6f);
    glEnd();
}

void drawFullCircleTile_medium(int col, float y) {
    float x = col * COL_W;
    float w = COL_W;
    float cx = x + w / 2.0f;
    float cy = y + w / 2.0f;
    glColor3f(0.0f, 0.5f, 1.0f);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 16; i++) {
            double angle = 2.0 * M_PI * i / 16.0;
            glVertex2f(cx + 2.5f * cos(angle), cy + 2.5f * sin(angle));
        }
    glEnd();
    glColor3f(0.0f, 0.2f, 0.4f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        for(int i = 0; i < 16; i++) {
            double angle = 2.0 * M_PI * i / 16.0;
            glVertex2f(cx + 2.5f * cos(angle), cy + 2.5f * sin(angle));
        }
    glEnd();
}

void drawBombTile_medium(int col, float y) {
    float x = col * COL_W;
    float w = COL_W;
    float cx = x + w / 2.0f;
    float cy = y + w / 2.0f;

    glColor3f(0.1f, 0.1f, 0.1f);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 16; i++) {
            double angle = 2.0 * M_PI * i / 16.0;
            glVertex2f(cx + 2.0f * cos(angle), cy + 2.0f * sin(angle));
        }
    glEnd();

    glColor3f(0.8f, 0.4f, 0.0f);
    glLineWidth(3.0f);
    glBegin(GL_LINE_STRIP);
        glVertex2f(cx - 0.5f, cy + 2.0f);
        glVertex2f(cx - 0.8f, cy + 2.3f);
        glVertex2f(cx - 0.3f, cy + 2.6f);
        glVertex2f(cx - 0.7f, cy + 2.9f);
        glVertex2f(cx - 0.2f, cy + 3.2f);
        glVertex2f(cx - 0.6f, cy + 3.5f);
    glEnd();

    glColor3f(1.0f, 0.8f, 0.0f);
    glBegin(GL_POLYGON);
        glVertex2f(cx - 0.6f, cy + 3.5f);
        glVertex2f(cx - 0.4f, cy + 3.6f);
        glVertex2f(cx - 0.5f, cy + 3.8f);
        glVertex2f(cx - 0.7f, cy + 3.7f);
    glEnd();

    glColor3f(0.05f, 0.05f, 0.05f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        for(int i = 0; i < 16; i++) {
            double angle = 2.0 * M_PI * i / 16.0;
            glVertex2f(cx + 2.0f * cos(angle), cy + 2.0f * sin(angle));
        }
    glEnd();
}

void drawWheelTile_medium(int col, float y) {
    // Same as easy mode wheel
    static float angle = 0.0f;
    angle += 5.0f;
    if (angle >= 360.0f) angle -= 360.0f;

    float x = col * COL_W;
    float w = COL_W;
    float cx = x + w / 2.0f;
    float cy = y + w / 2.0f;

    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);

    glColor3f(0.22f, 0.22f, 0.22f);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 12; i++) {
            double a = 2.0 * M_PI * i / 12.0;
            glVertex2f(2.2f * cos(a), 2.2f * sin(a));
        }
    glEnd();

    glColor3f(0.85f, 0.85f, 0.85f);
    glLineWidth(2.2f);
    glPushMatrix();
    glRotatef(angle, 0.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
        for(int i = 0; i < 8; i++) {
            double a = 2.0 * M_PI * i / 8.0;
            glVertex2f(0.0f, 0.0f);
            glVertex2f(2.2f * cos(a), 2.2f * sin(a));
        }
    glEnd();
    glPopMatrix();

    glColor3f(0.5f, 0.5f, 0.5f);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 12; i++) {
            double a = 2.0 * M_PI * i / 12.0;
            glVertex2f(0.7f * cos(a), 0.7f * sin(a));
        }
    glEnd();

    glPopMatrix();
}

void drawCrossTile_medium(int col, float y) {
    static float angle = 0.0f;
    angle += 5.0f;
    if (angle >= 360.0f) angle -= 360.0f;

    float x = col * COL_W;
    float w = COL_W;
    float cx = x + w / 2.0f;
    float cy = y + w / 2.0f;

    glPushMatrix();
    glTranslatef(cx, cy, 0.0f);
    glRotatef(angle, 0.0f, 0.0f, 1.0f);

    glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_QUADS);
        glVertex2f(-0.5f, -2.5f);
        glVertex2f(0.5f, -2.5f);
        glVertex2f(0.5f, 2.5f);
        glVertex2f(-0.5f, 2.5f);
    glEnd();
    glBegin(GL_QUADS);
        glVertex2f(-2.5f, -0.5f);
        glVertex2f(2.5f, -0.5f);
        glVertex2f(2.5f, 0.5f);
        glVertex2f(-2.5f, 0.5f);
    glEnd();

    glColor3f(0.6f, 0.6f, 0.6f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-0.5f, -2.5f);
        glVertex2f(0.5f, -2.5f);
        glVertex2f(0.5f, 2.5f);
        glVertex2f(-0.5f, 2.5f);
    glEnd();
    glBegin(GL_LINE_LOOP);
        glVertex2f(-2.5f, -0.5f);
        glVertex2f(2.5f, -0.5f);
        glVertex2f(2.5f, 0.5f);
        glVertex2f(-2.5f, 0.5f);
    glEnd();
    glPopMatrix();
}

void drawBombExplosion_medium() {
    float x = playerCol_medium * COL_W;
    float w = COL_W;
    float cx = x + w / 2.0f;
    float cy = playerY_medium + 3.0f;

    float explosionSize = (explosionTimer_medium < 30) ? explosionTimer_medium * 0.5f : (60 - explosionTimer_medium) * 0.5f;

    glColor3f(1.0f, 0.3f, 0.0f);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 16; i++) {
            double angle = 2.0 * M_PI * i / 16.0;
            glVertex2f(cx + explosionSize * 3.0f * cos(angle), cy + explosionSize * 3.0f * sin(angle));
        }
    glEnd();

    glColor3f(1.0f, 1.0f, 0.0f);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 16; i++) {
            double angle = 2.0 * M_PI * i / 16.0;
            glVertex2f(cx + explosionSize * 1.5f * cos(angle), cy + explosionSize * 1.5f * sin(angle));
        }
    glEnd();

    glColor3f(1.0f, 1.0f, 1.0f);
    glBegin(GL_POLYGON);
        for(int i = 0; i < 16; i++) {
            double angle = 2.0 * M_PI * i / 16.0;
            glVertex2f(cx + explosionSize * 0.8f * cos(angle), cy + explosionSize * 0.8f * sin(angle));
        }
    glEnd();
}

void drawPlayer_medium() {
    float x = playerCol_medium * COL_W;
    float w = COL_W;
    glColor3f(0.0f, 0.0f, 1.0f);
    glBegin(GL_QUADS);
        glVertex2f(x + 0.0f, playerY_medium + 0.0f);
        glVertex2f(x + w, playerY_medium + 0.0f);
        glVertex2f(x + w, playerY_medium + 6.0f);
        glVertex2f(x + 0.0f, playerY_medium + 6.0f);
    glEnd();
    glColor3f(0.0f, 0.0f, 0.2f);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(x + 0.0f, playerY_medium + 0.0f);
        glVertex2f(x + w, playerY_medium + 0.0f);
        glVertex2f(x + w, playerY_medium + 6.0f);
        glVertex2f(x + 0.0f, playerY_medium + 6.0f);
    glEnd();
    glColor3f(1.0f, 1.0f, 1.0f);
    glLineWidth(1.0f);
    glBegin(GL_LINES);
        glVertex2f(x + w/2.0f, playerY_medium + 0.0f);
        glVertex2f(x + w/2.0f, playerY_medium + 6.0f);
        glVertex2f(x + 0.0f, playerY_medium + 3.0f);
        glVertex2f(x + w, playerY_medium + 3.0f);
    glEnd();
}

void drawGameOver_medium() {
    if (bombCaught_medium) {
        glColor3f(1.0f, 0.0f, 0.0f);
        glRasterPos2f(15.0f, 25.0f);
        const char* msg = "BOMB EXPLODED! - Press R to Restart";
        while (*msg) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *msg++);
    } else {
        glColor3f(1.0f, 0.0f, 0.0f);
        glRasterPos2f(20.0f, 20.0f);
        const char* msg = "GAME OVER - Press R to Restart| Press B to go to Home Page";
        while (*msg) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *msg++);
    }
}

void displayMediumMode() {
    glClear(GL_COLOR_BUFFER_BIT);
    drawBackground_medium();
    drawTrees_medium();

    if (!gameOver_medium) {
        drawSquareTile_medium(4, tileY_medium[0]);
        drawTriangleTile_medium(5, tileY_medium[1]);
        drawStarTile_medium(6, tileY_medium[2]);
        drawDiamondTile_medium(7, tileY_medium[3]);
        drawCrossTile_medium(8, tileY_medium[4]);
        drawPentagonTile_medium(9, tileY_medium[5]);
        drawBombTile_medium(10, tileY_medium[6]);
        drawWheelTile_medium(11, tileY_medium[7]);
        drawPlayer_medium();
    } else {
        if (showExplosion_medium && bombCaught_medium) {
            drawBombExplosion_medium();
        }
        drawGameOver_medium();
    }
    glFlush();
}

// =============== HARD MODE FUNCTIONS ===============
void drawBackground_hard() {
    glClearColor(0.5f, 0.7f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    for (int i = 0; i < NUM_COLS; ++i) {
        float lx = i * COL_W;
        float rx = (i + 1) * COL_W;
        glBegin(GL_QUADS);
            glColor3f(0.5f, 0.7f, 1.0f);
            glVertex2f(lx, 0.0f);
            glVertex2f(rx, 0.0f);
            glColor3f(0.8f, 0.9f, 1.0f);
            glVertex2f(rx, 40.0f);
            glVertex2f(lx, 40.0f);
        glEnd();

        glColor3f(0, 0, 0);
        glLineWidth(1.5f);
        glBegin(GL_LINE_LOOP);
            glVertex2f(lx, 0.0f);
            glVertex2f(rx, 0.0f);
            glVertex2f(rx, 40.0f);
            glVertex2f(lx, 40.0f);
        glEnd();
    }
}

void drawSquareTile_hard(int col, float y) {
    float x = col * COL_W + COL_W/2.0f, w = COL_W - 1.0f;
    glPushMatrix();
    glTranslatef(x, y + w/2.0f, 0);
    glColor3f(1, 0.5, 0.1);
    glBegin(GL_QUADS);
        glVertex2f(-w/2, -w/2);
        glVertex2f(w/2, -w/2);
        glVertex2f(w/2, w/2);
        glVertex2f(-w/2, w/2);
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-w/2, -w/2);
        glVertex2f(w/2, -w/2);
        glVertex2f(w/2, w/2);
        glVertex2f(-w/2, w/2);
    glEnd();
    glPopMatrix();
}

void drawTriangleTile_hard(int col, float y) {
    float x = col * COL_W + COL_W/2.0f, w = COL_W - 1.0f;
    glPushMatrix();
    glTranslatef(x, y + w/2.0f, 0);
    glColor3f(0.2, 0.8, 0.2);
    glBegin(GL_TRIANGLES);
        glVertex2f(-w/2, -w/2);
        glVertex2f(w/2, -w/2);
        glVertex2f(0, w/2);
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-w/2, -w/2);
        glVertex2f(w/2, -w/2);
        glVertex2f(0, w/2);
    glEnd();
    glPopMatrix();
}

void drawCircleTile_hard(int col, float y) {
    float x = col * COL_W + COL_W/2.0f, w = COL_W - 1.0f;
    float r = w/2;
    glPushMatrix();
    glTranslatef(x, y + w/2.0f, 0);
    glColor3f(0.2, 0.2, 1);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 16; i++) {
        float angle = i / 16.0f * 2 * 3.14159f;
        glVertex2f(r * cos(angle), r * sin(angle));
    }
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 16; i++) {
        float angle = i / 16.0f * 2 * 3.14159f;
        glVertex2f(r * cos(angle), r * sin(angle));
    }
    glEnd();
    glPopMatrix();
}

void drawDiamondTile_hard(int col, float y) {
    float x = col * COL_W + COL_W/2.0f, w = COL_W - 1.0f;
    glPushMatrix();
    glTranslatef(x, y + w/2.0f, 0);
    glColor3f(0.7, 0.1, 1);
    glBegin(GL_QUADS);
        glVertex2f(0, w/2);
        glVertex2f(w/2, 0);
        glVertex2f(0, -w/2);
        glVertex2f(-w/2, 0);
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(0, w/2);
        glVertex2f(w/2, 0);
        glVertex2f(0, -w/2);
        glVertex2f(-w/2, 0);
    glEnd();
    glPopMatrix();
}

void drawBomb_hard(int col, float y) {
    float x = col * COL_W + COL_W / 2.0f;
    float r = COL_W / 2.5f;
    glColor3f(0, 0, 0);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 16; i++) {
        float angle = i / 16.0f * 2 * 3.14159f;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
    glColor3f(1, 1, 0);
    glLineWidth(3.0f);
    glBegin(GL_LINES);
    glVertex2f(x, y + r);
    glVertex2f(x, y + r + 3);
    glEnd();

    glColor3f(0, 0, 0);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
    for (int i = 0; i < 16; i++) {
        float angle = i / 16.0f * 2 * 3.14159f;
        glVertex2f(x + r * cos(angle), y + r * sin(angle));
    }
    glEnd();
}

void drawExplosion_hard() {
    if (!bombExploding_hard) return;
    glPushMatrix();
    glTranslatef(shakeOffset_hard, 0, 0);
    glColor3f(1, 0, 0);
    glBegin(GL_POLYGON);
    for (int i = 0; i < 16; i++) {
        float angle = i / 16.0f * 2 * 3.14159f;
        glVertex2f(explodeX_hard + explodeRadius_hard * cos(angle),
                   explodeY_hard + explodeRadius_hard * sin(angle));
    }
    glEnd();
    glPopMatrix();
}

void drawPlayer_hard() {
    float x = playerCol_hard * COL_W + COL_W/2.0f, w = COL_W - 1.0f;
    glPushMatrix();
    glTranslatef(x, playerY_hard + 3, 0);
    glColor3f(0, 0, 1);
    glBegin(GL_QUADS);
        glVertex2f(-w/2, -3);
        glVertex2f(w/2, -3);
        glVertex2f(w/2, 3);
        glVertex2f(-w/2, 3);
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-w/2, -3);
        glVertex2f(w/2, -3);
        glVertex2f(w/2, 3);
        glVertex2f(-w/2, 3);
    glEnd();
    glPopMatrix();

    float x2 = playerCol2_hard * COL_W + COL_W/2.0f;
    glPushMatrix();
    glTranslatef(x2, playerY2_hard + 3, 0);
    glColor3f(1, 0, 1);
    glBegin(GL_QUADS);
        glVertex2f(-w/2, -3);
        glVertex2f(w/2, -3);
        glVertex2f(w/2, 3);
        glVertex2f(-w/2, 3);
    glEnd();
    glColor3f(0, 0, 0);
    glLineWidth(2.0f);
    glBegin(GL_LINE_LOOP);
        glVertex2f(-w/2, -3);
        glVertex2f(w/2, -3);
        glVertex2f(w/2, 3);
        glVertex2f(-w/2, 3);
    glEnd();
    glPopMatrix();
}

void drawGameOver_hard() {
    glColor3f(1, 0, 0);
    const char* msg = "GAME OVER - Press R to Restart |Press B to go to Home Page";
    glRasterPos2f(20.0f, 20.0f);
    for (const char* p = msg; *p; ++p)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
}

void drawScore_hard() {
    glColor3f(0, 0, 0);
    char buf[50];
    sprintf(buf, "Score: %d", score_hard);
    const char* p = buf;
    glRasterPos2f(2, SCREEN_H - 2);
    while (*p)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p++);
}

void drawWinMessage_hard() {
    glColor3f(0, 0.5f, 1);
    const char* msg = "YOU WON!   - Press R to restart | Press B to go to Home Page";
    glRasterPos2f(20.0f, 20.0f);
    for (const char* p = msg; *p; ++p)
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
}

void displayHardMode() {
    drawBackground_hard();
    if (!gameOver_hard) {
        for (int i = 0; i < 16; i++) {
            if (i % 4 == 0) drawSquareTile_hard(i, tileY_hard[i]);
            else if (i % 4 == 1) drawTriangleTile_hard(i, tileY_hard[i]);
            else if (i % 4 == 2) drawCircleTile_hard(i, tileY_hard[i]);
            else drawDiamondTile_hard(i, tileY_hard[i]);
        }
        for (int i = 0; i < 3; i++)
            drawBomb_hard(bombCols_hard[i], bombY_hard[i]);
        drawExplosion_hard();
        drawPlayer_hard();
        drawScore_hard();
    } else {
        if (gameWon_hard) drawWinMessage_hard();
        else drawGameOver_hard();
        drawScore_hard();
    }
    glFlush();
}

// =============== MAIN CONTROL FUNCTIONS ===============
void display() {
    switch (currentState) {
        case MAIN_MENU:
            displayMainMenu();
            break;
        case EASY_MODE:
            displayEasyMode();
            break;
        case MEDIUM_MODE:
            displayMediumMode();
            break;
        case HARD_MODE:
            displayHardMode();
            break;
    }
}

void update(int value) {
    switch (currentState) {
        case MAIN_MENU:
            rotationAngle += 2;
            if (rotationAngle >= 360) {
                rotationAngle -= 360;
            }
            glutPostRedisplay();
            glutTimerFunc(50, update, 0);
            break;

        case EASY_MODE:
            if (!gameOver_easy) {
                for (int i = 0; i < 8; i++) {
                    int col = i + 4;
                    float fall = (col == 10) ? speed_easy * 2.0f : speed_easy;
                    tileY_easy[i] -= fall;
                    if (collision_easy(tileY_easy[i], col)) {
                        tileY_easy[i] = (float)SCREEN_H + 10.0f;
                        sound_easy();
                    }
                    if (tileY_easy[i] <= 0.0f && col != 10) {
                        gameOver_easy = true;
                    }
                }
            }
            glutPostRedisplay();
            glutTimerFunc(30, update, 0);
            break;

        case MEDIUM_MODE:
            if (!gameOver_medium) {
                if (speed_medium < maxSpeed_medium) {
                    speed_medium += speedIncrement_medium;
                }
                for (int i = 0; i < 8; i++) {
                    int col = i + 4;
                    float fall = (col == 10) ? speed_medium * 2.0f : speed_medium;
                    tileY_medium[i] -= fall * 10.0f;
                    if (collision_medium(tileY_medium[i], col)) {
                        if (col == 10) {
                            bombCaught_medium = true;
                            gameOver_medium = true;
                            showExplosion_medium = true;
                            explosionTimer_medium = 0;
                        } else {
                            tileY_medium[i] = (float)SCREEN_H + 10.0f;
                            sound_medium();
                        }
                    }
                    if (tileY_medium[i] <= 0.0f && col != 10) {
                        gameOver_medium = true;
                    }
                    if (tileY_medium[i] <= 0.0f && col == 10) {
                        tileY_medium[i] = (float)SCREEN_H + 10.0f;
                    }
                }
            }
            if (showExplosion_medium) {
                explosionTimer_medium++;
                if (explosionTimer_medium > 60) {
                    showExplosion_medium = false;
                }
            }
            glutPostRedisplay();
            glutTimerFunc(30, update, 0);
            break;

        case HARD_MODE:
            if (!gameOver_hard) {
                speedupTimer_hard += 30;
                if (speedupTimer_hard >= 10000) {
                    speed_hard += 0.08f;
                    speedupTimer_hard = 0;
                }
                for (int i = 0; i < 16; i++) {
                    int col = i;
                    tileY_hard[i] -= speed_hard;
                    if (collision_hard(tileY_hard[i], col, playerCol_hard, playerY_hard) ||
                        collision_hard(tileY_hard[i], col, playerCol2_hard, playerY2_hard)) {
                        if (i % 4 == 0) score_hard += 40;
                        else if (i % 4 == 1) score_hard += 30;
                        else if (i % 4 == 2) score_hard += 100;
                        else if (i % 4 == 3) score_hard += 50;
                        tileY_hard[i] = SCREEN_H + (rand() % 20) * 10;
                        sound_hard();
                        if (score_hard >= 5000) {
                            gameWon_hard = true;
                            gameOver_hard = true;
                        }
                    }
                    if (tileY_hard[i] <= 0) tileY_hard[i] = SCREEN_H + (rand() % 20) * 10;
                }
                for (int i = 0; i < 3; i++) {
                    bombY_hard[i] -= speed_hard * 0.8f;
                    if (collision_hard(bombY_hard[i], bombCols_hard[i], playerCol_hard, playerY_hard) ||
                        collision_hard(bombY_hard[i], bombCols_hard[i], playerCol2_hard, playerY2_hard)) {
                        bombExploding_hard = true;
                        explodeX_hard = bombCols_hard[i] * COL_W + COL_W / 2.0f;
                        explodeY_hard = bombY_hard[i];
                        explodeRadius_hard = 1.0f;
                        shakeOffset_hard = 0;
                        shakeFrame_hard = 0;
                        bombCols_hard[i] = rand() % NUM_COLS;
                        bombY_hard[i] = SCREEN_H + (rand() % 50) + 50;
                    }
                    if (bombY_hard[i] <= 0) {
                        bombCols_hard[i] = rand() % NUM_COLS;
                        bombY_hard[i] = SCREEN_H + (rand() % 50) + 50;
                    }
                }
                if (bombExploding_hard) {
                    explodeRadius_hard += 0.5f;
                    shakeFrame_hard++;
                    shakeOffset_hard = (shakeFrame_hard % 2 == 0) ? -0.5f : 0.5f;
                    if (explodeRadius_hard >= 10.0f) {
                        bombExploding_hard = false;
                        shakeOffset_hard = 0;
                        gameOver_hard = true;
                    }
                }
            }
            glutPostRedisplay();
            glutTimerFunc(30, update, 0);
            break;
    }
}

void keyboard(int key, int x, int y) {
    switch (currentState) {
        case EASY_MODE:
            if (gameOver_easy) return;
            if (key == GLUT_KEY_LEFT && playerCol_easy > 4) playerCol_easy--;
            if (key == GLUT_KEY_RIGHT && playerCol_easy < 11) playerCol_easy++;
            glutPostRedisplay();
            break;
        case MEDIUM_MODE:
            if (gameOver_medium) return;
            if (key == GLUT_KEY_LEFT && playerCol_medium > 4) playerCol_medium--;
            if (key == GLUT_KEY_RIGHT && playerCol_medium < 11) playerCol_medium++;
            glutPostRedisplay();
            break;
        case HARD_MODE:
            if (gameOver_hard) return;
            if (key == GLUT_KEY_LEFT && playerCol_hard > 0) playerCol_hard--;
            if (key == GLUT_KEY_RIGHT && playerCol_hard < 7) playerCol_hard++;
            glutPostRedisplay();
            break;
    }
}

void normalKeys(unsigned char key, int x, int y) {


    switch (currentState) {
        case MAIN_MENU:
            if (key == 'e' || key == 'E') {
                currentState = EASY_MODE;
                for (int i = 0; i < 8; i++) tileY_easy[i] = 60.0f + i * 15.0f;
                playerCol_easy = NUM_COLS / 2;
                gameOver_easy = false;
                glutPostRedisplay();
            } else if (key == 'm' || key == 'M') {
                currentState = MEDIUM_MODE;
                for (int i = 0; i < 8; i++) tileY_medium[i] = 60.0f + i * 15.0f;
                playerCol_medium = NUM_COLS / 2;
                gameOver_medium = false;
                bombCaught_medium = false;
                showExplosion_medium = false;
                explosionTimer_medium = 0;
                speed_medium = 0.015f;
                glutPostRedisplay();
            } else if (key == 'h' || key == 'H') {

                currentState = HARD_MODE;
                srand(static_cast<unsigned int>(time(0)));
                for (int i = 0; i < 16; i++) tileY_hard[i] = SCREEN_H + (rand() % 20) * 10;
                for (int i = 0; i < 3; i++) {
                    bombCols_hard[i] = rand() % NUM_COLS;
                    bombY_hard[i] = SCREEN_H + (rand() % 50) + 50;
                }
                playerCol_hard = 3;
                playerCol2_hard = 11;
                gameOver_hard = false;
                gameWon_hard = false;
                bombExploding_hard = false;
                explodeRadius_hard = 0;
                shakeOffset_hard = 0;
                shakeFrame_hard = 0;
                speed_hard = 0.05f;
                score_hard = 0;
                speedupTimer_hard = 0;
                glutPostRedisplay();
            }
            break;
        case EASY_MODE:
            if (key == 'b' || key == 'B') {
                printf("Returning to MAIN_MENU from EASY_MODE\n");
                currentState = MAIN_MENU;
                glutPostRedisplay();
            } else if (gameOver_easy && (key == 'r' || key == 'R')) {
                printf("Restarting EASY_MODE\n");
                for (int i = 0; i < 8; i++) tileY_easy[i] = 60.0f + i * 15.0f;
                playerCol_easy = NUM_COLS / 2;
                gameOver_easy = false;
                glutPostRedisplay();
            }
            break;
        case MEDIUM_MODE:
            if (key == 'b' || key == 'B') {
                printf("Returning to MAIN_MENU from MEDIUM_MODE\n");
                currentState = MAIN_MENU;
                glutPostRedisplay();
            } else if (gameOver_medium && (key == 'r' || key == 'R')) {
                printf("Restarting MEDIUM_MODE\n");
                for (int i = 0; i < 8; i++) tileY_medium[i] = 60.0f + i * 15.0f;
                playerCol_medium = NUM_COLS / 2;
                gameOver_medium = false;
                bombCaught_medium = false;
                showExplosion_medium = false;
                explosionTimer_medium = 0;
                speed_medium = 0.015f;
                glutPostRedisplay();
            }
            break;
        case HARD_MODE:
            if (key == 'b' || key == 'B') {
                printf("Returning to MAIN_MENU from HARD_MODE\n");
                currentState = MAIN_MENU;
                glutPostRedisplay();
            } else if (gameOver_hard && (key == 'r' || key == 'R')) {
                printf("Restarting HARD_MODE\n");
                srand(static_cast<unsigned int>(time(0)));
                for (int i = 0; i < 16; i++) tileY_hard[i] = SCREEN_H + (rand() % 20) * 10;
                for (int i = 0; i < 3; i++) {
                    bombCols_hard[i] = rand() % NUM_COLS;
                    bombY_hard[i] = SCREEN_H + (rand() % 50) + 50;
                }
                playerCol_hard = 3;
                playerCol2_hard = 11;
                gameOver_hard = false;
                gameWon_hard = false;
                bombExploding_hard = false;
                explodeRadius_hard = 0;
                shakeOffset_hard = 0;
                shakeFrame_hard = 0;
                speed_hard = 0.05f;
                score_hard = 0;
                speedupTimer_hard = 0;
                glutPostRedisplay();
            } else if (!gameOver_hard) {
                if (key == 'a' || key == 'A')
                    if (playerCol2_hard > 8) playerCol2_hard--;
                if (key == 'd' || key == 'D')
                    if (playerCol2_hard < 15) playerCol2_hard++;
                glutPostRedisplay();
            }
            break;
    }
}

void reshape(int width, int height) {
    windowWidth = width;
    windowHeight = height;
    if (height == 0) height = 1;

    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, SCREEN_W, 0, SCREEN_H);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    glutPostRedisplay();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);  // FIXED: Use SINGLE buffer
    glutInitWindowSize(windowWidth, windowHeight);
    glutInitWindowPosition(100, 100);
    int window = glutCreateWindow("Magic Shapes Display - Complete Game System");

    glutSetWindow(window);

    // Proper OpenGL initialization
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, SCREEN_W, 0, SCREEN_H);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    // Set viewport
    glViewport(0, 0, windowWidth, windowHeight);

    // Enable basic features
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

    currentState = MAIN_MENU;

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(normalKeys);
    glutSpecialFunc(keyboard);

    glutTimerFunc(50, update, 0);

    printf("Window created. Click on the window and press E/M/H\n");

    glutMainLoop();
    return 0;
}
