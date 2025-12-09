// main.cpp
#include <GL/freeglut.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <math.h>

struct Vec3
{
    float x, y, z;
    Vec3(float x = 0, float y = 0, float z = 0) : x(x), y(y), z(z) {}
};

float rotateX = 0.0f;
float rotateY = 0.0f;
float zoom = 30.0f;

std::vector<Vec3> vertices;
std::vector<unsigned int> indices;

bool loadOBJ(const char *path)
{
    std::ifstream file(path);
    if (!file.is_open())
    {
        std::cerr << "Cannot open OBJ file: " << path << std::endl;
        return false;
    }

    std::string line;
    while (std::getline(file, line))
    {
        std::istringstream iss(line);
        std::string prefix;
        iss >> prefix;

        if (prefix == "v")
        {
            float x, y, z;
            iss >> x >> y >> z;
            vertices.push_back(Vec3(x, y, z));
        }
        else if (prefix == "f")
        {
            unsigned int a, b, c;
            iss >> a >> b >> c;
            indices.push_back(a - 1);
            indices.push_back(b - 1);
            indices.push_back(c - 1);
        }
    }
    file.close();
    std::cout << "✅ Loaded " << vertices.size() << " vertices, " << indices.size() / 3 << " triangles." << std::endl;
    return true;
}
void background()
{
}
void display()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glPointSize(3.0f);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0, 0, zoom, // Eye
              0, 0, 0,    // Center
              0, 1, 0);   // Up
    glRotatef(rotateX, 1, 0, 0);
    glRotatef(rotateY, 0, 1, 0);

    // Gold material
    GLfloat mat_ambient[] = {0.24725f, 0.1995f, 0.0745f, 1.0f};
    GLfloat mat_diffuse[] = {0.75164f, 0.60648f, 0.22648f, 1.0f};
    GLfloat mat_specular[] = {0.628281f, 0.555802f, 0.366065f, 1.0f};
    GLfloat shininess = 51.2f;

    glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient);
    glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
    glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
    glMaterialf(GL_FRONT, GL_SHININESS, shininess);

    glBegin(GL_TRIANGLES);
    for (size_t i = 0; i < indices.size(); i += 3)
    {
        Vec3 &v0 = vertices[indices[i]];
        Vec3 &v1 = vertices[indices[i + 1]];
        Vec3 &v2 = vertices[indices[i + 2]];

        Vec3 edge1 = {v1.x - v0.x, v1.y - v0.y, v1.z - v0.z};
        Vec3 edge2 = {v2.x - v0.x, v2.y - v0.y, v2.z - v0.z};
        Vec3 normal = {
            edge1.y * edge2.z - edge1.z * edge2.y,
            edge1.z * edge2.x - edge1.x * edge2.z,
            edge1.x * edge2.y - edge1.y * edge2.x};
        float len = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
        if (len > 0)
        {
            normal.x /= len;
            normal.y /= len;
            normal.z /= len;
        }

        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(v0.x, v0.y, v0.z);

        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(v1.x, v1.y, v1.z);

        glNormal3f(normal.x, normal.y, normal.z);
        glVertex3f(v2.x, v2.y, v2.z);
    }
    glEnd();

    glutSwapBuffers();
}

void specialKey(int key, int x, int y)
{
    const float rotStep = 5.0f;
    switch (key)
    {
    case GLUT_KEY_UP:
        rotateX -= rotStep;
        break;
    case GLUT_KEY_DOWN:
        rotateX += rotStep;
        break;
    case GLUT_KEY_LEFT:
        rotateY -= rotStep;
        break;
    case GLUT_KEY_RIGHT:
        rotateY += rotStep;
        break;
    }
    rotateX = fmodf(rotateX, 360.0f);
    rotateY = fmodf(rotateY, 360.0f);
    glutPostRedisplay();
}

void keyboard(unsigned char key, int x, int y)
{
    const float zoomStep = 2.0f;
    switch (key)
    {
    case '+':
    case '=':
        zoom -= zoomStep;
        if (zoom < 1.0f)
            zoom = 1.0f;
        break;
    case '-':
        zoom += zoomStep;
        if (zoom > 100.0f)
            zoom = 100.0f;
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

void init()
{
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_NORMALIZE);

    glFrontFace(GL_CCW);

    GLfloat ambient[] = {0.0f, 0.0f, 0.0f, 1.0f};
    GLfloat diffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat specular[] = {1.0f, 1.0f, 1.0f, 1.0f};
    GLfloat position[] = {5.0f, 5.0f, 5.0f, 1.0f};

    glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, specular);
    glLightfv(GL_LIGHT0, GL_POSITION, position);

    GLfloat lmodel_ambient[] = {0.2f, 0.2f, 0.2f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, 1.0, 0.1, 1000.0);

    glMatrixMode(GL_MODELVIEW);
}

int main(int argc, char **argv)
{
    const char *objFile = "object/dragon.obj";

    if (!loadOBJ(objFile))
    {
        return -1;
    }

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Golden OBJ Viewer - FreeGLUT");

    init();
    glutDisplayFunc(display);
    glutSpecialFunc(specialKey);
    glutKeyboardFunc(keyboard);
    glutMainLoop();
    return 0;
}