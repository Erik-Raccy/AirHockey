#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <gl\freeglut.h>
#include <gl\GLU.h>
#include <vector>
#include <math.h>
#include <sstream>
#include <string>
#include "puckresource.h"

int AIScore, PlayerScore = 0;
float SCREEN_WIDTH = 500;
float SCREEN_HEIGHT = 500;

bool gameStartup = true;
bool gamePaused = false;
bool gameEnded = false;
int selectedMenuOption = 0;
int endGameScore = 7;

int bound[] = {75, 150, -75, -150}; //east north, west, south
int net[] = {25, -25};

float playPlaneNorm[3] = {0, 1, 0};
float camPos[3] = {0,80,300};
float rayDir[3];
float rayOrg[3];
float rayPos[3] = {0.01,0.01,0.01};

GLUquadricObj *quadratic;

_interaction inter;
std::vector<puck> coords;
std::vector<paddle> player;

float light_pos0[] = {0, 20, -200, 1.0};
float light_pos1[] = {0, 20, 200, 1.0};
float light_pos2[] = {100, 50, -175, 1.0};

float amb0[4] = {0, 0, 0, 1}; //Colour of shadow
float diff0[4] = {1, 1, 1, 1}; //Colour of light
float spec0[4] = {1, 1, 1, 1}; //Colour of reflection

float table_amb[] = {0.3, 0.3, 0.3, 1.0}; //colour without light
float table_diff[] = {0.5, 0.5, 0.5, 1.0}; //colour of material
float table_spec[] = {0.6, 0.6, 0.6, 1.0};//colour of reflection
float table_shiny = 20;

float border_amb[] = {0.0, 0.0, 0.4, 1.0}; //colour without light
float border_diff[] = {0.1, 0.1, 0.6, 1.0}; //colour of material
float border_spec[] = {0.2, 0.2, 0.6, 1.0};//colour of reflection
float border_shiny = 20;

float puck_amb[] = {1, 0.0, 0.0, 1.0}; //colour without light
float puck_diff[] = {0.6, 0.0, 0.0, 1.0}; //colour of material
float puck_spec[] = {0.8, 0.8, 0.8, 1.0};//colour of reflection
float puck_shiny = 50;

float player_amb[] = {0, 0.4, 0, 1};
float player_diff[]= {0.2, 0.6, 0.2, 1};
float player_spec[] = {0.3, 0.8, 0.3, 1};
float player_shiny = 40;

/*********END INITAL VALUES**************/


//void AIColor(){
//	ai_amb[0] = player[1].getred();
//	ai_amb[1] = player[1].getgreen();
//	ai_amb[2] = player[1].getblue();
//	ai_amb[3] = 1;
//	ai_diff[0] = player[1].getred() + 0.2;
//	ai_diff[1] = player[1].getgreen() + 0.2;
//	ai_diff[2] = player[1].getblue() + 0.2;
//	ai_diff[3] = 1;
//	ai_spec[0] = player[1].getred() + 0.4;
//	ai_spec[1] = player[1].getgreen() + 0.4;
//	ai_spec[2] = player[1].getblue() + 0.4;
//	ai_diff[3] = 1;
//}
//
//void PlayerColor(){
//	player_amb[0] = player[0].getred();
//	player_amb[1] = player[0].getgreen();
//	player_amb[2] = player[0].getblue();
//	player_amb[3] = 1;
//	player_diff[0] = player[0].getred() + 0.2;
//	player_diff[1] = player[0].getgreen() + 0.2;
//	player_diff[2] = player[0].getblue() + 0.2;
//	player_diff[3] = 1;
//	player_spec[0] = player[0].getred() + 0.4;
//	player_spec[1] = player[0].getgreen() + 0.4;
//	player_spec[2] = player[0].getblue() + 0.4;
//	player_diff[3] = 1;
//}

//Text for Score Display
void drawStrokeText(char* text, int x, int y)
{
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, SCREEN_WIDTH, 0, SCREEN_HEIGHT);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glTranslatef(x, y, 0);
	glScalef(0.25, 0.25, 0.25);

	char* c;  //character iterator (ptr)
	for (c = text; *c != '\0'; c++) //stop when we hit null character 
	{
		glutStrokeCharacter(GLUT_STROKE_ROMAN, *c); //print one char 
	}

	//Set the view back up properly
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 500);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camPos[0], camPos[1], camPos[2], 0, 0, 0, 0, 1, 0);
}

void drawDeveloperCreditsMenu(){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 175, 0, 0, 0, 0, 1, 0);
	glPushMatrix();
	glBegin(GL_QUADS);
	glColor3f(1, 1, 0);
	glVertex3f(150, 150, 155); //southeast point
	glVertex3f(-150, 150, 155); //northeast point
	glVertex3f(-150, -150, 155); //northwest point
	glVertex3f(150, -150, 155); //southwest point
	glEnd();
	glPopMatrix();
	glColor3f(0, 0, 0);
	drawStrokeText("Made By:", SCREEN_WIDTH / 10, SCREEN_HEIGHT - SCREEN_HEIGHT / 10);
	drawStrokeText("Sean Wikant", 1.5*SCREEN_WIDTH / 10, SCREEN_HEIGHT - 3.2*SCREEN_HEIGHT / 10);
	drawStrokeText("and", 1.5*SCREEN_WIDTH / 10, SCREEN_HEIGHT - 4.2*SCREEN_HEIGHT / 10);
	drawStrokeText("Eric Johanson", 1.5*SCREEN_WIDTH / 10, SCREEN_HEIGHT - 5.2*SCREEN_HEIGHT / 10);
}

void drawSelectionBox(float x, float y, float horizontal, float vertical){
	glPushMatrix();
	glBegin(GL_QUADS);
	glColor3f(0, 0, 1);
	glVertex3f(x, y, 160); //southeast point
	glVertex3f(x + horizontal, y, 160); //northeast point
	glVertex3f(x + horizontal, y + vertical, 160); //northwest point
	glVertex3f(x, y + vertical, 160); //southwest point
	glEnd();
	glPopMatrix();
}

void drawStartupMenu(){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0, 0, 175, 0, 0, 0, 0, 1, 0);

	glPushMatrix();
	glBegin(GL_QUADS);
	glColor3f(1, 1, 0);
	glVertex3f(150, 150, 155); //southeast point
	glVertex3f(-150, 150, 155); //northeast point
	glVertex3f(-150, -150, 155); //northwest point
	glVertex3f(150, -150, 155); //southwest point
	glEnd();
	glPopMatrix();

	if (selectedMenuOption == 0){
		drawSelectionBox(-4.9, 2, 6, 1.2);
	}
	else if (selectedMenuOption == 1){
		drawSelectionBox(-4.9, 0.7, 5, 1.2);
	}
	else if (selectedMenuOption == 2){
		drawSelectionBox(-4.9, -0.6, 8, 1.2);
	}
	glColor3f(0, 0, 0);

	drawStrokeText("Hit a Puck and Win!!", SCREEN_WIDTH / 10, SCREEN_HEIGHT - SCREEN_HEIGHT / 10);
	drawStrokeText("Start Game", 1.5*SCREEN_WIDTH / 10, SCREEN_HEIGHT - 3.2*SCREEN_HEIGHT / 10);
	drawStrokeText("Options", 1.5*SCREEN_WIDTH / 10, SCREEN_HEIGHT - 4.2*SCREEN_HEIGHT / 10);
	drawStrokeText("Developer Credits", 1.5*SCREEN_WIDTH / 10, SCREEN_HEIGHT - 5.2*SCREEN_HEIGHT / 10);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 500);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(camPos[0], camPos[1], camPos[2], 0, 0, 0, 0, 1, 0);
}

void drawTable(){
	glBegin(GL_QUADS);
		
	//glColor3f(0, 1, 1);
	glNormal3f(0, 1, 0);
	glVertex3f(bound[0], 0, bound[3]); //southeast point
	glVertex3f(bound[0], 0, bound[1]); //northeast point
	glVertex3f(bound[2], 0, bound[1]); //northwest point
	glVertex3f(bound[2], 0, bound[3]); //southwest point

	glEnd();
}

void drawBorders(){
	glBegin(GL_QUADS);

	//glColor3f(1, 1, 1);
	glNormal3f(-1, 0, 0);
	glVertex3f(bound[0], 0, bound[3]);
	glVertex3f(bound[0], 10, bound[3]);
	glVertex3f(bound[0], 10, bound[1]);
	glVertex3f(bound[0], 0, bound[1]);

	//glColor3f(1, 1, 1);
	glNormal3f(1, 0, 0);
	glVertex3f(bound[2], 0, bound[3]);
	glVertex3f(bound[2], 10, bound[3]);
	glVertex3f(bound[2], 10, bound[1]);
	glVertex3f(bound[2], 0, bound[1]);

	//glColor3f(1, 1, 1);
	glNormal3f(0, 0, -1);
	glVertex3f(bound[0], 0, bound[1]);
	glVertex3f(bound[0], 10, bound[1]);
	glVertex3f(net[0], 10, bound[1]);
	glVertex3f(net[0], 0, bound[1]);

	//glColor3f(1, 1, 1);
	glNormal3f(0, 0, -1);
	glVertex3f(bound[2], 0, bound[1]);
	glVertex3f(bound[2], 10, bound[1]);
	glVertex3f(net[1], 10, bound[1]);
	glVertex3f(net[1], 0, bound[1]);

	//glColor3f(1, 1, 1);
	glNormal3f(0, 0, 1);
	glVertex3f(bound[0], 0, bound[3]);
	glVertex3f(bound[0], 10, bound[3]);
	glVertex3f(net[0], 10, bound[3]);
	glVertex3f(net[0], 0, bound[3]);

	//glColor3f(1, 1, 1);
	glNormal3f(0, 0, 1);
	glVertex3f(bound[2], 0, bound[3]);
	glVertex3f(bound[2], 10, bound[3]);
	glVertex3f(net[1], 10, bound[3]);
	glVertex3f(net[1], 0, bound[3]);

	glNormal3f(0, 1, 0);
	glVertex3f(bound[0] + 20, 10, bound[1] + 20);
	glVertex3f(bound[0] + 20, 10, bound[3] - 20);
	glVertex3f(bound[0], 10, bound[3] - 20);
	glVertex3f(bound[0], 10, bound[1] + 20);

	glNormal3f(0, 1, 0);
	glVertex3f(bound[2] - 20, 10, bound[1] + 20);
	glVertex3f(bound[2] - 20, 10, bound[3] - 20);
	glVertex3f(bound[2], 10, bound[3] - 20);
	glVertex3f(bound[2], 10, bound[1] + 20);

	glNormal3f(0, 1, 0);
	glVertex3f(bound[0], 10, bound[3]);
	glVertex3f(bound[0], 10, bound[3] - 20);
	glVertex3f(25, 10, bound[3] - 20);
	glVertex3f(25, 10, bound[3]);

	glNormal3f(0, 1, 0);
	glVertex3f(bound[2], 10, bound[3]);
	glVertex3f(bound[2], 10, bound[3] - 20);
	glVertex3f(-25, 10, bound[3] - 20);
	glVertex3f(-25, 10, bound[3]);

	glNormal3f(0, 1, 0);
	glVertex3f(bound[0], 10, bound[1]);
	glVertex3f(bound[0], 10, bound[1] + 20);
	glVertex3f(25, 10, bound[1] + 20);
	glVertex3f(25, 10, bound[1]);

	glNormal3f(0, 1, 0);
	glVertex3f(bound[2], 10, bound[1]);
	glVertex3f(bound[2], 10, bound[1] + 20);
	glVertex3f(-25, 10, bound[1] + 20);
	glVertex3f(-25, 10, bound[1]);

	glNormal3f(0, 0, 1);
	glVertex3f(bound[0] + 20, 10, bound[1] + 20);
	glVertex3f(bound[0] + 20, -50, bound[1] + 20);
	glVertex3f(bound[2] - 20, -50, bound[1] + 20);
	glVertex3f(bound[2] - 20, 10, bound[1] + 20);

	glEnd();
}

void drawNet(){
	glBegin(GL_QUADS);
	
	glNormal3f(0, 1, 0);
	glVertex3f(25, 10, bound[1]);
	glVertex3f(25, 10, bound[1] + 20);
	glVertex3f(-25, 10, bound[1] + 20);
	glVertex3f(-25, 10, bound[1]);

	glNormal3f(0, 1, 0);
	glVertex3f(25, 10, bound[3]);
	glVertex3f(25, 10, bound[3] - 20);
	glVertex3f(-25, 10, bound[3] - 20);
	glVertex3f(-25, 10, bound[3]);

	glEnd();
}

void drawPuck(){
	//glColor3f(1, 1, 1);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, -coords[0].getheight());
	gluCylinder(quadratic,coords[0].getsize(),coords[0].getsize(),coords[0].getheight(),16,16);
	glRotatef(180, 1, 0, 0);
	gluDisk(quadratic, 0, coords[0].getsize(), 16, 16);
	glPopMatrix();
}

void drawPlayerPaddle(){
	//glColor3f(1, 1, 1);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, -player[0].getheight());
	gluCylinder(quadratic, player[0].getsize(), player[0].getsize(), player[0].getheight(),16,16);
	glRotatef(180, 1, 0, 0);
	gluDisk(quadratic, 0, player[0].getsize(), 16, 16);
	glRotatef(180, 1, 0, 0);
	glTranslatef(0, 0, -player[0].getheight());
	glutSolidSphere(player[0].getsize()*0.5, 16, 16);
	gluCylinder(quadratic, player[0].getsize()*0.5, player[0].getsize()*0.5, player[0].getheight(),16,16);
	glPopMatrix();
}

void drawAIPaddle(){
	//glColor3f(0, 0, 1);
	glPushMatrix();
	glRotatef(90, 1, 0, 0);
	glTranslatef(0, 0, -player[1].getheight());
	gluCylinder(quadratic,player[1].getsize(), player[1].getsize(),player[1].getheight(),16,16);
	glRotatef(180, 1, 0, 0);
	gluDisk(quadratic, 0, player[1].getsize(), 16, 16);
	glRotatef(180, 1, 0, 0);
	glTranslatef(0, 0, -player[1].getheight());
	glutSolidSphere(player[1].getsize()*0.5, 16, 16);
	gluCylinder(quadratic, player[1].getsize()*0.5, player[1].getsize()*0.5, player[1].getheight(),16,16);
	glPopMatrix();
}

/*********END DRAWING FUNCTIONS**************/

void drawPlayerScore(){
	std::stringstream ss;
	ss << "Player's Score: " << PlayerScore;
	std::string s = ss.str();
	const char * c = s.c_str();
	char *cstr = new char[s.length() + 1];
	strcpy(cstr, s.c_str());
	drawStrokeText(cstr, SCREEN_WIDTH/10, 9*SCREEN_HEIGHT/10);
	delete[] cstr;
}

void drawAIScore(){
	std::stringstream ss;
	ss << "Computer's Score: " << AIScore;
	std::string s = ss.str();
	const char * c = s.c_str();
	char *cstr = new char[s.length() + 1];
	strcpy(cstr, s.c_str());
	drawStrokeText(cstr, SCREEN_WIDTH/10, 8*SCREEN_HEIGHT/10);
	delete[] cstr;
}
/***********END 2D SCORE***************/

void checkGameOver(){
	if (PlayerScore >= endGameScore){
		glColor3f(0, 1, 0);
		drawStrokeText("PLAYER VICTORY", SCREEN_WIDTH / 2 - SCREEN_WIDTH / 4, SCREEN_HEIGHT / 2);
		gameEnded = true;
		gamePaused = true;
	}
	else if (AIScore >= endGameScore){
		glColor3f(1, 0, 0);
		drawStrokeText("COMPUTER VICTORY", SCREEN_WIDTH / 2 - SCREEN_WIDTH / 3, SCREEN_HEIGHT / 2);
		gameEnded = true;
		gamePaused = true;
	}
}

void updatePaddlePos()
{
	player[0].setdirx(rayPos[0] - player[0].getx());
	//paddleDir[1] = rayPos[1] - paddlePos[1];
	player[0].setdirz(rayPos[2] - player[0].getz());

	player[0].normalize();

	player[0].setspeed( sqrt(pow(player[0].getx() - rayPos[0], 2.0f) + pow(player[0].getz() - rayPos[2], 2.0f))*0.1 ); //increases paddle speed based on how far the mouse is
	if ( player[0].getspeed() > player[0].getsize() + coords[0].getsize() ){
		player[0].setspeed( player[0].getsize() + coords[0].getsize() );
	}
	
	if ( (player[0].getx() + player[0].getdirx() * player[0].getspeed()) <= (bound[0] - player[0].getsize()) && (player[0].getx() + player[0].getdirx() * player[0].getspeed()) >= (bound[2] + player[0].getsize()) ){
		player[0].setx( player[0].getx() + player[0].getdirx() * player[0].getspeed() );
	}
	if ( (player[0].getz() + player[0].getdirz() * player[0].getspeed()) <= (bound[1] - player[0].getsize()) && (player[0].getz() + player[0].getdirz() * player[0].getspeed()) >= (player[0].getsize()) ){
		player[0].setz(player[0].getz() + player[0].getdirz() * player[0].getspeed() );
	}
}

void updateAIPos()
{
	player[1].setdirx( coords[0].getx() - player[1].getx() );
	player[1].setspeed( (player[1].getAIdifficulty()-1)*(player[1].getAIdifficulty()-1)*0.02 );
	//player[1].setspeed(1);
	
	if ( coords[0].getz() >= player[1].getz() ){ // if puck infront of AI
		//Z Direction Tests
		if ( (inter.distance(coords[0], player[1]) < player[1].getAIdifficulty()*10) && (coords[0].getdirz() <= 0) ){
			player[1].setspeed(5);
			player[1].setdirz(coords[0].getdirz()*-1);
			player[1].normalize();
		}// if in range and puck heading towards, then set up for hit
		if ( coords[0].getz() > 0 && coords[0].getspeed() < 0.01 ){
			player[1].setspeed(2);
			player[1].setdirz(1);
			player[1].normalize();
		}// if in range and puck heading towards, then set up for hit
		if ( (inter.distance(coords[0], player[1]) < player[1].getAIdifficulty()*10) && (coords[0].getdirz() > 0) ){
			player[1].setdirz( bound[3]+player[1].getsize() - player[1].getz() );
			player[1].normalize();
		}// if in range and puck moving away, move to origin
		if( (inter.distance(coords[0], player[1]) >= player[1].getAIdifficulty()*10) && (player[1].getz() != bound[3]+player[1].getsize()) ){
			player[1].setdirz( bound[3]+player[1].getsize() - player[1].getz() );
			player[1].normalize();
		}// if not in range and not at origin, move to origin z
		if ( (inter.distance(coords[0], player[1]) >= player[1].getAIdifficulty()*10) && (player[1].getz() == bound[3]+player[1].getsize()) ){
			player[1].setdirz( 0 );
		}
	}
	else{ //if puck behind AI (defends the net)
		if ( player[1].getx() >= 0 ){
			player[1].setdirx( 0 - player[1].getx() );
			player[1].setdirz( bound[3]+player[1].getsize() - player[1].getz() );
		}
		else{
			player[1].setdirx( player[1].getx() );
			player[1].setdirz( bound[3]+player[1].getsize() - player[1].getz() );
			player[1].setspeed( (player[1].getAIdifficulty()-1)*(player[1].getAIdifficulty()-1)*0.002 );
		}
		player[1].normalize();
	}
	//Moving AI
	if ( (player[1].getx() + player[1].getdirx() * player[1].getspeed()) <= (bound[0] - player[1].getsize()) && (player[1].getx() + player[1].getdirx() * player[1].getspeed()) >= (bound[2] + player[1].getsize()) ){
		player[1].setx( player[1].getx() + player[1].getdirx() * player[1].getspeed() );
	}
	if ( (player[1].getz() + player[1].getdirz() * player[1].getspeed()) <= (player[1].getsize()) && (player[1].getx() + player[1].getdirx() * player[1].getspeed()) >= (bound[2] + player[1].getsize()) ){
		player[1].setz( player[1].getz() + player[1].getdirz() * player[1].getspeed() );
	}
}


void collisionTest()
{
	if (player[0].gety() == 0)
	{
		if ( inter.distance(coords[0], player[0]) < (player[0].getsize() + coords[0].getsize() ) )
		{
			coords[0].setdirx(coords[0].getx() - player[0].getx());
			coords[0].setdirz(coords[0].getz() - player[0].getz());
			coords[0].setx( coords[0].getx() + player[0].getdirx()*player[0].getspeed() );
			coords[0].setz( coords[0].getz() + player[0].getdirz()*player[0].getspeed() ); // fixes bug where paddle would push past puck and send it backwards
			coords[0].setspeed( coords[0].getspeed() + player[0].getspeed()*0.1 ); //pucks speed based off how hard it was hit
			coords[0].normalize();
		}
	}
	if (inter.distance(coords[0], player[1]) < (player[1].getsize() + coords[0].getsize()))
	{
		coords[0].setdirx(coords[0].getx() - player[1].getx());
		coords[0].setdirz(coords[0].getz() - player[1].getz());
		coords[0].normalize();
	}
	/******END PADDLE PUCK COLLISION********/

	if ( coords[0].getx() > (bound[0]-coords[0].getsize()) && coords[0].getdirx() > 0 ){
		coords[0].setdirx(coords[0].getdirx()*-1);
	}
	if ( coords[0].getx() < (bound[2]+coords[0].getsize()) && coords[0].getdirx() < 0 ){
		coords[0].setdirx(coords[0].getdirx()*-1);
	} //end x collisions
	
	if ( coords[0].getz() > (bound[1]-coords[0].getsize()) )
	{
		if ( (coords[0].getx() > -25) && (coords[0].getx() < 25) )
		{
			coords[0].giveToPlayer();
			AIScore++;
		}
		else{
			coords[0].setdirz(coords[0].getdirz()*-1);
		}
	}
	if ( coords[0].getz() < (bound[3]+coords[0].getsize()) )
	{
		if ( (coords[0].getx() > -25) && (coords[0].getx() < 25) )
		{
			coords[0].giveToAI();
			PlayerScore++;
		}
		else{
			coords[0].setdirz(coords[0].getdirz()*-1);
		}
		
	}//end z collisions
	/***********END PADDLE TABLE COLLISION*********/
}

/*************MENU MANAGEMENT************/
#include <gl\freeglut.h>
#include <gl\GLU.h>

#define RED 1
#define GREEN 2
#define BLUE 3

#define ONE 4
#define TWO 5
#define THREE 6
#define FOUR 7
#define FIVE 8
#define SIX 9
#define SEVEN 10
#define EIGHT 11
#define NINE 12
#define TEN 13

#define FOG 14
#define SLOW 15

#define SETUP1 16
#define SETUP2 17

#define BIG 18
#define MEDIUM 19
#define SMALL 20

void optionsSelect(int selection){
}

//Player Options Navigation
void playerOptionsSelect(int selection){
}

void playerColourSelect(int selection){
	switch (selection) {
	case RED:
		;
	case GREEN:
		;
	case BLUE:
		;
	}
}

void playerSizeSelect(int selection){
	switch (selection) {
	case BIG:
		player[0].setsize(25);
		break;
	case MEDIUM:
		player[0].setsize(15);
		break;
	case SMALL:
		player[0].setsize(5);
		break;
	}
}
//End Player Options Navigation

//AI Options Navigation
void AIOptionsSelect(int selection){
}

void AIColourSelect(int selection){
	switch (selection) {
	case RED:
		;
	case GREEN:
		;
	case BLUE:
		;
	}
}

void AISizeSelect(int selection){
	switch (selection) {
	case BIG:
		player[1].setsize(25);
		break;
	case MEDIUM:
		player[1].setsize(15);
		break;
	case SMALL:
		player[1].setsize(5);
		break;
	}
}

void AIDifficultySelect(int selection){
	switch (selection){
	case ONE:
		player[1].setAIdifficulty(1);
		break;
	case TWO:
		player[1].setAIdifficulty(2);
		break;
	case THREE:
		player[1].setAIdifficulty(3);
		break;
	case FOUR:
		player[1].setAIdifficulty(4);
		break;
	case FIVE:
		player[1].setAIdifficulty(5);
		break;
	case SIX:
		player[1].setAIdifficulty(6);
		break;
	case SEVEN:
		player[1].setAIdifficulty(7);
		break;
	case EIGHT:
		player[1].setAIdifficulty(8);
		break;
	case NINE:
		player[1].setAIdifficulty(9);
		break;
	case TEN:
		player[1].setAIdifficulty(10);
		break;
	}
}
//End AI Options Navigation

//Game Options Navigation
void gameOptionsSelect(int selection){
}

void gameModeSelect(int selection){
	switch (selection){
	case FOG:
		;
	case SLOW:
		;
	}
}

void puckSizeSelect(int selection){
	switch (selection) {
	case BIG:
		coords[0].setsize(15);
		break;
	case MEDIUM:
		coords[0].setsize(10);
		break;
	case SMALL:
		coords[0].setsize(5);
		break;
	}
}

void stageModeSelect(int selection){
	switch (selection){
	case SETUP1:
		;
	case SETUP2:
		;
	}
}
//End Game Options Navigation

void createMenus(){
	//Player Options Navigation
	int playerSizeMenu = glutCreateMenu(playerSizeSelect);
	glutAddMenuEntry("Big", BIG);
	glutAddMenuEntry("Medium", MEDIUM);
	glutAddMenuEntry("Small", SMALL);

	int playerColourMenu = glutCreateMenu(playerColourSelect);
	glutAddMenuEntry("Red", RED);
	glutAddMenuEntry("Green", GREEN);
	glutAddMenuEntry("Blue", BLUE);

	int playerOptionsMenu = glutCreateMenu(playerOptionsSelect);
	glutAddSubMenu("Colour", playerColourMenu);
	glutAddSubMenu("Paddle Size", playerSizeMenu);

	//End Player Options Navigation

	//AI Options Navigation
	int AIColourMenu = glutCreateMenu(AIColourSelect);
	glutAddMenuEntry("Red", RED);
	glutAddMenuEntry("Green", GREEN);
	glutAddMenuEntry("Blue", BLUE);

	int AIDifficultyMenu = glutCreateMenu(AIDifficultySelect);
	glutAddMenuEntry("1 (Painfully Easy)", ONE);
	glutAddMenuEntry("2 (Infant)",TWO);
	glutAddMenuEntry("3 (Dextrous Infant)", THREE);
	glutAddMenuEntry("4 (My Grandmother)", FOUR);
	glutAddMenuEntry("5 (Passable)", FIVE);
	glutAddMenuEntry("6 (Average)", SIX);
	glutAddMenuEntry("7 (Catlike)", SEVEN);
	glutAddMenuEntry("8 (Spiderman)", EIGHT);
	glutAddMenuEntry("9 (Time Traveller)", NINE);
	glutAddMenuEntry("10 (Rob Tether)", TEN);

	int AISizeMenu = glutCreateMenu(AISizeSelect);
	glutAddMenuEntry("Big", BIG);
	glutAddMenuEntry("Medium", MEDIUM);
	glutAddMenuEntry("Small", SMALL);

	int AIOptionsMenu = glutCreateMenu(AIOptionsSelect);
	glutAddSubMenu("Colour", AIColourMenu);
	glutAddSubMenu("Difficulty", AIDifficultyMenu);
	glutAddSubMenu("Paddle Size", AISizeMenu);
	//End AI Options Navigation

	//Game Options Navigation
	int stageMenu = glutCreateMenu(stageModeSelect);
	glutAddMenuEntry("Stage 1", SETUP1);
	glutAddMenuEntry("Stage 2", SETUP2);

	int gameModeMenu = glutCreateMenu(gameModeSelect);
	glutAddMenuEntry("Fog", FOG);
	glutAddMenuEntry("Slow Motion", SLOW);

	int puckSizeMenu = glutCreateMenu(puckSizeSelect);
	glutAddMenuEntry("Big", BIG);
	glutAddMenuEntry("Medium", MEDIUM);
	glutAddMenuEntry("Small", SMALL);

	int gameOptionsMenu = glutCreateMenu(gameOptionsSelect);
	glutAddSubMenu("Game Modes", gameModeMenu);
	glutAddSubMenu("Stage Configuration", stageMenu);
	glutAddSubMenu("Puck Size", puckSizeMenu);
	//End Game Options Navigation

	int selectOptionsMenu = glutCreateMenu(optionsSelect);
	glutAddSubMenu("Player Options", playerOptionsMenu);
	glutAddSubMenu("AI Options", AIOptionsMenu);
	glutAddSubMenu("Game Options", gameOptionsMenu);

	glutAttachMenu(GLUT_RIGHT_BUTTON);

}
/*************END MENU MANAGEMENT************/

void display(void)
{
	if(gameStartup){
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glDisable(GL_LIGHTING);
		drawStartupMenu();
	}
	if (gamePaused == false && gameStartup == false){
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1,1,1);
	drawPlayerScore();
	drawAIScore();
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHT1);
	//glEnable(GL_LIGHT2);

	coords[0].setspeed( coords[0].getspeed() - 0.002*coords[0].getspeed()*coords[0].getspeed() ); //slipperiness of the puck
	updatePaddlePos();
	coords[0].move();
	updateAIPos();
	collisionTest();

	glLightfv(GL_LIGHT0, GL_POSITION, light_pos0);
	glLightfv(GL_LIGHT0, GL_AMBIENT, amb0);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diff0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, spec0);
	
	glLightfv(GL_LIGHT1, GL_POSITION, light_pos1);
	glLightfv(GL_LIGHT1, GL_AMBIENT, amb0);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diff0);
	glLightfv(GL_LIGHT1, GL_SPECULAR, spec0);

	//glLightfv(GL_LIGHT2, GL_POSITION, light_pos2);
	//glLightfv(GL_LIGHT2, GL_AMBIENT, amb0);
	//glLightfv(GL_LIGHT2, GL_DIFFUSE, diff0);
	//glLightfv(GL_LIGHT2, GL_SPECULAR, spec0);
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, table_amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, table_diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, table_spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, table_shiny);
	glPushMatrix();
	drawTable();
	glPopMatrix();
	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, border_amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, border_diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, border_spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, border_shiny);
	glPushMatrix();
	drawBorders();
	glPopMatrix();

	
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, puck_amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, puck_diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, puck_spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, puck_shiny);
	glPushMatrix();
	drawNet();
	glPopMatrix();

	glPushMatrix();
	glTranslatef(coords[0].getx(), 0, coords[0].getz());
	drawPuck();
	glPopMatrix();


	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, player_amb);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, player_diff);
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, player_spec);
	glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, player_shiny);
	glPushMatrix();
	glTranslatef(player[0].getx(), player[0].gety(), player[0].getz());
	drawPlayerPaddle();
	glPopMatrix();
	glPushMatrix();
	glTranslatef(player[1].getx(), 0, player[1].getz());
	drawAIPaddle();
	glPopMatrix();

	glDisable(GL_LIGHTING);
	checkGameOver();
	}
	glutSwapBuffers();
	
}


void Get3DPos(int x, int y, float winz, GLdouble point[3])
{
	GLint viewport[4];
	GLdouble modelview[16];
	GLdouble projection[16];

	//get the matrices
	glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
	glGetDoublev(GL_PROJECTION_MATRIX, projection);
	glGetIntegerv(GL_VIEWPORT, viewport);

	//"un-project" the 2D point giving the 3D position corresponding to the provided depth (winz)
	gluUnProject((float)x, (float)(viewport[3] - y), winz, modelview, projection, viewport, &point[0], &point[1], &point[2]);
}

void rayCast(int x, int y)
{
	GLdouble pNear[3];
	GLdouble pFar[3]; //declare the two points

	//get 3D position of mouse cursor on near and far clipping planes
	Get3DPos(x, y, 0.0, pNear);
	Get3DPos(x, y, 1.0, pFar);

	//create a ray originating at the camera position, and using the vector between the two points for the direction
	
	rayOrg[0] = camPos[0];
	rayOrg[1] = camPos[1];
	rayOrg[2] = camPos[2];

	//ray direction is the vector (pFar - pNear)
	rayDir[0] = pFar[0] - pNear[0];
	rayDir[1] = pFar[1] - pNear[1];
	rayDir[2] = pFar[2] - pNear[2];


	float normFactor = sqrt((rayDir[0] * rayDir[0]) + (rayDir[1] * rayDir[1]) + (rayDir[2] * rayDir[2]));
	rayDir[0] = rayDir[0]/normFactor;
	rayDir[1] = rayDir[1]/normFactor;
	rayDir[2] = rayDir[2]/normFactor;

	//update the position of the object to the intersection point
	float lengthFactorNumerator = -(playPlaneNorm[0] * rayOrg[0] + playPlaneNorm[1] * rayOrg[1] + playPlaneNorm[2] * rayOrg[2]);
	float lengthFactorDenominator =playPlaneNorm[0] * rayDir[0] + playPlaneNorm[1] * rayDir[1] + playPlaneNorm[2] * rayDir[2];
	float lengthFactor = lengthFactorNumerator / lengthFactorDenominator;
	rayPos[0] = rayOrg[0] + rayDir[0] * lengthFactor;
	rayPos[2] = rayOrg[2] + rayDir[2] * lengthFactor;
}

/**************************END RAYCASTING************************/


void keyboard(unsigned char key, int x, int y)
{
	if (key == 'p' || key == 'P'){
		gamePaused = !gamePaused;
	}
	if (key == 's' && gameStartup && selectedMenuOption == 0){
		gameStartup = false;
	}
	//if (key == 's' && gameStartup && selectedMenuOption == 1){
	//	gameOptions = true;
		//gameOptionsMenu();
	//}
	//if (key == 's' && gameStartup && selectedMenuOption == 2){
	//	developerCredits = true;
	//}
}

void keyboardother(int key, int x, int y)
{
	y = 500 - y;
	int i;
	if (key == GLUT_KEY_UP && gameStartup)
	{
		selectedMenuOption--;
		if (selectedMenuOption < 0){
			selectedMenuOption = 2;
		}
	}
	if (key == GLUT_KEY_DOWN && gameStartup)
	{
		selectedMenuOption++;
		if (selectedMenuOption > 2){
			selectedMenuOption = 0;
		}
	}
	if (key == GLUT_KEY_LEFT)
	{
	}
	if (key == GLUT_KEY_RIGHT)
	{
	}
	display();
}

void motion(int x, int y)
{
	rayCast(x,y);
}

void mouse(int click, int state, int x, int y)
{
		//rayCast to the playing plane on click 
	if (click == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		player[0].sety(30);
		rayCast(x,y);
	}
	if (click == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		rayCast(x,y);
		if (inter.distance(coords[0], player[0]) < (player[0].getsize() + coords[0].getsize()) ){
			coords[0].setspeed( 0 );
			player[0].sety( coords[0].getheight() );
		}
		else {
			player[0].sety(0);
		}

	}
}

void idle()
{
	display();
}

/**********************************END GLUT FUNCTIONS*******************************/


int main(int argc, char** argv){
	
	quadratic = gluNewQuadric();
	coords.resize(1);
	player.resize(2);
	coords[0] = puck(0, 0, 0);
	player[0] = paddle(0, 0, 100);
	player[1] = paddle(0, 0, -100);
	
	std::cout << "Mouse: Move Paddle";
	std::cout << "Right Click: Options\n";
	std::cout << "p: Pause game\n";
	std::cout << "Left Click: Lift Paddle (you can catch the puck)\n";
	std::cout << "q: Quit\n";
	std::cout << "The Pucks speed depends on how hard it is hit and will slow over time\n";
	std::cout << "Game is to 7 points\n";
	
	glutInit(&argc, argv);		//initialize glut
	glutInitDisplayMode (GLUT_DOUBLE);
	glutInitWindowSize(SCREEN_WIDTH, SCREEN_HEIGHT);
	glutCreateWindow("Air Hockey");	//creates 500x500 window
	createMenus();
	glEnable(GL_DEPTH_TEST);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, 1, 1, 500);

	glMatrixMode(GL_MODELVIEW);
	gluLookAt(camPos[0], camPos[1], camPos[2], 0, 0, 0, 0, 1, 0);

	glutMouseFunc(mouse);
	glutMotionFunc(motion);
	glutPassiveMotionFunc(motion);
	glutKeyboardFunc(keyboard);
	glutSpecialFunc(keyboardother);
	glutDisplayFunc(display);
	glutIdleFunc(idle);

	glutMainLoop();				//starts the event loop

	return(0);
}
