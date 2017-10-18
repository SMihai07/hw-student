#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lista.h"
#include "materie.h"
#include "student.h"

/*
 * data - key to compute hash
 * len - sizeof of the data
 * range - the number of buckets
 * Return the index of the bucket
 */

unsigned int hash_f(const void *data, size_t len, size_t range) {
	unsigned int hash = 0u;
	size_t idx = 0;
	size_t cof = 1;
	size_t rate = 2;
	const char *d = (const char*)data;
	for(; idx < len; ++idx) {
		hash += d[idx] * cof;
		cof *= rate;
	}
	return hash % range;
}

typedef unsigned int (*TFHash)(const void*, size_t, size_t);  //functia hash
typedef void (*faficheie)(void*, FILE *);	//functie afisare cheie
typedef void (*felibcheie)(void*);	//functie eliberare cheie
typedef int (*fcompchei)(void*, void*);  //functie comparare chei


typedef struct tabela {
	size_t M;
	TFHash fh;
	TLG *v;
	faficheie fafc;
	felibcheie felc;
	fcompchei fcc;
}TH;

typedef void (*fafivaloare)(void*, FILE *); //functie afisare valoare
typedef void (*felibvaloare)(void*); //functie eliberare valoare

typedef struct informatie {
	fafivaloare fafv;
	felibvaloare felv;
	void *info_val;					//campul info din celule
	void *info_cheie;
}TInfo;

void afiVALs(void *data, FILE *out)
{
	fprintf(out, "[Nume: %s, Grupa: %s, Medie: %.2f, Varsta: %d]", ((TStudent*)data)->nume, ((TStudent*)data)->grupa, ((TStudent*)data)->medie, ((TStudent*)data)->varsta);
} //afisare valori studenti

void afiVALm(void *data, FILE *out)
{
	fprintf(out, "[Materie: %s, Ore_curs: %d, Ore_lab: %d, Credit: %d, Teme: %d]", ((TMaterie*)data)->materie, ((TMaterie*)data)->ore_curs, ((TMaterie*)data)->ore_lab, ((TMaterie*)data)->credit, ((TMaterie*)data)->teme);
} //afisare valori materii

void afiKEYs(void *data, FILE *out)
{
	TInfo* P = (TInfo*)data;
	char *cheie = (char*)P->info_cheie;
	fprintf(out, "%s",(char*)cheie);				//afisare chei string
}

void afiKEYd(void *data, FILE *out)
{
	TInfo *P = (TInfo*)data;
	int *cheie = (int*)P->info_cheie;
	fprintf(out, "%d", *(int *)cheie);							//afisare chei int
}

int compareKEYSs(void *key1, void *key2)
{
	return strcmp((char *)key1, (char *)key2);					//comparare chei string
}

int compareKEYSd(void *key1, void *key2)
{
	return *(int *)key1 - *(int *)key2;						//comparare chei int
}

void elibVALs(void *data)
{
	free(((TStudent*)data)->nume);
	free(data);												//eliberare studenti
}

void elibVALm(void *data)
{
	free(((TMaterie*)data)->materie);						//eliberare materii
	free(data);
}
											
void elibKEY(void *data)
{						
	free(data);												//eliberare chei
}



TH* InitTH(size_t M, TFHash fh, faficheie fc, felibcheie fe, fcompchei fcomp)
{
	TH* ath = (TH*)malloc(sizeof(TH));
	if(!ath) return NULL;
	ath->v = (TLG*)calloc(M,sizeof(TLG));
	if(!ath->v) 
		{ free(ath); return NULL; }					//initializare tabela
	ath->M = M;
	ath->fh = fh;
	ath->fafc = fc;
	ath->felc = fe;
	ath->fcc = fcomp;

	return ath;
}

void AfiTH(TH* a, FILE *out)
{
	TLG p, el;
	int i;
	for ( i =0; i< a->M; i ++)
	{
	p = a->v[i];

	if( p == NULL)
				{fprintf(out,"[%d] :\n", i);			//parcurgere tabela pt afisare
			continue;}
	else
		{
			fprintf(out, "[%d] : ", i);

			for(el = p; el != NULL; el = el->urm)
				{
			
					TInfo *aux = (TInfo*)el->info;
					fprintf(out, "(");
					a->fafc(aux, out);
					fprintf(out, " -> ");
					aux->fafv(aux->info_val, out);				//castam informatia la TInfo pt a accesa mai usor campurile din info
					fprintf(out, ")");							//si apelam functiile de afisare 
					if(!(el->urm == NULL))
						fprintf(out, " ");
				}

			fprintf(out, "\n");
		}
	
	}


}

int InsLG(ALG al, void *ae)
{

	TLG p, ultim = NULL, aux;
	p = *al;
	aux = malloc(sizeof(TCelulaG));						//inserare la sfarsit de lista
	aux->info = ae;										//punem informatia sa pointeze la zona de memorie trimisa ca parametru

	while(p != NULL)
	{
		ultim = p;
		p = p->urm;
	}

	if(ultim) 
		ultim->urm = aux;									// daca lista nu e vida
	else 
		{
			aux->urm = *al;									//daca lista e vida
			*al = aux;
		}

	return 1;
}

int InsTH(TH *a, void *ae, char type,int dim)
{
	
	TInfo *aux2 = (TInfo*)ae;
	int cod, rez;
	if(type == 'd')
		cod = a->fh(aux2->info_cheie, sizeof(int), a->M);
	else if(type == 's')																//calculam codul hash
			cod = a->fh(aux2->info_cheie, sizeof(char)* dim, a->M);

	if(cod < 0) return 0;

	TLG el;

	for(el = a->v[cod]; el != NULL; el = el->urm)								//parcugem bucket-ul corespunzator codului
	{
		TInfo *aux = (TInfo*)el->info;											//castam info la TInfo
		if(!a->fcc(aux->info_cheie, aux2->info_cheie))							//verificam daca cheile sunt identice			
			{		
				aux->felv(aux->info_val);										//eliberam valoarea si cheia si apoi punem informatia
				a->felc(aux->info_cheie);										//sa pointeze la zona de memorie trimisa ca parametru
				free(aux);
				el->info = ae;
				return 2;														//returnam 2 pt a stii sa nu crestem numarul de intrari
			}
	}

	rez = InsLG(&a->v[cod], ae);												//apelam functia de inserare la sf de lista
	return rez;

}

TH* ResizeTH(TH* a, char type)
{
	TH* rez = (TH*)InitTH(2*(a->M), a->fh, a->fafc, a->felc, a->fcc);				//initializam o noua tabela cu dimensiunea dublata

	TLG p, el;
	int i, result;
	TInfo *aux;

	for(i = 0; i < a->M; i++)
	{
		p = a->v[i];

		if(p == NULL) continue;
		else for(el = p; el != NULL; el = el->urm)						//parcurgem tabela veche si inseram fiecare intrare in tabela noua
				{
					
					aux = (TInfo*)el->info;
					if(type == 's')
						result = InsTH(rez, aux, type, strlen((char*)aux->info_cheie));
					else result = InsTH(rez, aux, type, sizeof(int));
				}		

	}

	free(a->v);							//eliberam tabela veche											
	free(a);

	return rez;							//returnam rezultatul
}


void FindKey(TH *a, void *tofind, FILE *out)
{
	TLG p, el;
	int i;

	for(i = 0; i < a->M; i++)
	{
		p = a->v[i];

		if(p == NULL)
			continue;
		else for(el = p; el != NULL; el = el->urm)							//parcurgem tabela bucket cu bucket
				{
					TInfo *aux = (TInfo*)el->info;

					if(!a->fcc(aux->info_cheie, tofind))					//cand am gasit match pt cheie
					{	
						aux->fafv(aux->info_val, out);						//afisam si iesim din functie
						fprintf(out, "\n");
						return;
					}
					
				}
	}

	fprintf(out, "Nu exista\n");
}

int RemoveKey(TH *a, void *todelete, size_t d)
{
	int cod = a->fh(todelete, d, a->M);								//calculam codul bucketului unde se afla cheia de eliminat

	if(cod < 0) return 0;

	TLG p = a->v[cod];
	TLG ant = NULL;

	for(; p != NULL; ant = p, p = p->urm)						//parcurgem bucketul pana gasim cheia respectiva
	{
		TInfo *aux = (TInfo*)p->info;

		if(!a->fcc(todelete, aux->info_cheie))					//daca am gasit-o		
		{	
			TLG aux2 = p;																					

			if(ant == NULL)										//daca lista este vida modificam valoarea de la adresa listei 
				*(a->v+cod) = p->urm;
			else
			{
				ant->urm = aux2->urm;							//altfel refacem legaturile
			}

			free(aux2);											//eliberam celula pe care am eliminat-o
			return 1;
		}
	}

	return 0;
}


int main()
{
	FILE *in = fopen("input.in", "rt");								//deschidem fisierul de input
	char init[10], type;
	int M;

	if(!in) return -1;

	FILE *out = fopen("output.out", "wt");							//deschidem fisierul de output

	if(!out) return -1;

	TH *a = NULL;													//declaram tabela			

	fscanf(in, "%s %c %d", init, &type, &M);						//citim prima linie din fisier pentru a initializa tabela

	if(type == 's')
	{
		a = (TH*)InitTH(M, hash_f, afiKEYs, elibKEY, compareKEYSs);
	}
	else if(type == 'd')															//in functie de tipul cheii initializam tabela cu functiile corespunzatoare
	{
		a = (TH*)InitTH(M, hash_f, afiKEYd, elibKEY, compareKEYSd);
	}

	int nr_op;

	fscanf(in, "%d", &nr_op);														//citim nr de operatii

	int i, *key1 = NULL, rez, dim=0, k = 0;
	char operation[20], *key2 = NULL, type_insert[10];								//variabile folosite in implementarea operatiilor
	float threshold = 0.75;															//threshold-ul pt resize
	TInfo *inform = NULL;															//zona de memorie unde vom citi valorile si cheia

	for(i = 0; i < nr_op; i++)														//bucla pt a citi toate operatiile
	{
		fscanf(in, "%s", operation);												//citim operatia

		if(strcmp(operation, "insert") == 0)										//inserare
		{
			inform = malloc(sizeof(TInfo));

			if(!inform) return 1;											//alocam zona de memorie pt informatie

			key1 = malloc(sizeof(int));										//alocam zonele de memorie in care vom citi cheile

			if(!key1) return 1;

			key2 = malloc(50*sizeof(char));

			if(!key2) return 1;	

			if(type == 'd')
				fscanf(in, "%d", key1);												//citim cheia in functie de tipul ei	
			else fscanf(in, "%s", key2);

			fscanf(in, "%s", type_insert);											//vedem ce trebuie inserat

			if(strcmp(type_insert, "stud") == 0)									//daca trebuie introdus un student
			{
				TStudent *stud = malloc(sizeof(TStudent));							//alocam o zona de memorie pt un student

				if(!stud) return 1;								
																					
				stud->nume = malloc(50*sizeof(char));								//alocam numele			

				if(!stud->nume) return 1;

				fscanf(in, "%s %f %s %d", stud->nume, &stud->medie, stud->grupa, &stud->varsta);			//citim valorile

				inform->info_val = (void*)stud;						//le introducem in zona pt informatie
				
				if(type == 'd')													//in functie de tipul cheii
					{
						inform->info_cheie = malloc(sizeof(int));			//alocam cheia din informatie

						if(!inform->info_cheie) return 1;

						inform->info_cheie = key1;				//o intrducem in informatie

						dim = sizeof(int);						//dimensiunea pt functia hash
					}

				else {
						inform->info_cheie = malloc(50*sizeof(char));			//alocam cheia din informatie

						if(!inform->info_cheie) return 1;

						strcpy(inform->info_cheie ,key2);					//o copiem in zona pt cheie

					 	dim = strlen(key2);							//dimensiunea pt functia hash
					 }

				inform->fafv = afiVALs;
				inform->felv = elibVALs;							//functiile din informatie pt tipul student

				rez = InsTH(a, inform, type, dim);					//inserarea in tabela hash

				if(rez == 1)
					k++;											//daca inserarea a reusit crestem numarul de intrari	

			}
			else if(strcmp(type_insert, "mat") == 0)
				{
					TMaterie *mat = malloc(sizeof(TMaterie));				//alocam o zona de memorie pt o materie

					if(!mat) return 1;

					mat->materie = malloc(50*sizeof(char));					//alocam numele materiei

					if(!mat->materie) return 1;

					fscanf(in, "%s %d %d %d %d", mat->materie, &mat->ore_curs, &mat->ore_lab, &mat->credit, &mat->teme);		//citim valorile

					inform->info_val = (void*)mat;			//adaugam valorile in informatie

					if(type == 'd')									//in functie de tipul cheii
						{
							inform->info_cheie = malloc(sizeof(int));			//alocam spatiu pt cheia din informatie

							if(!inform->info_cheie) return 1;

							inform->info_cheie = key1;							//introducem cheia in informatie

							dim = sizeof(int);									//dimensiunea pt functia hash
						}
					else {
							inform->info_cheie = malloc(50*sizeof(char));		//alocam spatiu pt cheia din informatie	

							if(!inform->info_cheie) return 1;

							strcpy(inform->info_cheie, key2);					//o introducem in informatie

							dim = strlen(key2);									//dimensiunea pt functia hash		
						}

					inform->fafv = afiVALm;
					inform->felv = elibVALm;									//introducem in informatie functiile pt materie

					rez = InsTH(a, inform, type, dim);							//inseram in tabela hash

					if(rez == 1)												//daca am reusit crestem numarul de intrari
						k++;
			
				}
				

				if(((k / (float)a->M) >= threshold))							//dupa fiecare inserare verificam load factorul
				{
					a = (TH*)ResizeTH(a, type);									//daca s-a depasit se redimensioneaza tabela
				}

		}

		if(strcmp(operation, "print") == 0)										//daca comanda este print
		{
			AfiTH(a, out);														//se apeleaza afisarea

		}

		if(strcmp(operation, "find") == 0)										//daca operatia este find
		{
			if(type == 's')														//in functie de tipul cheii
			{
				char *keytofind1 = malloc(50*sizeof(char));						//alocam o cheie pt a citi din fisier

				if(!keytofind1) return 1;

				fscanf(in, "%s", keytofind1);									//citim cheia

				FindKey(a, keytofind1, out);									//apelam functia de cautare

				free(keytofind1);												//eliberam cheia
			}
			else if(type == 'd')
				{
					int *keytofind2 = malloc(sizeof(int));					//alocam o cheie pt a citi din fisier

					if(!keytofind2) return 1;										

					fscanf(in, "%d", keytofind2);							//citim cheia		

					FindKey(a, keytofind2, out);							//apelam functia de cautare

					free(keytofind2);										//eliberam cheia
				}


		}


		if(strcmp(operation, "delete") == 0)						//daca operatie este delete
		{

			int dimtoremove;										//dimensiunea cheii de eliminat

			if(type == 's')
			{
				char *keytodelete1 = malloc(50*sizeof(char));			//alocam o cheie pt a citi din fisier

				if(!keytodelete1) return 1;

				fscanf(in, "%s", keytodelete1);							//citim cheia din fisier	

				dimtoremove = strlen(keytodelete1);						//dimensiunea pt functia hash

				int removed = RemoveKey(a, keytodelete1, dimtoremove);		//apelam functia de delete

				free(keytodelete1);											//eliberam cheia

				if(removed) k--;											//daca am sters cu succes, micsoram numarul de intrari
			}
			else if(type == 'd')
				{
					int *keytodelete2 = malloc(sizeof(int));					//alocam o cheie pt a citi din fisier

					if(!keytodelete2) return 1;			

					fscanf(in, "%d", keytodelete2);										//citim cheia din fisier

					dimtoremove = sizeof(int);									//dimensiunea pt functia hash

					int removed = RemoveKey(a, keytodelete2, dimtoremove);			//apelam functia de delete

					free(keytodelete2);												//eliberam cheia			

					if(removed) k--;												//daca am sters cu succes, micsoram numarul de intrari
				}


		}


	}

	fclose(in);															//inchidem fisierele deschise
	fclose(out);

	return 0;

}