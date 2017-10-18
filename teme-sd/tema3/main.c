#include "trie.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char* argv[])
{
	FILE *in, *out;
	TTrie root = initialize_node();  //initializare trie

	in = fopen(argv[1], "rt");		//deschidere fisier de input + testare

	if(!in)
		return 1;

	out = fopen(argv[2], "wt");		//deschidere fisier de output + testare

	if(!out)
		return 1;

	int n;
	
	
	char *empty = malloc(100 * sizeof(char));	//string pentru golirea bufferului dupa ce citim numarul de operatii
	char *command; 					//pointeri pentru parsarea cu strtok a fisierului de input
	char *argument; 
	

	fscanf(in, "%d", &n);				//citirea numarului de operatii
	fgets(empty, 100, in);			//golirea bufferului

	int i;

	for(i = 0; i < n; i++)
	{
		
		char *line = malloc(5000 * sizeof(char));	//string pentru citirea liniei

		fgets(line, 5000, in);		//citirea efectiva din fisier
		command = strtok(line, " \n");		//parsarea liniei
		argument = strtok(NULL, " \n");		//comanda + argument

		if(strcmp("add", command) == 0)		//add
		{
			add_word(root, argument);
		}

		if(strcmp("find", command) == 0)		//find
		{
			int ok = find(root, argument);

			if(ok == 1)
				fprintf(out, "True\n");
			else fprintf(out, "False\n");
		}

		if(strcmp("remove", command) == 0)		//remove
		{
			delete(root, argument);
		}

		if(strcmp("find_longest_prefix", command) == 0)		//find_longest_prefix
		{
			char *result;	//pointer pt rezultatul intors de functie

			result = find_longest_prefix(root, argument);

			fprintf(out, "%s\n", result);

			free(result);	//eliberare memorie alocata
		}

		if(strcmp("find_all_with_prefix", command) == 0)		//find_all_with_prefix
		{
			if(argument)						//daca argumentul nu e sirul vid
				find_all_with_prefix(root, argument, out);
			else find_all_with_prefix(root, "", out);
		}

		if(strcmp("mean_length", command) == 0)			//mean_length
		{
			if(argument)				//daca argumentul nu e sirul vid
				mean_length(root, argument, out);	
			else mean_length(root, "", out);
		}

		free(line);						//eliberarea memoriei alocate pt linie
	}

	fclose(in);					//inchiderea fisierelor
	fclose(out);

	free(empty);			//eliberarea memoriei pentru stringul in care am golit bufferul

	erase_trie(&root);		//eliberarea memoriei pt trie

	return 0;

}


