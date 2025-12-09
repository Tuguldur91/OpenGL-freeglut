#include <GL/glut.h>
#include <cmath>

void drawPattern() {
    glColor3f(0.0f, 0.0f, 0.0f);
    glLineWidth(1.0f);
    
    for (float r = 0.5f; r < 15.0f; r += 0.5f) {
        glBegin(GL_LINE_LOOP);
        int sides = (int)(r * 2) + 3;
        for (int i = 0; i < sides; ++i) {
            float angle = 2.0f * M_PI * i / sides + r;
            float x = r * cos(angle);
            float y = r * sin(angle);
            glVertex2f(x, y);
        }
        glEnd();
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-15, 15, -15, 15, -1, 1);
    
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    
    drawPattern();
    
    glutSwapBuffers();
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutCreateWindow("Unique Pattern");
    
    glClearColor(1.0, 1.0, 1.0, 1.0);
    
    glutDisplayFunc(display);
    glutMainLoop();
    
    return 0;
}