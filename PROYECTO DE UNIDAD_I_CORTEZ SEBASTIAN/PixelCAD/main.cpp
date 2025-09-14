#include <GL/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

// Variables globales
int x1_, y1_, x2_, y2_;
bool primerClick = true;

// Dibujo de un pixel
void drawPixel(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

// Recta - Método Directo
void lineaDirecta(int x1, int y1, int x2, int y2) {
    if (x1 == x2) { // vertical
        for (int y = min(y1,y2); y <= max(y1,y2); y++)
            drawPixel(x1, y);
        return;
    }

    float m = float(y2 - y1) / float(x2 - x1);
    float b = y1 - m * x1;

    if (fabs(m) <= 1) { // pendiente baja
        for (int x = min(x1,x2); x <= max(x1,x2); x++) {
            int y = round(m * x + b);
            drawPixel(x, y);
        }
    } else { // pendiente alta
        for (int y = min(y1,y2); y <= max(y1,y2); y++) {
            int x = round((y - b) / m);
            drawPixel(x, y);
        }
    }
}

// Display
void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

// Mouse: dos clics para extremos de recta
void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (primerClick) {
            x1_ = x;
            y1_ = 600 - y; // invertir eje Y
            primerClick = false;
        } else {
            x2_ = x;
            y2_ = 600 - y;
            primerClick = true;

            glColor3f(0,0,0);   // negro
            glPointSize(1);     // grosor fijo
            lineaDirecta(x1_, y1_, x2_, y2_);
            glFlush();
        }
    }
}

// Inicialización
void init() {
    glClearColor(1,1,1,1); // fondo blanco
    gluOrtho2D(0, 800, 0, 600);
}

int main(int argc, char** argv) {
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_SINGLE | GLUT_RGB);
    glutInitWindowSize(800,600);
    glutInitWindowPosition(100,100);
    glutCreateWindow("Avance 1 - Recta Directa");
    init();
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutMainLoop();
    return 0;
}
