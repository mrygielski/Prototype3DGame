//////////////////////////////////////////////////////////////
// GOLDEN GATE ENGINE
// ------------------
// Features:
// - texture mapping
// - mg³a
// - ró¿ny kszta³t i wysokoœæ bloków
// - s³oñce
// - sufit i pod³oga
// - mo¿liwoœæ u¿ycia czcionek TTF (True Type Font)
// - ognie
// - œnieg i deszcz
// - textury w formacie BMP lub TGA (kompresja)
// - drzwi
// - drzewa
//
//
//////////////////////////////////////////////////////////////

#include <windows.h>
#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <gl\gl.h>
#include <gl\glu.h>
#include <gl\glaux.h>
#include <gl\glut.h>


#include "helper.h"
#include "jpeglib.h"
#include "jpg.h"
#include "ggate.h"



#pragma comment( lib, "winmm.lib" )
#pragma comment(lib, "jpeg.lib")



struct blok
{
	float x1,x2,z1,z2,y,xx1,zz1;
	int text1,text2,text3,text4,text5;
	bool door,open,close;
	int oindex,cindex;
};

struct _xyz_rec
{
	float x,y,z;
};

struct _bombelki
{
	float max_wys;
	unsigned char r,g,b,a;
	int ilosc;
	_xyz_rec bom[101];
};

struct snieg
{
	float x,y,z,speed;
};

struct drzewo
{
	float x,z,wysokosc;
};

struct _tree
{
	GLUquadricObj *pien, *korona1,*korona2,*korona3;
	int typ;
	float x,z;
	float h_pien,sz_pien,h_kor,sz_kor;
	int text_p,text_k;
	int quality;
};

// STA£E
float slonce[4]={0,1.25f,-20,5.0f};
int kolor_podlogi[4]={255,255,255,255};
int kolor_sufitu[4]={255,255,255,255};
const float wysokosc_nieba=3.5f;
const float max_zb = 0.2f;
const float max_zb_tree = 0.6f;
const float szybkosc_chodzenia = 0.1f;
const float szybkosc_obrotu = 0.5f;
const int mapsize = 10;
bool noclip=false;
const float RAD = 0.017453292519943295769236907684886f;
const float door_speed=0.010f;


font_ttf font1,font2;

float mgla=0.04f;

// TEXTURY
GLuint texture[50];
GLuint texture2[50];

// CHODZENIE
float xpos,zpos,heading,yrot,krok,kr;
float xtrans,ytrans,ztrans;
float tempx,tempy,syx,syy;

// KLOCKI
const int ilosc=700;
blok sciany[ilosc+1];
int scian_rys;

// ŒNIEG
const int ilosc_sniegu = 200;
snieg snow[ilosc_sniegu+1];
int opady=0;

// DODATKOWE ZMIENNE
int los;
int i=0,j=0,i2,j2,_11,_12;
float ii,jj;

// DRZWI
int opening[11],closing[11],toopen=0,toclose=0;

drzewo tree[3];

_bombelki bom;

// DRZEWA
const int ilosc_drzew=2;
_tree drzewa[ilosc_drzew];

// CHMURY
float kick; 


void RysujBloczek(int numer);
void Draw_Floor();
void Rysuj_Slonce();
_bombelki Rysuj_bombelki(_bombelki ff);
void Rysuj_Snieg(int nr);
void losowanie_sniegu();
void Draw_Snow();
void open_door(int nr);
int w_srodku(float x,float y);
void Draw_Gun();
void Draw_Tree(int nr);


void Draw_Clouds()
{
	glEnable(GL_FOG);

	glRotatef(kick+=0.008f,0,1,0);
	for (i=-6;i<=mapsize+4/2<<1;i++)
	for (j=-6;j<=mapsize+4/2<<1;j++)
	{
		ii=(i<<1)-(float)mapsize;
		jj=(j<<1)-(float)mapsize;
		glPolygonMode( GL_BACK, GL_POINT );
		glPolygonMode( GL_FRONT, GL_FILL );
		BarT((float)ii,wysokosc_nieba,(float)jj-2,
			 (float)ii,wysokosc_nieba,(float)jj,
			 (float)ii+2,wysokosc_nieba,(float)jj,
			 (float)ii+2,wysokosc_nieba,(float)jj-2,kolor_sufitu[0],kolor_sufitu[1],kolor_sufitu[2],kolor_sufitu[3],texture[12]);
	}
	glPolygonMode( GL_BACK, GL_FILL );
	glDisable(GL_FOG);
}

void Draw_Clouds2()
{
	glEnable(GL_FOG);

	//przesówanie chmurek 
	// begin
	glTranslatef(kick+=0.005f,0,0);
	if (kick>=4.f) kick=0;
	// end

	for (i=-1;i<=1+mapsize/4<<1;i++)
	for (j=0;j<=(1+mapsize/4<<1);j++)
	{
 		ii=(i<<2)-(float)mapsize-5;
		jj=(j<<2)-(float)mapsize-2;
		glPolygonMode( GL_BACK, GL_POINT );
		glPolygonMode( GL_FRONT, GL_FILL );
		BarT((float)ii,wysokosc_nieba,(float)jj-4,
			 (float)ii,wysokosc_nieba,(float)jj,
			 (float)ii+4,wysokosc_nieba,(float)jj,
			 (float)ii+4,wysokosc_nieba,(float)jj-4,kolor_sufitu[0],kolor_sufitu[1],kolor_sufitu[2],kolor_sufitu[3],texture[12]);
	}

/*	for (i=-3;i<=mapsize/2<<1;i++)
	for (j=-2;j<=mapsize/2<<1;j++)
	{
		ii=(i<<1)-(float)mapsize;
		jj=(j<<1)-(float)mapsize;
		glPolygonMode( GL_BACK, GL_POINT );
		glPolygonMode( GL_FRONT, GL_FILL );
		BarT((float)ii,wysokosc_nieba,(float)jj-2,
			 (float)ii,wysokosc_nieba,(float)jj,
			 (float)ii+2,wysokosc_nieba,(float)jj,
			 (float)ii+2,wysokosc_nieba,(float)jj-2,kolor_sufitu[0],kolor_sufitu[1],kolor_sufitu[2],kolor_sufitu[3],texture[12]);
	}*/

	glPolygonMode( GL_BACK, GL_FILL );
	glDisable(GL_FOG);
}


float xrot = 0;

void DrawGLScene(GLvoid)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	GLfloat xtrans = -xpos;
	GLfloat ztrans = -zpos;
	glRotatef(75.0f - xrot,1,0,0);
	glLoadIdentity();
	glRotatef(360.0f - yrot,0,1,0);
	glTranslatef(xtrans, ytrans, ztrans);

	for (i=0;i<=scian_rys;i++) RysujBloczek(i);

	Draw_Floor();
	glPushMatrix();
	Draw_Clouds2();
	glPopMatrix();

	bom=Rysuj_bombelki(bom);


	Draw_Snow();

	Draw_Tree(0);
	Draw_Tree(1);

	
		

	/*glLoadIdentity();
	glTranslatef(-13.6f,10,-25);
	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	pisz_tekst(0,0,0.5f,0,texture[0],"Ilosc scian: %d",scian_rys);
	pisz_tekst(0,-0.5f,0.5f,0,texture[0],"Noclip: %d",noclip);
	pisz_tekst(0,-1.0f,0.5f,0,texture[0],"topen index: %d",toopen);
	pisz_tekst(0,-1.5f,0.5f,0,texture[0],"toclose index: %d",toclose);
	pisz_tekst(0,-2.0f,0.5f,0,texture[0],"Player X: %.1f",xpos);
	pisz_tekst(0,-2.5f,0.5f,0,texture[0],"Player Y: %.1f",zpos);
	pisz_tekst(0,-3.0f,0.5f,0,texture[0],"%d %d %d",LBM,MBM,RBM);
	pisz_tekst(0,-3.5f,0.5f,0,texture[0],"%d %d",mouseX,mouseY);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);*/

	glLoadIdentity();
	glTranslatef(0,0.13f+(ytrans /2),-2);
	Draw_Gun();


 
}

void Draw_Tree(int nr)
{
	
	if (drzewa[nr].typ==1)
	{
		SetTextures(true);
		glBindTexture(GL_TEXTURE_2D,texture[drzewa[nr].text_p]);
		glRotatef(90,1,0,0);
		glTranslatef(drzewa[nr].x,drzewa[nr].z,0.5f-drzewa[nr].h_pien);
		gluCylinder(drzewa[nr].pien,drzewa[nr].sz_pien, drzewa[nr].sz_pien, drzewa[nr].h_pien, drzewa[nr].quality, drzewa[nr].quality); 
		glBindTexture(GL_TEXTURE_2D,texture[drzewa[nr].text_k]);
		glTranslatef(0,0,-drzewa[nr].h_kor);
		gluCylinder(drzewa[nr].korona1,0.0, drzewa[0].sz_kor, drzewa[nr].h_kor, drzewa[nr].quality, drzewa[nr].quality);
		GLfloat xtrans = -xpos;
		GLfloat ztrans = -zpos;
		glLoadIdentity();
		glRotatef(360.0f - yrot,0,1,0);
		glTranslatef(xtrans, ytrans, ztrans);
	}
	if (drzewa[nr].typ==2)
	{
		SetTextures(true);
		glBindTexture(GL_TEXTURE_2D,texture[drzewa[nr].text_p]);
		glRotatef(90,1,0,0);
		glTranslatef(drzewa[nr].x,drzewa[nr].z,0.5f-drzewa[nr].h_pien);
		gluCylinder(drzewa[nr].pien,drzewa[nr].sz_pien, drzewa[nr].sz_pien, drzewa[nr].h_pien, drzewa[nr].quality, drzewa[nr].quality); 
		glBindTexture(GL_TEXTURE_2D,texture[drzewa[nr].text_k]);
		glTranslatef(0,0,-drzewa[nr].h_kor/2);
		gluCylinder(drzewa[nr].korona1,0.0, drzewa[0].sz_kor, drzewa[nr].h_kor/2, drzewa[nr].quality, drzewa[nr].quality);
		glTranslatef(0,0,-drzewa[nr].h_kor/4);
		gluCylinder(drzewa[nr].korona2,0.0, drzewa[0].sz_kor*0.8f, drzewa[nr].h_kor/2, drzewa[nr].quality, drzewa[nr].quality);
		glTranslatef(0,0,-drzewa[nr].h_kor/4);
		gluCylinder(drzewa[nr].korona3,0.0, drzewa[0].sz_kor*0.6f, drzewa[nr].h_kor/2, drzewa[nr].quality, drzewa[nr].quality);
		GLfloat xtrans = -xpos;
		GLfloat ztrans = -zpos;
		glLoadIdentity();
		glRotatef(360.0f - yrot,0,1,0);
		glTranslatef(xtrans, ytrans, ztrans);
	}
	if (drzewa[nr].typ==3)
	{
		SetTextures(true);
		glBindTexture(GL_TEXTURE_2D,texture[drzewa[nr].text_p]);
		glRotatef(90,1,0,0);
		glTranslatef(drzewa[nr].x,drzewa[nr].z,0.5f-drzewa[nr].h_pien);
		gluCylinder(drzewa[nr].pien,drzewa[nr].sz_pien, drzewa[nr].sz_pien, drzewa[nr].h_pien, drzewa[nr].quality, drzewa[nr].quality); 
		glBindTexture(GL_TEXTURE_2D,texture[drzewa[nr].text_k]);
		glTranslatef(0,0,-drzewa[nr].h_kor+0.025f);
		gluSphere(drzewa[nr].korona1, drzewa[nr].h_kor, drzewa[nr].quality, drzewa[nr].quality);
		GLfloat xtrans = -xpos;
		GLfloat ztrans = -zpos;
		glLoadIdentity();
		glRotatef(360.0f - yrot,0,1,0);
		glTranslatef(xtrans, ytrans, ztrans);
	}
	glDisable(GL_FOG);
}

void Draw_Gun()
{
	glBlendFunc(GL_DST_COLOR,0);
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_FOG);
	BarT(1,-0.3f,0, 
		 1,-1,0, 
		 0,-1,0,
		 0,-0.3f,0, 255,255,255,255,texture2[8]);
	glBlendFunc(GL_SRC_ALPHA,1);
	BarT(1,-0.3f,0, 
		 1,-1,0, 
		 0,-1,0,
		 0,-0.3f,0, 255,255,255,255,texture[8]);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}


void Draw_Snow()
{
	for (i=1;i<=ilosc_sniegu;i++)
	{
		Rysuj_Snieg(i);
		snow[i].y=snow[i].y-snow[i].speed;
		if (snow[i].y<-0.7f)
		{
			snow[i].y=3.5f;
		}
	}
}

void Rysuj_Snieg(int nr)
{
	glEnable(GL_BLEND);
    glEnable(GL_FOG);
	BarT(snow[nr].x+0.05f,snow[nr].y,snow[nr].z,
		 snow[nr].x+0.05f,snow[nr].y+0.05f,
		 snow[nr].z,snow[nr].x,snow[nr].y+0.05f,
		 snow[nr].z,snow[nr].x,snow[nr].y,snow[nr].z,255,255,255,255,texture[5]);
	BarT(snow[nr].x+0.025f,snow[nr].y,snow[nr].z+0.025f,
		 snow[nr].x+0.025f,snow[nr].y+0.05f,snow[nr].z+0.025f,
		 snow[nr].x+0.025f,snow[nr].y+0.05f,snow[nr].z-0.025f,
		 snow[nr].x+0.025f,snow[nr].y,snow[nr].z-0.025f,255,255,255,255,texture[5]);
	glDisable(GL_FOG);
	glDisable(GL_BLEND);
}




void Rysuj_Slonce()
{
	glLoadIdentity();
	glRotatef(360.0f - yrot,0,1,0);
	glEnable(GL_BLEND);
	BarT(slonce[0]+slonce[3],slonce[1]-slonce[3],slonce[2],
		 slonce[0]+slonce[3],slonce[1]+slonce[3],slonce[2],
		 slonce[0]-slonce[3],slonce[1]+slonce[3],slonce[2],
		 slonce[0]-slonce[3],slonce[1]-slonce[3],slonce[2],255,255,255,255,texture[4]);
	glDisable(GL_BLEND);
}

void Draw_Floor()
{
	glEnable(GL_FOG);
	for (i=-2;i<=mapsize/2<<1;i++)
	for (j=-2;j<=mapsize/2<<1;j++)
	{
		ii=(i<<1)-(float)mapsize;
		jj=(j<<1)-(float)mapsize;
		glPolygonMode( GL_BACK, GL_FILL );
		glPolygonMode( GL_FRONT, GL_POINT );
		BarT((float)ii,-0.5f,(float)jj-2,
			 (float)ii,-0.5f,(float)jj,
			 (float)ii+2,-0.5f,(float)jj,
			 (float)ii+2,-0.5f,(float)jj-2,kolor_podlogi[0],kolor_podlogi[1],kolor_podlogi[2],kolor_podlogi[3],texture[2]);
		glPolygonMode( GL_BACK, GL_POINT );
		glPolygonMode( GL_FRONT, GL_FILL );
		/*BarT((float)ii,wysokosc_nieba,(float)jj-2,
			 (float)ii,wysokosc_nieba,(float)jj,
			 (float)ii+2,wysokosc_nieba,(float)jj,
			 (float)ii+2,wysokosc_nieba,(float)jj-2,kolor_sufitu[0],kolor_sufitu[1],kolor_sufitu[2],kolor_sufitu[3],texture[3]);*/
	}
	glPolygonMode( GL_BACK, GL_FILL );
	glDisable(GL_FOG);
}



void RysujBloczek(int numer)
{
	glEnable(GL_FOG);
	glColor3ub(255,255,255);
	glPolygonMode( GL_FRONT, GL_POINT );
	glPolygonMode( GL_BACK, GL_FILL );
	if (sciany[numer].text1!=-1)
		BarT(sciany[numer].x1,-0.5f+sciany[numer].y,sciany[numer].z1,sciany[numer].x1,-0.5f,sciany[numer].z1,sciany[numer].x2,-0.5f,sciany[numer].z1,sciany[numer].x2,-0.5f+sciany[numer].y,sciany[numer].z1,255,255,255,255,texture[sciany[numer].text1]);
	glPolygonMode( GL_FRONT, GL_FILL );
	glPolygonMode( GL_BACK, GL_POINT );
	if (sciany[numer].text2!=-1)
		BarT(sciany[numer].x1,-0.5f+sciany[numer].y,sciany[numer].z2,sciany[numer].x1,-0.5f,sciany[numer].z2,sciany[numer].x2,-0.5f,sciany[numer].z2,sciany[numer].x2,-0.5f+sciany[numer].y,sciany[numer].z2,255,255,255,255,texture[sciany[numer].text2]);
	glPolygonMode( GL_FRONT, GL_FILL );
	glPolygonMode( GL_BACK, GL_POINT );
	if (sciany[numer].text3!=-1)
		BarT(sciany[numer].x1,-0.5f+sciany[numer].y,sciany[numer].z1,sciany[numer].x1,-0.5f,sciany[numer].z1,sciany[numer].x1,-0.5f,sciany[numer].z2,sciany[numer].x1,-0.5f+sciany[numer].y,sciany[numer].z2,255,255,255,255,texture[sciany[numer].text3]);
	glPolygonMode( GL_FRONT, GL_POINT );
	glPolygonMode( GL_BACK, GL_FILL );
	if (sciany[numer].text4!=-1)
		BarT(sciany[numer].x2,-0.5f+sciany[numer].y,sciany[numer].z1,sciany[numer].x2,-0.5f,sciany[numer].z1,sciany[numer].x2,-0.5f,sciany[numer].z2,sciany[numer].x2,-0.5f+sciany[numer].y,sciany[numer].z2,255,255,255,255,texture[sciany[numer].text4]);
	glPolygonMode( GL_FRONT, GL_POINT );
	glPolygonMode( GL_BACK, GL_FILL );
	if ((sciany[numer].text5!=-1)&&(sciany[numer].y<0.5f))
		BarT(sciany[numer].x1,-0.5f+sciany[numer].y,sciany[numer].z1,sciany[numer].x2,-0.5f+sciany[numer].y,sciany[numer].z1,sciany[numer].x2,-0.5f+sciany[numer].y,sciany[numer].z2,sciany[numer].x1,-0.5f+sciany[numer].y,sciany[numer].z2,255,255,255,255,texture[sciany[numer].text5]);
	glDisable(GL_FOG);
}


void losowanie_sniegu()
{
	snow[i].x=(Random(0,260)/10.0f)-14.0f;
	snow[i].y=wysokosc_nieba+(Random(1,10)/10.0f);
	snow[i].z=(Random(0,260)/10.0f)-16.0f;
	los=rand()%5;
	if (opady==1)
	{
		if (los==0) snow[i].speed=0.012f;
		if (los==1) snow[i].speed=0.013f;
		if (los==2) snow[i].speed=0.014f;
		if (los==3) snow[i].speed=0.011f;
		if (los==4) snow[i].speed=0.015f;
	}
	if (opady==2)
	{
		if (los==0) snow[i].speed=0.052f;
		if (los==1) snow[i].speed=0.053f;
		if (los==2) snow[i].speed=0.054f;
		if (los==3) snow[i].speed=0.051f;
		if (los==4) snow[i].speed=0.056f;
	}
	bool jest=false;
	for (j=0;j<=scian_rys;j++)
	{
		do
		{
			jest=false;
			if ((snow[i].x>=sciany[j].x1-max_zb)&&(snow[i].x<=sciany[j].x2+max_zb)&&
				(snow[i].z>=sciany[j].z2-max_zb)&&(snow[i].z<=sciany[j].z1+max_zb)&&
				(sciany[j].y==wysokosc_nieba))
			{
				jest=true;
				snow[i].x=(Random(0,260)/10.0f)-14.0f;
				snow[i].z=(Random(0,260)/10.0f)-16.0f;
			}
		}
		while (jest!=false);
	}
}




_bombelki Rysuj_bombelki(_bombelki ff)
{
	_bombelki f=ff;
	glEnable(GL_FOG);
	glEnable(GL_BLEND);
	for (i=0;i<=f.ilosc;i++)
	{
		f.bom[i].y+=Random(1,5)/300.0f;
		if (f.bom[i].y>f.max_wys) f.bom[i].y=-0.45f;
		BarT(f.bom[i].x+0.05f,f.bom[i].y-0.05f,f.bom[i].z,f.bom[i].x,f.bom[i].y-0.05f,f.bom[i].z,f.bom[i].x,f.bom[i].y,f.bom[i].z,f.bom[i].x+0.05f,f.bom[i].y,f.bom[i].z,bom.r,bom.g,bom.b,bom.a,texture[4]);
		BarT(f.bom[i].x+0.025f,f.bom[i].y-0.05f,f.bom[i].z+0.025f,f.bom[i].x+0.025f,f.bom[i].y-0.05f,f.bom[i].z-0.025f,f.bom[i].x+0.025f,f.bom[i].y,f.bom[i].z-0.025f,f.bom[i].x+0.025f,f.bom[i].y,f.bom[i].z+0.025f,bom.r,bom.g,bom.b,bom.a,texture[4]);
	}

	glDisable(GL_FOG);
	glDisable(GL_BLEND);
	return f;
}

_bombelki Losuj_bombelki(float x,float y,float z,int ile,int radius)
{
	_bombelki f;
	f.ilosc=ile;
	if (f.ilosc>100) f.ilosc=100;
	randomize();
	int los;
	for (i=0;i<=f.ilosc;i++)
	{
		los=Random(0,360);
		f.bom[i].x=x+sin(los/RAD)*(Random(1,radius*10)/50.0f);
		f.bom[i].y=y-(Random(1,(wysokosc_nieba+0.5f)*10)/(wysokosc_nieba+0.5f));
		f.bom[i].z=z+cos(los/RAD)*(Random(1,radius*10)/50.0f);
	}
	f.max_wys=wysokosc_nieba;
	f.r=255;
	f.g=255;
	f.b=255;
	f.a=255;
	return f;
}

void Forward(float szybkosc_chodzenia)
{
	tempx=syx;
	tempy=syy;
	syx-=((float)sin(heading*RAD) * szybkosc_chodzenia);
	syy-=((float)cos(heading*RAD) * szybkosc_chodzenia);
	if (w_srodku(syx,syy)!=1)
	{
		xpos = syx;
		zpos = syy;
		if (kr==0)
		{
			krok+=0.25f;
			ytrans=sin(krok)/25;
			kr=1;
		}
	}
	else
	{
		syx=tempx;
		syy=tempy;
	}
}

void Backward(float szybkosc_chodzenia)
{
	tempx=syx;
	tempy=syy;
	syx+=((float)sin(heading*RAD) * szybkosc_chodzenia);
	syy+=((float)cos(heading*RAD) * szybkosc_chodzenia);
	if (w_srodku(syx,syy)!=1)
	{
		xpos = syx;
		zpos = syy;
		if (kr==0)
		{
			krok-=0.25f;
			ytrans=sin(krok)/25.0f;
			kr=1;
		}
	}
	else
	{
		syx=tempx;
		syy=tempy;
	}
}

void Put_Wall(int nr,float x1,float x2,float z1,float z2,float y,int t1,int t2,int t3,int t4,int t5,bool door)
{
	float xx,xxx,zz,zzz;
	if (x1<x2) {xx=x1;xxx=x2;} else {xx=x2;xxx=x1;}
	if (z1<z2) {zz=z1;zzz=z2;} else {zz=z2;zzz=z1;}
	sciany[nr].x1=xx;
	sciany[nr].xx1=xx;
	sciany[nr].x2=xxx;
	sciany[nr].z1=zzz;
	sciany[nr].zz1=zzz;
	sciany[nr].z2=zz;
	sciany[nr].text1=t1;
	sciany[nr].text2=t2;
	sciany[nr].text3=t3;
	sciany[nr].text4=t4;
	sciany[nr].text5=t5;
	sciany[nr].y=y;
	sciany[nr].close=false;
	sciany[nr].open=false;
	sciany[nr].door=door;
	sciany[nr].oindex=-1;
	sciany[nr].cindex=-1;
}


void Mapa()
{
	for (i=0;i<=ilosc;i+=1)
	{
		Put_Wall(i,-10000,-100000,0,0,-1,-1,-1,-1,-1,0,false);
	}
	for (i=0;i<=26;i+=1)
	{		
		Put_Wall(i,(float)(i)-(mapsize*2)+6,(float)(i)-(mapsize*2)+7,-(mapsize*2)+4,-(mapsize*2)+3,wysokosc_nieba+0.5f,1,-1,-1,-1,-1,false);
	}
	for (i=26;i<=26+26;i+=1)
	{		
		Put_Wall(i,(float)(i-26)-(mapsize*2)+6,(float)(i-26)-(mapsize*2)+7,(mapsize),(mapsize)+1,wysokosc_nieba+0.5f,-1,1,-1,-1,-1,false);
	}
	
	for (i=52;i<=78;i+=1)
	{		
		Put_Wall(i,(mapsize)+2,(mapsize)+3,(float)(i-53)-(mapsize*2)+6,(float)(i-53)-(mapsize*2)+5,wysokosc_nieba+0.5f,-1,-1,1,-1,-1,false);
	}
	
	for (i=78;i<=78+25;i+=1)
	{		
		Put_Wall(i,-(mapsize*2)+5,-(mapsize*2)+6,(float)(i-78)-(mapsize*2)+5,(float)(i-78)-(mapsize*2)+4,wysokosc_nieba+0.5f,-1,-1,-1,1,-1,false);
	}

	Put_Wall(i,2,3,4,5,1,2,2,2,2,-1,false);i++;
	Put_Wall(i,3,4,4,5,1,2,2,-1,-1,-1,false);i++;
	Put_Wall(i,4,5,4,5,1,2,2,2,2,-1,false);i++;
	Put_Wall(i,5,6,4,4,1,1,1,-1,-1,-1,true);i++;
	Put_Wall(i,6,7,4,5,1,2,2,2,2,-1,false);i++;
	
	Put_Wall(i,6,7,5,6,1,2,2,2,2,-1,false);i++;
	Put_Wall(i,6,7,6,7,1,2,2,2,2,-1,false);i++;
	Put_Wall(i,7,7,7,8,1,-1,-1,1,1,-1,true);i++;
	Put_Wall(i,6,7,8,9,1,2,2,2,2,-1,false);i++;
	Put_Wall(i,6,7,9,10,1,2,2,2,2,-1,false);i++;
	Put_Wall(i,1,2,5,6,1,2,2,2,2,-1,false);i++;
	Put_Wall(i,1,2,6,7,1,2,2,2,2,-1,false);i++;
	Put_Wall(i,1,2,7,8,1,2,2,2,2,-1,false);i++;
	Put_Wall(i,1,2,8,9,1,2,2,2,2,-1,false);i++;
	Put_Wall(i,1,2,9,10,1,2,2,2,2,-1,false);i++;

	Put_Wall(i,0,0.3f,-2,-2.3f,0.3f,7,7,7,7,7,false);i++;
	Put_Wall(i,0.5f,0.8f,-2.2f,-2.5f,0.3f,7,7,7,7,7,false);i++;

	scian_rys=i;
}

void Wait(char* text)
{
	glClearColor(0.0f,0.0f,0.0f,0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	glTranslatef(0,0,-1);
	glPrint(-0.05f,0.05f,0,255,255,0,font1,"LOADING...");
	glPrint(-0.05f,-0.05f,0,150,150,0,font1,text);
	SwapBuffers(hDC);
}

void open_door(int nr)
{
	sciany[nr].open=true;
	if ((sciany[nr].x1<sciany[nr].x2) || (sciany[nr].z1>sciany[nr].z2))
	{
		if (sciany[nr].z1>sciany[nr].z2) sciany[nr].z1-=door_speed;
		if (sciany[nr].x1<sciany[nr].x2) sciany[nr].x1+=door_speed;
		if (sciany[nr].oindex==-1) 
		{
			PlaySound("snd\\doors\\2.wav",NULL,SND_ASYNC);
			if (toopen>=10) toopen=0;
			sciany[nr].oindex=++toopen;
			opening[sciany[nr].oindex]=nr;
		}
	}
	else
	{
		sciany[nr].open=false;
		opening[sciany[nr].oindex]=0;
		sciany[nr].oindex=-1;
		if (sciany[nr].cindex==-1) 
		{
			PlaySound("snd\\doors\\1.wav",NULL,SND_ASYNC);
			if (toclose>=10) toclose=0;
			sciany[nr].cindex=++toclose;
			closing[sciany[nr].cindex]=nr;
		}
	}
}

void close_door(int nr)
{
	sciany[nr].close=true;
	if ((sciany[nr].x1>sciany[nr].xx1) ||(sciany[nr].z1<sciany[nr].zz1))
	{
		if (sciany[nr].z1<sciany[nr].zz1) sciany[nr].z1+=door_speed;
		if (sciany[nr].x1>sciany[nr].xx1) sciany[nr].x1-=door_speed;
	}
	else
	{
		sciany[nr].close=false;
		closing[sciany[nr].cindex]=0;
		sciany[nr].cindex=-1;
		PlaySound("snd\\doors\\1.wav",NULL,SND_ASYNC);
	}
}

int w_srodku(float x,float y)
{
	if (noclip==true)
	{
		return 0;
	}
	else
	{
		for (i=0;i<=ilosc;i++)
			if ((x>=sciany[i].x1-max_zb)&&(x<=sciany[i].x2+max_zb)&&
			(y>=sciany[i].z2-max_zb)&&(y<=sciany[i].z1+max_zb))
			{
				if ((sciany[i].door==true) && (sciany[i].close==false) && (sciany[i].open==false))
				{
					open_door(i);
				}
				return 1;
			}
		for (i=0;i<ilosc_drzew;i++)
			if ((x>=drzewa[i].x-max_zb_tree)&&(x<=drzewa[i].x+max_zb_tree)&&
					(y>=drzewa[i].z-max_zb_tree)&&(y<=drzewa[i].z+max_zb_tree))
			{
				return 1;
			}
		return 0;
	}
}

void Drzwi()
{
	if (toopen>10 || toclose>10) return;
	for(i=1;i<=toopen;i++)
	{
		if (opening[i]>0) open_door(opening[i]);
	}
	for(i=1;i<=toclose;i++)
	{
		if (closing[i]>0) close_door(closing[i]);
	}
}

void Repair_Map()
{
	int badany;
	
	for (badany=0;badany<=scian_rys;badany++)
	{
		for (i=0;i<=scian_rys;i++)
			if (badany!=i)
			{
				if (
					 (sciany[badany].x1==sciany[i].x1)&&(sciany[badany].x2==sciany[i].x2)&&
					 (sciany[badany].z1==sciany[i].z2))
				{
					sciany[badany].text1=-1;
					sciany[i].text2=-1;
				}
				if (
					 (sciany[badany].x1==sciany[i].x1)&&(sciany[badany].x2==sciany[i].x2)&&
					 (sciany[badany].z2==sciany[i].z1))
				{
					sciany[badany].text2=-1;
					sciany[i].text1=-1;
				}
				if (
					 (sciany[badany].z1==sciany[i].z1)&&(sciany[badany].z2==sciany[i].z2)&&
					 (sciany[badany].x2==sciany[i].x1))
				{
					sciany[badany].text4=-1;
					sciany[i].text3=-1;
				}
				if (
					 (sciany[badany].z1==sciany[i].z1)&&(sciany[badany].z2==sciany[i].z2)&&
					 (sciany[badany].x1==sciany[i].x2))
				{
					sciany[badany].text3=-1;
					sciany[i].text4=-1;
				}
			}
	}
	for (i=0;i<=scian_rys;i++)
		if (sciany[i].y>0.5f) sciany[i].text5=-1;
	for (i=0;i<=scian_rys;i++)
		if (sciany[i].y>wysokosc_nieba+0.5f) sciany[i].y=wysokosc_nieba+0.5f;
}

void Mysz()
{
	float amp=(abs(windowX2-mouseX)*szybkosc_obrotu)/2.63575454f;
	if (mouseX<windowX2)
	{
		heading+=amp;
		yrot = heading;
		SetCursorPos(windowX2,windowY2);
	}
	if (mouseX>windowX2)
	{
		heading-=amp;
		yrot = heading;
		SetCursorPos(windowX2,windowY2);
	}
}

void Set_Tree(int nr,int typ,float x,float z,float h_pien,float h_kor,
							float sz_pien,float sz_kor,int text_k,int text_p,int quality)
{
	drzewa[nr].pien=gluNewQuadric(); 
	gluQuadricNormals(drzewa[nr].pien, GLU_NONE);
	gluQuadricTexture(drzewa[nr].pien, GL_TRUE); 
	drzewa[nr].korona1=gluNewQuadric(); 
	gluQuadricNormals(drzewa[nr].korona1, GLU_NONE);
	gluQuadricTexture(drzewa[nr].korona1, GL_TRUE); 
	drzewa[nr].korona2=gluNewQuadric(); 
	gluQuadricNormals(drzewa[nr].korona2, GLU_NONE);
	gluQuadricTexture(drzewa[nr].korona2, GL_TRUE); 
	drzewa[nr].korona3=gluNewQuadric(); 
	gluQuadricNormals(drzewa[nr].korona3, GLU_NONE);
	gluQuadricTexture(drzewa[nr].korona3, GL_TRUE); 
	drzewa[nr].typ=typ;
	drzewa[nr].x=x;
	drzewa[nr].z=z;
	drzewa[nr].h_pien=h_pien;
	drzewa[nr].h_kor=h_kor;
	drzewa[nr].sz_pien=sz_pien;
	drzewa[nr].sz_kor=sz_kor;
	drzewa[nr].text_k=text_k;
	drzewa[nr].text_p=text_p;
	drzewa[nr].quality=quality;
}

int WINAPI WinMain(	HINSTANCE	hInstance,
					HINSTANCE	hPrevInstance,
					LPSTR		lpCmdLine,
					int			nCmdShow)
{	

	
	randomize();
	bom=Losuj_bombelki(0,0.5f,-10,50,2);
	opady=1;


	MSG		msg;
	BOOL	done=FALSE;
	
	if (CreateGLWindow("GG ENGINE",320*2,240*2,16,fullscreen)==FALSE)
	{
		return 0;
	}

	// CZCIONKI
	font1=BuildFont(12,6,FONT_9,FALSE,FALSE,FALSE,"Times New Roman CE");
	font2=BuildFont(12,6,FONT_1,FALSE,FALSE,FALSE,"Arial CE");

	

	glPolygonMode( GL_BACK, GL_FILL );
	glPolygonMode( GL_FRONT, GL_FILL );

	Wait("Loading textures...");

	// TEKSTURY
	texture[0]=LoadTexture("bmp\\font.bmp",2);
	texture[1]=LoadTexture("bmp\\sciana\\8.bmp",2);
	texture[2]=LoadTexture("bmp\\podloga\\13.bmp",2);
	

	texture[3]=LoadTexture("bmp\\sufit\\1.bmp",2);
	texture[4]=LoadTexture("bmp\\efekty\\bombel.bmp",2);
	texture2[4]=LoadTexture("bmp\\efekty\\bombel2.bmp",2);
	if (opady==1) texture[5]=LoadTexture("bmp\\efekty\\snieg.bmp",2); else texture[5]=LoadTexture("bmp\\efekty\\rain.bmp",2);

	texture[7]=LoadTexture("bmp\\otoczenie\\crate.bmp",2);
	
	texture[8]=LoadTexture("bmp\\inne\\gun1.bmp",2);
	texture2[8]=LoadTexture("bmp\\inne\\gun2.bmp",2);

	texture[9]=LoadTexture("bmp\\otoczenie\\kora.bmp",2);
	texture[10]=LoadTexture("bmp\\otoczenie\\korona.bmp",2);
	
	texture[11]=LoadTexture("bmp\\otoczenie\\korona2.bmp",2);

	texture[12]=LoadTexture("bmp\\chmury.bmp",2);
	
	// MG£A
	GLfloat fogColor[4]= {0.0f, 0.0f, 0.5f, 0};
	glClearColor(fogColor[0],fogColor[1],fogColor[2],fogColor[3]);
	glFogi(GL_FOG_MODE,GL_EXP2);
	glFogfv(GL_FOG_COLOR, fogColor);
	glHint(GL_FOG_HINT, GL_FASTEST);
	glEnable(GL_FOG);
	
	// PRZEZROCZYSTOŒÆ
	glBlendFunc(GL_DST_COLOR,0);
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER,0);


	Wait("Loading world...");
	Mapa();
	Repair_Map();	

	Set_Tree(0,1,0,-5,0.1f,0.9f,0.07f,0.4f,11,9,6);
	Set_Tree(1,1,-2,-5,0.1f,0.9f,0.07f,0.5f,11,9,6);




	for (i=1;i<=ilosc_sniegu;i++)
	{
		randomize();
		losowanie_sniegu();
	}

	while(!done)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (msg.message==WM_QUIT)
			{
				done=TRUE;
			}
			else
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}
		else
		{
			DrawGLScene();
			SwapBuffers(hDC);
			Drzwi();
			kr=0;

			glFogf(GL_FOG_DENSITY, mgla);

			
			if (keys[VK_HOME]) {mgla+=0.01f;}
			if (keys[VK_END]) mgla-=0.01f;
			if (keys[VK_UP] || keys['W'])
			{
				Forward(szybkosc_chodzenia);
			}
			if (keys[VK_DOWN] || keys['S'])
			{
				Backward(szybkosc_chodzenia);
			}
			if (keys[VK_RIGHT])
			{
				heading -= szybkosc_obrotu;
				yrot = heading;
			}
			if (keys[VK_LEFT])
			{
				heading += szybkosc_obrotu;	
				yrot = heading;
			}
			if (keys['A']) 
			{
				heading += 90;	
				yrot = heading;
				Forward(szybkosc_chodzenia/3);
				heading -= 90;	
				yrot = heading;
			}
			if (keys['D']) 
			{
				heading -= 90;	
				yrot = heading;
				Forward(szybkosc_chodzenia/3);
				heading += 90;	
				yrot = heading;
			}
			
			if (keys['Q'] && !keydown['Q']) 
			{
				noclip=!noclip;
			}
			if (keys['Z']) PlaySound("snd\\ambient\\1.wav",NULL,SND_ASYNC|SND_LOOP);
			
			if (keys[VK_F1]) Change_Resolution(320,240,16,"GG ENGINE");

			Mysz();
			Keyboard();						
			

			if (keys[VK_ESCAPE])
			{
				done=TRUE;
			}
		}
	}

	// Shutdown
	KillFont(font1);
	KillFont(font2);

	KillGLWindow();
	return (msg.wParam);
}