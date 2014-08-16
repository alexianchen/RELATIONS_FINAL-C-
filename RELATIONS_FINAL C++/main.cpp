#define AMOUNT_OF_PEOPLE 59
#define TOTAL_FRAMES 120
#define RS_SCALE (1.0 / (1.0 + RAND_MAX))
//Using SDL, SDL OpenGL, standard IO, and, strings
#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>
#include <OpenGL/glu.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <iostream>
#include "positions.h"
#include "relationshipDB.h"
#include "lodepng.h"
GLenum error;
GLuint listId;
int screenWidth;
int screenHeight;
int ii = 0;
int highlightNumber1;
int highlightNumber2;
char textbox1[2]; int textBox1CursorPos = 0; bool textBox1highlight = false; int oldtextBox1CursorPos = 0;
char textbox2[2]; int textBox2CursorPos = 0; bool textBox2highlight = false; int oldtextBox2CursorPos = 0;
char filePath1[] = "Cards/00.png";
char filePath2[] = "Cards/00.png";
FlatCoords oldMousePos;
FlatCoords MousePos;
int SliderPosx;
bool mouseDown = false;
bool initanimate = true;
bool windowHidden = false;
bool node_in_camera_space[AMOUNT_OF_PEOPLE];
GLuint numbers[10];
GLuint interface[2];
GLuint highlightCards[2];
coordinates camera;
coordinates oldCamera;
coordinates nodes[AMOUNT_OF_PEOPLE];
coordinates unitDistanceChange[AMOUNT_OF_PEOPLE];
coordinates finalPositions[AMOUNT_OF_PEOPLE];


const int   SCREEN_WIDTH    = 1200;
const int   SCREEN_HEIGHT   = 640;

//Starts up SDL, creates window, and initializes OpenGL
bool init();

//Initializes matrices and clear color
bool initGL();

//Per frame update
void update();

//Renders quad to the screen
void render();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* gWindow = NULL;

//OpenGL context
SDL_GLContext gContext;


//My functions
void loadTexture(const char* filename, GLuint texture){
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_NEAREST);
    std::vector<unsigned char> image;
    unsigned int ImageWidth, ImageHeight;
    lodepng::decode(image, ImageWidth, ImageHeight, filename);
    // If there's an error, display it.
    //printf("%s \n", lodepng_error_text(error));
    glTexImage2D(GL_TEXTURE_2D, 0, 4, ImageWidth, ImageHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);
    std::vector<unsigned char>().swap(image);
}

void toOrtho(){
    glDisable(GL_DEPTH_TEST);
    glViewport(0, 0, (screenWidth / 5), screenHeight);
    glScissor(0, 0, (screenWidth / 5), screenHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, (screenWidth / 5), 0, screenHeight);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}


void toPerspective(){
    glViewport((screenWidth / 5), 0, (GLsizei)(screenWidth * 0.8), (GLsizei)screenHeight);
    glScissor((screenWidth / 5), 0, (GLsizei)(screenWidth  * 0.8), (GLsizei)screenHeight);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, (float)((SCREEN_WIDTH * 0.8) / SCREEN_HEIGHT), 1.0f, 500.0f); // FOV, AspectRatio, NearClip, FarClip
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glEnable(GL_DEPTH_TEST);
}

void drawNodes(){
    if((textBox1CursorPos > 0) && (textBox2CursorPos > 0)){
        glPushMatrix();
            glTranslatef(nodes[highlightNumber1].x, nodes[highlightNumber1].y, nodes[highlightNumber1].z);
            //Draw cube
            glCallList(listId);
        glPopMatrix();
        glPushMatrix();
            glTranslatef(nodes[highlightNumber2].x, nodes[highlightNumber2].y, nodes[highlightNumber2].z);
            //Draw cube
            glCallList(listId);
        glPopMatrix();
    }
    else{
        for (int i = 0; i < AMOUNT_OF_PEOPLE; i++){
            if(node_in_camera_space[i] == true){
            glPushMatrix();
                glTranslatef(nodes[i].x, nodes[i].y, nodes[i].z);
                //Draw cube
                glCallList(listId);
            glPopMatrix();
            }
        }
	}
}

void drawText(){
    glEnable(GL_TEXTURE_2D);
    glColor4f(1.0, 1.0, 1.0, 1.0);
    for(int i = 0;i < AMOUNT_OF_PEOPLE; i++){
        if(node_in_camera_space[i] == true){
            glPushMatrix();
            glTranslatef(nodes[i].x, nodes[i].y, nodes[i].z);
            if(i == highlightNumber1){
                if(textBox1CursorPos > 0){
                    glBindTexture(GL_TEXTURE_2D, highlightCards[0]);
                    glBegin(GL_QUADS);
                    glTexCoord2d( 0, 1); glVertex3f(-25, 80, 5);
                    glTexCoord2d( 0, 0); glVertex3f(-25, 10, 5);
                    glTexCoord2d( 1, 0); glVertex3f(25, 10, 5);
                    glTexCoord2d( 1, 1); glVertex3f(25, 80, 5);
                    glEnd();
                }
            }
            else if(i == highlightNumber2){
                if((textBox2CursorPos > 0) & (i == highlightNumber2)){
                    glBindTexture(GL_TEXTURE_2D, highlightCards[1]);
                    glBegin(GL_QUADS);
                    glTexCoord2d( 0, 1); glVertex3f(-25, 80, 5);
                    glTexCoord2d( 0, 0); glVertex3f(-25, 10, 5);
                    glTexCoord2d( 1, 0); glVertex3f(25, 10, 5);
                    glTexCoord2d( 1, 1); glVertex3f(25, 80, 5);
                    glEnd();
                }
            }
            else{
                if((textBox1CursorPos == 0) || (textBox2CursorPos == 0)){
                    if((i + 1) < 10){
                        glBindTexture(GL_TEXTURE_2D, numbers[(i + 1)]);
                        glBegin(GL_QUADS);
                            glTexCoord2d( 0, 1); glVertex3f(-0.85, 5 , 0);
                            glTexCoord2d( 0, 0); glVertex3f(-0.85, 2, 0);
                            glTexCoord2d( 1, 0); glVertex3f(1.85, 2, 0);
                            glTexCoord2d( 1, 1); glVertex3f(1.85, 5, 0);
                        glEnd();
                    }
                    else{
                        glBindTexture(GL_TEXTURE_2D, numbers[(((i + 1) - ((i + 1) % 10)) / 10)]);
                        glBegin(GL_QUADS);
                            glTexCoord2d( 0, 1); glVertex3f(-2, 5, 0);
                            glTexCoord2d( 0, 0); glVertex3f(-2, 2, 0);
                            glTexCoord2d( 1, 0); glVertex3f(-0.15, 2, 0);
                            glTexCoord2d( 1, 1); glVertex3f(-0.15, 5, 0);
                        glEnd();
                        glBindTexture(GL_TEXTURE_2D, numbers[((i + 1) % 10)]);
                        glBegin(GL_QUADS);
                            glTexCoord2d( 0, 1); glVertex3f(0.15, 5, 0);
                            glTexCoord2d( 0, 0); glVertex3f(0.15, 2, 0);
                            glTexCoord2d( 1, 0); glVertex3f(2, 2, 0);
                            glTexCoord2d( 1, 1); glVertex3f(2, 5, 0);
                        glEnd();
                    }
                }
            }
            glPopMatrix();
        }
    }
    glDisable(GL_TEXTURE_2D);
}

void drawLines(){
    if((textBox1CursorPos > 0) && (textBox2CursorPos > 0)){
        //This is when we want only the highlights
        changePersonColor(highlightNumber1, highlightNumber2, 255);
        glBegin(GL_LINES);
            glVertex3f(nodes[highlightNumber1].x, nodes[highlightNumber1].y, nodes[highlightNumber1].z);
            glVertex3f(nodes[highlightNumber2].x, nodes[highlightNumber2].y, nodes[highlightNumber2].z);
        glEnd();
    }
    else{
        for(int i = 0;i < AMOUNT_OF_PEOPLE; i++){
            for(int a = (i + 1); a < AMOUNT_OF_PEOPLE; a++){
                if((node_in_camera_space[i] && node_in_camera_space[a]) == true){
                    //When both cubes are in range of camera
                    changePersonColor(i, a, 128);
                    glBegin(GL_LINES);
                        glVertex3f(nodes[i].x, nodes[i].y, nodes[i].z);
                        glVertex3f(nodes[a].x, nodes[a].y, nodes[a].z);
                    glEnd();
                }
            }
        }
    }
}
void drawinfo(){
    toOrtho();
    glEnable(GL_TEXTURE_2D);
    glPushAttrib(GL_LIGHTING);
    glDisable(GL_LIGHTING);
    glColor3f(1.0, 1.0, 1.0);
    //Main interface
    glBindTexture(GL_TEXTURE_2D, interface[0]);
    glBegin(GL_QUADS);
        glTexCoord2d( 0, 1); glVertex2f(0, screenHeight);
        glTexCoord2d( 0, 0); glVertex2f(0, 0);
        glTexCoord2d( 1, 0); glVertex2f((screenWidth / 5), 0);
        glTexCoord2d( 1, 1); glVertex2f((screenWidth / 5), screenHeight);
    glEnd();
    //slider button
    glBindTexture(GL_TEXTURE_2D, interface[1]);
    glBegin(GL_QUADS);
        glTexCoord2d( 0, 1); glVertex2f(25 + SliderPosx, 450);
        glTexCoord2d( 0, 0); glVertex2f(25 + SliderPosx, 430);
        glTexCoord2d( 1, 0); glVertex2f(75 + SliderPosx, 430);
        glTexCoord2d( 1, 1); glVertex2f(75 + SliderPosx, 450);
    glEnd();
    //textbox background
    if(textBox1highlight == true){
        //background for textbox
        glColor3f(0, 1, 1);
        glBegin(GL_QUADS);
        glVertex2f(0, 650);
        glVertex2f(0, 565);
        glVertex2f(250, 565);
        glVertex2f(250, 650);
        glEnd();
    }
    if(textBox2highlight == true){
        //background for textbox
        glColor3f(0, 1, 1);
        glBegin(GL_QUADS);
        glVertex2f(0, 564);
        glVertex2f(0, 490);
        glVertex2f(250, 490);
        glVertex2f(250, 564);
        glEnd();
    }
    //The text
    if(textBox1CursorPos > 0){
        if(textBox1highlight == false){
            //this is when the textbox is NOT highlighted BUT there is still text
            glDisable(GL_TEXTURE_2D);
            glColor4ub(178, 178, 178, 255);
            glBegin(GL_QUADS);
            glVertex2f(0, 650);
            glVertex2f(0, 566);
            glVertex2f(250, 566);
            glVertex2f(250, 650);
            glEnd();
            glEnable(GL_TEXTURE_2D);
            }
        if(textBox1CursorPos == 1){
            //1 dight in the box, from 1 ~ 9
            int number = textbox1[0] - '0';
            //The number is the real number(starts with 1), not the 0+ array number
            if(number > 0 && number <= 9){
                glBindTexture(GL_TEXTURE_2D, numbers[(number)]);
                glBegin(GL_QUADS);
                    glTexCoord2d( 0, 1); glVertex2f(100, 630);
                    glTexCoord2d( 0, 0); glVertex2f(100, 580);
                    glTexCoord2d( 1, 0); glVertex2f(125, 580);
                    glTexCoord2d( 1, 1); glVertex2f(125, 630);
                glEnd();
            }
        }
        //>0: checked. ==1: checked. only possible alternative: 2
        else{
            int number = (((textbox1[0] - '0') * 10) + (textbox1[1] - '0'));
            if(number >= 10){
                glBindTexture(GL_TEXTURE_2D, numbers[((number - (number % 10)) / 10)]);
                glBegin(GL_QUADS);
                    glTexCoord2d( 0, 1); glVertex2f(90, 630);
                    glTexCoord2d( 0, 0); glVertex2f(90, 580);
                    glTexCoord2d( 1, 0); glVertex2f(114, 580);
                    glTexCoord2d( 1, 1); glVertex2f(114, 630);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, numbers[(number % 10)]);
                glBegin(GL_QUADS);
                    glTexCoord2d( 0, 1); glVertex2f(116, 630);
                    glTexCoord2d( 0, 0); glVertex2f(116, 580);
                    glTexCoord2d( 1, 0); glVertex2f(140, 580);
                    glTexCoord2d( 1, 1); glVertex2f(140, 630);
                glEnd();
            }
        }
    }
    //The text, for box 2
    if(textBox2CursorPos > 0){
        if(textBox2highlight == false){
            //this is when the textbox is NOT highlighted BUT there is still text
            glDisable(GL_TEXTURE_2D);
            glColor4ub(178, 178, 178, 255);
            glBegin(GL_QUADS);
            glVertex2f(0, 564);
            glVertex2f(0, 490);
            glVertex2f(250, 490);
            glVertex2f(250, 564);
            glEnd();
            glEnable(GL_TEXTURE_2D);
        }
        if(textBox2CursorPos == 1){
            //1 dight in the box, from 1 ~ 9
            int number = textbox2[0] - '0';
            //The number is the real number(starts with 1), not the 0+ array number
            if(number > 0 && number <= 9){
                glBindTexture(GL_TEXTURE_2D, numbers[(number)]);
                glBegin(GL_QUADS);
                    glTexCoord2d( 0, 1); glVertex2f(100, 555);
                    glTexCoord2d( 0, 0); glVertex2f(100, 505);
                    glTexCoord2d( 1, 0); glVertex2f(125, 505);
                    glTexCoord2d( 1, 1); glVertex2f(125, 555);
                glEnd();
            }
        }
        //>0: checked. ==1: checked. only possible alternative: 2
        else{
            int number = (((textbox2[0] - '0') * 10) + (textbox2[1] - '0'));
            if(number >= 10){
                glBindTexture(GL_TEXTURE_2D, numbers[((number - (number % 10)) / 10)]);
                glBegin(GL_QUADS);
                    glTexCoord2d( 0, 1); glVertex2f(90, 555);
                    glTexCoord2d( 0, 0); glVertex2f(90, 505);
                    glTexCoord2d( 1, 0); glVertex2f(114, 505);
                    glTexCoord2d( 1, 1); glVertex2f(114, 555);
                glEnd();
                glBindTexture(GL_TEXTURE_2D, numbers[(number % 10)]);
                glBegin(GL_QUADS);
                    glTexCoord2d( 0, 1); glVertex2f(116, 555);
                    glTexCoord2d( 0, 0); glVertex2f(116, 505);
                    glTexCoord2d( 1, 0); glVertex2f(140, 505);
                    glTexCoord2d( 1, 1); glVertex2f(140, 555);
                glEnd();
            }
        }
    }
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_LIGHTING);
    glPopAttrib();
    toPerspective();
}

void animateLines(){
    initAnimations(nodes[ii].x, nodes[ii].y, nodes[ii].z, &nodes[0], &finalPositions[0], ii);
	for (int a = 0; a < AMOUNT_OF_PEOPLE; a++){
		if (a < ii){
			for (int i = (a + 1); i < AMOUNT_OF_PEOPLE; i++){
				if((node_in_camera_space[i] && node_in_camera_space[a]) == true){
                    //When both cubes are in range of camera
                    changePersonColor(i, a, 128);
                    glBegin(GL_LINES);
                        glVertex3f(nodes[i].x, nodes[i].y, nodes[i].z);
                        glVertex3f(nodes[a].x, nodes[a].y, nodes[a].z);
                    glEnd();
                }
			}
		}
        if(ii != a){
            if((node_in_camera_space[ii] && node_in_camera_space[a]) == true){
                //When both cubes are in range of camera
                changePersonColor(ii, a, 128);
                glBegin(GL_LINES);
                    glVertex3f(nodes[ii].x, nodes[ii].y, nodes[ii].z);
                    glVertex3f(finalPositions[a].x, finalPositions[a].y, finalPositions[a].z);
                glEnd();
            }
        }
	}
	if (ii >= (AMOUNT_OF_PEOPLE - 1)){
		initanimate = false;
		animate = true;
	}
	if (count == 1){
		ii++;
		count = 0;
	}
	else(count++);
    
}
void initLights(){
    GLfloat lightKa[] = {.5f, .5f, .5f, 1.0f};  // ambient light
    GLfloat lightKd[] = {.7f, .7f, .7f, 1.0f};  // diffuse light
    GLfloat lightKs[] = {1.0f, 1.0f, 1.0f, 1.0f};  // specular light
    glLightfv(GL_LIGHT0, GL_AMBIENT, lightKa);
    glLightfv(GL_LIGHT0, GL_DIFFUSE, lightKd);
    glLightfv(GL_LIGHT0, GL_SPECULAR, lightKs);
    float lightPos[4] = {0, 0, -30, 1}; // positional light
    glLightfv(GL_LIGHT0, GL_POSITION, lightPos);
    glEnable(GL_LIGHT0);
}

//SDL functions
bool init()
{
	//Initialization flag
	bool success = true;
    
	//Initialize SDL
	SDL_Init(SDL_INIT_VIDEO);
    
    //Use OpenGL 2.1
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
        
    //Create window
    gWindow = SDL_CreateWindow("CLASS 3, YEAR 11", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN);
    
    //Create context
    gContext = SDL_GL_CreateContext(gWindow);
    
    //Use Vsync
    SDL_GL_SetSwapInterval(1);
    
    //Initialize OpenGL
    initGL();

    //Other inits
    unsigned int iseed = (unsigned int)time(NULL);
    srand(iseed);
    
    create_coords(&nodes[0], AMOUNT_OF_PEOPLE);
    subtle_coords_change(&nodes[0], &finalPositions[0], AMOUNT_OF_PEOPLE);
    getDistance(&nodes[0], &finalPositions[0], &unitDistanceChange[0], AMOUNT_OF_PEOPLE);
    CheckNodeVisible(&nodes[0], AMOUNT_OF_PEOPLE, &node_in_camera_space[0], camera);
    screenWidth = SCREEN_WIDTH;
    screenHeight = SCREEN_HEIGHT;
    SliderPosx = 70;
    
    camera.x = 0;
    camera.y = 0;
    camera.z = -(50 + SliderPosx);
    
    MousePos.x = 0;
    MousePos.y = 0;
    
    oldCamera.x = camera.x;
    oldCamera.y = camera.y;
    oldCamera.z = camera.z;
    
	return success;
}

bool initGL()
{
    glShadeModel(GL_FLAT);                    // shading mathod: GL_SMOOTH or GL_FLAT
    glPixelStorei(GL_UNPACK_ALIGNMENT, 4);      // 4-byte pixel alignment
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);
    glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LIGHTING);
    glEnable(GL_CULL_FACE);
    glEnable(GL_SCISSOR_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // track material ambient and diffuse from surface color, call it before glEnable(GL_COLOR_MATERIAL)
    glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    
    glGenTextures(10, &numbers[0]);
    glGenTextures(2, &interface[0]);
    glGenTextures(2, &highlightCards[0]);
    loadTexture("Numbers/0.png", numbers[0]);
    loadTexture("Numbers/1.png", numbers[1]);
    loadTexture("Numbers/2.png", numbers[2]);
    loadTexture("Numbers/3.png", numbers[3]);
    loadTexture("Numbers/4.png", numbers[4]);
    loadTexture("Numbers/5.png", numbers[5]);
    loadTexture("Numbers/6.png", numbers[6]);
    loadTexture("Numbers/7.png", numbers[7]);
    loadTexture("Numbers/8.png", numbers[8]);
    loadTexture("Numbers/9.png", numbers[9]);
    loadTexture("Interface/interface.png", interface[0]);
    loadTexture("Interface/button.png", interface[1]);
    
    glLineWidth(4.0);

    listId = glGenLists(1);
    glNewList(listId, GL_COMPILE);
        glColor4f(1.0, 0.0, 0.0, 0.9);
        glBegin(GL_QUADS);
        // front faces
        glNormal3f(0,0,1);
        glVertex3f(1,1,1);
        glVertex3f(-1,1,1);
        glVertex3f(-1,-1,1);
        glVertex3f(1,-1,1);
        // right faces
        glNormal3f(1,0,0);
        glVertex3f(1,1,1);
        glVertex3f(1,-1,1);
        glVertex3f(1,-1,-1);
        glVertex3f(1,1,-1);
        // top faces
        glNormal3f(0,1,0);
        glVertex3f(1,1,1);
        glVertex3f(1,1,-1);
        glVertex3f(-1,1,-1);
        glVertex3f(-1,1,1);
        // left faces
        glNormal3f(-1,0,0);
        glVertex3f(-1,1,1);
        glVertex3f(-1,1,-1);
        glVertex3f(-1,-1,-1);
        glVertex3f(-1,-1,1);
        // bottom faces
        glNormal3f(0,-1,0);
        glVertex3f(-1,-1,-1);
        glVertex3f(1,-1,-1);
        glVertex3f(1,-1,1);
        glVertex3f(-1,-1,1);
        // back faces
        glNormal3f(0,0,-1);
        glVertex3f(1,-1,-1);
        glVertex3f(-1,-1,-1);
        glVertex3f(-1,1,-1);
        glVertex3f(1,1,-1);
        glEnd();
    glEndList();
    
    glClearColor(0, 0, 0, 0);
    glClearStencil(0);
    glClearDepth(10.0f);
    glDepthFunc(GL_LEQUAL);
    initLights();
    return true;
}

void handleInterface(int xpos, int ypos){
    //Update slider
    if(ypos >= 180 && ypos <= 220){
        if(xpos >= 50 && xpos <= 200){
            SliderPosx = xpos - 50;
        }
    }
    //highlight textbox1
    if(ypos >= 0 && ypos <= 75){
        textBox1highlight = true;
    }
    else(textBox1highlight = false);
    //highlight textbox2
    if(ypos > 75 && ypos <= 140){
        textBox2highlight = true;
    }
    else(textBox2highlight = false);
}

void handleMouse(SDL_Event e){
    if(e.type == SDL_MOUSEBUTTONDOWN){
        SDL_GetMouseState(&oldMousePos.x, &oldMousePos.y);
        oldCamera.x = camera.x;
        oldCamera.y = camera.y;
        oldCamera.z = camera.z;
        mouseDown = true;
    }
    if(e.type == SDL_MOUSEBUTTONUP){
        mouseDown = false;
    }
    //Mouse Related Funcs
    if(mouseDown == true && e.button.button == SDL_BUTTON_LEFT){
        SDL_GetMouseState(&MousePos.x, &MousePos.y);
        if(oldMousePos.x < SCREEN_WIDTH / 5){
            handleInterface(MousePos.x, MousePos.y);
        }
        else{
            camera.x = oldCamera.x + ((MousePos.x - oldMousePos.x) / 4);
            camera.y = oldCamera.y + ((oldMousePos.y - MousePos.y) / 4);
        }
    }
}
void handleKeys(SDL_Event e){
    //This is for text INPUT
    if(textBox1highlight == true){
        if(e.type == SDL_TEXTINPUT){
            if(textBox1CursorPos < 2){
                //textbox is limited to 0,1(2 digits)
                if('0' <= e.text.text[0] &&  e.text.text[0] <= '9'){
                    strncat(textbox1, e.text.text, 1);
                    textBox1CursorPos++;
                }
            }
        }
        //This is for REMOVING the text
        if(e.type == SDL_KEYDOWN){
            if(e.key.keysym.sym == SDLK_BACKSPACE){
                if((textBox1CursorPos > 0)){
                    textbox1[textBox1CursorPos - 1] = '\0';
                    textBox1CursorPos--;
                }
            }
        }
    }
    //textbox 2
    if(textBox2highlight == true){
        if(e.type == SDL_TEXTINPUT){
            if(textBox2CursorPos < 2){
                //textbox is limited to 0,1(2 digits)
                if('0' <= e.text.text[0] &&  e.text.text[0] <= '9'){
                    strncat(textbox2, e.text.text, 1);
                    textBox2CursorPos++;
                }
            }
        }
        //This is for REMOVING the text
        if(e.type == SDL_KEYDOWN){
            if(e.key.keysym.sym == SDLK_BACKSPACE){
                if((textBox2CursorPos > 0)){
                    textbox2[textBox2CursorPos - 1] = '\0';
                    textBox2CursorPos--;
                }
            }
        }
    }

}
void update(){
	if(initanimate == false){
        if(animate == false && ((textBox1CursorPos > 0) || (textBox2CursorPos > 0))){
            edit_coords_highlight(&finalPositions[0], AMOUNT_OF_PEOPLE, highlightNumber1, highlightNumber2);
            getDistance(&nodes[0], &finalPositions[0], &unitDistanceChange[0], AMOUNT_OF_PEOPLE);
            animate = true;
            count = 1;
        }
		else if(irand(100) == 1){
			animate = true;
		}
		if(animate == true){
			changeCoordinates(&nodes[0], &unitDistanceChange[0], &finalPositions[0]);
		}
    }
    CheckNodeVisible(&nodes[0], AMOUNT_OF_PEOPLE, &node_in_camera_space[0], camera);
    // Slider Position --> Camera z distance
    camera.z = -(200 - SliderPosx);
    //highlights
    if(textBox1CursorPos != oldtextBox1CursorPos){
        if(textBox1CursorPos != 0){
            if(textBox1CursorPos == 1){
                filePath1[6] = '0';
                filePath1[7] = textbox1[0];
            }
            else{
                filePath1[6] = textbox1[0];
                filePath1[7] = textbox1[1];
            }
        }
        loadTexture(filePath1, highlightCards[0]);
        oldtextBox1CursorPos = textBox1CursorPos;
    }
    if(textBox2CursorPos != oldtextBox2CursorPos){
        if(textBox2CursorPos != 0){
            if(textBox2CursorPos == 1){
                filePath2[6] = '0';
                filePath2[7] = textbox2[0];
            }
            else{
                filePath2[6] = textbox2[0];
                filePath2[7] = textbox2[1];
            }
        }
        loadTexture(filePath2, highlightCards[1]);
        oldtextBox2CursorPos = textBox2CursorPos;
    }
    if(textBox1CursorPos > 0){
        if(textBox1CursorPos == 1){
            highlightNumber1 = (textbox1[0] - '0') - 1;
        }
        else(highlightNumber1 = (((textbox1[0] - '0') * 10) + (textbox1[1] - '0')) - 1);
    }
    else (highlightNumber1 = 0);
    if(textBox2CursorPos > 0){
        if(textBox2CursorPos == 1){
            highlightNumber2 = (textbox2[0] - '0') - 1;
        }
        else(highlightNumber2 = (((textbox2[0] - '0') * 10) + (textbox2[1] - '0')) - 1);
    }
    else (highlightNumber2 = 0);
    //printf("%d, %d\n", highlightNumber1, highlightNumber2);
    //printf("%s, %s\n", filePath1, filePath2);
}

void render(){
    glClearColor(0.9,0.9,0.9,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glPushMatrix();
    glTranslatef(camera.x, camera.y, camera.z);
    if(initanimate == true){
        animateLines();
    }
    else{
        drawLines();
    }
    drawText();
    drawNodes();
    glPopMatrix();
    drawinfo();
}

void close(){
	//Destroy window
	SDL_DestroyWindow(gWindow);
	gWindow = NULL;
    //Quit SDL subsystems
	SDL_Quit();
}

int main(int argc, char* args[]){
	//Start up SDL and create window
	init();
    //Main loop flag
    bool quit = false;
    
    //Event handler
    SDL_Event e;
    
    //Enable text input
    SDL_StartTextInput();
        
    //While application is running
    while(!quit){
        //Handle events on queue
        while( SDL_PollEvent(&e) != 0 ){
            //User requests quit
            if(e.type == SDL_QUIT){
                quit = true;
            }
            if(e.window.event == SDL_WINDOWEVENT_HIDDEN || e.window.event == SDL_WINDOWEVENT_MINIMIZED){
                windowHidden = true;
            }
            if(e.window.event == SDL_WINDOWEVENT_SHOWN || e.window.event == SDL_WINDOWEVENT_RESTORED){
                windowHidden = false;
            }
            handleKeys(e);
        }
        if(windowHidden == false){
            handleMouse(e);
            render();
            update();
            //Update screen
            SDL_GL_SwapWindow(gWindow);
        }
        //sleep for 0.1 seconds, save processing power
        else(usleep(100000));
    }
    //Disable text input
    SDL_StopTextInput();
    //Free resources and close SDL
	close();
    return 0;
}
