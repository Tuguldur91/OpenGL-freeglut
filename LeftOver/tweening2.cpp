#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <algorithm>

#define M_PI 3.14

struct Vec2 {
    float x, y;
    Vec2(float _x = 0, float _y = 0) : x(_x), y(_y) {}
};

struct Player {
    Vec2 pos;
    Vec2 targetPos;
    Vec2 velocity;
    float radius;
    float r, g, b;
    
    Player() : pos(0, 0), targetPos(0, 0), velocity(0, 0), radius(1.5f) {
        r = 0.3f; g = 0.8f; b = 1.0f;
    }

    void tweeningPlayer() {
        // formula:  a + (b - a) * t
        float t= 0.15f;
        pos.x = pos.x + (targetPos.x - pos.x) * t;
        pos.y = pos.y + (targetPos.y - pos.y) * t;
    }
    
    void draw() {
        // Draw glow effect
        for (int layer = 3; layer >= 0; layer--) {
            float alpha = 0.2f - layer * 0.05f;
            glColor4f(r, g, b, alpha);
            
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(pos.x, pos.y);
            for (int i = 0; i <= 30; i++) {
                float angle = (2.0f * M_PI * i) / 30;
                float rad = radius + layer * 0.5f;
                glVertex2f(pos.x + rad * cos(angle), pos.y + rad * sin(angle));
            }
            glEnd();
        }
        
        // Draw main circle
        glColor3f(r, g, b);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(pos.x, pos.y);
        for (int i = 0; i <= 30; i++) {
            float angle = (2.0f * M_PI * i) / 30;
            glVertex2f(pos.x + radius * cos(angle), pos.y + radius * sin(angle));
        }
        glEnd();
        
        // White center
        glColor3f(1.0f, 1.0f, 1.0f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(pos.x, pos.y);
        for (int i = 0; i <= 20; i++) {
            float angle = (2.0f * M_PI * i) / 20;
            glVertex2f(pos.x + radius * 0.4f * cos(angle), pos.y + radius * 0.4f * sin(angle));
        }
        glEnd();
    }
};

struct Collectible {
    Vec2 pos;
    Vec2 currentPos;
    float radius;
    float r, g, b;
    float pulseTime;
    bool collected;
    
    Collectible(float x, float y) : pos(x, y), currentPos(x, y), radius(0.8f), pulseTime(0), collected(false) {
        r = 0.9f + (rand() % 10) * 0.01f;
        g = 0.3f + (rand() % 40) * 0.01f;
        b = 0.4f + (rand() % 30) * 0.01f;
    }
    
    void update(float dt) {
        pulseTime += dt;
        
        // Smooth float animation
        float wobble = sin(pulseTime * 3.0f) * 0.15f;
        currentPos.y = pos.y + wobble;
    }
    
    void draw() {
        float pulse = 1.0f + sin(pulseTime * 5.0f) * 0.15f;
        float drawRadius = radius * pulse;
        
        // Glow
        glColor4f(r, g, b, 0.3f);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(currentPos.x, currentPos.y);
        for (int i = 0; i <= 20; i++) {
            float angle = (2.0f * M_PI * i) / 20;
            glVertex2f(currentPos.x + drawRadius * 1.8f * cos(angle), 
                      currentPos.y + drawRadius * 1.8f * sin(angle));
        }
        glEnd();
        
        // Main circle
        glColor3f(r, g, b);
        glBegin(GL_TRIANGLE_FAN);
        glVertex2f(currentPos.x, currentPos.y);
        for (int i = 0; i <= 20; i++) {
            float angle = (2.0f * M_PI * i) / 20;
            glVertex2f(currentPos.x + drawRadius * cos(angle), 
                      currentPos.y + drawRadius * sin(angle));
        }
        glEnd();
    }
};

Player player;
std::vector<Collectible> collectibles;
int score = 0;
float gameTime = 0.0f;

void spawnCollectible() {
    float x = -25.0f + (rand() % 500) * 0.1f;
    float y = -20.0f + (rand() % 400) * 0.1f;
    collectibles.push_back(Collectible(x, y));
}

void checkCollisions() {
    for (size_t i = 0; i < collectibles.size(); i++) {
        if (collectibles[i].collected) continue;
        
        float dx = player.pos.x - collectibles[i].currentPos.x;
        float dy = player.pos.y - collectibles[i].currentPos.y;
        float dist = sqrt(dx*dx + dy*dy);
        
        if (dist < player.radius + collectibles[i].radius) {
            collectibles[i].collected = true;
            score++;
        }
    }
    
    // Remove collected items
    collectibles.erase(
        std::remove_if(collectibles.begin(), collectibles.end(),
            [](const Collectible& c) { return c.collected; }),
        collectibles.end()
    );
}

void drawScore() {
    glColor3f(1.0f, 1.0f, 1.0f);
    glRasterPos2f(-28.0f, 18.0f);
    
    char scoreText[50];
    sprintf(scoreText, "Score: %d", score);
    for (char* c = scoreText; *c != '\0'; c++) {
        glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *c);
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    // Draw collectibles
    for (auto& c : collectibles) {
        c.draw();
    }
    
    // Draw player
    player.draw();
    
    // Draw score
    drawScore();
    
    glutSwapBuffers();
}

void update(int value) {
    float dt = 0.016f;
    gameTime += dt;
    
    player.tweeningPlayer();
    
    for (auto& c : collectibles) {
        c.update(dt);
    }
    
    checkCollisions();
    
    // Spawn new collectibles
    if (collectibles.size() < 8 && rand() % 30 == 0) {
        spawnCollectible();
    }
    
    glutPostRedisplay();
    glutTimerFunc(16, update, 0);
}

void mouse(int x, int y) {
    int width = glutGet(GLUT_WINDOW_WIDTH);
    int height = glutGet(GLUT_WINDOW_HEIGHT);
    
    float aspect = (float)width / (float)height;
    float worldWidth = 30.0f * aspect;
    float worldHeight = 30.0f;
    
    player.targetPos.x = (x / (float)width) * (2.0f * worldWidth) - worldWidth;
    player.targetPos.y = worldHeight - (y / (float)height) * (2.0f * worldHeight);
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
    
    float aspect = (float)w / (float)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    
    if (aspect >= 1.0f) {
        glOrtho(-30.0 * aspect, 30.0 * aspect, -30.0, 30.0, -1.0, 1.0);
    } else {
        glOrtho(-30.0, 30.0, -30.0 / aspect, 30.0 / aspect, -1.0, 1.0);
    }
}

int main(int argc, char** argv) {
    srand(time(0));
    
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Fluid Dot Collector");
    
    glClearColor(0.05f, 0.05f, 0.15f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    for (int i = 0; i < 6; i++) {
        spawnCollectible();
    }
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    glutPassiveMotionFunc(mouse);
    glutMotionFunc(mouse);
    glutTimerFunc(0, update, 0);
    
    glutMainLoop();
    return 0;
}