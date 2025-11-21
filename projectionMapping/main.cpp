#include <gl/glut.h>

GLfloat whiteLight[]={0.2, 0.2, 0.2, 1.0};

GLfloat sourceLight[]={0.8, 0.8, 0.8, 1.0};

GLfloat lightPos[]={0.0, 0.0, 0.0, 1.0};



void display(void) {



static float fMoonRot=0.0;

static float fEarthRot=0.0;


glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

glMatrixMode(GL_MODELVIEW);

glPushMatrix();

glLightfv(GL_LIGHT0, GL_POSITION, lightPos);



//Translate the whole scene out and into view

glTranslatef(0.0, 0.0, -300.0);



//Sun

glColor3ub(255, 255,0);

glutSolidSphere(15.0, 15, 15);



//Move the light after we draw the sun!

glLightfv(GL_LIGHT0, GL_POSITION, lightPos);


//Rotate coordinate system

glRotatef(fEarthRot, 0.0, 1.0, 0.0);

//Draw the Earth

glColor3ub(0, 0, 255);

glTranslatef(105.0, 0.0, 0.0);

glutSolidSphere(15.0, 15, 15);



//Rotate from Earth based coordinates and draw Moon

glColor3ub(200, 200, 200);

glRotatef(fMoonRot, 0.0, 1.0, 0.0);

glTranslatef(30.0, 0.0, 0.0);

fMoonRot += 15.0;

if (fMoonRot > 360.0)

fMoonRot=0.0;

glutSolidSphere(6.0, 15, 15);

glPopMatrix();



fEarthRot += 15.0;

if (fEarthRot > 360.0) 

fEarthRot=0.0;



glutSwapBuffers();

}

void setup() {

glEnable(GL_DEPTH_TEST); //hidden surface removal

glFrontFace(GL_CCW); //counter clock-wise polygons face out

glEnable(GL_CULL_FACE); //Do not calculate inside of jet



glEnable(GL_LIGHTING);



glLightModelfv(GL_LIGHT_MODEL_AMBIENT, whiteLight);

glLightfv(GL_LIGHT0, GL_DIFFUSE, sourceLight);

glLightfv(GL_LIGHT0, GL_POSITION, lightPos);



glEnable(GL_LIGHT0);

glEnable(GL_COLOR_MATERIAL);

glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);

glClearColor(0.0, 0.0, 0.0, 1.0);

}

void timer(int value) {

glutPostRedisplay();

glutTimerFunc(100, timer, 1);

}

void resize(int w, int h) {

GLfloat fAspect;

if (h==0) h=1;



glViewport(0, 0, w, h);

fAspect=(GLfloat) w/(GLfloat) h;

glMatrixMode(GL_PROJECTION);

glLoadIdentity();



gluPerspective(45.0, fAspect, 1.0, 425.0);



glMatrixMode(GL_MODELVIEW);

glLoadIdentity();

} 

int main(int argc, char* argv[]) {

glutInit(&argc, argv);

glutInitDisplayMode(GLUT_DOUBLE | 

GLUT_RGB | GLUT_DEPTH);

glutCreateWindow("Earth/Moon/Sun System");

glutReshapeFunc(resize);

glutDisplayFunc(display);

glutTimerFunc(500, timer, 1);

setup();

glutMainLoop();


return 0;

}