// point_viewer.cpp
#include <GL/freeglut.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>

struct Vec3 { float x, y, z; };
float rotateX = 0.0f;   
float rotateY = 0.0f;
float zoom = 30.0f;     

std::vector<Vec3> vertices;

bool loadVerticesOnly(const char* path) {
    std::ifstream file(path);
    std::string line;
    while (std::getline(file, line)) {
        if (line.size() >= 2 && line.substr(0, 2) == "v ") {
            std::istringstream iss(line);
            char v;
            float x, y, z;
            if (iss >> v >> x >> y >> z) {
                vertices.push_back({x, y, z});
            }
        }
    }
    std::cout << "Loaded " << vertices.size() << " points.\n";
    return true;
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glPointSize(3.0f);
    glColor3f(1, 1, 1); 

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(0, 0, zoom,   // Eye
              0, 0, 0,      // Center
              0, 1, 0);     // Up

    glRotatef(rotateX, 1, 0, 0); 
    glRotatef(rotateY, 0, 1, 0); 

    glBegin(GL_POINTS);
    for (const auto& v : vertices) {
        glVertex3f(v.x, v.y, v.z);
    }
    glEnd();

    glutSwapBuffers();
}

// Handle arrow keys (special keys)
void specialKey(int key, int x, int y) {
    const float rotStep = 5.0f;
    switch (key) {
        case GLUT_KEY_UP:    rotateX -= rotStep; break;
        case GLUT_KEY_DOWN:  rotateX += rotStep; break;
        case GLUT_KEY_LEFT:  rotateY -= rotStep; break;
        case GLUT_KEY_RIGHT: rotateY += rotStep; break;
    }
    // Normalize angles (optional)
    if (rotateX > 360) rotateX -= 360;
    if (rotateX < 0) rotateX += 360;
    if (rotateY > 360) rotateY -= 360;
    if (rotateY < 0) rotateY += 360;
    glutPostRedisplay();
}

// Handle regular keys: +, -, R
void keyboard(unsigned char key, int x, int y) {
    const float zoomStep = 2.0f;
    switch (key) {
        case '+': 
        case '=': // '+' is often Shift+'='
            zoom -= zoomStep;
            if (zoom < 1.0f) zoom = 1.0f;
            break;
        case '-': 
            zoom += zoomStep;
            if (zoom > 100.0f) zoom = 100.0f;
            break;
        case 'r':
        case 'R':
            rotateX = 0.0f;
            rotateY = 0.0f;
            zoom = 30.0f;
            break;
    }
    glutPostRedisplay();
}

void init() {
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glMatrixMode(GL_PROJECTION);
    gluPerspective(45, 1, 0.1, 1000);
    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char** argv) {
    if (!loadVerticesOnly("object/bunny.obj")) {
        std::cerr << "Failed to load model.\n";
        return 1;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("model viewer");

    init();
    glutDisplayFunc(display);
    glutSpecialFunc(specialKey); 
    glutKeyboardFunc(keyboard);  

    glutMainLoop();
    return 0;
}