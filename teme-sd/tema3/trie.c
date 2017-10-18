#include "trie.h"
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdio.h>

TTrie initialize_node()
{
	TTrie node = NULL;
	node = malloc(sizeof(TNod));  //alocare nod + testare

	if(!node)
		return NULL;

	int i;

	for(i = 0; i < 36; i++)
		node->children[i] = NULL;		//initializare copii 

	node->isWord = 0;			//variabila pentru frunza setata automat la 0

	return node;
}


void add_word(TTrie root, char *word)
{
	int i;
	TTrie aux = root;			//variabila auxiliara pentru parcurgerea trie-ului

	for(i = 0; i < strlen(word); i++)		//pentru fiecare litera din cuvantul de adaugat
	{
		if(isdigit(word[i]))			//verificam daca e litera sau cifra pentru a putea stii cum sa calculam indexul
		{
			if(aux->children[(int)word[i] - '0'] == NULL)		//daca pointerul corespunzator literei la care ne aflam e NULL
				aux->children[(int)word[i] - '0'] = initialize_node();	//initializam nodul

			aux = aux->children[(int)word[i] - '0'];	//ne mutam pe urmatorul nod
		}
		else if(isalpha(word[i]))
			{
				if(aux->children[(int)word[i] - 'a' + 10] == NULL)
					aux->children[(int)word[i] - 'a' + 10] = initialize_node();

				aux = aux->children[(int)word[i] - 'a' + 10];
			}
	}

	aux->isWord = 1;		//la final setam variabila pt frunza din ultimul nod la 1 
}

int find(TTrie root, char *word)
{
	TTrie aux = root;		//parcurgere trie
	int i, index;

	if(root == NULL)		//daca trie-ul e gol
		return 0;

	for(i = 0; i < strlen(word); i++)	//parcurgem cuvantul
	{
		if(isdigit(word[i]))			//calculam indexul
			index = (int)word[i] - '0';
		else if(isalpha(word[i]))
				index = (int)word[i] - 'a' + 10;

		if(aux->children[index] == NULL)	//daca lipseste o litera ne oprim si retunam 0 => False
			return 0;

		aux = aux->children[index];		//avansam in trie 
	}

	if(aux)
		if(aux->isWord)		//daca am ajuns la o frunza
			return 1;		//returnam 1 => True

	return 0;	//returnam 0 daca am terminat  de parcurs cuvantul si nu am ajuns la o frunza
}

int numberOfChildren(TTrie node)
{
	int i, nr = 0;		//nr de copii ai unui nod

	for(i = 0; i < 36; i++)
		if(node->children[i])		//parcurgem copiii nodului si ii numaram
			nr++;

	return nr;		//returnam nr obtinut

}

int delete(TTrie node, char *word)
{
	int index;

	
	if(strlen(word) == 0)			//daca am ajuns la finalul cuvantului setam variabila pentru frunza la 0
		node->isWord = 0;
	else 
		{
			if(isdigit(word[0]))				//calculam indexul
				index = (int)word[0] - '0';
			else if(isalpha(word[0]))
					index = (int)word[0] - 'a' + 10;


			if(node->children[index] && delete(node->children[index], word+1))	//avansam in trie daca exista copil pentru litera la care suntem
				node->children[index] = NULL;									//si apelam din nou functia cu word + 1, avansand o litera in string
		}

	if(node != NULL && numberOfChildren(node) == 0 && node->isWord == 0) //odata ajunsi la finalul cuvantului urcam recursiv in trie si daca
	{																	//nodul la care suntem exista, nu are copii si nu e frunza il eliberam 
		free(node);														//returnam 1 pentru a putea stii daca trebuie sa facem pointerul null mai sus^
		return 1;
	}

	return 0;			//daca nu sunt indeplinite cele 3 conditii de mai sus returnam 0 si nu este eliberat nimic
}

char *find_longest_prefix(TTrie root, char *word)
{
	int n = 100;		//lungime initiala sir
	TTrie aux = root;		//parcurgere trie
	char *result = calloc(n , sizeof(char));	//alocare string pentru rezultat + testare

	if(!result)
		return NULL;

	int i, index, len = 0;	//lungimea stringului

	for(i = 0; i < strlen(word); i++)		//parcurgem stringul primit ca parametru
	{
		if(isdigit(word[i]))			//calculam indexul
				index = (int)word[i] - '0';
		else if(isalpha(word[i]))
				index = (int)word[i] - 'a' + 10;

		if(aux->children[index])		//daca copilul exista
		{

			if(len > strlen(result))		//daca am depasit lungimea sirului realocam
			{
				result = realloc(result, 2 * n * sizeof(char));
				n = 2 * n;
			}

			result[len++] = word[i];	//punem la finalul stringului litera corespunzatoare
				
		}
		else break;	//altfel ne oprim

		aux =  aux->children[index];	//avansam in trie
	}

	if(strlen(result) > 0)		//daca am obtinut un string
		result[len++] = '\0';	//ii punem null terminator la final
	else strcpy(result, "None\n");	//altfel copiem in el None deoarece nu s-a gasit niciun cuvant

	return result;		//returnam stringul
}

void print_word(TTrie node, char *prefix, FILE *out, char *word, int len)
//functia construieste cuvintele din trie pornind de la prefixul prefix, len e lungimea cuvantului(la primul apel e 0), word e stringul in care se pune sufixul
{
	

	if(!node)		//daca nodul la care am ajuns e null
		return;			

	if(node->isWord == 1)		//daca am ajuns la o frunza
	{
		word[len] = '\0';		//punem null terminator la cuvantul obtinut
		if(strlen(prefix) == 0)		//daca prefixul este de fapt sirul vid
			fprintf(out, "%s ", word);		//printam direct cuvantul obtinut
		else 
			{
				fprintf(out, "%s%s ", prefix, word);		//altfel printam prefixul si sufixul obtinut unul langa altul
			}	

	}

	int i;

	for(i = 0; i < 36; i++)		//parcurgem copiii
	{
		if(node->children[i])		//daca copilul exista
		{
			char c;

			if(i <= 9)		//daca avem cifra
			{
				c = i + '0';	//calculam caracterul

				word[len] = c;		//il punem la finalul stringului

				print_word(node->children[i], prefix, out, word, len + 1);	//apelam din nou functia pt copil cu acelasi prefix, stringul nou format, si lungimea + 1
			}
			else 
				{
					c = i + 'a' - 10;		//acelasi lucru dar pt litere

					word[len] = c;

					print_word(node->children[i], prefix, out, word, len + 1);
				}
		}
	}


}


void find_all_with_prefix(TTrie root, char *prefix, FILE *out)
{
	TTrie aux = root;		//parcurgem trie

	int i, index;

	for(i = 0; i < strlen(prefix); i++)		//ne ducem pana la nodul in care se termina prefixul
	{
		if(isdigit(prefix[i]))			//calculam indicele
				index = (int)prefix[i] - '0';
		else if(isalpha(prefix[i]))
				index = (int)prefix[i] - 'a' + 10;

		if(!aux->children[index])		//daca o litera nu se potriveste printam None deoarece prefixul trebuie sa fie exact
		{	
			fprintf(out, "None\n");
			return;
		}

		aux = aux->children[index];		//avansam in trie
	}

	char *word = malloc(5000 * sizeof(char));		//string in care se va construi fiecare cuvant din trie

	print_word(aux, prefix, out, word, 0);			//functia care construieste si printeaza cuvintele din trie pornind de la prefix, 0 reprezinta lungimea stringului de mai sus ^

	fprintf(out, "\n");				

	free(word);			//eliberare memorie alocata
}

void mean_length_helper(TTrie node, char *prefix, char *word, int *sum, int *nr, int len)
/*acelasi principiu ca la functia de construire si printare a cuvintelor din word de la trie, 
numai ca adunam lungimile si numaram cuvintele, transmitandu-le prin efect lateral*/
{
	if(node == NULL)		//daca nodul e nul
		return;

	if(node->isWord)		//daca am ajuns la o frunza
	{
		(*nr)++;			//crestem numarul de cuvinte care incep cu prefixul prefix

		word[len] = '\0';	//punem null terminator la stringul creat

		if(strlen(prefix))		//daca nu am apelat cu sirul vid
		{
			(*sum) += strlen(prefix) + strlen(word);	//adaugam la suma literelor lungimea prefixului si lungimea sufixului aka stringul creat
		}
		else (*sum) += strlen(word);		//altfel doar lungimea stringului creat(este tot cuvantul in acest caz)
			
	}

	int i;

	for(i = 0; i < 36; i++)		//parcurgem copiii nodului la care suntem
	{
		if(node->children[i])		//daca copilul exista
		{
			char c;

			if(i <= 9)		//daca e cifra
			{
				c = i + '0';		//calculam caracterul

				word[len] = c;		//il punem la finalul stringului pe care il formam

				mean_length_helper(node->children[i], prefix, word, sum, nr, len + 1); 
					/*apelam functia din nou pentru copil, cu acelasi, prefix, noul string, cei 2 pointeri pt efect lateral
					si lungimea + 1*/
			}
			else
				{
					c = i + 'a' - 10;	//acelasi lucru dar pt litere

					word[len] = c;

					mean_length_helper(node->children[i], prefix, word, sum, nr, len + 1);
				}
			
		}
	}

}


void mean_length(TTrie root, char *prefix, FILE *out)
{
	TTrie aux = root;		//parcurgem trie

	int i, index;

	for(i = 0; i < strlen(prefix); i++)		//ne ducem pana la nodul in care se termina prefixul
	{
		if(isdigit(prefix[i]))		//calculam indexul
				index = (int)prefix[i] - '0';
		else if(isalpha(prefix[i]))
				index = (int)prefix[i] - 'a' + 10;

		if(!aux->children[index])		//daca o litera nu se potriveste printam 0, prefixul trebuie sa fie exact
		{
			fprintf(out, "0\n");
			return;
		}

		aux = aux->children[index];		//avansam in trie

	}

	int *sum = malloc(sizeof(int));		//alocam 2 pointeri la int si initializam valorile de la adresele respective cu 0
	int *nr = malloc(sizeof(int));
	*sum = 0;
	*nr = 0;

	char *word = malloc(5000 * sizeof(char));		//alocam un string pentru formarea cuvintelor

	mean_length_helper(aux, prefix, word, sum, nr, 0);		//apelam functia cu prefixul dorit, stringul in care vom crea cuvintele, cele 2 valori pt efect lateral si lungimea initiala 0

	float result = (*sum) / (float)(*nr);		//calculam rezultatul care ni se cere castand impartitorul la float

	fprintf(out, "%.3f\n", result);			//printam in fisier rezultatul

	free(word);
	free(nr);		//eliberam memoria
	free(sum);

}

void erase_trie(TTrie *root)
{
	if(!root)		//daca nodul nu exista
		return;			

	int i;

	for(i = 0; i < 36; i++)		//parcurgem copiii
		if((*root)->children[i])		//daca copilul exista
			erase_trie(&(*root)->children[i]);	//avansam in trie

	free((*root));		//la final urcam in trie eliberand pe parcurs nodurile
}


