#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "bmp_header.h"

//definitii pt structuri

typedef struct {
	unsigned char r, g, b;
} pixel;

typedef struct {
	short row, col;
	unsigned char r, g, b;
} comp_pixel;

typedef struct {
	int row, col;
} position;

typedef struct bmp_fileheader fileheader;

typedef struct bmp_infoheader infoheader;

int get_padding(int width);

void apply_filter2(pixel **bitmap, fileheader fileh, infoheader infoh, char *bmp_image, char filter[4][4], char index);

char *filter_name(char *name, char filter[4]);

char *bw_name(char *name);

//functie pt citirea imaginii din fisierul bmp

void read_image(pixel ***bitmap, fileheader *fileh, infoheader *infoh, char *bmp_image)
{
	FILE *in;

	if((in = fopen(bmp_image, "rb")) == NULL)
		exit(EXIT_FAILURE);

	fread(fileh, sizeof(fileheader), 1, in);

	fread(infoh, sizeof(infoheader), 1, in);

	(*bitmap) = malloc(infoh->height * sizeof(pixel *));

	if((*bitmap) == NULL)
		exit(EXIT_FAILURE);

	int i, j;

	int padding = get_padding(infoh->width);

	for(i = 0; i < infoh->height; i++)
	{
		(*bitmap)[i] = malloc((infoh->width + padding) * sizeof(pixel));

		if((*bitmap)[i] == NULL)
		{
			for(j = 0; j < i; j++)
				free((*bitmap)[i]);

			free((*bitmap));

			exit(EXIT_FAILURE);
		}
	}

	fseek(in, fileh->imageDataOffset, SEEK_SET);

	for(i = 0; i < infoh->height; i++)
		for(j = 0; j < infoh->width; j++)
		{
			fread(&(*bitmap)[infoh->height - 1 -i][j].b, sizeof(unsigned char), 1, in);
			fread(&(*bitmap)[infoh->height - 1 -i][j].g, sizeof(unsigned char), 1, in);
			fread(&(*bitmap)[infoh->height - 1 -i][j].r, sizeof(unsigned char), 1, in);
		}

	fclose(in);
}

//functie pt aflarea paddingului
int get_padding(int width)
{
	if(width % 4 == 0)
		return 0;
	else return 4 - width % 4;
}

//functie pt scrierea imaginii

void write_image(pixel **bitmap, fileheader fileh, infoheader infoh, char *new_name)
{
	FILE *out;

	int i, j, k;

	if((out = fopen(new_name, "wb")) == NULL)
		exit(EXIT_FAILURE);

	fwrite(&fileh, sizeof(fileheader), 1, out);

	fwrite(&infoh, sizeof(infoheader), 1, out);

	for(i = ftell(out); i < fileh.imageDataOffset; i++)
		fputc(0, out);

	int padding = get_padding(infoh.width);

	for(i = infoh.height - 1; i>= 0; i--)
		for(j = 0; j < infoh.width + padding; j++)
			if(j > infoh.width - 1)
			{
				for(k = 0; k < 3; k++)
					fputc(0, out);
			}
			else
			{
				fwrite(&bitmap[i][j].b, sizeof(unsigned char), 1, out);
				fwrite(&bitmap[i][j].g, sizeof(unsigned char), 1, out);
				fwrite(&bitmap[i][j].r, sizeof(unsigned char), 1, out);
			}

	fclose(out);
}

//functie pt formarea imaginii alb-negru

void black_white(pixel **bitmap, fileheader fileh, infoheader infoh, char *bmp_image)
{
	int i, j;

	pixel **aux_bitmap = NULL;

	aux_bitmap = malloc(infoh.height * sizeof(pixel *));

	if(aux_bitmap == NULL)
		exit(EXIT_FAILURE);

	for(i = 0; i < infoh.height; i++)
	{
		aux_bitmap[i] = malloc(infoh.width * sizeof(pixel));

		if(aux_bitmap[i] == NULL)
		{
			for(j = 0; j < i; j++)
				free(aux_bitmap[i]);

			free(aux_bitmap);

			exit(EXIT_FAILURE);
		}
	}

	for(i = 0; i < infoh.height; i++)
		for(j = 0; j < infoh.width; j++)
			aux_bitmap[i][j] = bitmap[i][j];		//lucram cu o copie a matricii


	for(i = 0; i < infoh.height; i++)
		for(j = 0; j < infoh.width; j++)
		{
			unsigned char auxr, auxg, auxb;

			auxr = aux_bitmap[i][j].r;
			auxg = aux_bitmap[i][j].g;					//copii ale valorilor pixelilor
			auxb = aux_bitmap[i][j].b;


			aux_bitmap[i][j].r = (auxr + auxg + auxb) / 3;
			aux_bitmap[i][j].g = (auxr + auxg + auxb) / 3;
			aux_bitmap[i][j].b = (auxr + auxg + auxb) / 3;
		}

	//formarea numelui noii imagini	

	char *new_name = NULL;

	new_name = malloc(100 * sizeof(char));

	memset(new_name, 0, 100 * sizeof(char));

	strncpy(new_name, bmp_image, strlen(bmp_image) - 4);

	strcat(new_name, "_black_white.bmp");

	write_image(aux_bitmap, fileh, infoh, new_name);

	free(new_name);

	for(i = 0; i < infoh.height; i++)
		free(aux_bitmap[i]);

	free(aux_bitmap);
}

//functie pentru a crea din nou numele imaginii alb-negru

char *bw_name(char *name)
{
	char *bw_name = NULL;

	bw_name = malloc(100 * sizeof(char));

	memset(bw_name, 0, 100 * sizeof(char));

	strncpy(bw_name, name, strlen(name) - 4);

	strcat(bw_name, "_black_white.bmp");

	return bw_name;

}

//functie care creeaza copii ale matricii pt a aplica filtrele

void apply_filter1(pixel **bitmap, fileheader fileh, infoheader infoh, char *bmp_image)
{
	
	char f1[4][4] = { {-1, -1, -1}, {-1, 8, -1}, {-1, -1, -1} };
	char f2[4][4] = { {0, 1, 0}, {1, -4, 1}, {0, 1, 0} };
	char f3[4][4] = { {1, 0, -1}, {0, 0, 0}, {-1, 0, 1} };

	//copii ale matricii pt a putea aplica filtrele

	pixel **aux_bitmap1 = NULL;

	signed int auxh, auxw;

	auxh = infoh.height + 2;

	auxw = infoh.width + 2;

	aux_bitmap1 = malloc(auxh * sizeof(pixel *));

	if(aux_bitmap1 == NULL)
		exit(EXIT_FAILURE);

	int i, j;

	for(i = 0; i < auxh; i++)
	{
		aux_bitmap1[i] = calloc(auxw, sizeof(pixel));

		if(aux_bitmap1[i] == NULL)
		{
			for(j = 0; j < i; j++)
				free(aux_bitmap1[i]);

			free(aux_bitmap1);

			exit(EXIT_FAILURE);
		}
	}


	for(i = 1; i <= infoh.height; i++)
		for(j = 1; j <= infoh.width; j++)
			aux_bitmap1[i][j] = bitmap[i - 1][j - 1];

	apply_filter2(aux_bitmap1, fileh, infoh, bmp_image, f1, 1);
	

	for(i = 0; i < auxh; i++)
		free(aux_bitmap1[i]);

	free(aux_bitmap1);


	pixel **aux_bitmap2 = NULL;

	aux_bitmap2 = malloc(auxh * sizeof(pixel *));

	if(aux_bitmap2 == NULL)
		exit(EXIT_FAILURE);

	for(i = 0; i < auxh; i++)
	{
		aux_bitmap2[i] = calloc(auxw, sizeof(pixel));

		if(aux_bitmap2[i] == NULL)
		{
			for(j = 0; j < i; j++)
				free(aux_bitmap2[i]);

			free(aux_bitmap2);

			exit(EXIT_FAILURE);
		}
	}

	

	for(i = 1; i <= infoh.height; i++)
		for(j = 1; j <= infoh.width; j++)
			aux_bitmap2[i][j] = bitmap[i - 1][j - 1];

	apply_filter2(aux_bitmap2, fileh, infoh, bmp_image, f2, 2);

	for(i = 0; i < auxh; i++)
		free(aux_bitmap2[i]);

	free(aux_bitmap2);	

	pixel **aux_bitmap3 = NULL;

	aux_bitmap3 = malloc(auxh * sizeof(pixel *));

	if(aux_bitmap3 == NULL)
		exit(EXIT_FAILURE);

	for(i = 0; i < auxh; i++)
	{
		aux_bitmap3[i] = calloc(auxw, sizeof(pixel));

		if(aux_bitmap3[i] == NULL)
		{
			for(j = 0; j < i; j++)
				free(aux_bitmap3[i]);

			free(aux_bitmap3);

			exit(EXIT_FAILURE);
		}
	}

	for(i = 1; i <= infoh.height; i++)
		for(j = 1; j <= infoh.width; j++)
			aux_bitmap3[i][j] = bitmap[i - 1][j - 1];


	apply_filter2(aux_bitmap3, fileh, infoh, bmp_image, f3, 3);

	for(i = 0; i < auxh; i++)
		free(aux_bitmap3[i]);

	free(aux_bitmap3);

}															

//functie care aplica filtrul																																																													

void apply_filter2(pixel **bitmap, fileheader fileh, infoheader infoh, char *bmp_image, char filter[4][4], char index)
{
	int i, j;

	pixel **aux_bitmap = NULL;

	signed int auxh, auxw;

	auxh = infoh.height + 2;

	auxw = infoh.width + 2;

	aux_bitmap = malloc(auxh * sizeof(pixel *));

	if(aux_bitmap == NULL)
		exit(EXIT_FAILURE);

	for(i = 0; i < auxh; i++)
	{
		aux_bitmap[i] = malloc(auxw * sizeof(pixel));

		if(aux_bitmap[i] == NULL)
		{
			for(j = 0; j < i; j++)
				free(aux_bitmap[i]);

			free(aux_bitmap);

			exit(EXIT_FAILURE);
		}
	}

	for(i = 0; i < auxh; i++)
		for(j = 0; j < auxw; j++)
			aux_bitmap[i][j] = bitmap[i][j];

	//parcurgerea matricii pt algoritmul de aplicare al filtrului	

	for(i = 1; i <= infoh.height; i++)
		for(j = 1; j <= infoh.width; j++)
		{

			int new_pixelr, new_pixelg, new_pixelb;

			new_pixelr = 0;
			new_pixelg = 0;
			new_pixelb = 0;

			new_pixelr += aux_bitmap[i - 1][j - 1].r * filter[0][0] + aux_bitmap[i - 1][j].r * filter[0][1] + aux_bitmap[i - 1][j + 1].r * filter[0][2];
			new_pixelr += aux_bitmap[i][j - 1].r * filter[1][0] + aux_bitmap[i][j].r * filter[1][1] + aux_bitmap[i][j + 1].r * filter[1][2];
			new_pixelr += aux_bitmap[i + 1][j - 1].r * filter[2][0] + aux_bitmap[i + 1][j].r * filter[2][1] + aux_bitmap[i + 1][j + 1].r * filter[2][2];

			new_pixelg += aux_bitmap[i - 1][j - 1].g * filter[0][0] + aux_bitmap[i - 1][j].g * filter[0][1] + aux_bitmap[i - 1][j + 1].g * filter[0][2];
			new_pixelg += aux_bitmap[i][j - 1].g * filter[1][0] + aux_bitmap[i][j].g * filter[1][1] + aux_bitmap[i][j + 1].g * filter[1][2];
			new_pixelg += aux_bitmap[i + 1][j - 1].g * filter[2][0] + aux_bitmap[i + 1][j].g * filter[2][1] + aux_bitmap[i + 1][j + 1].g * filter[2][2];

			new_pixelb += aux_bitmap[i - 1][j - 1].b * filter[0][0] + aux_bitmap[i - 1][j].b * filter[0][1] + aux_bitmap[i - 1][j + 1].b * filter[0][2];
			new_pixelb += aux_bitmap[i][j - 1].b * filter[1][0] + aux_bitmap[i][j].b * filter[1][1] + aux_bitmap[i][j + 1].b * filter[1][2];
			new_pixelb += aux_bitmap[i + 1][j - 1].b * filter[2][0] + aux_bitmap[i + 1][j].b * filter[2][1] + aux_bitmap[i + 1][j + 1].b * filter[2][2];

			if(new_pixelr > 255)
				new_pixelr = 255;
			else if(new_pixelr < 0)
					new_pixelr = 0;

			if(new_pixelg > 255)
				new_pixelg = 255;
			else if(new_pixelg < 0)
					new_pixelg = 0;

			if(new_pixelb > 255)
				new_pixelb = 255;
			else if(new_pixelb < 0)
					new_pixelb = 0;

			bitmap[i][j].r = (unsigned char)new_pixelr;
			bitmap[i][j].g = (unsigned char)new_pixelg;
			bitmap[i][j].b = (unsigned char)new_pixelb;

		}

		for(i = 0; i < auxh; i++)
			free(aux_bitmap[i]);

		free(aux_bitmap);

		aux_bitmap = malloc(infoh.height * sizeof(pixel *));

		if(aux_bitmap == NULL)
			exit(EXIT_FAILURE);

		for(i = 0; i < infoh.height; i++)
		{
			aux_bitmap[i] = malloc(infoh.width * sizeof(pixel));

			if(aux_bitmap[i] == NULL)
			{
				for(j = 0; j < i; j++)
					free(aux_bitmap[i]);

				free(aux_bitmap);

				exit(EXIT_FAILURE);
			}
		}

		for(i = 1; i <= infoh.height; i++)
			for(j = 1; j <= infoh.width; j++)
				aux_bitmap[i - 1][j - 1] = bitmap[i][j];


		if(index == 1)
		{
			char *new_name = filter_name(bmp_image, "_f1");
			write_image(aux_bitmap, fileh, infoh, new_name);
			free(new_name);
		}

		if(index == 2)
		{
			char *new_name = filter_name(bmp_image, "_f2");
			write_image(aux_bitmap, fileh, infoh, new_name);
			free(new_name);
		}

		if(index == 3)
		{
			char *new_name = filter_name(bmp_image, "_f3");
			write_image(aux_bitmap, fileh, infoh, new_name);
			free(new_name);
		}

		for(i = 0; i < infoh.height; i++)
			free(aux_bitmap[i]);

		free(aux_bitmap);

}

//functie care creeaza numele pt filtru

char *filter_name(char *name, char filter[4])
{

	char *filter_name = NULL;

	filter_name = malloc(100 * sizeof(char));

	memset(filter_name, 0, 100 * sizeof(char));

	strncpy(filter_name, name, strlen(name) - 16);

	strcat(filter_name, filter);

	strcat(filter_name, ".bmp");

	return filter_name;
}

//compresia imaginii (matrice de zone + vectori de pozitii)

void compress_image(pixel **bitmap, fileheader fileh, infoheader infoh, int threshold)
{
	pixel **aux_bitmap = NULL;

	signed int auxh, auxw;

	auxh = infoh.height + 2;

	auxw = infoh.width + 2;

	aux_bitmap = malloc(auxh * sizeof(pixel *));

	if(aux_bitmap == NULL)
		exit(EXIT_FAILURE);

	short i, j;

	for(i = 0; i < auxh; i++)
	{
		aux_bitmap[i] = calloc(auxw, sizeof(pixel));

		if(aux_bitmap[i] == NULL)
		{
			for(j = 0; j < i; j++)
				free(aux_bitmap[i]);

			free(aux_bitmap);

			exit(EXIT_FAILURE);
		}
	}


	for(i = 1; i <= infoh.height; i++)
		for(j = 1; j <= infoh.width; j++)
			aux_bitmap[i][j] = bitmap[i - 1][j - 1];

	int **compressed_zones = NULL;

	compressed_zones = malloc(auxh * sizeof(int *));

	if(compressed_zones == NULL)
		exit(EXIT_FAILURE);

	for(i = 0; i < auxh; i++)
	{
		compressed_zones[i] = calloc(auxw, sizeof(int));

		if(compressed_zones[i] == NULL)
		{
			for(j = 0; j < i; j++)
				free(compressed_zones[i]);

			free(compressed_zones);

			exit(EXIT_FAILURE);
		}
	}

	//matrice de zone

	for(j = 0; j < auxw; j++)
	{
		compressed_zones[0][j] = -1;
		compressed_zones[auxh - 1][j] = -1;
	}

	for(i = 0; i < auxh; i++)
	{
		compressed_zones[i][0] = -1;
		compressed_zones[i][auxw - 1] = -1;
	}


	//aflam zonele de pixeli, punand intr-un vetor pixelul original

	int zone_index = 1;

	pixel *originals = NULL;

	originals = malloc(infoh.width * infoh.height * sizeof(pixel));

	int h = 0;

	for(i = 1; i <= infoh.height; i++)
		for(j = 1; j <= infoh.width; j++)
		{
			if(compressed_zones[i][j] == 0)
			{
				position *zone = malloc(infoh.width * infoh.height * sizeof(position));

				if(zone == NULL)
					exit(EXIT_FAILURE);

				int k = 0;

				int left = 1;

				int w = 0; 

				zone[k].row = i;
				zone[k].col = j;

				compressed_zones[i][j] = zone_index;

				originals[h++] = aux_bitmap[i][j];

				//parcurgem vectorul de pixeli

				while(left)
				{

					if(compressed_zones[zone[w].row - 1][zone[w].col] == 0)
					{
						if((abs(aux_bitmap[zone[0].row][zone[0].col].r - aux_bitmap[zone[w].row - 1][zone[w].col].r) 
							+ abs(aux_bitmap[zone[0].row][zone[0].col].g - aux_bitmap[zone[w].row - 1][zone[w].col].g)
							+ abs(aux_bitmap[zone[0].row][zone[0].col].b - aux_bitmap[zone[w].row - 1][zone[w].col].b)) 
							<= threshold)
						{
							compressed_zones[zone[w].row - 1][zone[w].col] = zone_index;

							k++;

							zone[k].row = zone[w].row - 1;
							zone[k].col = zone[w].col;

							left++;

						}
					}

					if(compressed_zones[zone[w].row][zone[w].col + 1] == 0)
					{
						if((abs(aux_bitmap[zone[0].row][zone[0].col].r - aux_bitmap[zone[w].row][zone[w].col + 1].r) 
							+ abs(aux_bitmap[zone[0].row][zone[0].col].g - aux_bitmap[zone[w].row][zone[w].col + 1].g)
							+ abs(aux_bitmap[zone[0].row][zone[0].col].b - aux_bitmap[zone[w].row][zone[w].col + 1].b)) 
							<= threshold)
						{
							compressed_zones[zone[w].row][zone[w].col + 1] = zone_index;

							k++;

							zone[k].row = zone[w].row ;
							zone[k].col = zone[w].col + 1;

							left++;

						}
					}

					if(compressed_zones[zone[w].row][zone[w].col - 1] == 0)
					{
						if((abs(aux_bitmap[zone[0].row][zone[0].col].r - aux_bitmap[zone[w].row][zone[w].col - 1].r) 
							+ abs(aux_bitmap[zone[0].row][zone[0].col].g - aux_bitmap[zone[w].row][zone[w].col - 1].g)
							+ abs(aux_bitmap[zone[0].row][zone[0].col].b - aux_bitmap[zone[w].row][zone[w].col - 1].b)) 
							<= threshold)
						{
							compressed_zones[zone[w].row][zone[w].col - 1] = zone_index;

							k++;

							zone[k].row = zone[w].row;
							zone[k].col = zone[w].col - 1;

							left++;

						}
					}

					if(compressed_zones[zone[w].row + 1][zone[w].col] == 0)
					{
						if((abs(aux_bitmap[zone[0].row][zone[0].col].r - aux_bitmap[zone[w].row + 1][zone[w].col].r) 
							+ abs(aux_bitmap[zone[0].row][zone[0].col].g - aux_bitmap[zone[w].row + 1][zone[w].col].g)
							+ abs(aux_bitmap[zone[0].row][zone[0].col].b - aux_bitmap[zone[w].row + 1][zone[w].col].b)) 
							<= threshold)
						{
							compressed_zones[zone[w].row + 1][zone[w].col] = zone_index;

							k++;

							zone[k].row = zone[w].row + 1;
							zone[k].col = zone[w].col;

							left++;


						}
					}

					w++;
					left--;
				
				}

				zone_index++;

				free(zone);


			}
		}


	FILE *out;

	if((out = fopen("compressed.bin", "wb")) == NULL)
		exit(EXIT_FAILURE);

	fwrite(&fileh, sizeof(fileheader), 1, out);

	fwrite(&infoh, sizeof(infoheader), 1, out);

	for(i = ftell(out); i < fileh.imageDataOffset; i++)
		fputc(0, out);

	printf("%ld", ftell(out));

	//scriem zonele in fisier

	for(i = 1; i <= infoh.height; i++)
		for(j = 1; j <= infoh.width; j++)
		{
			if(i == 1)
			{

				fwrite(&i, sizeof(short), 1, out);
				fwrite(&j, sizeof(short), 1, out);
				fwrite(&originals[compressed_zones[i][j] - 1], sizeof(pixel), 1, out);

				continue;
			}
			else if(i == infoh.height)
				{
					fwrite(&i, sizeof(short), 1, out);
					fwrite(&j, sizeof(short), 1, out);
					fwrite(&originals[compressed_zones[i][j] - 1], sizeof(pixel), 1, out);

					continue;
				}
				else if(j == 1)
					{
						fwrite(&i, sizeof(short), 1, out);
						fwrite(&j, sizeof(short), 1, out);
						fwrite(&originals[compressed_zones[i][j] - 1], sizeof(pixel), 1, out);

						continue;
					}
					else if(j == infoh.width)
						{
							fwrite(&i, sizeof(short), 1, out);
							fwrite(&j, sizeof(short), 1, out);
							fwrite(&originals[compressed_zones[i][j] - 1], sizeof(pixel), 1, out);

							continue;
						}

			if(compressed_zones[i - 1][j] != compressed_zones[i][j])
			{
				fwrite(&i, sizeof(short), 1, out);
				fwrite(&j, sizeof(short), 1, out);
				fwrite(&originals[compressed_zones[i][j] - 1], sizeof(pixel), 1, out);
			}
			else if(compressed_zones[i][j - 1] != compressed_zones[i][j])
				{
					fwrite(&i, sizeof(short), 1, out);
					fwrite(&j, sizeof(short), 1, out);
					fwrite(&originals[compressed_zones[i][j] - 1], sizeof(pixel), 1, out);
				}
				else if(compressed_zones[i][j + 1] != compressed_zones[i][j])
					{
						fwrite(&i, sizeof(short), 1, out);
						fwrite(&j, sizeof(short), 1, out);
						fwrite(&originals[compressed_zones[i][j] - 1], sizeof(pixel), 1, out);
					}
					else if(compressed_zones[i + 1][j] != compressed_zones[i][j])
						{
							fwrite(&i, sizeof(short), 1, out);
							fwrite(&j, sizeof(short), 1, out);
							fwrite(&originals[compressed_zones[i][j] - 1], sizeof(pixel), 1, out);
						}


		}

		fclose(out);

		for(i = 0; i < auxh; i++)
		{
			free(aux_bitmap[i]);
			free(compressed_zones[i]);
		}

		free(aux_bitmap);
		free(compressed_zones);
		free(originals);



}

//functie pt decompresie

void decompress(char *comp_image)
{
	int i, j;

	FILE *in;

	if((in = fopen(comp_image, "rb")) == NULL)
		exit(EXIT_FAILURE);

	fileheader fileh;
	infoheader infoh;

	fread(&fileh, sizeof(fileheader), 1, in);
	fread(&infoh, sizeof(infoheader), 1, in);

	pixel **bitmap = NULL;

	bitmap = malloc(infoh.height * sizeof(pixel *));

	for(i = 0; i < infoh.height; i++)
	{
		bitmap[i] = malloc(infoh.width * sizeof(pixel));

		if(bitmap[i] == NULL)
		{
			for(j = 0; j < i; j++)
				free(bitmap[i]);
			
			free(bitmap);

			exit(EXIT_FAILURE);

		}
	}

	//matrice de pixeli colorati

	int **colored_pixels = NULL;

	colored_pixels = malloc(infoh.height * sizeof(int *));

	for(i = 0; i < infoh.height; i++)
	{
		colored_pixels[i] = calloc(infoh.width, sizeof(int));

		if(colored_pixels[i] ==  NULL)
		{
			for(j = 0; j < i; j++)
				free(colored_pixels[i]);

			free(colored_pixels);

			exit(EXIT_FAILURE);
		}
	}

	fseek(in, fileh.imageDataOffset, SEEK_SET);

	//citim pixelii din fisier si ii punem in matricea de pixeli si de zone

	while(feof(in) == 0)
	{
		comp_pixel pix;

		fread(&pix.row, sizeof(short), 1, in);
		fread(&pix.col, sizeof(short), 1, in);
		fread(&pix.r, sizeof(unsigned char), 1, in);
		fread(&pix.g, sizeof(unsigned char), 1, in);
		fread(&pix.b, sizeof(unsigned char), 1, in);


		bitmap[pix.row - 1][pix.col - 1].r = pix.r;
		bitmap[pix.row - 1][pix.col - 1].g = pix.g;
		bitmap[pix.row - 1][pix.col - 1].b = pix.b;

		colored_pixels[pix.row - 1][pix.col - 1] = 1;
	}

	//verificam pixelii care nu au culoare si ii coloram pe restul

	for(i = 0; i < infoh.height; i++)
		for(j = 0; j< infoh.width; j++)
		{
			if(colored_pixels[i][j] == 0)
			{
				if(colored_pixels[i - 1][j] == 1)
				{
					bitmap[i][j] = bitmap[i - 1][j];
					colored_pixels[i][j] = 1;
				}

				if(colored_pixels[i][j - 1] == 1)
				{
					bitmap[i][j] = bitmap[i][j - 1];
					colored_pixels[i][j] = 1;
				}

				if(colored_pixels[i][j + 1] == 1)
				{
					bitmap[i][j] = bitmap[i][j + 1];
					colored_pixels[i][j] = 1;
				}

				if(colored_pixels[i + 1][j] == 1)
				{
					bitmap[i][j] = bitmap[i + 1][j];
					colored_pixels[i][j] = 1;
				}
			}
			else continue;
		}

	write_image(bitmap, fileh, infoh, "decompressed.bmp");

	fclose(in);

	for(i = 0; i < infoh.height; i++)
	{
		free(bitmap[i]);
		free(colored_pixels[i]);
	}

	free(bitmap);
	free(colored_pixels);

}

int main(void)
{
	FILE *in;

	if((in = fopen("input.txt", "rt")) == NULL)
		return 1;

	char *bmp_image = NULL;

	bmp_image = malloc(100 * sizeof(char));

	fscanf(in, "%s", bmp_image);

	pixel **bitmap = NULL;

	fileheader fileh;

	infoheader infoh;

	read_image(&bitmap, &fileh, &infoh, bmp_image);

	//facem imaginea alb-negru

	black_white(bitmap, fileh, infoh, bmp_image);

	char *bw = bw_name(bmp_image);

	pixel **bw_bitmap = NULL;

	fileheader bw_fileh;

	infoheader bw_infoh;

	//citim imaginea alb-negru pt filtre

	read_image(&bw_bitmap, &bw_fileh, &bw_infoh, bw);

	apply_filter1(bw_bitmap, bw_fileh, bw_infoh, bw);

	int threshold;

	fscanf(in, "%d", &threshold);

	//comprimam imaginea

	compress_image(bitmap, fileh, infoh, threshold);

	char *decompress_file = NULL;

	decompress_file = malloc(100 * sizeof(char));

	fscanf(in, "%s", decompress_file);

	//decompresie de imagine

	decompress(decompress_file);

	int i;

	for(i = 0; i < infoh.height; i++)
	{
		free(bitmap[i]);
		free(bw_bitmap[i]);
	}

	free(bitmap);
	free(bw_bitmap);
	free(bmp_image);
	free(decompress_file);

	fclose(in);	

	return 0;
}