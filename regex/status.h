#ifndef STATUS_H
#define STATUS_H

struct status
{
	int c;
	char *set;
	status *out;
	status *out1;
};

union status_list
{
	status* st;
	status_list* next;
};

struct edge
{
	status* start;
	status_list* list;
};


typedef struct status status;
typedef struct edge edge;
typedef union status_list status_list; 


#endif