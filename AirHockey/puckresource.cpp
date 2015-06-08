#include "puckresource.h"
#include <math.h>

puck::puck(){}

puck::puck(float inx, float iny, float inz){
	x = inx;
	y = iny;
	z = inz;

	dirx = 0;
	dirz = 0;
	
	speed = 0;

	size = 10;
	height = 2;
}

float puck::getx(){
	return x;
}

float puck::getz(){
	return z;
}

float puck::getdirx(){
	return dirx;
}

float puck::getdirz(){
	return dirz;
}

float puck::getspeed(){
	return speed;
}

float puck::getsize(){
	return size;
}

float puck::getheight(){
	return height;
}

float puck::getmag(){
	return sqrt(pow(dirx, 2.0f) + pow(dirz, 2.0f));
}

void puck::move(){
	setx(x + dirx*speed);
	setz(z + dirz*speed);
}

void puck::normalize(){
	setdirx(dirx/getmag());
	setdirz(dirz/getmag());
}

void puck::giveToAI(){
	setx(0);
	setz(-20);
	setdirx(0);
	setdirz(-1);
	setspeed(0.1);
}

void puck::giveToPlayer(){
	setx(0);
	setz(20);
	setdirx(0);
	setdirz(0);
	setspeed(0);
}


paddle::paddle(){}

paddle::paddle(float inx, float iny, float inz){
	x = inx;
	y = iny;
	z = inz;

	dirx = 0;
	diry = 0;
	dirz = 0;

	speed = 0;

	red = 0;
	green = 0.4;
	blue = 0;

	size = 15;
	height = 5;

	AIdifficulty = 4;
}

float paddle::getx(){
	return x;
}

float paddle::gety(){
	return y;
}

float paddle::getz(){
	return z;
}

float paddle::getdirx(){
	return dirx;
}

float paddle::getdiry(){
	return diry;
}

float paddle::getdirz(){
	return dirz;
}

float paddle::getred(){
	return red;
}

float paddle::getgreen(){
	return red;
}

float paddle::getblue(){
	return red;
}

float paddle::getspeed(){
	return speed;
}

float paddle::getsize(){
	return size;
}

float paddle::getheight(){
	return height;
}

float paddle::getAIdifficulty(){
	return AIdifficulty;
}

float paddle::getmag(){
	return sqrt(pow(dirx, 2.0f) + pow(dirz, 2.0f));
}

void paddle::normalize(){
	setdirx(dirx/getmag());
	setdirz(dirz/getmag());
}


_interaction::_interaction(){
}

float _interaction::distance(puck object, paddle player){
	return sqrt(pow(player.getx() - object.getx(), 2.0f) + pow(player.getz() - object.getz(), 2.0f));
}



