#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>

#define PI 3.14159265

struct Shape {
    int type;       // 0=triangle,1=square,2=circle
    int targetType; // type to morph into
    float x, y;
    float r, g, b;
    float size;
    float speed;
    int seg;

    bool animating = false;
    float animTime = 0.0f;
};

std::vector<Shape> shapes;
bool gameOver = false;
int spawnCounter = 0;

void spawnShape() {
    Shape s;
    s.type = rand() % 3;
    s.x = -0.9f + static_cast<float>(rand()) / RAND_MAX * 1.8f;
    s.y = 1.1f;
    s.r = 0.2f + static_cast<float>(rand()) / RAND_MAX * 0.8f;
    s.g = 0.2f + static_cast<float>(rand()) / RAND_MAX * 0.8f;
    s.b = 0.2f + static_cast<float>(rand()) / RAND_MAX * 0.8f;
    s.size = 0.05f + static_cast<float>(rand()) / RAND_MAX * 0.1f;
    s.speed = 0.005f + static_cast<float>(rand()) / RAND_MAX * 0.02f;
    s.seg = 20; // fixed circle vertices for smooth morph
    shapes.push_back(s);
}

// Convert a shape to a list of vertices
void getVertices(int type, float x, float y, float size, int seg, std::vector<float>& vx, std::vector<float>& vy) {
    vx.clear();
    vy.clear();
    if(type==0){ // triangle
        std::vector<std::pair<float,float>> pts = {
            {0, size*1.5f}, {-size, -size}, {size, -size}
        };
        int n = pts.size();
        for(int i=0;i<seg;i++){
            int idx = i*n/seg; // repeat vertices to match seg
            vx.push_back(x + pts[idx].first);
            vy.push_back(y + pts[idx].second);
        }
    }
    else if(type==1){ // square
        std::vector<std::pair<float,float>> pts = {
            {-size,-size},{size,-size},{size,size},{-size,size}
        };
        int n = pts.size();
        for(int i=0;i<seg;i++){
            int idx = i*n/seg;
            vx.push_back(x + pts[idx].first);
            vy.push_back(y + pts[idx].second);
        }
    }
    else{ // circle
        for(int i=0;i<seg;i++){
            float angle = 2*PI*i/seg;
            vx.push_back(x + size*cos(angle));
            vy.push_back(y + size*sin(angle));
        }
    }
}

void drawTweenShape(Shape& s){
    std::vector<float> vx0, vy0, vx1, vy1;
    getVertices(s.type, s.x, s.y, s.size, s.seg, vx0, vy0);
    getVertices(s.targetType, s.x, s.y, s.size, s.seg, vx1, vy1);

    float t = s.animTime;
    glColor3f(s.r, s.g, s.b);
    glBegin(GL_POLYGON);
    for(int i=0;i<s.seg;i++){
        float vx = vx0[i]*(1-t)+vx1[i]*t;
        float vy = vy0[i]*(1-t)+vy1[i]*t;
        glVertex2f(vx, vy);
    }
    glEnd();
}

// Draw static shape
void drawShape(const Shape& s){
    std::vector<float> vx, vy;
    getVertices(s.type, s.x, s.y, s.size, s.seg, vx, vy);
    glColor3f(s.r, s.g, s.b);
    glBegin(GL_POLYGON);
    for(int i=0;i<s.seg;i++) glVertex2f(vx[i], vy[i]);
    glEnd();
}

void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    if(gameOver){
        glColor3f(1,0,0);
        glRasterPos2f(-0.2f,0);
        const char* msg = "GAME OVER";
        for(const char* c=msg;*c;c++)
            glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18,*c);
    } else {
        for(auto &s: shapes){
            if(s.animating) drawTweenShape(s);
            else drawShape(s);
        }
    }
    glFlush();
}

float screenToGLX(int x){ return 2.0f*x/glutGet(GLUT_WINDOW_WIDTH)-1.0f;}
float screenToGLY(int y){ return 1.0f - 2.0f*y/glutGet(GLUT_WINDOW_HEIGHT);}

void mouseClick(int button,int state,int x,int y){
    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN && !gameOver){
        float gx = screenToGLX(x);
        float gy = screenToGLY(y);
        for(auto &s: shapes){
            if(s.animating) continue;
            bool hit=false;
            if(s.type==0){ // triangle
                if(gx>=s.x-s.size && gx<=s.x+s.size && gy<=s.y+1.5f*s.size && gy>=s.y-s.size) hit=true;
            }
            else if(s.type==1){ // square
                if(gx>=s.x-s.size && gx<=s.x+s.size && gy>=s.y-s.size && gy<=s.y+s.size) hit=true;
            }
            else{ // circle
                float dx=gx-s.x, dy=gy-s.y;
                if(dx*dx+dy*dy<=s.size*s.size) hit=true;
            }
            if(hit){
                s.animating=true;
                s.targetType=(s.type+1)%3;
                s.animTime=0.0f;
            }
        }
    }
}

void update(int value){
    if(gameOver) return;
    for(auto it=shapes.begin(); it!=shapes.end();){
        it->y -= it->speed;
        if(it->y<-1.0f) gameOver=true;
        if(it->animating){
            it->animTime+=0.05f;
            if(it->animTime>=1.0f){
                it = shapes.erase(it);
                continue;
            }
        }
        ++it;
    }

    spawnCounter++;
    if(spawnCounter>=30){
        spawnShape();
        spawnCounter=0;
    }

    glutPostRedisplay();
    glutTimerFunc(30, update, 0);
}

int main(int argc, char** argv){
    srand(static_cast<unsigned int>(time(0)));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(600, 600);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Morph Shapes Game");

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(-1, 1, -1, 1);

    glutDisplayFunc(display);
    glutMouseFunc(mouseClick);
    glutTimerFunc(30, update, 0);

    glutMainLoop();
    return 0;
}
