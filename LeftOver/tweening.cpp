#include <GL/freeglut.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// Config
#define GRID_SIZE 20
#define CELL_SIZE 1.0f
#define SPEED 3.0f  // cells per second
#define TWEEN_STEPS 8

// State
typedef struct { float x, y; } Vec2;
Vec2 snake[100];
int len = 5;
Vec2 food;
Vec2 next_dir = {1, 0}, dir = {1, 0};
float tween_progress = 0.0f;
int score = 0, game_over = 0;
int win_w = 640, win_h = 640;

float ease(float t) {
    return t < 0.5f ? 2 * t * t : 1 - powf(-2 * t + 2, 2) / 2;
}

Vec2 randFood() {
    Vec2 p;
    do {
        p.x = (rand() % GRID_SIZE) - GRID_SIZE/2 + 0.5f;
        p.y = (rand() % GRID_SIZE) - GRID_SIZE/2 + 0.5f;
    } while (0); // no overlap check for simplicity
    return p;
}

void reset() {
    len = 5;
    dir = (Vec2){1, 0};
    next_dir = dir;
    tween_progress = 0;
    score = 0;
    game_over = 0;
    for (int i = 0; i < len; i++) {
        snake[i].x = -i - 1;
        snake[i].y = 0;
    }
    food = randFood();
}

void update(int val) {
    if (game_over) { glutTimerFunc(100, update, 0); return; }

    tween_progress += SPEED / GRID_SIZE * 0.016f; // dt = 16ms

    if (tween_progress >= 1.0f) {
        // Commit new direction
        dir = next_dir;
        tween_progress = 0;

        // Move head
        Vec2 new_head = {
            snake[0].x + dir.x,
            snake[0].y + dir.y
        };

        // Wall wrap
        if (new_head.x < -GRID_SIZE/2) new_head.x += GRID_SIZE;
        if (new_head.x >= GRID_SIZE/2) new_head.x -= GRID_SIZE;
        if (new_head.y < -GRID_SIZE/2) new_head.y += GRID_SIZE;
        if (new_head.y >= GRID_SIZE/2) new_head.y -= GRID_SIZE;

        // Check self-collision (skip last segment — it's moving away)
        for (int i = 0; i < len - 1; i++) {
            if (fabs(new_head.x - snake[i].x) < 0.1f &&
                fabs(new_head.y - snake[i].y) < 0.1f) {
                game_over = 1;
                return;
            }
        }

        // Shift body
        for (int i = len - 1; i > 0; i--) {
            snake[i] = snake[i - 1];
        }
        snake[0] = new_head;

        // Eat food
        if (fabs(new_head.x - food.x) < 0.6f && fabs(new_head.y - food.y) < 0.6f) {
            if (len < 99) len++;
            score += 10;
            food = randFood();
        }
    }

    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void drawRect(float x, float y, float w, float h, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
        glVertex2f(x - w/2, y - h/2);
        glVertex2f(x + w/2, y - h/2);
        glVertex2f(x + w/2, y + h/2);
        glVertex2f(x - w/2, y + h/2);
    glEnd();
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glLoadIdentity();

    // Draw grid (optional)
    glColor3f(0.15, 0.15, 0.2);
    glBegin(GL_LINES);
    for (int i = -GRID_SIZE/2; i <= GRID_SIZE/2; i++) {
        glVertex2f(i, -GRID_SIZE/2); glVertex2f(i, GRID_SIZE/2);
        glVertex2f(-GRID_SIZE/2, i); glVertex2f(GRID_SIZE/2, i);
    }
    glEnd();

    // Draw food
    drawRect(food.x, food.y, 0.8f, 0.8f, 1, 0.3, 0.3);

    // Draw snake with tweening
    for (int i = 0; i < len; i++) {
        Vec2 pos = snake[i];
        Vec2 next_pos = (i == 0) ? 
            (Vec2){snake[0].x + dir.x * tween_progress, snake[0].y + dir.y * tween_progress} :
            snake[i - 1];

        float alpha = ease(tween_progress);
        float x = pos.x + (next_pos.x - pos.x) * alpha;
        float y = pos.y + (next_pos.y - pos.y) * alpha;

        float hue = fmodf(score * 0.01f + i * 0.3f, 1.0f);
        drawRect(x, y, 0.9f, 0.9f, 
                 hue < 0.33f ? 1 : (hue < 0.66f ? (1 - (hue - 0.33f)*3) : 0),
                 hue < 0.33f ? (hue * 3) : (hue < 0.66f ? 1 : (1 - (hue - 0.66f)*3)),
                 hue < 0.66f ? 0 : ((hue - 0.66f) * 3));
    }

    // UI
    char buf[64];
    snprintf(buf, sizeof(buf), "Score: %d", score);
    glColor3f(1, 1, 0);
    glRasterPos2f(-GRID_SIZE/2 + 0.5f, GRID_SIZE/2 - 0.8f);
    for (int i = 0; buf[i]; i++) glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, buf[i]);

    if (game_over) {
        glColor4f(0, 0, 0, 0.7f);
        glBegin(GL_QUADS);
            glVertex2f(-6, -2); glVertex2f(6, -2);
            glVertex2f(6, 2); glVertex2f(-6, 2);
        glEnd();
        glColor3f(1, 0.3, 0.3);
        glRasterPos2f(-2.5f, 0.5f);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*)"GAME OVER");
        glRasterPos2f(-3.2f, -0.5f);
        glutBitmapString(GLUT_BITMAP_HELVETICA_18, (unsigned char*)"Press R to restart");
    }

    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    if (game_over && key == 'r') reset();
    if (key == 27) exit(0);
}

void special(int key, int x, int y) {
    if (game_over) return;
    Vec2 dirs[4] = {{0,-1}, {0,1}, {-1,0}, {1,0}}; // up, down, left, right
    int idx = -1;
    switch (key) {
        case GLUT_KEY_UP: idx = 0; break;
        case GLUT_KEY_DOWN: idx = 1; break;
        case GLUT_KEY_LEFT: idx = 2; break;
        case GLUT_KEY_RIGHT: idx = 3; break;
    }
    if (idx >= 0) {
        Vec2 d = dirs[idx];
        // Prevent 180° turn
        if (fabs(d.x + dir.x) > 0.5f || fabs(d.y + dir.y) > 0.5f) {
            next_dir = d;
        }
    }
}

void reshape(int w, int h) {
    win_w = w; win_h = h;
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    float s = GRID_SIZE * CELL_SIZE * 0.6f;
    glOrtho(-s, s, -s, s, -1, 1);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    srand(time(0));
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(640, 640);
    glutCreateWindow("Snake with Tweening!");

    glClearColor(0.05f, 0.08f, 0.12f, 1);
    reset();

    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutTimerFunc(0, update, 0);


    glutMainLoop();
    return 0;
}