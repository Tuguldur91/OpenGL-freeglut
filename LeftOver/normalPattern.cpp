#include <GL/freeglut.h>

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glColor3f(0.2f, 0.8f, 0.5f);

    int rows = 20;
    int cols = 20;
    float spacing = 0.2f;

    for (int i = -cols; i < cols; i++) {
        for (int j = -rows; j < rows; j++) {
            float x = i * spacing + ((j % 2) ? spacing/2 : 0);
            float y = j * spacing;

            float triangle[3][2] = {
                {x, y},
                {x + spacing/2, y + spacing},
                {x - spacing/2, y + spacing}
            };

            glBegin(GL_LINE_LOOP);
                for (int k = 0; k < 3; k++)
                    glVertex2f(triangle[k][0], triangle[k][1]);
            glEnd();
        }
    }
    
    glFlush();
}
void viewport(
    
)
void init() {
    glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-2, 2, -2, 2, -1, 1);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800, 800);
    glutCreateWindow("GL_LINE_LOOP Pattern");
    init();
    glutDisplayFunc(display);
    glutMainLoop();
    return 0;
    // viewport ashiglaad 
}
