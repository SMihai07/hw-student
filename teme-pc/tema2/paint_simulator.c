#include <stdio.h>
#include <stdlib.h>

#define EPERM 1
#define EINVAL 2													//codurile de eroare
#define ENOMEM 3

struct pixel 
{
	unsigned char r, g, b;											//structura de pixel pentru implementare mai usoara
};

typedef struct pixel Pixel;											//tip definit pentru implementare si mai usoara

Pixel **initialize(int width, int height)							
{
	int i, j;																
	Pixel **image;

	image = malloc(height * sizeof(Pixel *));						//alocam matricea 
	if(image == NULL)
	{
		fflush(stdout);
																	//verificam daca s-a alocat
		fprintf(stderr, "%d\n", ENOMEM);

		exit(EXIT_FAILURE);
	}

	for(i = 0; i < height; i++)
	{
		image[i] = malloc(width * sizeof(Pixel));
		if(image[i] == NULL) 
		{
			for(j = 0; j < i; j++)
				free(image[j]);
		
			free(image);											/*alocam matricea in continuare si daca nu s-a alocat
																	eliberam memoria alocata pana acolo si avem eroare*/
			fflush(stdout);												

			fprintf(stderr, "%d\n", ENOMEM);

			exit(EXIT_FAILURE);
		}
	}

	return image;
}


void read_image(Pixel **image, int width, int height)
{
	int i, j;

	for(i = 0; i < height; i++)
		for(j = 0; j < width; j++)
		{
			scanf("%hhu %hhu %hhu", &image[i][j].r, &image[i][j].g, &image[i][j].b);																					//citim matricea efectiv si verificam valorile
			
			if(!((image[i][j].r >= 0) && (image[i][j].r <= 255) && (image[i][j].g >= 0)						//citim elementele si verificam daca sunt valide 
				&& (image[i][j].g <= 255) && (image[i][j].b >= 0) && (image[i][j].b <= 255)))
			{
				fflush(stdout);

				fprintf(stderr, "%d\n", EINVAL);

				exit(EXIT_FAILURE);
			}
		}	 	
}

Pixel **crop_image(Pixel ***image, int start_col, int start_line, int end_col, int end_line, int height)
{
	Pixel **cropped;
	int i, j, k, z;

	cropped = malloc((end_line - start_line + 1) * sizeof(Pixel *));			//alocam matricea cropata

	if(cropped == NULL)
	{
		fflush(stdout);

		fprintf(stderr, "%d\n", ENOMEM);									//verificam daca s-a alocat

		exit(EXIT_FAILURE);
	}

	for(i = 0; i < (end_line - start_line + 1); i++)
	{
		cropped[i] = malloc((end_col - start_col + 1) * sizeof(Pixel));			//alocam matricea in continuare cu tot cu verificare si eliberare in caz contrar

		if(cropped[i] == NULL)
		{
			for(j = 0; j < i; j++)
				free(cropped[i]);
			free(cropped);
			
			fflush(stdout);

			fprintf(stderr, "%d\n", ENOMEM);

			exit(EXIT_FAILURE);
		}
	}

	for(i = start_line, k = 0; i <= end_line; i++, k++)						//copiem valorile care trebuiesc copiate din matricea initiala in cea cropata
		for(j =  start_col, z = 0; j <= end_col; j++, z++)					
				cropped[k][z] = (*image)[i][j];

	for(i = 0; i < height; i++)
		free((*image)[i]);													//eliberam matricea veche
	free(*image);
	*image = NULL;

	(*image) = malloc((end_line - start_line + 1) * sizeof(Pixel *));			//o realocam cu noile dimensiuni

	if(*image == NULL)
	{
		fflush(stdout);

		fprintf(stderr, "%d\n", ENOMEM);									//verificam alocarea

		exit(EXIT_FAILURE);
	}

	for(i = 0; i < (end_line - start_line + 1); i++)
	{
		(*image)[i] = malloc((end_col - start_col + 1) * sizeof(Pixel));

		if((*image)[i] == NULL)																//alocam in continuare +verificare si eliberare in caz contrar
		{
			for(j = 0; j < i; j++)
				free((*image)[i]);
			free(*image);
			*image = NULL;
			
			fflush(stdout);

			fprintf(stderr, "%d\n", ENOMEM);

			exit(EXIT_FAILURE);
		}
	}

	for(i = 0; i < (end_line - start_line + 1); i++)
		for(j = 0; j < (end_col - start_col + 1); j++)								//copiem matricea cropata din cropped in cea pe care o vom returna
			(*image)[i][j] = cropped[i][j];

	for(i = 0; i < (end_line - start_line + 1); i++)
		free(cropped[i]);															//eliberam matricea cropped
	free(cropped);

	return (*image);

}

Pixel **resize_image(Pixel ***image, int new_width, int new_height, int old_width, int old_height)
{
	if((new_width < old_height) && (new_height < old_height))
		return crop_image(image, 0, 0, new_width - 1, new_height - 1, old_height);					//daca dimensiunile sunt mai mici avem crop
	else
	{
		Pixel **resized;
		int i, j;

		resized = malloc(new_height * sizeof(Pixel *));							//alocam o noua matrice cu noile dimensiuni
		
		if(resized == NULL)
		{
			fflush(stdout);

			fprintf(stderr, "%d\n", ENOMEM);								//verificare

			exit(EXIT_FAILURE);
		}

		for(i = 0; i < new_height; i++)
		{
			resized[i] = malloc(new_width * sizeof(Pixel));						//continuam alocarea + verificare si eliberare in caz contrar
			
			if(resized[i] ==  NULL)
			{
				for(j = 0; j < i; j++)
					free(resized[i]);
				free(resized);
				
				fflush(stdout);

				fprintf(stderr, "%d\n", ENOMEM);									

				exit(EXIT_FAILURE);
			}
		}

		for(i = 0; i < new_height; i++)
			for(j = 0; j < new_width; j++)
			{
				resized[i][j].r = 255;
				resized[i][j].g = 255;										//initializam matricea noua cu 255 255 255 adica alb
				resized[i][j].b = 255;
			}

		if((new_height > old_height) && (new_width > old_width))
		{
			for(i = 0; i < old_height; i++)
				for(j = 0; j < old_width; j++)
					resized[i][j] = (*image)[i][j];
		}
		else if(new_height > old_height)
			{
				for(i = 0; i < old_height; i++)
					for(j = 0; j < new_width; j++)
						resized[i][j] = (*image)[i][j];							//verificam cum difera noile dimensiuni de cele vechi si copiem in matricea noua fie toate matricea veche, fie doar o parte
			}																	
			else if(new_width > old_width)
				{
					for(i = 0; i < new_height; i++)
					for(j = 0; j < old_width; j++)
						resized[i][j] = (*image)[i][j];
				}

		for(i = 0; i < old_height; i++)
			free((*image)[i]);
		free(*image);														//eliberam matricea veche
		*image = NULL;

		(*image) = malloc(new_height * sizeof(Pixel *));					//o alocam din nou cu noile dimensiuni

		if(*image == NULL)
		{
			fflush(stdout);

			fprintf(stderr, "%d\n", ENOMEM);								//verificam alocarea

			exit(EXIT_FAILURE);
		}

		for(i = 0; i < new_height; i++)
		{
			(*image)[i] = malloc(new_width * sizeof(Pixel));				//continuam alocarea + verificare si eliberare in caz contrar

			if((*image)[i] == NULL)
			{
				for(j = 0; j < i; j++)
					free((*image)[i]);
				free(*image);
				*image = NULL;
				
				fflush(stdout);

				fprintf(stderr, "%d\n", ENOMEM);

				exit(EXIT_FAILURE);
			}
		}

		for(i = 0; i < new_height; i++)
			for(j = 0; j < new_width; j++)
				(*image)[i][j] = resized[i][j];							//copiem matricea resized in matricea pe care o vom returna

		for(i = 0; i < new_height; i++)
			free(resized[i]);											//eliberam matricea resized
		free(resized);

		return (*image);	

	}
}

Pixel **color_region(Pixel ***image, int start_col, int start_line, int end_col, int end_line, unsigned char r, unsigned char g, unsigned char b)
{
	int i, j;

	for(i = start_line; i <= end_line; i++)
		for(j = start_col; j <= end_col; j++)
		{
			(*image)[i][j].r = r;
			(*image)[i][j].g = g;											//coloram efectiv regiunea data ca parametru
			(*image)[i][j].b = b;
		}

	return (*image);	
}

Pixel **blur_image(Pixel ***image, int num_iter, int width, int height)
{
	int i, j;

	Pixel **blur;

	blur = malloc(height * sizeof(Pixel *));			//alocam o matrice in care vom retine valorile initiale

	if(blur == NULL)
	{
		fflush(stdout);

		fprintf(stderr, "%d\n", ENOMEM);				//verificare

		exit(EXIT_FAILURE);
	}

	for(i = 0; i < height; i++)
	{
		blur[i] = malloc(width * sizeof(Pixel));			//alocam in continuare + verificare si eliberare in caz contrar

		if(blur[i] == NULL)
		{
			for(j = 0; j < i; j++)
				free(blur[i]);
			free(blur);
			
			fflush(stdout);

			fprintf(stderr, "%d\n", ENOMEM);

			exit(EXIT_FAILURE);
		}
	}

	while(num_iter)											//efectuam de num_iter ori
	{
		

		for(i = 0; i < height; i++)
			for(j = 0; j < width; j++)
				blur[i][j] = (*image)[i][j];						//copiem valorile initiale in blur

		for(i = 0; i < height; i++)
			for(j = 0; j < width; j++)
			{
				if((i == 0) && (j == 0))												//luam toate cele 8 cazuri(colturi si laturi) ale matricei valoarea blurata in functie de ce caz suntem si trunchiem la int
				{																		
					(*image)[i][j].r = (int)((blur[i][j+1].r + blur[i+1][j].r)/2);
					(*image)[i][j].g = (int)((blur[i][j+1].g + blur[i+1][j].g)/2);
					(*image)[i][j].b = (int)((blur[i][j+1].b + blur[i+1][j].b)/2);
				}
				else if((i == 0) && (j != width - 1) && (j != 0))
					{
						(*image)[i][j].r = (int)((blur[i][j-1].r + blur[i+1][j].r + blur[i][j+1].r)/3);
						(*image)[i][j].g = (int)((blur[i][j-1].g + blur[i+1][j].g + blur[i][j+1].g)/3);
						(*image)[i][j].b = (int)(((blur)[i][j-1].b + blur[i+1][j].b + blur[i][j+1].b)/3);
					}
					else if((i == 0) && (j == width - 1))
						{
							(*image)[i][j].r = ((blur[i][j-1].r + blur[i+1][j].r)/2);
							(*image)[i][j].g = ((blur[i][j-1].g + blur[i+1][j].g)/2);
							(*image)[i][j].b = ((blur[i][j-1].b + blur[i+1][j].b)/2);
						}
						else if((j == 0) && (i != 0) && (i != height - 1))
							{
								(*image)[i][j].r = ((blur[i-1][j].r + blur[i][j+1].r + blur[i+1][j].r)/3);
								(*image)[i][j].g = ((blur[i-1][j].g + blur[i][j+1].g + blur[i+1][j].g)/3);
								(*image)[i][j].b = ((blur[i-1][j].b + blur[i][j+1].b + blur[i+1][j].b)/3);
							}
							else if((j == width - 1) && (i != 0) && (i != height - 1))
								{
									(*image)[i][j].r = ((blur[i-1][j].r + blur[i][j-1].r + blur[i+1][j].r)/3);
									(*image)[i][j].g = ((blur[i-1][j].g + blur[i][j-1].g + blur[i+1][j].g)/3);
									(*image)[i][j].b = ((blur[i-1][j].b + blur[i][j-1].b + blur[i+1][j].b)/3);
								}
								else if((i == height - 1) && (j == 0))
									{
										(*image)[i][j].r = ((blur[i-1][j].r + blur[i][j+1].r)/2);
										(*image)[i][j].g = ((blur[i-1][j].g + blur[i][j+1].g)/2);
										(*image)[i][j].b = ((blur[i-1][j].b + blur[i][j+1].b)/2);
									}
									else if((i == height - 1) && (j != 0) && (j != width - 1))
										{
											(*image)[i][j].r = ((blur[i][j-1].r + blur[i-1][j].r + blur[i][j+1].r)/3);
											(*image)[i][j].g = ((blur[i][j-1].g + blur[i-1][j].g + blur[i][j+1].g)/3);
											(*image)[i][j].b = ((blur[i][j-1].b + blur[i-1][j].b + blur[i][j+1].b)/3);
										}
										else if((i == height - 1) && (j == width - 1))
											{
												(*image)[i][j].r = ((blur[i][j-1].r + blur[i-1][j].r)/2);
												(*image)[i][j].g = ((blur[i][j-1].g + blur[i-1][j].g)/2);
												(*image)[i][j].b = ((blur[i][j-1].b + blur[i-1][j].b)/2);
											}
											else 
											{
												(*image)[i][j].r = ((blur[i-1][j].r + blur[i][j-1].r + blur[i][j+1].r + blur[i+1][j].r)/4);
												(*image)[i][j].g = ((blur[i-1][j].g + blur[i][j-1].g + blur[i][j+1].g + blur[i+1][j].g)/4);
												(*image)[i][j].b = ((blur[i-1][j].b + blur[i][j-1].b + blur[i][j+1].b + blur[i+1][j].b)/4);
											}
			}
		num_iter--;				//trecem la pasul urm
	}

	for(i = 0 ; i < height; i++)
		free(blur[i]);									//eliberam blur
	free(blur);

	return (*image);
}

Pixel **rotate_image(Pixel ***image, int width, int height, int num_rot)
{
	int i, j;

	int width_ = width, height_ = height;			//folosim niste copii ale dimensiunilor

	Pixel **rotated;

	while(num_rot)
	{

		rotated = malloc(height_ * sizeof(Pixel *));			//la fiecare pas alocam o matrice cu aceleasi dimensiuni ca cea originala

		if(rotated == NULL)
		{
			fflush(stdout);

			fprintf(stderr, "%d\n", ENOMEM);				//verificare

			exit(EXIT_FAILURE);
		}

		for(i = 0; i < height_; i++)
		{
			rotated[i] = malloc(width_ * sizeof(Pixel));				//alocam in continuare + verificare si eliberare in caz contrar

			if(rotated[i] == NULL)
			{	
				for(j = 0; j < i; j++)
					free(rotated[i]);
				free(rotated);
				
				fflush(stdout);

				fprintf(stderr, "%d\n", ENOMEM);

				exit(EXIT_FAILURE);
			}	
		}

		for(i = 0; i < height_; i++)
			for(j = 0; j < width_; j++)
				rotated[i][j] = (*image)[i][j];							//copiem matricea initiala in cea rotated

		for(i = 0; i < height_; i++)
			free((*image)[i]);
		free(*image);													//eliberam matricea initiala
		*image = NULL;

		int new_width, new_height;
		
		new_width =  height_;											//interschimbam dimensiunile intre ele
		new_height = width_;

		(*image) = malloc(new_height * sizeof(Pixel *));				//alocam matricea initiala din nou cu noile dimensiuni

		if((*image) == NULL)
		{

			fflush(stdout);

			fprintf(stderr, "%d\n", ENOMEM);							//verificare

			exit(EXIT_FAILURE);
		}

		for(i = 0; i < new_height; i++)
		{
			(*image)[i] = malloc(new_width * sizeof(Pixel));				//alocare in continuare + verificare si eliberare in caz contrar

			if((*image)[i] == NULL)
			{
				for(j = 0; j < i; j++)
					free((*image)[i]);
				free(*image);
				*image = NULL;

				fflush(stdout);
				
				fprintf(stderr, "%d\n", ENOMEM);

				exit(EXIT_FAILURE);
			}
		}

		for(i = 0; i < new_height; i++)
			for(j = 0; j < new_width; j++)
				(*image)[i][j] = rotated[new_width - 1 - j][i];						//parcurgem matricea noua cu noile dimensiuni si cea veche pe coloane de jos in sus
																					
		for(i = 0; i < height_; i++)
			free(rotated[i]);														//eliberam matricea rotated
		free(rotated);

		num_rot--;	

		width_ = new_width;																//interschimbam valorile si trecem la pasul urmator
		height_ = new_height;

	}

	return (*image);
}

int pixel_same_color(Pixel p1, Pixel p2)
{
	if((p1.r == p2.r) && (p1.g == p2.g) && (p1.b == p2.b))						//comparam 2 pixeli
		return 1;

	return 0;
}

void fill_image(Pixel ***image, int start_col, int start_line, unsigned char r, unsigned char g, unsigned char b, int width, int height, Pixel old)
{


	if(pixel_same_color(old, (*image)[start_line][start_col]))					//verificam daca pixelul la care suntem are aceeasi culoare cu cel initial
	{

		(*image)[start_line][start_col].r = r;
		(*image)[start_line][start_col].g = g;									//schimbare efectiva a pixelilor
		(*image)[start_line][start_col].b = b;


		if(width - 1 >= start_col + 1)
		{
			if(pixel_same_color((*image)[start_line][start_col + 1], old))
				fill_image(image, start_col + 1, start_line, r, g, b, width, height, old);
			
		}	

		if(0 <= start_col - 1)
		{	
			if(pixel_same_color((*image)[start_line][start_col - 1], old))
				fill_image(image, start_col - 1, start_line, r, g, b, width, height, old);			//verificam daca exista vecini in sus, jos, stanga, dreapta in pozitia in care suntem si apelam recursiv daca exista
																										
		}
		
		if(height - 1 >= start_line + 1)
		{	
			if(pixel_same_color((*image)[start_line + 1][start_col], old))
				fill_image(image, start_col, start_line + 1, r, g, b, width, height, old);
			
		}
		
		if(0 <= start_line - 1)
		{

			if(pixel_same_color((*image)[start_line - 1][start_col], old))
				fill_image(image, start_col, start_line - 1, r, g, b, width, height, old);
			
		}
		else return;

	}
	else return;
}

void print(Pixel **image, int width, int height)
{
	int i, j;

	printf("%d %d\n", width, height);

	for(i = 0; i < height; i++)																		//printarea matricii
	{
		for(j = 0; j < width; j++)
			printf("%hhu %hhu %hhu ", image[i][j].r, image[i][j].g, image[i][j].b);
		printf("\n");
	}
}

int main(void)
{
	int op, width, height, i;
	Pixel **image = NULL;

	scanf("%d", &op);													//citim codul operatiei
	
	while(op)
	{

		switch(op)												
		{
			case 1:
			{
				if(image == NULL)													//daca imaginea nu a mai fost initializata o citim
				{
					scanf("%d %d", &width, &height);

					if(((height >= 1) && (height <= 1024)) && ((width >= 1) &&(width <= 1024)))				//parametri corecti
					{
						image = initialize(width, height);

						read_image(image, width, height);
					}
					else 
						{
							fflush(stdout);

							fprintf(stderr, "%d\n", EINVAL);					//altfel eroare

							exit(EXIT_FAILURE);
						}	
				}
				else
				{
					for(i = 0; i < height; i++)												//daca imaginea a mai fost initializata o eliberam apoi o citim
						free(image[i]);
					free(image);

					scanf("%d %d", &width, &height);

					if(((height >= 1) && (height <= 1024)) && ((width >= 1) &&(width <= 1024)))				//parametri corecti
					{
						image = initialize(width, height);

						read_image(image, width, height);
					}
					else 
						{
							fflush(stdout);

							fprintf(stderr, "%d\n", EINVAL);				//altfel eroare

							exit(EXIT_FAILURE);
						}	
				}			
			} break;

			case 2:
			{
			
				int start_col, start_line, end_col, end_line;

				scanf("%d %d %d %d", &start_col, &start_line, &end_col, &end_line);

				if(image == NULL)
				{
					
					fflush(stdout);

					fprintf(stderr, "%d\n", EPERM);			//daca nu avem imagine eroare


					exit(EXIT_FAILURE);
				}

				if(((start_line >= 0) && (end_line >= start_line) && (height > end_line)) && ((start_col >= 0) 
				&& (end_col >= start_col) && (width > end_col)))		//parametri corecti
				{

					image = crop_image(&image, start_col, start_line, end_col, end_line, height);
																										//cropam efectiv si schimbam dimensiunile
					height = (end_line - start_line + 1); width =  (end_col - start_col + 1);
				}
				else 
				{
					
					fflush(stdout);

					fprintf(stderr, "%d\n", EINVAL);

					for(i = 0; i < height; i++)						//altfel eroare si eliberam memoria
						free(image[i]);
					free(image);

					exit(EXIT_FAILURE);
				}	


			} break;

			case 3:
			{
				int new_width, new_height;

				scanf("%d %d", &new_width, &new_height);

				if(image == NULL)
				{
					
					fflush(stdout);

					fprintf(stderr, "%d\n", EPERM);			//imagine neinitializata => eroare

					exit(EXIT_FAILURE);
				}

				if(((new_width >= 1) && (new_width <= 1024)) && ((new_height >= 1) && (new_height <= 1024)))		//parametri corecti
				{

					image = resize_image(&image, new_width, new_height, width, height);

					width = new_width;														//facem resize si schimbam dimensiunile

					height =  new_height;
				}
				else 
				{
					
					fflush(stdout);

					fprintf(stderr, "%d\n", EINVAL);

					for(i = 0; i < height; i++)					//altfel eroare si eliberam memoria
						free(image[i]);
					free(image);

					exit(EXIT_FAILURE);
				}	


			} break;

			case 4:
			{
				int start_col, start_line, end_col, end_line;
				
				unsigned char r, g, b;

				scanf("%d %d %d %d %hhu %hhu %hhu", &start_col, &start_line, &end_col, &end_line, &r, &g, &b);

				if(image == NULL)
				{
					
					fflush(stdout);

					fprintf(stderr, "%d\n", EPERM);					//imagine neinitializata => eroare

					exit(EXIT_FAILURE);
				}

				if(((start_line >= 0) && (end_line >= start_line) && (height > end_line)) && ((start_col >= 0) 
				&& (end_col >= start_col) && (width > end_col)) && ((r >= 0) && (r <= 255) 
				&& (g >= 0) && (g <= 255) && (b >= 0) && (b <= 255)))		//parametri corecti
					image = color_region(&image, start_col, start_line, end_col, end_line, r, g, b);	//coloram regiunea
				else
				{
					
					fflush(stdout);

					fprintf(stderr, "%d\n", EINVAL);			//altfel eroare si eliberam memoria

					for(i = 0; i < height; i++)
						free(image[i]);
					free(image);

					exit(EXIT_FAILURE);
				}
			
			} break;

			case 5:
			{				
				int num_iter;

				scanf("%d", &num_iter);

				if(image == NULL)
				{
					
					fflush(stdout);

					fprintf(stderr, "%d\n", EPERM);				//imagine neinitializata => eroare

					exit(EXIT_FAILURE);
				}

				if((num_iter >= 0) && (num_iter <= 2000)) //parametrii corecti
					image = blur_image(&image, num_iter, width, height);		//bluram imaginea
				else
				{
					fflush(stdout);

					fprintf(stderr, "%d\n", EINVAL);			//altfel eroare si eliberam memoria

					for(i = 0; i < height; i++)
						free(image[i]);
					free(image);

					exit(EXIT_FAILURE);
				}	
			} break;

			case 6:
			{
				int num_rot;

				scanf("%d", &num_rot);

				if(image == NULL)
				{
					
					fflush(stdout);

					fprintf(stderr, "%d\n", EPERM);				//imagine neinitializata => eroare

					exit(EXIT_FAILURE);
				}

				if((num_rot >= 1) && (num_rot <= 3))			//parametri corecti
				{

					image = rotate_image(&image, width, height, num_rot);		//rotim imaginea

					if(num_rot == 1)
					{
						int temp;
						temp = height;
						height = width;
						width = temp;												//in functie de cate ori am rotit schimbam dimensiunile intre ele
					}
					else if(num_rot == 3)
						{		
							int temp;

							temp = height;
							height = width;
							width = temp;
						}
				}
				else
				{
					
					fflush(stdout);

					fprintf(stderr, "%d\n", EINVAL);

					for(i = 0; i < height; i++)					//altfel eroare si eliberam memoria
						free(image[i]);
					free(image);

					exit(EXIT_FAILURE);
				}
				}break;

			case 7:
			{
				
				if(image == NULL)
				{
					
					fflush(stdout);

					fprintf(stderr, "%d\n", EPERM);			//imagine neinitializata => eroare

					exit(EXIT_FAILURE);
				}

				int start_col, start_line;

				unsigned char r, g, b;

				scanf("%d %d %hhu %hhu %hhu", &start_col, &start_line, &r, &g, &b);

				if((start_line >= 0) && (height > start_line) && (start_col >= 0) && (width > start_col) 
				&& (r >= 0) && (r <= 255) && (g >= 0) && (g <= 255) && (b >= 0) && (b <=255))  //parametri corecti
				{

					Pixel new;

					new.r = r;
					new.g = g;
					new.b = b;

					if(!(pixel_same_color(image[start_line][start_col], new)))		//verificam daca au aceeasi culoare pixelii 
						fill_image(&image, start_col, start_line, r, g, b, width, height, image[start_line][start_col]);		//daca nu au incepem sa inlocuim pixelii
				}
				else
				{
					fflush(stdout);

					fprintf(stderr, "%d\n", EINVAL);

					for(i = 0; i < height; i++)						//altfel eroare si eliberam memoria
						free(image[i]);
					free(image);

					exit(EXIT_FAILURE);
				}
			} break;	


			case 8: if(image == NULL)
					{
						fflush(stdout);

						fprintf(stderr, "%d\n", EPERM);			//imagine neinitializata => eroare

						exit(EXIT_FAILURE);
					}
					else print(image, width, height);       //printam matricea 

					break;				

			case 0: break;				//iesim cu succes
		 default: {

		 		fflush(stdout);

				fprintf(stderr, "%d\n", EPERM);							//daca codul operatiei este invalid => eroare

				exit(EXIT_FAILURE);
			} break;
		}
		
		scanf("%d", &op);								//citim alt cod de operatie				
		
	}

	for(i = 0; i < height; i++)
		free(image[i]);						//eliberam memoria
	free(image);	
	
	return 0;
}
