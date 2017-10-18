/*STANESCU Mihai-Florin 314CB*/
#include <stdio.h>
#include "stack.h"
#include "queue.h"
#include <stdlib.h>
#include <string.h>



int main(int argc, char *argv[])
{

	FILE *in = fopen(argv[1], "rt");
	FILE *out = fopen(argv[2], "wt");				//deschidere fisiere

	if(!in) return 1;

	if(!out) return 1;

	int nr_op, nrs, nrc;

	fscanf(in, "%d %d %d", &nr_op, &nrs, &nrc);		//citire prima linie

	void **s = NULL, **c = NULL;
	s = malloc(nrs * sizeof(void *));						//alocare vectori de cozi si stive
	c = malloc(nrc * sizeof(void *));
 	int i, id;

	for(i = 0; i < nrs; i++)
		s[i] = InitS();												//initializari

	for(i = 0; i < nrc; i++)
		c[i] = InitQ();

	char operation[20];
	TInfo *informatie = NULL, *deleted = NULL;

	for(i = 0; i < nr_op; i++)
	{
		fscanf(in, "%s", operation);

		if(strcmp(operation, "push") == 0)
		{

			informatie = malloc(sizeof(TInfo));

			if(!informatie) return 1;

			fscanf(in, "%d", &id);

			fscanf(in, "%d %c", &informatie->id_paranteza, &informatie->type);

			Push(s[id], informatie);
		}

		if(strcmp(operation, "pop") == 0)
		{
			
			fscanf(in, "%d", &id);

			deleted = Pop(s[id]);

			free(deleted);

		}

		if(strcmp(operation, "prints") == 0)
		{
			PrintS(s, out, nrs);
		}

		if(strcmp(operation, "sorts") == 0)
		{
			fscanf(in, "%d", &id);
			s[id] = SortS(s[id]);
		}

		if(strcmp(operation, "intrq") == 0)
		{
			informatie = malloc(sizeof(TInfo));

			if(!informatie) return 1;

			fscanf(in, "%d", &id);

			fscanf(in, "%d %c", &informatie->id_paranteza, &informatie->type);

			IntrQ(c[id], informatie);
		}

		if(strcmp(operation, "extrq") == 0)
		{
			fscanf(in, "%d", &id);

			deleted = ExtrQ(c[id]);

			free(deleted);
		}

		if(strcmp(operation, "printq") == 0)
		{
			PrintQ(c, out, nrc);
		}

		if(strcmp(operation, "sortq") == 0)
		{
			fscanf(in, "%d", &id);
			SortQ(c[id]);
		}

		if(strcmp(operation, "corrects") == 0)
		{
			fscanf(in, "%d", &id);
			fprintf(out, "%d\n", CorrectS(s[id]));
		}

		if(strcmp(operation, "correctq") == 0)
		{
			fscanf(in, "%d", &id);
			fprintf(out, "%d\n", CorrectQ(c[id]));
		}

	}

	fclose(in);
	fclose(out);

	return 0;

}