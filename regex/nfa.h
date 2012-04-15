#ifndef NFA_H
#define NFA_H

enum
{
	STARTSTATE=0,
	SPLITE=256,
	MATCHSTATE,
	LCATCH,
	RCATCH,
	ALLCHAR,
	CHARSET,
};
 

status_list* make_list(status**);
status_list* append(status_list* a,status_list* b);
void patch(status*,edge*);
edge new_edge(status*s ,status_list* list);
status* new_status(int c,status* o,status* o2);


status* parse(char* str);
void match(char* str,status* s);


#endif