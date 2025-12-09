#include <GL/freeglut.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cmath>
#include <cstdio>
#include <algorithm>

const int GRID_SIZE = 20;
const int CELL_SIZE = 30;
const int WINDOW_SIZE = GRID_SIZE * CELL_SIZE;
const float MOVE_INTERVAL = 300.0f; 

enum Direction { UP, DOWN, LEFT, RIGHT };

struct Point {
    int x, y;
    Point(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Point& other) const {
        return x == other.x && y == other.y;
    }
};

std::vector<Point> snake;
Point food;
Direction dir = RIGHT;
Direction nextDir = RIGHT;
bool gameOver = false;
int score = 0;
float tweenProgress = 0.0f;
int lastUpdateTime = 0;

void initGame() {
    snake.clear();
    snake.push_back(Point(GRID_SIZE / 2, GRID_SIZE / 2));
    snake.push_back(Point(GRID_SIZE / 2 - 1, GRID_SIZE / 2));
    snake.push_back(Point(GRID_SIZE / 2 - 2, GRID_SIZE / 2));
    
    dir = RIGHT;
    nextDir = RIGHT;
    gameOver = false;
    score = 0;
    tweenProgress = 0.0f;
    
    // Place food
    srand(time(0));
    do {
        food.x = rand() % GRID_SIZE;
        food.y = rand() % GRID_SIZE;
    } while (std::find(snake.begin(), snake.end(), food) != snake.end());
    
    lastUpdateTime = glutGet(GLUT_ELAPSED_TIME);
}

void drawCell(float x, float y, float r, float g, float b) {
    glColor3f(r, g, b);
    glBegin(GL_QUADS);
    glVertex2f(x * CELL_SIZE + 2, y * CELL_SIZE + 2);
    glVertex2f((x + 1) * CELL_SIZE - 2, y * CELL_SIZE + 2);
    glVertex2f((x + 1) * CELL_SIZE - 2, (y + 1) * CELL_SIZE - 2);
    glVertex2f(x * CELL_SIZE + 2, (y + 1) * CELL_SIZE - 2);
    glEnd();
}

float easeInOutQuad(float t) {
    return t < 0.5f ? 2 * t * t : 1 - pow(-2 * t + 2, 2) / 2;
}

void drawSnake() {
    float t = easeInOutQuad(tweenProgress);
    
    for (size_t i = 0; i < snake.size(); i++) {
        float x = snake[i].x;
        float y = snake[i].y;
        
        // Tween the head
        if (i == 0 && tweenProgress < 1.0f) {
            Point prev = (snake.size() > 1) ? snake[1] : snake[0];
            float dx = (snake[0].x - prev.x) * t;
            float dy = (snake[0].y - prev.y) * t;
            x = prev.x + dx;
            y = prev.y + dy;
        }
        // Tween body segments
        else if (i > 0 && i < snake.size() && tweenProgress < 1.0f) {
            Point next = snake[i - 1];
            float dx = (next.x - snake[i].x) * t;
            float dy = (next.y - snake[i].y) * t;
            x = snake[i].x + dx;
            y = snake[i].y + dy;
        }
        
        // Color gradient from head to tail
        float brightness = 1.0f - (i * 0.3f / snake.size());
        drawCell(x, y, 0.2f * brightness, 0.8f * brightness, 0.2f * brightness);
    }
}

void drawFood() {
    // Pulsing effect
    int time = glutGet(GLUT_ELAPSED_TIME);
    float pulse = 0.8f + 0.2f * sin(time * 0.005f);
    glColor3f(1.0f * pulse, 0.2f, 0.2f);
    glBegin(GL_QUADS);
    glVertex2f(food.x * CELL_SIZE + 4, food.y * CELL_SIZE + 4);
    glVertex2f((food.x + 1) * CELL_SIZE - 4, food.y * CELL_SIZE + 4);
    glVertex2f((food.x + 1) * CELL_SIZE - 4, (food.y + 1) * CELL_SIZE - 4);
    glVertex2f(food.x * CELL_SIZE + 4, (food.y + 1) * CELL_SIZE - 4);
    glEnd();
}

void drawGrid() {
    glColor3f(0.15f, 0.15f, 0.15f);
    glBegin(GL_LINES);
    for (int i = 0; i <= GRID_SIZE; i++) {
        glVertex2f(i * CELL_SIZE, 0);
        glVertex2f(i * CELL_SIZE, WINDOW_SIZE);
        glVertex2f(0, i * CELL_SIZE);
        glVertex2f(WINDOW_SIZE, i * CELL_SIZE);
    }
    glEnd();
}

void drawText(const char* text, float x, float y) {
    glRasterPos2f(x, y);
    while (*text) {
        glutBitmapCharacter(GLUT_BITMAP_9_BY_15, *text);
        text++;
    }
}

void updateGame() {
    if (gameOver) return;
    
    dir = nextDir;
    
    Point newHead = snake[0];
    switch (dir) {
        case UP: newHead.y--; break;
        case DOWN: newHead.y++; break;
        case LEFT: newHead.x--; break;
        case RIGHT: newHead.x++; break;
    }
    
    // Check wall collision
    if (newHead.x < 0 || newHead.x >= GRID_SIZE || 
        newHead.y < 0 || newHead.y >= GRID_SIZE) {
        gameOver = true;
        return;
    }
    
    // Check self collision
    for (size_t i = 0; i < snake.size(); i++) {
        if (snake[i] == newHead) {
            gameOver = true;
            return;
        }
    }
    
    snake.insert(snake.begin(), newHead);
    
    // Check food collision
    if (newHead == food) {
        score += 10;
        do {
            food.x = rand() % GRID_SIZE;
            food.y = rand() % GRID_SIZE;
        } while (std::find(snake.begin(), snake.end(), food) != snake.end());
    } else {
        snake.pop_back();
    }
    
    tweenProgress = 0.0f;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    drawGrid();
    drawFood();
    drawSnake();
    
    glColor3f(1.0f, 1.0f, 1.0f);
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    drawText(scoreText, 10, WINDOW_SIZE - 20);
    
    if (gameOver) {
        glColor3f(1.0f, 0.0f, 0.0f);
        drawText("GAME OVER! Press R to restart", WINDOW_SIZE / 2 - 120, WINDOW_SIZE / 2);
    }
    
    glutSwapBuffers();
}

void timer(int value) {
    int currentTime = glutGet(GLUT_ELAPSED_TIME);
    float deltaTime = currentTime - lastUpdateTime;
    
    if (!gameOver) {
        tweenProgress += deltaTime / MOVE_INTERVAL;
        
        if (tweenProgress >= 1.0f) {
            updateGame();
            lastUpdateTime = currentTime;
        }
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, timer, 0); 
}

void keyboard(unsigned char key, int x, int y) {
    if (key == 'r' || key == 'R') {
        initGame();
    }
    if (key == 27) { // ESC
        exit(0);
    }
}

void specialKeys(int key, int x, int y) {
    if (gameOver) return;
    
    switch (key) {
        case GLUT_KEY_UP:
            if (dir != DOWN) nextDir = UP;
            break;
        case GLUT_KEY_DOWN:
            if (dir != UP) nextDir = DOWN;
            break;
        case GLUT_KEY_LEFT:
            if (dir != RIGHT) nextDir = LEFT;
            break;
        case GLUT_KEY_RIGHT:
            if (dir != LEFT) nextDir = RIGHT;
            break;
    }
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(WINDOW_SIZE, WINDOW_SIZE);
    glutCreateWindow("Snake Game with Tweening");
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WINDOW_SIZE, WINDOW_SIZE, 0);
    glMatrixMode(GL_MODELVIEW);
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    
    initGame();
    
    glutDisplayFunc(display);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(specialKeys);
    glutTimerFunc(16, timer, 0);
    
    glutMainLoop();
    return 0;
}