#include <GL/glut.h>
#include <cmath>
#include <iostream>
using namespace std;

int figuraSeleccionada = 1;
int x1_, y1_, x2_, y2_;
bool primerClick = true;
float color[3] = {0,0,0};
int grosor = 1;

void drawPixel(int x, int y) {
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
}

void lineaDirecta(int x1, int y1, int x2, int y2) {
    if (x1 == x2) {
        for (int y = min(y1,y2); y <= max(y1,y2); y++) drawPixel(x1, y);
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
    int dx = x2 - x1, dy = y2 - y1;
    int steps = max(abs(dx), abs(dy));
    float xInc = dx / (float)steps, yInc = dy / (float)steps;
    float x = x1, y = y1;
    for (int i = 0; i <= steps; i++) {
        drawPixel(round(x), round(y));
        x += xInc; y += yInc;
    }
}

void circuloPuntoMedio(int xc, int yc, int r) {
    int x = 0, y = r, p = 1 - r;
    while (x <= y) {
        drawPixel(xc + x, yc + y); drawPixel(xc - x, yc + y);
        drawPixel(xc + x, yc - y); drawPixel(xc - x, yc - y);
        drawPixel(xc + y, yc + x); drawPixel(xc - y, yc + x);
        drawPixel(xc + y, yc - x); drawPixel(xc - y, yc - x);
        if (p < 0) p += 2*x + 3;
        else { p += 2*(x - y) + 5; y--; }
        x++;
    }
}

void elipsePuntoMedio(int xc, int yc, int rx, int ry) {
    float dx, dy, d1, d2, x=0, y=ry;
    d1 = (ry*ry) - (rx*rx*ry) + (0.25*rx*rx);
    dx = 2*ry*ry*x; dy = 2*rx*rx*y;

    while (dx < dy) {
        drawPixel(xc+x, yc+y); drawPixel(xc-x, yc+y);
        drawPixel(xc+x, yc-y); drawPixel(xc-x, yc-y);
        if (d1 < 0) { x++; dx += 2*ry*ry; d1 += dx + (ry*ry); }
        else { x++; y--; dx += 2*ry*ry; dy -= 2*rx*rx; d1 += dx - dy + (ry*ry); }
    }

    d2 = (ry*ry)*((x+0.5)*(x+0.5)) + (rx*rx)*((y-1)*(y-1)) - (rx*rx*ry*ry);
    while (y >= 0) {
        drawPixel(xc+x, yc+y); drawPixel(xc-x, yc+y);
        drawPixel(xc+x, yc-y); drawPixel(xc-x, yc-y);
        if (d2 > 0) { y--; dy -= 2*rx*rx; d2 += (rx*rx) - dy; }
        else { y--; x++; dx += 2*ry*ry; dy -= 2*rx*rx; d2 += dx - dy + (rx*rx); }
    }
}

void display() {
    glClear(GL_COLOR_BUFFER_BIT);
    glFlush();
}

void mouse(int button, int state, int x, int y) {
    if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
        if (primerClick) {
            x1_ = x; y1_ = 600 - y; primerClick = false;
        } else {
            x2_ = x; y2_ = 600 - y; primerClick = true;
            glColor3f(color[0], color[1], color[2]);
            glPointSize(grosor);
            switch (figuraSeleccionada) {
                case 1: lineaDirecta(x1_, y1_, x2_, y2_); break;
                case 2: lineaDDA(x1_, y1_, x2_, y2_); break;
                case 3: {
                    int r = sqrt(pow(x2_-x1_,2) + pow(y2_-y1_,2));
                    circuloPuntoMedio(x1_, y1_, r); break;
                }
                case 4: {
                    int rx = abs(x2_-x1_), ry = abs(y2_-y1_);
                    elipsePuntoMedio(x1_, y1_, rx, ry); break;
                }
            }
            glFlush();
        }
    }
}

void menuFiguras(int option) { figuraSeleccionada = option; }
void menuColor(int option) {
    switch(option) {
        case 1: color[0]=0; color[1]=0; color[2]=0; break;
        case 2: color[0]=1; color[1]=0; color[2]=0; break;
        case 3: color[0]=0; color[1]=1; color[2]=0; break;
        case 4: color[0]=0; color[1]=0; color[2]=1; break;
    }
}
void menuGrosor(int option) { grosor = option; }
void menuPrincipal(int option) {
    if (option == 1) { glClear(GL_COLOR_BUFFER_BIT); glFlush(); }
}

void init() { glClearColor(1,1,1,1); gluOrtho2D(0,800,0,600); }

void initMenu() {
    int submenuFig = glutCreateMenu(menuFiguras);
    glutAddMenuEntry("Recta Directa", 1);
    glutAddMenuEntry("Recta DDA", 2);
    glutAddMenuEntry("Circulo Punto Medio", 3);
    glutAddMenuEntry("Elipse Punto Medio", 4);

    int submenuCol = glutCreateMenu(menuColor);
    glutAddMenuEntry("Negro", 1);
    glutAddMenuEntry("Rojo", 2);
    glutAddMenuEntry("Verde", 3);
    glutAddMenuEntry("Azul", 4);

    int submenuGro = glutCreateMenu(menuGrosor);
    glutAddMenuEntry("1 px", 1);
    glutAddMenuEntry("2 px", 2);
    glutAddMenuEntry("3 px", 3);
    glutAddMenuEntry("5 px", 5);

    glutCreateMenu(menuPrincipal);
    glutAddSubMenu("Dibujo", submenuFig);
    glutAddSubMenu("Color", submenuCol);
    glutAddSubMenu("Grosor", submenuGro);
    glutAddMenuEntry("Limpiar", 1);

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
