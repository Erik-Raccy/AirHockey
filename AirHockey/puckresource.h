#ifndef _H_puckresource_
#define _H_puckresource_

class puck{
public:
	puck();
	puck(float inx, float iny, float inz);
	~puck(){}
	
	float x;
	float y;
	float z;
	
	float getx();
	float getz();

	void setx(float inx){x = inx;}
	void setz(float inz){z = inz;}
	

	float dirx;
	float dirz;

	float getdirx();
	float getdirz();

	void setdirx(float indirx){dirx = indirx;}
	void setdirz(float indirz){dirz = indirz;}
	
	float speed;
	float getspeed();
	void setspeed(float inspeed){speed = inspeed;}

	float size;
	float getsize();
	void setsize(float insize){size = insize;}
	
	float height;
	float getheight();
	void setheight(float inheight){height = inheight;}

	void move();

	float getmag();
	void normalize();

	void giveToAI();
	void giveToPlayer();
};

class paddle{
public:
	paddle();
	paddle(float x, float y, float z);
	~paddle(){}

	float x;
	float y;
	float z;

	float getx();
	float gety();
	float getz();

	void setx(float inx){x = inx;}
	void sety(float iny){y = iny;}
	void setz(float inz){z = inz;}
	
	float dirx;
	float diry;
	float dirz;

	float getdirx();
	float getdiry();
	float getdirz();

	void setdirx(float indirx){dirx = indirx;}
	void setdiry(float indiry){diry = indiry;}
	void setdirz(float indirz){dirz = indirz;}

	float speed;
	float getspeed();
	void setspeed(float inspeed){speed = inspeed;}

	float red;
	float green;
	float blue;
	float getred();
	float getgreen();
	float getblue();
	void setred(float inred){red = inred;}
	void setgreen(float ingreen){green = ingreen;}
	void setblue(float inblue){blue = inblue;}

	float size;
	float getsize();
	void setsize(float insize){size = insize;}

	float height;
	float getheight();
	void setheight(float inheight){height = inheight;}

	float AIdifficulty;
	float getAIdifficulty();
	void setAIdifficulty(float inAIdifficulty){AIdifficulty = inAIdifficulty;};
	
	float getmag();
	void normalize();
};

static class _interaction{
public:
	_interaction();
	float distance(puck object, paddle player);
};


#endif