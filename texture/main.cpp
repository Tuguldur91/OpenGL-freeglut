#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <GL/glut.h>
#include <math.h>

#define MAX_VERTICES 10000
#define MAX_TEXCOORDS 10000
#define MAX_NORMALS 10000
#define MAX_FACES 10000

typedef struct { float x, y, z; } vertex_type;
typedef struct { float u, v; } texcoord_type;
typedef struct { float x, y, z; } normal_type;
typedef struct { int v[4], vt[4], vn[4], count; } face_type;

typedef struct {
    vertex_type vertices[MAX_VERTICES];
    texcoord_type texcoords[MAX_TEXCOORDS];
    normal_type normals[MAX_NORMALS];
    face_type faces[MAX_FACES];
    int num_vertices, num_texcoords, num_normals, num_faces;
    GLuint id_texture;
} obj_type;

// Globals
int screen_width = 800, screen_height = 600;
double rotation_x = 0, rotation_y = 0, rotation_z = 0;
float zoom = -10.0f;
float center_x = 0, center_y = 0, center_z = 0;
float initial_zoom = -10.0f;
int filling = 1;
obj_type model;

// Parse face vertex: "v", "v/vt", "v//vn", "v/vt/vn"
void parse_face_vertex(const char *str, int *v, int *vt, int *vn) {
    *v = *vt = *vn = -1;
    int count = sscanf(str, "%d/%d/%d", v, vt, vn);
    if (count == 1) { *vt = *vn = -1; }
    else if (count == 2) { *vn = -1; }
    else if (count == 3 && *vt == 0) { *vt = -1; }
    if (*v > 0) (*v)--;
    if (*vt > 0) (*vt)--;
    if (*vn > 0) (*vn)--;
}

GLuint LoadBitmap(const char *filename) {
    FILE *f = fopen(filename, "rb");
    if (!f) { printf(" Cannot open %s\n", filename); return 0; }

    BITMAPFILEHEADER fh; BITMAPINFOHEADER ih;
    fread(&fh, sizeof(fh), 1, f);
    fread(&ih, sizeof(ih), 1, f);
    if (fh.bfType != 0x4D42 || ih.biBitCount != 24) {
        fclose(f); return 0;
    }

    int w = ih.biWidth, h = ih.biHeight;
    int row = ((w * 3 + 3) / 4) * 4, pad = row - w * 3;
    unsigned char *data = (unsigned char *)malloc(w * h * 4);
    if (!data) { fclose(f); return 0; }
    memset(data, 0, w * h * 4);

    for (int y = h - 1; y >= 0; y--) {
        for (int x = 0; x < w; x++) {
            RGBTRIPLE px; fread(&px, 3, 1, f);
            int i = (y * w + x) * 4;
            data[i] = px.rgbtRed;
            data[i+1] = px.rgbtGreen;
            data[i+2] = px.rgbtBlue;
            data[i+3] = 255;
        }
        if (pad) fseek(f, pad, SEEK_CUR);
    }
    fclose(f);

    GLuint id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
    free(data);
    return id;
}

int LoadOBJ(const char *filename, obj_type *obj) {
    FILE *f = fopen(filename, "r");
    if (!f) { printf("Cannot open %s\n", filename); return -1; }

    obj->num_vertices = obj->num_texcoords = obj->num_normals = obj->num_faces = 0;
    char line[512];

    while (fgets(line, sizeof(line), f)) {
        if (line[0] == '#' || line[0] == '\n' || line[0] == '\r') continue;

        if (strncmp(line, "v ", 2) == 0 && obj->num_vertices < MAX_VERTICES)
            sscanf(line + 2, "%f %f %f", &obj->vertices[obj->num_vertices].x,
                                        &obj->vertices[obj->num_vertices].y,
                                        &obj->vertices[obj->num_vertices].z), obj->num_vertices++;
        else if (strncmp(line, "vt ", 3) == 0 && obj->num_texcoords < MAX_TEXCOORDS)
            sscanf(line + 3, "%f %f", &obj->texcoords[obj->num_texcoords].u,
                                    &obj->texcoords[obj->num_texcoords].v), obj->num_texcoords++;
        else if (strncmp(line, "vn ", 3) == 0 && obj->num_normals < MAX_NORMALS)
            sscanf(line + 3, "%f %f %f", &obj->normals[obj->num_normals].x,
                                        &obj->normals[obj->num_normals].y,
                                        &obj->normals[obj->num_normals].z), obj->num_normals++;
        else if (strncmp(line, "f ", 2) == 0 && obj->num_faces < MAX_FACES){
            char *token = strtok(line + 2, " \t\r\n");

            int v_index[4], vt_index[4], vn_index[4];
            int count = 0;
            while (token && count < 4)
            {
                parse_face_vertex(token, &v_index[count], &vt_index[count], &vn_index[count]);
                token = strtok(NULL, " \t\r\n");
                count++;
            }

            if (count >= 3) {
                face_type *face = &obj->faces[obj->num_faces++];
                face->count = count;   
                for (int i = 0; i < count; i++){
                    face->v[i]  = v_index[i];
                    face->vt[i] = vt_index[i];
                    face->vn[i] = vn_index[i];
                }
            }
        }
    }
    fclose(f);
    
    if (obj->num_vertices > 0) {
        float min_x = obj->vertices[0].x, max_x = obj->vertices[0].x;
        float min_y = obj->vertices[0].y, max_y = obj->vertices[0].y;
        float min_z = obj->vertices[0].z, max_z = obj->vertices[0].z;
        for (int i = 1; i < obj->num_vertices; i++) {
            vertex_type v = obj->vertices[i];
            if (v.x < min_x) min_x = v.x; if (v.x > max_x) max_x = v.x;
            if (v.y < min_y) min_y = v.y; if (v.y > max_y) max_y = v.y;
            if (v.z < min_z) min_z = v.z; if (v.z > max_z) max_z = v.z;
        }
        center_x = (min_x + max_x) * 0.5f;
        center_y = (min_y + max_y) * 0.5f;
        center_z = (min_z + max_z) * 0.5f;

        float size = fmaxf(max_x - min_x, fmaxf(max_y - min_y, max_z - min_z));
        initial_zoom = zoom = -size * 1.8f;
        if (zoom > -1.0f) zoom = -1.0f;
        if (zoom < -50.0f) zoom = -50.0f;
    }
    printf("Loaded: %d verts, %d faces\n", obj->num_vertices, obj->num_faces);
    return 0;
}

void init(void) {
    glClearColor(0.1f, 0.1f, 0.2f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    GLfloat pos[] = {10,10,10,1}, amb[] = {0.2,0.2,0.2,1}, dif[] = {1,1,1,1};
    glLightfv(GL_LIGHT0, GL_POSITION, pos);
    glLightfv(GL_LIGHT0, GL_AMBIENT, amb);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, dif);

    if (LoadOBJ("texture/Character.obj", &model) != 0) exit(1);
    model.id_texture = LoadBitmap("texture/Texture.bmp");
    if (!model.id_texture) exit(1);
}

void display(void) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();

    gluLookAt(
        center_x, center_y, center_z + zoom,
        center_x, center_y, center_z,
        0, 1, 0
    );

    glTranslatef(center_x, center_y, center_z);
    glRotatef(rotation_x, 1, 0, 0);
    glRotatef(rotation_y, 0, 1, 0);
    glRotatef(rotation_z, 0, 0, 1);
    glTranslatef(-center_x, -center_y, -center_z);

    glBindTexture(GL_TEXTURE_2D, model.id_texture);
    glBegin(GL_QUADS);  
    for (int i = 0; i < model.num_faces; i++) {
        if (model.faces[i].count != 4) continue;  
        for (int j = 0; j < 4; j++) {
            int vi = model.faces[i].v[j];
            int vti = model.faces[i].vt[j];
            int vni = model.faces[i].vn[j];

            if (vni >= 0 && vni < model.num_normals)
                glNormal3f(model.normals[vni].x, model.normals[vni].y, model.normals[vni].z);
            if (vti >= 0 && vti < model.num_texcoords)
                glTexCoord2f(model.texcoords[vti].u, 1 - model.texcoords[vti].v);
            if (vi >= 0 && vi < model.num_vertices)
                glVertex3f(model.vertices[vi].x, model.vertices[vi].y, model.vertices[vi].z);
        }
    }
    glEnd();
    glutSwapBuffers();
}

void keyboard(unsigned char key, int x, int y) {
    const float R = 5.0f, Z = 0.5f;
    switch (key) {
        case 'w': case 'W': rotation_x -= R; break;
        case 's': case 'S': rotation_x += R; break;
        case 'a': case 'A': rotation_y -= R; break;
        case 'd': case 'D': rotation_y += R; break;
        case 'q': case 'Q': rotation_z -= R; break;
        case 'e': case 'E': rotation_z += R; break;
        case '+': case '=': zoom += Z; break;
        case '-': zoom -= Z; break;
        case '0':
            rotation_x = rotation_y = rotation_z = 0;
            zoom = initial_zoom;
            break;
        case 'r': case 'R':
            filling = !filling;
            glPolygonMode(GL_FRONT_AND_BACK, filling ? GL_FILL : GL_LINE);
            break;
        case 27: exit(0);
    }
    // Clamp zoom
    if (zoom > -1.0f) zoom = -1.0f;
    if (zoom < -50.0f) zoom = -50.0f;
    glutPostRedisplay();
}

void special(int key, int x, int y) {
    const float R = 5.0f, Z = 0.5f;
    switch (key) {
        case GLUT_KEY_UP:    rotation_x -= R; break;
        case GLUT_KEY_DOWN:  rotation_x += R; break;
        case GLUT_KEY_LEFT:  rotation_y -= R; break;
        case GLUT_KEY_RIGHT: rotation_y += R; break;
        case GLUT_KEY_PAGE_UP:   zoom += Z; break;
        case GLUT_KEY_PAGE_DOWN: zoom -= Z; break;
    }
    if (zoom > -1.0f) zoom = -1.0f;
    if (zoom < -50.0f) zoom = -50.0f;
    glutPostRedisplay();
}

void resize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)w / (float)h, 0.1f, 100.0f);
}

int main(int argc, char **argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("OBJ Viewer — Centered");
    init();
    glutDisplayFunc(display);
    glutReshapeFunc(resize);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);
    glutMainLoop();
    return 0;
}