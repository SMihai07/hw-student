#ifndef __STACK_H__
#define __STACK_H__

#include "lista.h"
#include "info.h"
#include <stdio.h>

typedef struct stiva
{
	size_t nr;	//nr elemente
	ACel vf;	//adresa celulei din varf

} TStiva, *ASt;

#define VF(a) (((ASt)(a))->vf)
#define NRS(a) (((ASt)(a))->nr)

void *InitS();
int Push(void *s, void *info);
TInfo *Pop(void *s);
void *SortS(void *s);
void PrintS(void **s, FILE *out, int nr);
int CorrectS(void *s);

#endif