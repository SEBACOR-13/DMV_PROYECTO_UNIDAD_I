#include <GL/glut.h>
#include <cmath>
#include <vector>
#include <fstream>
#include <string>
using namespace std;


enum Alg { LINE_DIRECT, LINE_INC, LINE_DDA, CIRCLE_PM, ELLIPSE_PM };


struct Figura {
    Alg alg;
    int x1, y1, x2, y2;
    float color[3];
    float grosor;
};
vector<Figura> pilaUndo;
vector<Figura> pilaRedo;
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

    ofstream out("C:/Users/ID/Desktop/Proyecto Unidad I_DMV_Cortez/PROYECTO DE UNIDAD_I_CORTEZ SEBASTIAN/PixelCAD/" + filename, ios::binary);
    if (!out) {
        printf("Error: No se creo el archivo\n");
        return;
    }

    out << "P6\n" << winW << " " << winH << "\n255\n";
    out.write((char*)pixels.data(), pixels.size());
    out.close();

    printf("Imagen exportada\n", filename.c_str());
}

void keyboard(unsigned char key, int x, int y) {
    switch(key) {
        case 'g': case 'G': showGrid = !showGrid; glutPostRedisplay(); break;
        case 'e': case 'E': showAxes = !showAxes; glutPostRedisplay(); break;
        case 'c': case 'C': figuras.clear(); pilaUndo.clear(); pilaRedo.clear(); glutPostRedisplay(); break;
        case 's': case 'S': exportToPPM("captura"); break;
        case 'z': case 'Z':
            if(!figuras.empty()){
                pilaUndo.push_back(figuras.back());
                figuras.pop_back();
                glutPostRedisplay();
            }
            break;
        case 'y': case 'Y':
            if(!pilaUndo.empty()){
                figuras.push_back(pilaUndo.back());
                pilaUndo.pop_back();
                glutPostRedisplay();
            }
            break;
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

void drawLineIncremental(int x1,int y1,int x2,int y2,float r,float g,float b,float grosor){
    int dx = x2 - x1, dy = y2 - y1;
    if(abs(dx) >= abs(dy)){
        float m = (dx != 0) ? (float)dy/dx : 0;
        float y = y1;
        for(int x = x1; x <= x2; x++){
            putPixel(x,(int)round(y),r,g,b,grosor);
            y += m;
        }
    } else {
        float mInv = (dy != 0) ? (float)dx/dy : 0;
        float x = x1;
        for(int y = y1; y <= y2; y++){
            putPixel((int)round(x),y,r,g,b,grosor);
            x += mInv;
        }
    }
}

void drawLineDDA(int x1,int y1,int x2,int y2,float r,float g,float b,float grosor){
    int dx=x2-x1, dy=y2-y1;
    int steps=max(abs(dx),abs(dy));
    float xInc=dx/(float)steps, yInc=dy/(float)steps;
    float x=x1, y=y1;
    for(int i=0;i<=steps;i++){
        putPixel((int)round(x),(int)round(y),r,g,b,grosor);
        x+=xInc; y+=yInc;
    }
}

void drawCirclePM(int xc,int yc,int r,float cr,float cg,float cb,float grosor){
    int x=0,y=r,p=1-r;
    while(x<=y){
        putPixel(xc+x,yc+y,cr,cg,cb,grosor);
        putPixel(xc-x,yc+y,cr,cg,cb,grosor);
        putPixel(xc+x,yc-y,cr,cg,cb,grosor);
        putPixel(xc-x,yc-y,cr,cg,cb,grosor);
        putPixel(xc+y,yc+x,cr,cg,cb,grosor);
        putPixel(xc-y,yc+x,cr,cg,cb,grosor);
        putPixel(xc+y,yc-x,cr,cg,cb,grosor);
        putPixel(xc-y,yc-x,cr,cg,cb,grosor);
        x++;
        if(p<0) p+=2*x+1; else { y--; p+=2*(x-y)+1; }
    }
}

void drawEllipsePM(int xc,int yc,int rx,int ry,float cr,float cg,float cb,float grosor){
    int x=0,y=ry;
    long rx2=rx*rx, ry2=ry*ry;
    long p=ry2-(rx2*ry)+rx2/4;
    long dx=2*ry2*x, dy=2*rx2*y;
    while(dx<dy){
        putPixel(xc+x,yc+y,cr,cg,cb,grosor);
        putPixel(xc-x,yc+y,cr,cg,cb,grosor);
        putPixel(xc+x,yc-y,cr,cg,cb,grosor);
        putPixel(xc-x,yc-y,cr,cg,cb,grosor);
        if(p<0){ x++; dx+=2*ry2; p+=dx+ry2; }
        else{ x++; y--; dx+=2*ry2; dy-=2*rx2; p+=dx-dy+ry2; }
    }
    p=ry2*(x+0.5)*(x+0.5)+rx2*(y-1)*(y-1)-rx2*ry2;
    while(y>=0){
        putPixel(xc+x,yc+y,cr,cg,cb,grosor);
        putPixel(xc-x,yc+y,cr,cg,cb,grosor);
        putPixel(xc+x,yc-y,cr,cg,cb,grosor);
        putPixel(xc-x,yc-y,cr,cg,cb,grosor);
        if(p>0){ y--; dy-=2*rx2; p+=rx2-dy; }
        else{ y--; x++; dx+=2*ry2; dy-=2*rx2; p+=dx-dy+rx2; }
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
        switch(f.alg){
            case LINE_DIRECT: drawLineDirect(f.x1,f.y1,f.x2,f.y2,f.color[0],f.color[1],f.color[2],f.grosor); break;
            case LINE_INC: drawLineIncremental(f.x1,f.y1,f.x2,f.y2,f.color[0],f.color[1],f.color[2],f.grosor); break;
            case LINE_DDA: drawLineDDA(f.x1,f.y1,f.x2,f.y2,f.color[0],f.color[1],f.color[2],f.grosor); break;
            case CIRCLE_PM:{
                int dx=f.x2-f.x1, dy=f.y2-f.y1;
                int r=(int)round(sqrt(dx*dx+dy*dy));
                drawCirclePM(f.x1,f.y1,r,f.color[0],f.color[1],f.color[2],f.grosor);
            } break;
            case ELLIPSE_PM:{
                int rx=abs(f.x2-f.x1), ry=abs(f.y2-f.y1);
                drawEllipsePM(f.x1,f.y1,rx,ry,f.color[0],f.color[1],f.color[2],f.grosor);
            } break;
        }
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
void motion(int x,int y){ mouseX=x; mouseY=winH-y; if(showCoords) glutPostRedisplay(); }


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
void menuGrosor(int op){ if(op==1)grosorActual=1; if(op==2)grosorActual=2; if(op==3)grosorActual=3; if(op==5)grosorActual=5; }
void menuVista(int op){ if(op==0)showGrid=!showGrid; if(op==1)showAxes=!showAxes; if(op==2)showCoords=!showCoords; glutPostRedisplay(); }
void menuHerr(int op){ if(op==0)figuras.clear(); if(op==1&&!figuras.empty())figuras.pop_back(); if(op==2)exportToPPM("captura"); glutPostRedisplay(); }
void menuAyuda(int opcion) {
    if(opcion==0){
        printf("Atajos:\n");
        printf("G = mostrar/ocultar cuadricula\n");
        printf("E = mostrar/ocultar ejes\n");
        printf("C = limpiar lienzo\n");
        printf("S = exportar imagen\n");
        printf("Z = deshacer\n");
        printf("Y = rehacer\n");
    } else if(opcion==1){
        printf("PixelCAD 2D con OpenGL & GLUT\nHecho por Sebastian Cortez Apaza\n");
    }
}

void createMenu(){
    int menuDibujo=glutCreateMenu(menuAlg);
    glutAddMenuEntry("Recta Directa",LINE_DIRECT);
    glutAddMenuEntry("Recta Incremental",LINE_INC);
    glutAddMenuEntry("Recta DDA",LINE_DDA);
    glutAddMenuEntry("Circulo PM",CIRCLE_PM);
    glutAddMenuEntry("Elipse PM",ELLIPSE_PM);

    int menuColorId=glutCreateMenu(menuColor);
    glutAddMenuEntry("Negro",0);
    glutAddMenuEntry("Rojo",1);
    glutAddMenuEntry("Verde",2);
    glutAddMenuEntry("Azul",3);
    glutAddMenuEntry("Personalizado (Amarillo)",4);

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

void init(){ glClearColor(1,1,1,1); gluOrtho2D(0,winW,0,winH); }

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
