/*STANESCU Mihai-Florin 314CB*/
#include <stdio.h>
#include "queue.h"
#include "stack.h"
#include "info.h"
#include <stdlib.h>

void *InitQ()
{
	AQ queue = (AQ)malloc(sizeof(TCoada));							//initializam coada

	if(!queue) return NULL;

	queue->ic = NULL;
	queue->sc = NULL;
	queue->nr = 0;

	return (void *)queue;
}

int IntrQ(void *q, void *info)
{
	if(SC(q) == NULL && IC(q) == NULL)
	{
		ACel temp = (ACel)malloc(sizeof(TCel));										//daca inseram in coada vida
		temp->info = info;
		temp->urm = NULL;
		SC(q) = IC(q) = temp;
		NRQ(q)++;
		return 1;																		//adaugare la sf de lista

	}
	else
	{
		ACel temp = (ACel)malloc(sizeof(TCel));											//altfel 
		temp->info = info;
		temp->urm = NULL;
		SC(q)->urm = temp;
		SC(q) = temp;
		NRQ(q)++;
		return 1;
	}
}

TInfo *ExtrQ(void *q)
{
	if(IC(q) == NULL) 
		return NULL;																//daca coada e vida
	else
	{
		ACel temp = IC(q);
		IC(q) = temp->urm;															//facem extragerea si vedem daca facem coada vida sau nu
		if (IC(q) == NULL) 
			SC(q) = NULL;
		NRQ(q)--;
		return (TInfo*)temp->info;
	}

}

void PrintQ(void **q, FILE *out, int nr)
{
	void *aux = InitQ();												//coada auxiliara pt printare

	int i;

	for(i = 0; i < nr; i++)
	{
		fprintf(out, "\"");

		while(IC(q[i]) != NULL && SC(q[i]) != NULL)									//mutam coada in coada aux si afisam in acelasi timp
		{
			TInfo *temp = malloc(sizeof(TInfo));
			temp = ExtrQ(q[i]);
			fprintf(out, "%c", temp->type);
			IntrQ(aux, temp);
		}

		while(IC(aux) != NULL && SC(aux) != NULL)
		{
			TInfo *temp = malloc(sizeof(TInfo));										//refacem coada
			temp = ExtrQ(aux);
			IntrQ(q[i], temp);
		}

		fprintf(out, "\"\n");
	}
}

void SortQ(void *q)
{
	
	void *aux = InitS();											//stiva auxiliara pt sortare
	TInfo *temp;

	while(IC(q) != NULL && SC(q) != NULL)
	{
		TInfo *current_cel = ExtrQ(q);								//extragem un element
		TInfo *current_aux;

		if(VF(aux))
			current_aux = (TInfo*)VF(aux)->info;

		while(VF(aux) && (current_cel->id_paranteza > current_aux->id_paranteza))				//cat timp elementul extras din coada e mai mare decat varful stivei aux
		{
			temp = Pop(aux);																	//scoatem elemente din stiva si le punem in coada
			IntrQ(q, temp);
			if(VF(aux))
				current_aux = (TInfo*)VF(aux)->info;
		}

		Push(aux, current_cel);												//punem in stiva elementul scos din coada
	}

	while(VF(aux) != NULL)
	{
		temp = Pop(aux);												//mutam stiva in coada si avem coada sortata astfel
		IntrQ(q, temp);
	}


	
}

int CorrectQ(void *q)													//uneori da uneori nu ?!?!?
{
	void *s_aux = InitS();												//stiva pt coada


	while(IC(q) != NULL && SC(q) != NULL)
	{
		TInfo *info = malloc(sizeof(TInfo));							//mutam coada in stiva
		info = ExtrQ(q);
		Push(s_aux, info);
	}

	int max = CorrectS(s_aux);											//apelam corrects pt stiva

	void *q_aux = InitQ();												//coada auxiliara pentru restaurarea cozii initiale

	while(VF(s_aux) != NULL)
	{
		TInfo *info = malloc(sizeof(TInfo));							//mai intai punem stiva in coada aux
		info = Pop(s_aux);
		IntrQ(q_aux, info);
	}

	while(IC(q_aux) != NULL && SC(q_aux) != NULL)
	{
		TInfo *info = malloc(sizeof(TInfo));
		info = ExtrQ(q_aux);												//apoi din coada aux in stiva aux
		Push(s_aux, info);
	}

	while(VF(s_aux) != NULL)
	{
		TInfo *info = malloc(sizeof(TInfo));									//in final refacem coada
		info = Pop(s_aux);
		IntrQ(q, info);
	}

	return max;													//returnam numarul
}