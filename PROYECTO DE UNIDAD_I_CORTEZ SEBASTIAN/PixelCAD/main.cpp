#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
using namespace std;


enum Alg { LINE_DIRECT };

struct Figura {
    Alg alg;
    int x1, y1, x2, y2;
    float color[3];
    float grosor;
};

vector<Figura> figuras;
int winW = 800, winH = 600;
Alg algoritmoActual = LINE_DIRECT;
float colorActual[3] = {0, 0, 0};
float grosorActual = 1.0f;
int clickCount = 0;
int tempX1, tempY1;
bool showGrid = true, showAxes = true, showCoords = false;
int gridSpacing = 25;
int mouseX = 0, mouseY = 0;
int capturaCount = 1;


void exportToPPM(const string& filenameBase) {
    string filename = filenameBase + to_string(capturaCount++) + ".ppm";
    vector<unsigned char> pixels(3 * winW * winH);
    glReadPixels(0, 0, winW, winH, GL_RGB, GL_UNSIGNED_BYTE, pixels.data());

    ofstream out(filename, ios::binary);
    if (!out) {
        printf("Error: no se pudo crear el archivo\n");
        return;
    }

    out << "P6\n" << winW << " " << winH << "\n255\n";
    out.write((char*)pixels.data(), pixels.size());
    out.close();

    printf("Imagen exportada: %s\n", filename.c_str());
}


void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'g': case 'G': showGrid = !showGrid; glutPostRedisplay(); break;
        case 'e': case 'E': showAxes = !showAxes; glutPostRedisplay(); break;
        case 'c': case 'C': figuras.clear(); glutPostRedisplay(); break;
        case 's': case 'S': exportToPPM("captura"); break;
    }
}


void drawText(int x, int y, const string& text){
    glColor3f(0,0,0);
    glRasterPos2i(x,y);
    for(char c: text) glutBitmapCharacter(GLUT_BITMAP_8_BY_13,c);
}
void putPixel(int x,int y,float r,float g,float b,float grosor){
    glColor3f(r,g,b);
    glPointSize(grosor);
    glBegin(GL_POINTS);
    glVertex2i(x,y);
    glEnd();
}


void drawLineDirect(int x1,int y1,int x2,int y2,float r,float g,float b,float grosor){
    if(x1==x2){
        for(int y=min(y1,y2); y<=max(y1,y2); y++) putPixel(x1,y,r,g,b,grosor);
        return;
    }
    float m=(float)(y2-y1)/(x2-x1);
    if(fabs(m)<=1){
        float b0 = y1 - m*x1;
        for(int x=min(x1,x2); x<=max(x1,x2); x++) putPixel(x,(int)round(m*x+b0),r,g,b,grosor);
    } else {
        for(int y=min(y1,y2); y<=max(y1,y2); y++) putPixel((int)round((y-y1)/m+x1),y,r,g,b,grosor);
    }
}


void drawAxes(){
    glColor3f(0.7,0.7,0.7);
    glBegin(GL_LINES);
    glVertex2i(0,winH/2); glVertex2i(winW,winH/2);
    glVertex2i(winW/2,0); glVertex2i(winW/2,winH);
    glEnd();
}
void drawGrid(){
    glColor3f(0.85,0.85,0.85);
    glBegin(GL_LINES);
    for(int x=0;x<=winW;x+=gridSpacing){ glVertex2i(x,0); glVertex2i(x,winH); }
    for(int y=0;y<=winH;y+=gridSpacing){ glVertex2i(0,y); glVertex2i(winW,y); }
    glEnd();
}


void display(){
    glClear(GL_COLOR_BUFFER_BIT);
    if(showGrid) drawGrid();
    if(showAxes) drawAxes();
    for(auto &f: figuras){
        if(f.alg == LINE_DIRECT)
            drawLineDirect(f.x1,f.y1,f.x2,f.y2,f.color[0],f.color[1],f.color[2],f.grosor);
    }
    if(showCoords){
        string coords = "X=" + to_string(mouseX) + " Y=" + to_string(mouseY);
        drawText(10,winH-20,coords);
    }
    glFlush();
}


void mouse(int button,int state,int x,int y){
    if(button==GLUT_LEFT_BUTTON && state==GLUT_DOWN){
        if(clickCount==0){ tempX1=x; tempY1=winH-y; clickCount=1; }
        else{
            Figura f;
            f.alg=algoritmoActual;
            f.x1=tempX1; f.y1=tempY1;
            f.x2=x; f.y2=winH-y;
            f.color[0]=colorActual[0]; f.color[1]=colorActual[1]; f.color[2]=colorActual[2];
            f.grosor=grosorActual;
            figuras.push_back(f);
            clickCount=0;
            glutPostRedisplay();
        }
    }
}
void motion(int x,int y){
    mouseX=x; mouseY=winH-y;
    if(showCoords) glutPostRedisplay();
}


void menuAlg(int op){ algoritmoActual=(Alg)op; }
void menuColor(int op){
    switch(op){
        case 0: colorActual[0]=0; colorActual[1]=0; colorActual[2]=0; break;
        case 1: colorActual[0]=1; colorActual[1]=0; colorActual[2]=0; break;
        case 2: colorActual[0]=0; colorActual[1]=1; colorActual[2]=0; break;
        case 3: colorActual[0]=0; colorActual[1]=0; colorActual[2]=1; break;
        case 4: colorActual[0]=1; colorActual[1]=1; colorActual[2]=0; break;
    }
}
void menuGrosor(int op){
    if(op==1) grosorActual=1;
    if(op==2) grosorActual=2;
    if(op==3) grosorActual=3;
    if(op==5) grosorActual=5;
}
void menuVista(int op){
    if(op==0) showGrid=!showGrid;
    if(op==1) showAxes=!showAxes;
    if(op==2) showCoords=!showCoords;
    glutPostRedisplay();
}
void menuHerr(int op){
    if(op==0) figuras.clear();
    if(op==1 && !figuras.empty()) figuras.pop_back();
    if(op==2) exportToPPM("captura");
    glutPostRedisplay();
}
void menuAyuda(int opcion) {
    switch(opcion) {
        case 0:
            printf("Atajos:\n");
            printf("G = mostrar/ocultar cuadricula\n");
            printf("E = mostrar/ocultar ejes\n");
            printf("C = limpiar todas las figuras\n");
            printf("S = Exportar imagen\n");
            break;
        case 1:
            printf("Editor 2D con OpenGL & GLUT\n");
            printf("Hecho por [Tu Nombre]\n");
            break;
    }
}

void createMenu(){
    int menuDibujo=glutCreateMenu(menuAlg);
    glutAddMenuEntry("Recta Directa",LINE_DIRECT);

    int menuColorId=glutCreateMenu(menuColor);
    glutAddMenuEntry("Negro",0);
    glutAddMenuEntry("Rojo",1);
    glutAddMenuEntry("Verde",2);
    glutAddMenuEntry("Azul",3);
    glutAddMenuEntry("Amarillo",4);

    int menuGrosorId=glutCreateMenu(menuGrosor);
    glutAddMenuEntry("1 px",1);
    glutAddMenuEntry("2 px",2);
    glutAddMenuEntry("3 px",3);
    glutAddMenuEntry("5 px",5);

    int menuVistaId=glutCreateMenu(menuVista);
    glutAddMenuEntry("Cuadricula on/off",0);
    glutAddMenuEntry("Ejes on/off",1);
    glutAddMenuEntry("Coordenadas puntero on/off",2);

    int menuHerrId=glutCreateMenu(menuHerr);
    glutAddMenuEntry("Limpiar",0);
    glutAddMenuEntry("Borrar ultima figura",1);
    glutAddMenuEntry("Exportar PPM",2);

    int menuAyudaId=glutCreateMenu(menuAyuda);
    glutAddMenuEntry("Atajos",0);
    glutAddMenuEntry("Acerca de",1);

    int menu=glutCreateMenu([](int){});
    glutAddSubMenu("Dibujo",menuDibujo);
    glutAddSubMenu("Color",menuColorId);
    glutAddSubMenu("Grosor",menuGrosorId);
    glutAddSubMenu("Vista",menuVistaId);
    glutAddSubMenu("Herramientas",menuHerrId);
    glutAddSubMenu("Ayuda",menuAyudaId);

    glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void init(){
    glClearColor(1,1,1,1);
    gluOrtho2D(0,winW,0,winH);
}

int main(int argc,char**argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_SINGLE|GLUT_RGB);
    glutInitWindowSize(winW,winH);
    glutCreateWindow("PixelCAD");
    init();
    createMenu();
    glutKeyboardFunc(keyboard);
    glutDisplayFunc(display);
    glutMouseFunc(mouse);
    glutPassiveMotionFunc(motion);
    glutMainLoop();
    return 0;
}
