#ifndef __LISTA_H__
#define __LISTA_H__

typedef struct cel
{ 
	struct cel *urm;  //adr urm celule
	void *info;		//adresa informatie

} TCel, *ACel;

#endif