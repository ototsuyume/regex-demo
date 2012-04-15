#include "status.h"
#include "nfa.h"
#include "stdio.h"
#include "string.h"

char* space_set()
{
	char *set = new char [256];
	memset(set,0,256);
	set[' ']=1;
	set['\t']=1;
	return set;
}

char* digit_set()
{
	char *set = new char [256];
	memset(set,0,256);
	memset(set+'0',1,10);
	return set;
}

char* word_set()
{
	char *set = new char[256];
	memset(set,1,256);
	memset(set+'0',0,10);
	set[0]=0;
	set[' ']=0;
	set['\t']=0;
	set['\r']=0;
	set['\n']=0;
	return set;
}

char* nonspace_set()
{
	char *set = new char [256];
	memset(set,1,256);
	set[' ']=0;
	set['\t']=0;
	return set;
}


edge new_edge(status*s ,status_list* list)
{
	edge p = {s,list};
	return p;
}

status_list* make_list(status** s)
{
	status_list *p = (status_list *)s;
	return p;
}

void patch(status* a,edge* b)
{
	status_list* p = b->list,*next;
	for(;p;p=next)
	{
		next = p->next;
		p->st = a;
	}
	
}

status_list* append(status_list* a,status_list* b)
{
	status_list* p = a;
	while(a->next) 
		a = a->next;
	a->next = b;
	return p;
}


status* new_status(int c,status* o,status* o2)
{
	status* p =new status;
	p->set = NULL;
	p->c = c;
	p->out = o;
	p->out1 = o2;
	return p;
}


status* _parse(char* str,size_t& size,edge &ret)
{
	char *p=str;
	size_t t;
	bool bfirst = false;
	bool bSub   = false;
	edge stack[1000];
	status* s,*s1,*s2;
	edge e1,e2;
	//	e1 = new_edge(s,make_list(&(s->out)));
	int base=0,count=0;
#define push(a) stack[base++]=a
#define pop()  stack[--base]
	while(*p!=0&&*p!=')')
	{
		switch(*p)
		{
		case '(':
			s = _parse(++p,t,e1);
			p+=t;
			if(*(p+1)!='*'&&*(p+1)!='+'&&*(p+1)!='?')
			{
				s = new_status(LCATCH,e1.start,NULL);
				e1.start = s;
				s = new_status(RCATCH,NULL,NULL);
				patch(s,&e1);
				e2 = new_edge(e1.start,make_list(&s->out));
				push(e2);
			}
			else
			{
				push(e1);
				bSub = true;
			}


			break;
		case '|':
			if(bfirst)
			{
				e1 = pop();
				e2 = pop();
				s = new_status(SPLITE,
					e1.start->c==0?e1.start->out:e1.start,
					e2.start->c==0?e2.start->out:e2.start);
				e2 = new_edge(s,append(e1.list,e2.list));
				push(e2);
				s = new_status(0,NULL,NULL);
				e1 = new_edge(s,make_list(&s->out));
				push(e1);
			}
			else
			{
				s = new_status(0,NULL,NULL);
				e1 = new_edge(s,make_list(&s->out));
				push(e1);
				bfirst = true;
			}
			break;
		case '*':
			e1 = pop();
			if(bSub)
			{
				s = new_status(SPLITE,e1.start,NULL);
				patch(s,&e1);
				s1 = new_status(LCATCH,s,NULL);
				s2 = new_status(RCATCH,NULL,NULL);
				s->out1 = s2;
				e2=new_edge(s1,make_list(&s2->out));

				bSub = false;
			}
			else
			{
				s = new_status(SPLITE,e1.start,NULL);
				patch(s,&e1);
				e2 = new_edge(s,make_list(&(s->out1)));
			}

			push(e2);
			break;
		case '?':
			e1 = pop();
			if(bSub)
			{
				s = new_status(SPLITE,e1.start,NULL);
				s1 = new_status(LCATCH,s,NULL);
				s2 = new_status(RCATCH,NULL,NULL);
				e2 = new_edge(s1,append(make_list(&s->out1),e1.list));
				patch(s2,&e2);
				e2 = new_edge(s1,make_list(&s2->out));
				bSub = false;
			}
			else
			{
				s = new_status(SPLITE,e1.start,NULL);
				e2 = new_edge(s,append(make_list(&s->out1),e1.list));
			}
			push(e2);
			break;
		case '+':
			e1 = pop();
			if(bSub)
			{
				s1 = new_status(RCATCH,NULL,NULL);
				s = new_status(SPLITE,e1.start,s1);
				patch(s,&e1);
				s2 = new_status(LCATCH,e1.start,NULL);
				e2 = new_edge(s2,make_list(&s1->out));
				bSub = false;
			}
			else
			{
				s = new_status(SPLITE,e1.start,NULL);
				patch(s,&e1);
				e2 = new_edge(e1.start,make_list(&s->out1));
			}
			push(e2);
			break;
		case '\\':
			++p;
			s = new_status(CHARSET,NULL,NULL);
			switch(*p)
			{
			case 's':
				s->set = space_set();
				break;
			case 'w':
				s->set = word_set();
				break;
			case 'd':
				s->set = digit_set();
				break;
			case 'S':
				break;
			case 'W':
				break;
			case 'D':
				break;
			default:
				s->c = *p;
			}
			if(*(p+1)=='?'||*(p+1)=='+'||*(p+1)=='*'||base==0)
			{
				e2 = new_edge(s,make_list(&(s->out)));
			}
			else
			{	
				e1 = pop();
				patch(s,&e1);
				e2 = new_edge(e1.start,make_list(&(s->out)));
			}
			push(e2);
			break;
		default:
			if(*(p+1)=='?'||*(p+1)=='+'||*(p+1)=='*'||base==0)
			{
				s = new_status(*p,NULL,NULL);
				e2 = new_edge(s,make_list(&(s->out)));
			}
			else
			{	
				e1 = pop();
				s = new_status(*p,NULL,NULL);
				patch(s,&e1);
				e2 = new_edge(e1.start,make_list(&(s->out)));

			}
			if(*p=='.')
				s->c = ALLCHAR;
			push(e2);
			break;
		}
		p++;
	}
	if(bfirst)
	{
		e1 = pop();
		e2 = pop();
		s = new_status(SPLITE,
			e1.start->c==0?e1.start->out:e1.start,
			e2.start->c==0?e2.start->out:e2.start);
		e1 = new_edge(s,append(e1.list,e2.list));
		//		push(e2);
	}
	else 
		e1 = pop();
	size = p-str;
	if(*p==0)
	{
		s=new_status(MATCHSTATE,NULL,NULL);
		//e2 = new_edge(s,make_list(&(s->out)));
		patch(s,&e1);
		s=new_status(0,NULL,NULL);
		s->out = e1.start;
	}
	else
	{
		s = e1.start;
		ret = e1;
	}

	/*
	push(e1);
	e1 = stack[0];
	for(int i=1;i<base;i++)
	{
	e2=stack[i];
	append(e1,e2);
	free_edge(e1);
	e1 = e2;
	}

	free_edge(e1);
	*/
	base;
	return s;

}

status* parse(char* str)
{
	edge e;
	size_t t;
	return _parse(str,t,e);
}