#include <GL/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

int figuraSeleccionada = 1;
int x1_, y1_, x2_, y2_;
bool primerClick = true;

void drawPixel(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

void lineaDirecta(int x1, int y1, int x2, int y2) {
    if (x1 == x2) {
        for (int y = min(y1,y2); y <= max(y1,y2); y++)
            drawPixel(x1, y);
        return;
    }

    float m = float(y2 - y1) / float(x2 - x1);
    float b = y1 - m * x1;

    if (fabs(m) <= 1) {
        for (int x = min(x1,x2); x <= max(x1,x2); x++) {
            int y = round(m * x + b);
            drawPixel(x, y);
        }
    } else {
        for (int y = min(y1,y2); y <= max(y1,y2); y++) {
            int x = round((y - b) / m);
            drawPixel(x, y);
        }
    }
}

void lineaDDA(int x1, int y1, int x2, int y2) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = max(abs(dx), abs(dy));

    float xInc = dx / (float)steps;
    float yInc = dy / (float)steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++) {
        drawPixel(round(x), round(y));
        x += xInc;
        y += yInc;
    }
}


void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (primerClick) {
            x1_ = x;
            y1_ = 600 - y;
            primerClick = false;
        } else {
            x2_ = x;
            y2_ = 600 - y;
            primerClick = true;

            glColor3f(0,0,0);
            glPointSize(1);

            if (figuraSeleccionada == 1)
                lineaDirecta(x1_, y1_, x2_, y2_);
            else if (figuraSeleccionada == 2)
                lineaDDA(x1_, y1_, x2_, y2_);

            glFlush();
        }
    }
}

void menuFiguras(int option) {
    figuraSeleccionada = option;
}

void init() {
    glClearColor(1,1,1,1);
    gluOrtho2D(0, 800, 0, 600);
}

void initMenu() {
    glutCreateMenu(menuFiguras);
    glutAddMenuEntry("Recta Directa", 1);
    glutAddMenuEntry("Recta DDA", 2);
    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800,600);
    glutInitWindowPosition(100,100);
    glutCreateWindow("PixelCAD");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    initMenu();
    glutMainLoop();
    return 0;
}
