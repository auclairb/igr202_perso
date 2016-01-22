// ----------------------------------------------
// Informatique Graphique 3D & R�alit� Virtuelle.
// Travaux Pratiques
// Algorithmes de Rendu
// Copyright (C) 2015 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------

#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <GL/glut.h>
#include "Vec3.h"
#include "Camera.h"
#include "Mesh.h"
#include "Ray.h"
#define PI 3.14

using namespace std;

static const unsigned int DEFAULT_SCREENWIDTH = 1024;
static const unsigned int DEFAULT_SCREENHEIGHT = 768;
static const string DEFAULT_MESH_FILE ("models/man.off");

static string appTitle ("Informatique Graphique & Realite Virtuelle - Travaux Pratiques - Algorithmes de Rendu");
static GLint window;
static unsigned int FPS = 0;
static bool fullScreen = false;

static Camera camera;
static Mesh mesh;
static int * affichage;

bool intersectionRayTriangle(Vec3f o, Vec3f w,Vec3f p0,Vec3f p1,Vec3f p2,float * intersection){
        
	Vec3f e0 = p1-p0;
	Vec3f e1 = p2-p0;
	Vec3f n = cross(e0,e1);
	n.normalize();
	Vec3f q = cross(w,e1);
	float a = dot(e0,q);
	if ( dot(n,w)>= 0.0f or abs(a)<0.000001f ){
	  return false;		  
	}
	Vec3f s = (o - p0)/a;
	std::cout<<"taille de s:"<<s.length()<<endl;
	Vec3f r = cross(s,e0);
	std::cout<<"taille de r:"<<r.length()<<endl;
	float b0 = dot(s,q);
	std::cout<<"taille de b0:"<<b0<<endl;
	float b1 = dot(r,w);
	std::cout<<"taille de b1:"<<b1<<endl;
	float b2 = 1.0f - b0 - b1;
	std::cout<<"taille de b2:"<<b2<<endl;
	if (b0<0.0f or b1<0.0f or b2<0.0f){
	  return false;
	}
	float t = dot(e1,r);
	if (t>=0){
		intersection[0]= b0;
		intersection[1]= b1;
		intersection[2]= b2;
		intersection[3]= t;
		return true;
	}
	return 	false;
}

void intersectAll(){
	affichage = new int[mesh.T.size ()];
	for (unsigned int i = 0; i < mesh.T.size (); i++) {
	   for (unsigned int j = 0; j < 3; j++) {
		   const Vertex & v = mesh.V[mesh.T[i].v[j]];
		   unsigned int i0 = 0;
		   bool test = true;
		   float * intersection = new float[4];
			   while (test == true and i0 < mesh.T.size ()){

			  	 Vertex & sommet1 = mesh.V[mesh.T[i0].v[0]];
			  	 Vertex & sommet2 = mesh.V[mesh.T[i0].v[1]];
			   	 Vertex & sommet3 = mesh.V[mesh.T[i0].v[2]];
				 Vec3f source(-10.0f,10.0f,-10.0f);
			  	 Vec3f w(10.0f-v.p[0], 0.0f-v.p[1], 0.0f-v.p[2]);
				 test = intersectionRayTriangle(source,w,sommet1.p,sommet2.p,sommet3.p,intersection);
				 i0++;
			   }
	  
		   if (!test){
			affichage[i]=1;
		   } else {
			affichage[i]=0;
	   	   }
	   }
	   //std::cout<<"numero case:"<<i<<" "<<"valeur tableau:"<<affichage[i]<<endl;
	}
}

void printUsage () {
	std::cerr << std::endl 
		 << appTitle << std::endl
         << "Author: Tamy Boubekeur" << std::endl << std::endl
         << "Usage: ./main [<file.off>]" << std::endl
         << "Commands:" << std::endl 
         << "------------------" << std::endl
         << " ?: Print help" << std::endl
		 << " w: Toggle wireframe mode" << std::endl
         << " <drag>+<left button>: rotate model" << std::endl 
         << " <drag>+<right button>: move model" << std::endl
         << " <drag>+<middle button>: zoom" << std::endl
         << " q, <esc>: Quit" << std::endl << std::endl; 
}

void init (const char * modelFilename) {

    glCullFace (GL_BACK);     // Specifies the faces to cull (here the ones pointing away from the camera)
    glEnable (GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
    glDepthFunc (GL_LESS); // Specify the depth test for the z-buffer
    glEnable (GL_DEPTH_TEST); // Enable the z-buffer in the rasterization
    glLineWidth (2.0); // Set the width of edges in GL_LINE polygon mode
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f); // Background color
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f);
	
    mesh.loadOFF (modelFilename);
    camera.resize (DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT);
    intersectAll();
}

void drawScene () {
    glBegin (GL_TRIANGLES);
    for (unsigned int i = 0; i < mesh.T.size (); i++) 
        for (unsigned int j = 0; j < 3; j++) {
            const Vertex & v = mesh.V[mesh.T[i].v[j]];
            // EXERCISE : the following color response shall be replaced with a proper reflectance evaluation/shadow test/etc.
            /* Mise en place de la BRDF de lambert
	    Vec3f direction(v.p[0]-10.0f, v.p[1]-10.0f, v.p[2]-10.0f);
            Vec3f normal(v.n[0], v.n[1], v.n[2]);
	    glColor3f (1.0f*0.6/PI*dot(normal,direction), 1.0f*1/PI*dot(normal,direction), 1.0f*0.6/PI*dot(normal,direction));*/

  	    /* Mise en place de bhlin-phong
	    Vec3f directionS(10.0f-v.p[0], 10.0f-v.p[1], 10.0f-v.p[2]);
            directionS.normalize();

	    float X,Y,Z;
            camera.getPos(X,Y,Z);
	    Vec3f directionC(X-v.p[0], Y-v.p[1], Z-v.p[2]);
	    directionC.normalize();

	    Vec3f halfvector= (directionS+directionC);
	    halfvector.normalize();

            Vec3f normal(v.n[0], v.n[1], v.n[2]);
	    float Ls = pow(std::max(0.0f, dot(normal,halfvector)), 10.0f) * std::max(0.0f, dot(normal,directionS));
            float Ld = 1.0f / PI * std::max(0.0f, dot(normal,directionS));
	    glColor3f (1.0f*(0.6 * Ld + 1 * Ls),
                       1.0f*(1 * Ld + 1 * Ls),
                       1.0f*(0.6 * Ld + 1 * Ls));*/
	    Vec3f directionS(10.0f-v.p[0], 0.0f-v.p[1], 0.0f-v.p[2]);
            directionS.normalize();

	    float X,Y,Z;
            camera.getPos(X,Y,Z);
	    Vec3f directionC(X-v.p[0], Y-v.p[1], Z-v.p[2]);
	    directionC.normalize();

	    Vec3f halfvector= (directionS+directionC);
	    halfvector.normalize();

            Vec3f normal(v.n[0], v.n[1], v.n[2]);
            float prod = dot(normal,halfvector);

	   if (affichage[i]){
		   //cook torrance
		   float diffusion = exp((pow(prod,2)-1)/(0.5*0.5*pow(prod,2))/(PI*0.5*0.5*pow(prod,4)));
		   float fresnel = 0.91 + (1-0.91)*pow(1-max(0.0f,dot(directionS,halfvector)),5.0f);
		   float geom1 = min(1.0f, 2*prod*dot(normal,directionS) / dot(directionC,halfvector));
		   float geom = min(geom1, 2*prod*dot(normal,directionC)/dot(directionC,halfvector));

		   //GGX
		  /* float diffusion = 0.5*0.5/(PI*pow(1+(0.5*0.5-1)*pow(prod,2),2));
		   float geom1 = 2*dot(normal,directionS)/(dot(normal,directionS)+sqrt(0.5*0.5+(1-0.5*0.5)*pow(dot(normal,directionS),2)));  
		   float geom2 = 2*dot(normal,directionC)/(dot(normal,directionC)+sqrt(0.5*0.5+(1-0.5*0.5)*pow(dot(normal,directionC),2)));  
		   float geom = geom1*geom2;*/


		   float Ls = 1.0f*diffusion*fresnel*geom*dot(normal,directionS)/(4*dot(normal,directionS)*dot(normal,directionC));
		   float Ld = 1.0f / PI * max(0.0f, dot(normal,directionS)); 
		   
		   glColor3f (1.0f*(0.6 * Ld + 1 * Ls),
			       1.0f*(1 * Ld + 1 * Ls),
			       1.0f*(0.6 * Ld + 1 * Ls));
	    } else {
	    	  glColor3f(0.0f,0.0f,0.0f); 
            }
           
            glNormal3f (v.n[0], v.n[1], v.n[2]); // Specifies current normal vertex   
            glVertex3f (v.p[0], v.p[1], v.p[2]); // Emit a vertex (one triangle is emitted each time 3 vertices are emitted)
        }

    glEnd (); 
}

void reshape(int w, int h) {
    camera.resize (w, h);
}

void display () {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.apply (); 
    drawScene ();
    glFlush ();
    glutSwapBuffers (); 
}

void key (unsigned char keyPressed, int x, int y) {
    switch (keyPressed) {
    case 'f':
        if (fullScreen) {
            glutReshapeWindow (camera.getScreenWidth (), camera.getScreenHeight ());
            fullScreen = false;
        } else {
            glutFullScreen ();
            fullScreen = true;
        }      
        break;
    case 'q':
    case 27:
        exit (0);
        break;
    case 'w':
        GLint mode[2];
		glGetIntegerv (GL_POLYGON_MODE, mode);
		glPolygonMode (GL_FRONT_AND_BACK, mode[1] ==  GL_FILL ? GL_LINE : GL_FILL);
        break;
        break;
    default:
        printUsage ();
        break;
    }
}

void mouse (int button, int state, int x, int y) {
    camera.handleMouseClickEvent (button, state, x, y);
}

void motion (int x, int y) {
    camera.handleMouseMoveEvent (x, y);
}

void idle () {
    static float lastTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    static unsigned int counter = 0;
    counter++;
    float currentTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    if (currentTime - lastTime >= 1000.0f) {
        FPS = counter;
        counter = 0;
        static char winTitle [128];
        unsigned int numOfTriangles = mesh.T.size ();
        sprintf (winTitle, "Number Of Triangles: %d - FPS: %d", numOfTriangles, FPS);
        glutSetWindowTitle (winTitle);
        lastTime = currentTime;
    }
    glutPostRedisplay (); 
}

int main (int argc, char ** argv) {
    if (argc > 2) {
        printUsage ();
        exit (1);
    }
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize (DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT);
    window = glutCreateWindow (appTitle.c_str ());
    init (argc == 2 ? argv[1] : DEFAULT_MESH_FILE.c_str ());
    glutIdleFunc (idle);
    glutReshapeFunc (reshape);
    glutDisplayFunc (display);
    glutKeyboardFunc (key);
    glutMotionFunc (motion);
    glutMouseFunc (mouse);
    printUsage ();  
    glutMainLoop ();
    return 0;
}

