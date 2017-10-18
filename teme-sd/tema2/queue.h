#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "lista.h"
#include "info.h"

typedef struct coada
{
	size_t nr;	//nr elemente
	ACel ic, sc;	//adr prima, ultima celula

} TCoada, *AQ;

#define NRQ(a) (((AQ)(a))->nr)
#define SC(a) (((AQ)(a))->sc)
#define IC(a) (((AQ)(a))->ic)


void *InitQ();
int IntrQ(void *q, void *info);
TInfo *ExtrQ(void *q);
void PrintQ(void **q, FILE *out, int nr);
void SortQ(void *q);
int CorrectQ(void *q);

#endif