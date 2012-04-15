#include "status.h"
#include "nfa.h"
#include <stdio.h>
#include <string.h>



struct matchstr
{
	char* begin;
	char* end;
	int cur;
	struct subm
	{
		char* begin;
		char* end;
	}sub[10];
};

struct context
{
	char *p;
	status* s;
};

typedef struct matchstr matchstr;
typedef struct context context;

void match(char* str,status* s)
{
	int base=0,n=0;
	char *nextchar = str+1;
	context stack[10000];
	stack[0].s=s->out;
	stack[0].p=str;
	matchstr mstr[20];
	memset(mstr,0,sizeof(matchstr)*20);
	mstr[0].begin = str;
	mstr[0].cur   = 0;
	while(base>=0)
	{
		int  c = stack[base].s->c;
		char* p = stack[base].p;
		if(*p==0)
		{
			if(stack[base].s->c==MATCHSTATE)
			{
				if(mstr[n].begin!=stack[base].p&&mstr[n].begin!=0)
				{
					mstr[n++].end=stack[base].p;
					break;
				}
			}
			if(base==0)break;
			else
			{
				--base;
				continue;
			}
			
		}
		 switch(c)
		 {
		 case SPLITE:
			stack[base+1].p=stack[base].p;
			stack[base+1].s=stack[base].s->out;
			stack[base].s=stack[base].s->out1;
			
			
			++base;
			break;
		 case MATCHSTATE:
			 if(mstr[n].begin==p||mstr[n].begin==0)
			 {
				 if(base==0) 
				 {
					 stack[0].s = s->out;
					 stack[0].p = nextchar;
					 ++nextchar;
					 mstr[n].cur = 0;
					 mstr[n].begin = stack[0].p;
				 }
				 else
					 --base;
				 break;
			 }
			 mstr[n++].end = p;
			 stack[0].s = s->out;
			 nextchar = stack[0].p = stack[base].p;
			 ++nextchar;
			 mstr[n].begin = stack[0].p;
			 base = 0;
			 break;
		 case LCATCH:
			 stack[base].s = stack[base].s->out;
			 mstr[n].sub[mstr[n].cur].begin=p;
			 break;
		 case RCATCH:
			 mstr[n].sub[mstr[n].cur++].end=p;
			 stack[base].s = stack[base].s->out;
			 break;
		 case ALLCHAR:
			 stack[base].s = stack[base].s->out;
			 ++stack[base].p;
			 break;
		 case CHARSET:
			 if(stack[base].s->set[*p]==1)
			 {
				 stack[base].s = stack[base].s->out;
				 ++stack[base].p;
			 }
			 else
			 {
				 if(base==0) 
				 {
					 stack[0].s = s->out;
					 stack[0].p = nextchar;
					 ++nextchar;
					 mstr[n].cur = 0;
					 mstr[n].begin = stack[0].p;
				 }
				 else
					 --base;
			 }
			 break;
		 default:
			if(c == *p)
			{
				stack[base].s = stack[base].s->out;
				++stack[base].p;
			}
			else
			{
				if(base==0) 
				{
					stack[0].s = s->out;
					stack[0].p = nextchar;
					++nextchar;
					stack[0].s = s->out;
					mstr[n].cur = 0;
					mstr[n].begin = stack[0].p;
				}
				else
					--base;
			}
		 }
		 
	}
	
	int i=0;
	while(*str)
	{
		if(mstr[i].begin==str)putchar('[');
		if(mstr[i].end==str)
		{
			putchar(']');
			++i;
			if(i>=n)
				break;
			if(mstr[i].begin==str)
				putchar('[');
		}
		
		putchar(*str);
		++str;
	}
	if(*str==0&&mstr[i].end==str){putchar(']');++i;}
	while(*str)
	{
		putchar(*str);
		++str;
	}
	putchar('\n');
}