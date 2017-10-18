/*STANESCU Mihai-Florin 314CB*/
#include <stdlib.h>
#include "stack.h"
#include "lista.h"
#include "info.h"
#include <stdio.h>

void *InitS()
{
	ASt stack = (ASt)malloc(sizeof(TStiva));  //initializare stiva

	if(!stack) return NULL;

	stack->nr = 0;
	stack->vf = NULL;

	return (void *)stack;
}

int Push(void *s, void *info)
{
	if(VF(s) == NULL)
	{
		ACel temp = (ACel)malloc(sizeof(TCel));

		if(!temp) return 0;

		temp->urm = NULL;							//inserare la inceputul listei
		temp->info = info;
		VF(s) = temp;
		NRS(s)++;
		return 1;
	}
	else
	{
		ACel temp = (ACel)malloc(sizeof(TCel));

		if(!temp) return 0;

		temp->urm = VF(s);
		temp->info = info;
		VF(s) = temp;
		NRS(s)++;
		return 1;
	}

	return 0;
}

TInfo *Pop(void *s)
{
	if(VF(s) == NULL)
		return NULL;
	ACel temp = malloc(sizeof(TCel));				//extragere de la inceputul listei
	temp = VF(s);
	VF(s) = temp->urm;
	NRS(s)--;
	return (TInfo*)temp->info;
}

void *SortS(void *s)
{
	void *aux = InitS();							//stiva auxiliara pt sortare
	TInfo *temp;

	while(VF(s) != NULL)
	{
		TInfo *current_cel = Pop(s);				//scoatem cate un el din s
		TInfo *current_aux;

		if(VF(aux))
			current_aux = (TInfo*)VF(aux)->info;

		while(VF(aux) != NULL && (current_aux->id_paranteza > current_cel->id_paranteza))	//cat timp el din vf stivei aux e mai mare decat cel scos din stiva s				
		{
			temp = Pop(aux);
			Push(s, temp);																	//scoatem el din stiva aux si le punem inapoi in stiva s
			if(VF(aux))
				current_aux = (TInfo*)VF(aux)->info;
		}

		
		Push(aux, current_cel);					//punem in stiva aux celula scoasa mai sus din stiva s
	}

	return (void*)aux;				//stiva aux va deveni stiva sortata
}

void PrintS(void **s, FILE *out, int nr)
{
	void *aux = InitS();			//stiva auxiliara pt afisare
	int i;

	for(i = 0; i < nr; i++)
	{
		fprintf(out, "\"");


		while(VF(s[i]) != NULL )
		{	
			TInfo *temp = malloc(sizeof(TInfo));			//turnam stiva noastra in stiva aux
			temp = Pop(s[i]);
			Push(aux, temp);

		}

		while(VF(aux) != NULL)
		{
			TInfo *temp = malloc(sizeof(TInfo));			//turnam stiva aux in stiva noastra la loc si afisam de fiecare data cand facem pop
			temp = Pop(aux);
			fprintf(out, "%c", temp->type);
			Push(s[i], temp);
		}

		fprintf(out, "\"\n");
	}
}

int CorrectS(void *s)								//merge doar pe unele cazuri?!!?
{
	void *check = InitS();
	void *recover = InitS();
	void *left = InitS();							//stive auxiliare pt determinarea celui mai lung subsir
	void *aux = InitS();
	TInfo *temp = NULL;
	int max = 0, l = 0;								//maximul returnat si lungimea unui subsir

	if(VF(s) == NULL)
		return 0;									//sirul vid

	while(VF(s) != NULL)
	{
		TInfo *move = malloc(sizeof(TInfo));			//rasturnam stiva pentru a o putea parcurge din baza
		move = Pop(s);
		Push(aux, move);
	}

	while(VF(aux) != NULL)
	{
		temp = Pop(aux);																//scoatem un el din stiva si incepem sa cautam un subsir corect
		l = 0;

		if(temp->type == ')' || temp->type == ']' || temp->type == '}')
			Push(recover, temp);														//daca incepem cu o paranteza inchisa o punem in stiva pe recuperarea stivei originale
		else if(temp->type == '(' || temp->type == '[' || temp->type == '{')
			{
				Push(check, temp);													//altfel o punem in stiva de verificare
				TInfo *temp2 = NULL, *deleted =  NULL;

				while(VF(aux) != NULL)													//parcurgem stiva rasturnata
				{
				
					temp2 = Pop(aux);									//scoatem cate un element si il punem intr-o stiva pe care o vom folosi
					Push(left, temp2);									//sa refacem stiva pana la primul element scos
					TInfo *varf;
					if(VF(check))
						varf = (TInfo*)VF(check)->info;						//daca varful stivei de verificare exista

					if(temp2->type == '(' || temp2->type == '[' || temp2->type == '{')			//daca mai gasim o paranteza deschisa o bagam in check
						Push(check, temp2);
					else if(temp2->type == ')' && varf->type == '(' && VF(check))				
							{
								deleted = Pop(check);
								l += 2;
							}
						else if(temp2->type == ']' && varf->type == '[' && VF(check))
							{
								deleted = Pop(check);															//daca paranteza scoasa din stiva rasturnata inchide
								l += 2;																			//prima paranteza din stiva check facem pop si crestem lungimea cu 2
							}																					//ne asiguram ca varful exista
							else if(temp2->type == '}' && varf->type == '{' && VF(check))
								{
									deleted = Pop(check);
									l += 2;
								}
								else{ 
										if(l > max)																//daca am gasit o greseala in sir ne oprim si verificam lungimea obtinuta
											max = l;

										break; 			
									}

				}				



				if(VF(check) != NULL)
				{
					while(VF(check) != NULL)								//daca stiva check nu a iesit vida o golim
						deleted = Pop(check);
				}
				else if(VF(check) == NULL)										//daca a iesit verificam iar lungimea
					{
						if(l > max)
							max = l;
					}

				while(VF(left) != NULL)
				{
					TInfo *aux2 = malloc(sizeof(TInfo));						//ne refacem stiva rasturnata pentru urmatoare "iteratie"
					aux2 = Pop(left);
					Push(aux, aux2);
				}

				Push(recover, temp);												//punem primul element scos in stiva care ne va ajuta sa ne recuperam stiva initiala


			}

	}

	while(VF(recover) != NULL)
	{
		TInfo *aux3 = malloc(sizeof(TInfo));

		aux3 = Pop(recover);										//refacem stiva rasturnata
		Push(aux, aux3);
	}

	while(VF(aux) != NULL)
	{
		TInfo* info = malloc(sizeof(TInfo));

		info = Pop(aux);													//refacem stiva initiala
		Push(s, info);
	}

	return max;											//returnam rezultatul
}