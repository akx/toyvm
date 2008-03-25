#include <allegro.h>
#include <winalleg.h>
#include "chargen.h"
#include "pal.h"
BITMAP *page;



volatile int drawflag=0;

void drawflag_handler(){drawflag=1;}
END_OF_FUNCTION(drawflag_handler);

void initGfx()
{
	PALETTE pal;
	int i;
	unsigned char *po;

	allegro_init();
	install_keyboard();
	install_timer();
	//set_color_depth(8);
	set_gfx_mode(GFX_AUTODETECT_WINDOWED, 640, 480, 0, 0);
	set_display_switch_mode(SWITCH_BACKGROUND);
	page=create_bitmap(192,144);
	LOCK_VARIABLE(drawflag);
	LOCK_FUNCTION(drawflag_handler);
	install_int_ex(drawflag_handler,59659); // 1/20*1193181


	po=paldef;
	for(i=0;i<PALCNT;i++)
	{
		pal[i].r=*po++;
		pal[i].g=*po++;
		pal[i].b=*po++;
	}
	set_palette(pal);

	rectfill(screen,0,0,640,480,0);
}

void closeGfx()
{
	destroy_bitmap(page);
	allegro_exit();
}

void drawGlyph(int idx,int x,int y,int color,int invert)
{
	static int xo,yo,b;
	char *g=cgrom+idx*6;
	xo=0;
	yo=0;
	b=0;
	for(yo=0;yo<6;yo++)
	{
		for(xo=0;xo<6;xo++)
		{
			b=((*g)&(1<<xo));
			if((b&&!invert)||(!b&&invert)) putpixel(page,x+xo,y+yo,color);
		}
		g++;
	}
}

void gfxSetPal(int n)
{
	RGB t;
	t.r=m[512]>>2;
	t.g=m[513]>>2;
	t.b=m[514]>>2;
	set_color(n,&t);
}



void drawGfx()
{
	static int y,x;
	static char *c,*d,ch;
	for(y=0;y<144;y++) hline(page,0,y,192,m[1792+y]);
	c=m+1024;
	d=m+1936; // 1936
	for(y=0;y<24;y++)
	{
		for(x=0;x<32;x++)
		{
			if(x==0||x==8||x==16||x==24) d++;
			ch=*c;
			if(ch>85) drawGlyph(ch-85,x*6,y*6,*d,1);
			else drawGlyph(ch,x*6,y*6,*d,0);
			c++;
		}
	}
	textprintf_ex(screen,font,2,2,7,0,"PC = %u",pc);
	stretch_blit(page,screen,0,0,192,144,32,24,576,432); // 3x stretch
	drawflag=0;
}