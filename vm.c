// accmd: run gcc -o vm.exe -DWITH_ZZZ -DDBG vm.c -lalleg -lkernel32 -luser32 -lgdi32 -lcomdlg32 -lole32 -ldinput -lddraw -ldxguid -lwinmm -ldsound
// accmdx: run gcc -o vmq.exe -O3 -s vm.c
// accmd: runw-if-success vm.exe
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include "opcodes.h"
#include "chargen.h"

#define MEMSIZE 65536
#define UOFF 4096
#define U16 unsigned short
#define ep(...) fprintf(stderr,...)
#define WORD(o) ((m[o]<<8) | m[(o)+1])
#define SWORD(o,w) m[o]=w>>8; m[o+1]=w&0xFF;

unsigned int pc=0;
unsigned char m[MEMSIZE];

#include "draw.h"



static U16 regs[8];

#ifndef DBG
	void dp(int l, char *format, ...) {}
#else
	void dp(int l, char *format, ...)
	{
		va_list ap;
		va_start(ap, format);
		if(pc>0) printf("pc=0x%04x (%05d): ",pc,pc);
		vfprintf(stderr, format, ap);
		va_end(ap);
	}
#endif
void err(char *format, ...)
{
	va_list ap;
	va_start(ap, format);
	printf("ERROR (pc=0x%04x): ",pc);
	vfprintf(stderr, format, ap);
	va_end(ap);
	//exit(1);
}

#ifdef WITH_ZZZ
#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
void zzz(int time)
{
	Sleep(time);
}
#endif
#else
#define zzz(n)
#endif



void dumpregs()
{
	int i;
	fprintf(stderr,"Regs [ ");
	for(i=0;i<8;i++) fprintf(stderr,"%02x ",regs[i]);
	fprintf(stderr,"]\n");
}

void dump(int to)
{
	int i=0,a;
	while(i<=to)
	{
		printf("%04x %05d | ",i,i);
		for(a=0;a<32;a++) printf("%02x ",m[i++]);
		printf("\n");
	}
}

int main(int argc,char **argv)
{
	char *fileName;
	FILE *f;
	int n,op,i;
	unsigned int oc=0;
	char *mpc;
	if(argc==1) fileName="default.md";
	else fileName=argv[1];
	f=fopen(fileName,"rb");
	if(!f)
	{
		dp(0,"Couldn't open \"%s\".\n",fileName);
		exit(0);
	}
	memset(m,0,MEMSIZE);
	n=fread(m+UOFF,1,MEMSIZE-UOFF,f);
	fclose(f);
	#ifdef DBG
		dp(0,"Read %d bytes of code.\n",n);
		for(i=0;i<n;i++) printf("%02x ",m[UOFF+i]);
		printf("\n");
	#endif
	pc=UOFF;
	initGfx();
	dp(0,"PC = %d, starting execution.\n",pc);
	while(1)
	{
		if(pc>=MEMSIZE)
		{
			err("PC overflow.");
			break;
		}
		op=m[pc];//*(mpc=m+pc);
		//dp(4,"pc=0x%04x op=0x%02x (%s)\n",pc,op,opnames[op]);
		//dumpregs();
		//printf("m(pc:pc+10) = ");
		//for(i=pc;i<pc+10;i++) printf("%-3d ",m[i]);
		//printf("\n");

		switch(op)
		{
			case OP_NOP:
				pc++;
				goto q;
			break;

			case OP_JMP:
				pc=WORD(pc+1);
			break;

			case OP_HLT:
				dp(0,"HLT encountered.");
				goto exit;
			break;

			case OP_L: // l <register> <offset> [register] (REto REfr OF OF)
				regs[m[pc+1]]=WORD(WORD(pc+3)+regs[m[pc+2]]);
				pc+=5;
			break;

			case OP_LB: // lb <register> <offset> [register] (REto REfr OF OF)
				regs[m[pc+1]]=m[WORD(pc+3)+regs[m[pc+2]]];
				pc+=5;
			break;

			case OP_LI: // li <register> <value> ( RE WO RD )
				//printf("Load %d into register %d\n",WORD(pc+2),m[pc+1]);
				regs[m[pc+1]]=WORD(pc+2);
				pc+=4;
			break;

			case OP_W: // w <register> <offset> [register] (REto REfr OF OF)
				n=WORD(pc+3)+regs[m[pc+2]];
				SWORD(n,regs[m[pc+1]]);
				pc+=5;
			break;

			case OP_WB: // wb
				m[WORD(pc+3)+regs[m[pc+2]]]=regs[m[pc+1]];
				pc+=5;
			break;

			case OP_INC: // inc <register>
				if(regs[m[pc+1]]<65535) regs[m[pc+1]]++;
				pc+=2;
			break;

			case OP_DEC: // dec <register>
				if(regs[m[pc+1]]>0) regs[m[pc+1]]--;
				pc+=2;
			break;

			case OP_OUT: // out <port> <register>
				i=m[pc+1];
				n=regs[m[pc+2]];
				if(i==1) printf("%c",n); // "output"
				else if(i==2) printf("%d\n",n); // debug out
				else if(i==3) dump(n);
				else if(i==4) gfxSetPal(n);
				pc+=3;
			break;

			case OP_JEQ: // jeq <reg1> <reg2> <target> ( R1 R2 OF OF )
			case OP_JNE:
				n=regs[m[pc+1]]==regs[m[pc+2]];
				if((op==OP_JEQ&&n)||(op==OP_JNE&&!n))
				{
					pc=WORD(pc+3);
					goto q;
				}
				pc+=5;
			break;

			case OP_AND: // and <reg1> <reg2>
				regs[m[pc+1]]&=regs[m[pc+2]];
				pc+=3;
			break;

			case OP_OR: // or <reg1> <reg2>
				regs[m[pc+1]]|=regs[m[pc+2]];
				pc+=3;
			break;

			case OP_XOR: // xor <reg1> <reg2>
				regs[m[pc+1]]^=regs[m[pc+2]];
				pc+=3;
			break;

			case OP_ADD: // add <regto> <reg1> <reg2>
				i=(regs[m[pc+2]]+regs[m[pc+3]]);
				if(i>0xFFFF) i=0xFFFF;
				regs[m[pc+1]]=i;
				pc+=4;
			break;

			case OP_SUB: // sub <regto> <reg1> <reg2>
				i=(regs[m[pc+2]]+regs[m[pc+3]]);
				if(i<0) i=0;
				regs[m[pc+1]]=i;
				pc+=4;
			break;

			case OP_MUL: // mul <regto> <reg1> <reg2>
				i=(regs[m[pc+2]]*regs[m[pc+3]]);
				if(i>0xFFFF) i=0xFFFF;
				regs[m[pc+1]]=i;
				pc+=4;
			break;

			case OP_DIV: // div <regto> <reg1> <reg2>
				if(regs[m[pc+3]])
				{
					i=(regs[m[pc+2]]/regs[m[pc+3]]);
					regs[m[pc+1]]=i;
				}
				else
				{
					regs[m[pc+1]]=regs[m[pc+2]]^0xFFFF;
				}
				pc+=4;
			break;

			case OP_WRT:
				while(!drawflag);
				pc++;
				break;

			default:
				printf("Unknown opcode %d (0x%2x).",op,op);
			break;
		}
		q:
		if(drawflag) drawGfx();
		oc++;
		if(key[KEY_ESC]) break;
	}
	exit:
	drawGfx();
	textprintf_ex(screen,font,2,2,7,0,"System halted. OC = %u",oc);
	while(!key[KEY_ESC]);
	closeGfx();
	return 0;
}
END_OF_MAIN();