/*OpenGL program to create a spotlight swing*/



#include <stdio.h>

#include <stdlib.h>  

#include <string.h>   

#include <math.h>   

#include <GL/glut.h>   

   

/* Some <math.h> files do not define M_PI... */   

#ifndef M_PI   

#define M_PI 3.14159265  

#endif  

 

#define TWO_PI  (2*M_PI)  



void mydisplay();

int flag=0;

void frontscreen();

void drawstring(float x,float y,char *string);

  

typedef struct lightRec 

{  

   	float amb[4]; /* R,G,B,A specify intensity High focussed light source */ 

   	float diff[4];  

   	float spec[4];  

   	float pos[4];  

   	float spotDir[3];  

	float spotExp;  

   	float spotCutoff; /* specifies maximum spread angle of light source[0~90,180(uniform light distribution)]. */ 

   	float atten[3];  

   	float trans[3];  

   	float rot[3];  

   	float swing[3];  

   	float arc[3];  

   	float arcIncr[3];  

} Light;  

 

static int useSAME_AMB_SPEC = 1;  

/* *INDENT-OFF* */  

static float modelAmb[4] = {0.2, 0.2, 0.2, 1.0};  

  

static float matAmb[4] = {0.2, 0.2, 0.2, 1.0};  

static float matDiff[4] = {0.8, 0.8, 0.8, 1.0};  

static float matSpec[4] = {0.4, 0.4, 0.4, 1.0};  

static float matEmission[4] = {0.0, 0.0, 0.0, 1.0};  

 /* *INDENT-ON* */  

 

#define NUM_LIGHTS 3 

 

static Light spots[] =  

{  

	{  

		{0.2, 0.0, 0.0, 1.0},  	/* ambient */  

		{0.8, 0.0, 0.0, 1.0},  	/* diffuse */  

	        {0.4, 0.0, 0.0, 1.0},  	/* specular */  

        	{0.0, 0.0, 0.0, 1.0},  	/* position */  

        	{0.0, -1.0, 0.0},   	/* direction */  

	        20.0,  

        	60.0,             	/* exponent, cutoff */  

        	{1.0, 0.0, 0.0},    	/* attenuation */  

        	{0.0, 1.25, 0.0},   	/* translation */  

        	{0.0, 0.0, 0.0},    	/* rotation */       

     	        {20.0, 0.0, 40.0},  	/* swing */  

        	{0.0, 0.0, 0.0},    	/* arc */  

	        {TWO_PI / 70.0, 0.0, TWO_PI / 140.0}  	/* arc increment */  

	},  

        {  

     		{0.0, 0.2, 0.0, 1.0},  	/* ambient */  

     		{0.0, 0.8, 0.0, 1.0},  	/* diffuse */  

     		{0.0, 0.4, 0.0, 1.0},  	/* specular */  

     		{0.0, 0.0, 0.0, 1.0},  	/* position */  

     		{0.0, -1.0, 0.0},   	/* direction */  

     		20.0,  

     		60.0,             	/* exponent, cutoff */  

     		{1.0, 0.0, 0.0},    	/* attenuation */  

     		{0.0, 1.25, 0.0},   	/* translation */  

     		{0.0, 0.0, 0.0},    	/* rotation */  

     		{20.0, 0.0, 40.0},  	/* swing */  

     		{0.0, 0.0, 0.0},    	/* arc */  

     		{TWO_PI / 120.0, 0.0, TWO_PI / 60.0}  	/* arc increment */

   	},  

    	{  

     		{0.0, 0.0, 0.2, 1.0},  	/* ambient */  

     		{0.0, 0.0, 0.8, 1.0},  	/* diffuse */  

     		{0.0, 0.0, 0.4, 1.0},  	/* specular */  

     		{0.0, 0.0, 0.0, 1.0},  	/* position */  

     		{0.0, -1.0, 0.0},   	/* direction */  

     		20.0,  

     		60.0,             	/* exponent, cutoff */  

     		{1.0, 0.0, 0.0},    	/* attenuation */  

     		{0.0, 1.25, 0.0},   	/* translation */  

     		{0.0, 0.0, 0.0},    	/* rotation */  

     		{20.0, 0.0, 40.0},  	/* swing */  

     		{0.0, 0.0, 0.0},    	/* arc */  

     		{TWO_PI / 50.0, 0.0, TWO_PI / 100.0}  	/* arc increment */  

   	}  

};  

static void initLights(void)

{

   	int k;

 

   	for (k = 0; k < NUM_LIGHTS; ++k) 

	{

     		int lt = GL_LIGHT0 + k;

    		Light *light = &spots[k];



    		glEnable(lt);

    		glLightfv(lt, GL_AMBIENT, light->amb);

   		glLightfv(lt, GL_DIFFUSE, light->diff);



     		if (useSAME_AMB_SPEC)

       			glLightfv(lt, GL_SPECULAR, light->amb);

     		else

      			glLightfv(lt, GL_SPECULAR, light->spec);

 

    		glLightf(lt, GL_SPOT_EXPONENT, light->spotExp);

     		glLightf(lt, GL_SPOT_CUTOFF, light->spotCutoff);

    		glLightf(lt, GL_CONSTANT_ATTENUATION, light->atten[0]);

     		glLightf(lt, GL_LINEAR_ATTENUATION, light->atten[1]);

     		glLightf(lt, GL_QUADRATIC_ATTENUATION, light->atten[2]);/* No attenuation */

   	}

}

 

static void aimLights(void)

{

  	int k;



   	for (k = 0; k < NUM_LIGHTS; ++k) 

	{

    		Light *light = &spots[k];



		/* rot = swing * sin(arc) */

		/* arc = arc + arcIncr */



     		light->rot[0] = light->swing[0] * sin(light->arc[0]);

     		light->arc[0] += light->arcIncr[0];



    		if (light->arc[0] > TWO_PI)

       			light->arc[0] -= TWO_PI;



     		light->rot[1] = light->swing[1] * sin(light->arc[1]);

    		light->arc[1] += light->arcIncr[1];



     		if (light->arc[1] > TWO_PI)

       			light->arc[1] -= TWO_PI;



     		light->rot[2] = light->swing[2] * sin(light->arc[2]);

    		light->arc[2] += light->arcIncr[2];



    		if (light->arc[2] > TWO_PI)

      			light->arc[2] -= TWO_PI;

   }

}

 

static void setLights(void)

{

   	int k;

 

  	for (k = 0; k < NUM_LIGHTS; ++k) 

	{

    		int lt = GL_LIGHT0 + k;

    		Light *light = &spots[k];



     		glPushMatrix();

    			glTranslatef(light->trans[0], light->trans[1], light->trans[2]);

    			glRotatef(light->rot[0], 1, 0, 0);

    			glRotatef(light->rot[1], 0, 1, 0);

     			glRotatef(light->rot[2], 0, 0, 1);

     			glLightfv(lt, GL_POSITION, light->pos);

    			glLightfv(lt, GL_SPOT_DIRECTION, light->spotDir);

     		glPopMatrix();

   	}

}

 

static void drawLights(void)

{

   	int k;



  	glDisable(GL_LIGHTING);/* simply associate the current color or index with each  vertex */

   	for (k = 0; k < NUM_LIGHTS; ++k) 

	{

     		Light *light = &spots[k];

 

     		glColor4fv(light->diff);

 

    		glPushMatrix();

    			glTranslatef(light->trans[0], light->trans[1], light->trans[2]);

     			glRotatef(light->rot[0], 1, 0, 0);

     			glRotatef(light->rot[1], 0, 1, 0);

     			glRotatef(light->rot[2], 0, 0, 1);

     			glBegin(GL_LINES);

     				glVertex3f(light->pos[0], light->pos[1], light->pos[2]);

    				glVertex3f(light->spotDir[0], light->spotDir[1], light->spotDir[2]);

    			glEnd();

    		glPopMatrix();

   	}

  	glEnable(GL_LIGHTING);/* use the current lighting parameters to compute the vertex color or index */

}

  

static void drawPlane(int w, int h)

{

  	int i, j;

   	float dw = 1.0 / w;

  	float dh = 1.0 / h;



  	glNormal3f(0.0, 0.0, 1.0);

   	for (j = 0; j < h; ++j) 

	{

     		glBegin(GL_TRIANGLE_STRIP);

     			for (i = 0; i <= w; ++i) 

			{

       				glVertex2f(dw * i, dh * (j + 1));

       				glVertex2f(dw * i, dh * j);

    			}

    		glEnd();

   	}

}



int spin = 0;

 

void display(void)

{

   	glClear(GL_COLOR_BUFFER_BIT);



	if(flag==0)

		frontscreen();

	if(flag==1)

	{ 

   		glPushMatrix();

   			glRotatef(spin, 0, 1, 0); 

	   		aimLights();

   			setLights();



	   		glPushMatrix();

   				glRotatef(-90.0, 1, 0, 0);

   				glScalef(1.9, 1.9, 1.0);

   				glTranslatef(-0.5, -0.5, 0.0);

  				drawPlane(16, 16);

   			glPopMatrix();



   		drawLights();

	   	glPopMatrix();

	} 

   	glutSwapBuffers();

}



void animate(void)

{

   	spin += 0.5;

   	if (spin > 360.0)

    		spin -= 360.0;

   	glutPostRedisplay();

}

 

void visibility(int state)

{

  	if (state == GLUT_VISIBLE) 

	{

     		glutIdleFunc(animate);

   	} 

	else 

	{

     		glutIdleFunc(NULL);

  	}

}


/*
void myMouse(int button,int state,int x,int y)

{

	if(button==GLUT_LEFT_BUTTON&&state==GLUT_DOWN) 		glutIdleFunc(NULL);

	if(button==GLUT_RIGHT_BUTTON&&state==GLUT_DOWN) 		glutIdleFunc(animate);

	if(button==GLUT_MIDDLE_BUTTON&&state==GLUT_DOWN) 		exit(0);

}
*/


void frontscreen(void)

{

	glClear(GL_COLOR_BUFFER_BIT);

	glColor3f(0,0,1);

	drawstring(-0.3,1.2,"ADD   COLLEGE  NAME");



	glColor3f(0.7,0,1);

	drawstring(-0.5,1.0,"DEPARTMENT OF COMPUTER SCIENCE AND ENGINEERING");



	glColor3f(1,0.5,0);

	drawstring(-0.4,0.8,"COMPUTER GRAPHICS LABORATORY[10CSL67]");



	glColor3f(1,0,0);

	drawstring(-0.3,0.5,"A MINI PROJECT ON");



	glColor3f(1,0,0);

	drawstring(-0.3,0.3,"SPOT LIGHT SWING");



	glColor3f(1,0.5,0);

	drawstring(1,-0.4,"SUBMITTED BY:");

	

	glColor3f(0.5,0,0.5);

	drawstring(1.05,-0.6,"ADD NAME");

	drawstring(1.1,-0.8,"ADD NAME");

	

	glColor3f(1,0.5,0);

	drawstring(-1.6,-0.4,"GUIDES:");

	

	glColor3f(0.5,0.2,0.2);

	drawstring(-1.65,-0.6,"ADD NAME");

	drawstring(-1.7,-0.8,"ADD NAME");

	

	glColor3f(1,0.1,1);

	drawstring(-0.50,-1.5,"PRESS ENTER TO START");

	

	glFlush();

}



void myKey( unsigned char key, int x, int y )

{

	if(key==13) //Ascii of 'enter' key is 13

		flag=1;

}



void drawstring(float x,float y,char *string)

{

	glRasterPos2f(x, y);

	int len, i;

	len = (int)strlen(string);

	for (i = 0; i < len; i++) 

	{

		glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, string[i]);

	}

}


void menu(int id)
{  
	switch(id)
	{
	case 1:
		glutIdleFunc(NULL);
		break;
	case 2: 
		glutIdleFunc(animate);
		break;
	case 3:
		exit(0);
		break;
	}
}
void menu1()

{
	glutCreateMenu(menu);
        glutAddMenuEntry("Pause Swing",1);
        glutAddMenuEntry("Resume SpotLight",2);
        glutAddMenuEntry("Quit",3);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

int main(int argc, char **argv)

{

   	int i;

 

   	glutInit(&argc, argv);

   	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);

   

	glutInitWindowSize(12200,600);

	glutInitWindowPosition(0,0);

   	glutCreateWindow("OPENGL SPOT LIGHT SWING");



   	glutDisplayFunc(display);



//	glutMouseFunc(myMouse);

	glutKeyboardFunc(myKey);



   	glutVisibilityFunc(visibility);

 

   	glMatrixMode(GL_PROJECTION);

   	glFrustum(-1, 1, -1, 1, 2, 6);



   	glMatrixMode(GL_MODELVIEW);

   	glTranslatef(0.0, 0.0, -3.0);

  	glRotatef(45.0, 1, 0, 0);



   	glEnable(GL_LIGHTING);

   	glEnable(GL_NORMALIZE);

 

   	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, modelAmb);

  	glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);

   	glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);



/* LIGHT: set diff & spec to color of light source & amb to same color but at 10% to 40% intensity

    MATERIAL: set amb, diff to natural colors & spec color to white */



   	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmb);

  	glMaterialfv(GL_FRONT, GL_DIFFUSE, matDiff);

   	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpec);

  	glMaterialfv(GL_FRONT, GL_EMISSION, matEmission);

   	glMaterialf(GL_FRONT, GL_SHININESS, 10.0);

   	initLights();
	menu1();
   	glutMainLoop();

   	return 0;             /* ANSI C requires main to return int. */

}
