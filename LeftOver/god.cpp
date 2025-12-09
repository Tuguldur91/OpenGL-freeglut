#include <GL/glut.h>
#include <cmath>
#include <vector>

struct Point {
    float x, y;
};

void drawSpiralPattern() {
    float centerX = 0.0f;
    float centerY = 0.0f;
    
    // Draw multiple spiral arms
    int numArms = 200;
    int pointsPerArm = 200;
    
    for (int arm = 0; arm < numArms; arm++) {
        float armOffset = (2.0f * M_PI * arm) / numArms;
        
        std::vector<Point> points;
        
        for (int i = 0; i < pointsPerArm; i++) {
            float t = i * 0.15f;
            float angle = t + armOffset;
            float radius = t * 0.8f;
            
            Point p;
            p.x = centerX + radius * cos(angle);
            p.y = centerY + radius * sin(angle);
            points.push_back(p);
        }
        
        // Assign color based on arm
        switch(arm % 5) {
            case 0: glColor3f(0.9f, 0.2f, 0.3f); break;  // Red
            case 1: glColor3f(0.2f, 0.7f, 0.9f); break;  // Blue
            case 2: glColor3f(0.9f, 0.7f, 0.2f); break;  // Yellow
            case 3: glColor3f(0.3f, 0.9f, 0.4f); break;  // Green
            case 4: glColor3f(0.8f, 0.3f, 0.9f); break;  // Purple
        }
        
        // Draw thick spiral line
        glLineWidth(4.0f);
        glBegin(GL_LINE_STRIP);
        for (const auto& p : points) {
            glVertex2f(p.x, p.y);
        }
        glEnd();
        
        // Draw circles along the spiral
        for (int i = 0; i < pointsPerArm; i += 8) {
            float size = 0.3f + (i * 0.008f);
            
            glBegin(GL_TRIANGLE_FAN);
            glVertex2f(points[i].x, points[i].y);
            for (int j = 0; j <= 20; j++) {
                float a = (2.0f * M_PI * j) / 20;
                glVertex2f(points[i].x + size * cos(a), 
                          points[i].y + size * sin(a));
            }
            glEnd();
            
            // White outline
            glColor3f(1.0f, 1.0f, 1.0f);
            glLineWidth(1.5f);
            glBegin(GL_LINE_LOOP);
            for (int j = 0; j < 20; j++) {
                float a = (2.0f * M_PI * j) / 20;
                glVertex2f(points[i].x + size * cos(a), 
                          points[i].y + size * sin(a));
            }
            glEnd();
            
            // Reset color
            switch(arm % 5) {
                case 0: glColor3f(0.9f, 0.2f, 0.3f); break;
                case 1: glColor3f(0.2f, 0.7f, 0.9f); break;
                case 2: glColor3f(0.9f, 0.7f, 0.2f); break;
                case 3: glColor3f(0.3f, 0.9f, 0.4f); break;
                case 4: glColor3f(0.8f, 0.3f, 0.9f); break;
            }
        }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-15.0, 15.0, -15.0, 15.0, -1.0, 1.0);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    drawSpiralPattern();
    
    glutSwapBuffers();
}

void reshape(int w, int h) {
    glViewport(0, 0, w, h);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutCreateWindow("Static Spiral Pattern");
    
    glClearColor(0.05f, 0.05f, 0.15f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_LINE_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    
    glutDisplayFunc(display);
    glutReshapeFunc(reshape);
    
    glutMainLoop();
    return 0;
}