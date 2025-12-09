#include "GL/freeglut.h"
#include <ctime>
#include <cmath>

// Camera rotation
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
// Wheel rotation angle animation
static GLfloat wheelAngle = 0.0f;
// Car pos movement
static GLfloat carPosX = 0.0f;
static GLfloat carPosZ = 0.0f;
static GLfloat carPosY = -1.0f;
static GLfloat carHeading = 0.0f;
static GLfloat carSpeed = 0.0f;
static const GLfloat maxSpeed = 0.1f;
static const GLfloat acceleration = 0.002f;
static bool keyW = false;
static bool keyA = false;
static bool keyS = false;
static bool keyD = false;

void ChangeSize(int w, int h)
{
    GLfloat fAspect;
    if (h == 0)
        h = 1;
    glViewport(0, 0, w, h);
    fAspect = (GLfloat)w / (GLfloat)h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(35.0f, fAspect, 1.0, 100.0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void SetupRC()
{
    GLfloat whiteLight[] = {0.05f, 0.05f, 0.05f, 1.0f};
    GLfloat sourceLight[] = {0.25f, 0.25f, 0.25f, 1.0f};
    GLfloat lightPos[] = {-10.f, 5.0f, 5.0f, 1.0f};
    glEnable(GL_DEPTH_TEST);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);
    glLightfv(GL_LIGHT0, GL_AMBIENT, sourceLight);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHT0);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

// Arrow keys for camera look-around
void SpecialKeys(int key, int x, int y)
{
    if (key == GLUT_KEY_UP)
        xRot -= 5.0f;
    if (key == GLUT_KEY_DOWN)
        xRot += 5.0f;
    if (key == GLUT_KEY_LEFT)
        yRot -= 5.0f;
    if (key == GLUT_KEY_RIGHT)
        yRot += 5.0f;

    // Clamp vertical rotation to prevent flipping
    if (xRot < -80.0f) xRot = -80.0f;
    if (xRot > 80.0f) xRot = 80.0f;

    glutPostRedisplay();
}

void KeyboardFunc(unsigned char key, int x, int y)
{
    if (key == 'w' || key == 'W')
        keyW = true;
    if (key == 'a' || key == 'A')
        keyA = true;
    if (key == 's' || key == 'S')
        keyS = true;
    if (key == 'd' || key == 'D')
        keyD = true;
    if (key == 'r' || key == 'R') {
        // Reset camera rotation
        xRot = 0.0f;
        yRot = 0.0f;
    }
    glutPostRedisplay();
}

void KeyboardUpFunc(unsigned char key, int x, int y)
{
    if (key == 'w' || key == 'W')
        keyW = false;
    if (key == 'a' || key == 'A')
        keyA = false;
    if (key == 's' || key == 'S')
        keyS = false;
    if (key == 'd' || key == 'D')
        keyD = false;
    glutPostRedisplay();
}

void TimerFunction(int value)
{
    if (keyW)
        carSpeed = std::min(carSpeed + acceleration, maxSpeed);
    else if (keyS)
        carSpeed = std::max(carSpeed - acceleration, -maxSpeed);
    else
        carSpeed *= 0.98f;

    wheelAngle += carSpeed * 50.0f;
    if (wheelAngle >= 360.0f)
        wheelAngle -= 360.0f;
    else if (wheelAngle < 0.0f)
        wheelAngle += 360.0f;

    if (keyA && fabs(carSpeed) > 0.01f)
        carHeading += 2.0f;
    if (keyD && fabs(carSpeed) > 0.01f)
        carHeading -= 2.0f;

    float radHeading = carHeading * 3.14159f / 180.0f;
    float newPosX = carPosX + carSpeed * cos(radHeading);
    float newPosZ = carPosZ - carSpeed * sin(radHeading);

    float roadX = 5.0f * sin(newPosZ * 0.1f);

    if (fabs(newPosX - roadX) < 2.0f || carSpeed == 0.0f) {
        carPosX = newPosX;
        carPosZ = newPosZ;
    } else {
        carSpeed = 0.0f;
    }

    glutPostRedisplay();
    glutTimerFunc(16, TimerFunction, 1);
}

void DrawRoad()
{
    glDisable(GL_LIGHTING);
    glPushMatrix();
    glColor3f(0.3f, 0.3f, 0.3f);
    glBegin(GL_QUAD_STRIP);
    for (float z = -50.0f; z <= 50.0f; z += 0.5f)
    {
        float x = 5.0f * sin(z * 0.1f);
        glVertex3f(x + 2.0f, -1.25f, z);
        glVertex3f(x - 2.0f, -1.25f, z);
    }
    glEnd();
    glEnable(GL_LIGHTING);
    glPopMatrix();
}

void RenderScene(void)
{
    GLUquadricObj *pObj;
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

    GLfloat lightPos[] = {carPosX - 10.0f, 5.0f, carPosZ + 5.0f, 1.0f};
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

    // Fixed camera setup
        // Chase camera: follows behind the car
        float radHeading = carHeading * 3.14159f / 180.0f;
        float camDist = 8.0f;
        float camHeight = 3.0f;

        // Camera position behind the car
        float camX = carPosX - camDist * cos(radHeading);
        float camZ = carPosZ + camDist * sin(radHeading);

        gluLookAt(camX, carPosY + camHeight, camZ,  // Eye position (behind car)
                  carPosX, carPosY + 0.5f, carPosZ,  // Look at car
                  0.0f, 1.0f, 0.0f);                 // Up vector
        // Apply arrow key rotations for orbit control
        glTranslatef(carPosX, carPosY + 0.5f, carPosZ);
        glRotatef(yRot, 0.0f, 1.0f, 0.0f);
        glRotatef(xRot, 1.0f, 0.0f, 0.0f);
        glTranslatef(-carPosX, -(carPosY + 0.5f), -carPosZ);

    DrawRoad();

    // Draw the car
    glPushMatrix();
    glTranslatef(carPosX, carPosY, carPosZ);
    glRotatef(carHeading, 0.0f, 1.0f, 0.0f);
    pObj = gluNewQuadric();
    gluQuadricNormals(pObj, GLU_SMOOTH);

    // Car Body lower
    glPushMatrix();
    glColor3f(0.9f, 0.1f, 0.1f);
    glTranslatef(0.0f, 0.2f, 0.0f);
    glScalef(1.8f, 0.3f, 0.9f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // Car Body upper
    glPushMatrix();
    glColor3f(0.9f, 0.1f, 0.1f);
    glTranslatef(0.5f, 0.45f, 0.0f);
    glScalef(0.8f, 0.2f, 0.9f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // Car Top
    glPushMatrix();
    glColor3f(0.8f, 0.05f, 0.05f);
    glTranslatef(-0.3f, 0.5f, 0.0f);
    glScalef(0.8f, 0.4f, 0.85f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // Windshield
    glPushMatrix();
    glColor3f(0.2f, 0.3f, 0.5f);
    glTranslatef(0.15f, 0.55f, 0.0f);
    glRotatef(-20.0f, 0.0f, 0.0f, 1.0f);
    glScalef(0.35f, 0.35f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // Rear Windshield
    glPushMatrix();
    glColor3f(0.2f, 0.3f, 0.5f);
    glTranslatef(-0.6f, 0.55f, 0.0f);
    glRotatef(15.0f, 0.0f, 0.0f, 1.0f);
    glScalef(0.3f, 0.3f, 0.8f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // Front Bumper
    glPushMatrix();
    glColor3f(0.1f, 0.1f, 0.1f);
    glTranslatef(1.0f, 0.12f, 0.0f);
    glScalef(0.2f, 0.15f, 0.95f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // Rear Bumper
    glPushMatrix();
    glColor3f(0.1f, 0.1f, 0.1f);
    glTranslatef(-1.0f, 0.12f, 0.0f);
    glScalef(0.2f, 0.15f, 0.95f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // Spoiler
    glPushMatrix();
    glColor3f(0.1f, 0.1f, 0.1f);
    glTranslatef(-0.95f, 0.75f, 0.0f);
    glScalef(0.15f, 0.05f, 1.0f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // Spoiler Supports
    glPushMatrix();
    glColor3f(0.1f, 0.1f, 0.1f);
    glTranslatef(-0.95f, 0.55f, 0.4f);
    glScalef(0.08f, 0.4f, 0.08f);
    glutSolidCube(1.0f);
    glPopMatrix();
    glPushMatrix();
    glColor3f(0.1f, 0.1f, 0.1f);
    glTranslatef(-0.95f, 0.55f, -0.4f);
    glScalef(0.08f, 0.4f, 0.08f);
    glutSolidCube(1.0f);
    glPopMatrix();
    // Wheels
    float wheelPositions[4][3] = {
        {0.7f, 0.0f, 0.6f},
        {0.7f, 0.0f, -0.75f},
        {-0.7f, 0.0f, 0.6f},
        {-0.7f, 0.0f, -0.75f}
    };

    for (int i = 0; i < 4; i++)
    {
        glPushMatrix();
        glTranslatef(wheelPositions[i][0], wheelPositions[i][1], wheelPositions[i][2]);
        glRotatef(wheelAngle, 0.0f, 0.0f, 1.0f);
        glDisable(GL_CULL_FACE);
        glColor3f(0.1f, 0.1f, 0.1f);
        gluCylinder(pObj, 0.25f, 0.25f, 0.15f, 20, 10);
        glColor3f(0.6f, 0.6f, 0.6f);
        gluDisk(pObj, 0.0f, 0.25f, 20, 10);
        glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(0.0f, 0.15f, 0.0f);
        glScalef(0.05f, 0.15f, 0.01f);
        glutSolidCube(1.0f);
        glPopMatrix();
        glTranslatef(0.0f, 0.0f, 0.15f);
        glColor3f(0.6f, 0.6f, 0.6f);
        gluDisk(pObj, 0.0f, 0.25f, 20, 10);
        glPushMatrix();
        glColor3f(1.0f, 1.0f, 1.0f);
        glTranslatef(0.0f, 0.15f, 0.0f);
        glScalef(0.05f, 0.15f, 0.01f);
        glutSolidCube(1.0f);
        glPopMatrix();
        glEnable(GL_CULL_FACE);
        glPopMatrix();
    }
    // Side Accessories
    glColor3f(0.3f, 0.3f, 0.3f);
    glPushMatrix();
    glTranslatef(-1.1f, 0.2f, 0.3f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(pObj, 0.06f, 0.06f, 0.3f, 10, 10);
    glPopMatrix();
    glPushMatrix();
    glTranslatef(-1.1f, 0.2f, -0.3f);
    glRotatef(90.0f, 0.0f, 1.0f, 0.0f);
    gluCylinder(pObj, 0.06f, 0.06f, 0.3f, 10, 10);
    glPopMatrix();
    gluDeleteQuadric(pObj);
    glPopMatrix();
    glPopMatrix();
    glutSwapBuffers();
}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("3D dudu mashin - WASD:Move, Arrows:Camera, R:Reset Camera");
    glutReshapeFunc(ChangeSize);
    glutSpecialFunc(SpecialKeys);
    glutKeyboardFunc(KeyboardFunc);
    glutKeyboardUpFunc(KeyboardUpFunc);
    glutDisplayFunc(RenderScene);
    glutTimerFunc(0, TimerFunction, 1);
    SetupRC();
    glutMainLoop();
    return 0;
}