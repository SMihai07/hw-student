#include<stdio.h>

void task0(int ip_1[5]) {

	printf("-0 %d.%d.%d.%d\n", ip_1[0], ip_1[1], ip_1[2], ip_1[3]);		//PRINT IP
}

void task1(int x){

	
	int mask_addr[4]={0};

	int i;

	for(i = 0; i < mask; i++)
		if(mask_addr[0] < 255)
			mask_addr[0] = (mask_addr[0]>>1) | 128;
		else 
			if (mask_addr[1] < 255)
				mask_addr[1] = (mask_addr[1]>>1) | 128;
			else
				if(mask_addr[2] < 255)
					mask_addr[2] = (mask_addr[2]>>1) | 128;
				else
					if(mask_addr[3] < 255)
						mask_addr[3] = (mask_addr[3]>>1) | 128;


	printf("-1 %d.%d.%d.%d\n", mask_addr[0], mask_addr[1], mask_addr[2], mask_addr[3]);


	
}

void task2(int ip_1[5]) {

	int i, mask_addr[4]={0}, net_addr[4];

	for(i = 0; i < ip_1[4] ; i++)
		if(mask_addr[0] < 255)
			mask_addr[0] = (mask_addr[0]>>1) | 128;
		else 
			if (mask_addr[1] < 255)
				mask_addr[1] = (mask_addr[1]>>1) | 128;
			else
				if(mask_addr[2] < 255)
					mask_addr[2] = (mask_addr[2]>>1) | 128;
				else
					if(mask_addr[3] < 255)
						mask_addr[3] = (mask_addr[3]>>1) | 128;


	for(i = 0; i < 4; i++){
		net_addr[i] = ip_1[i];
		net_addr[i] &= mask_addr[i];
	}

	printf("-2 %d.%d.%d.%d\n", net_addr[0], net_addr[1], net_addr[2], net_addr[3]);
}

void task3(int ip_1[5]) {

	int i, broad_addr[4], mask_addr[4]={0};

	for(i = 0; i < ip_1[4] ; i++)
		if(mask_addr[0] < 255)
			mask_addr[0] = (mask_addr[0]>>1) | 128;
		else 
			if (mask_addr[1] < 255)
				mask_addr[1] = (mask_addr[1]>>1) | 128;
			else
				if(mask_addr[2] < 255)
					mask_addr[2] = (mask_addr[2]>>1) | 128;
				else
					if(mask_addr[3] < 255)
						mask_addr[3] = (mask_addr[3]>>1) | 128;

	for(i = 0; i < 4; i++){
		broad_addr[i] = ip_1[i];
		broad_addr[i] |= (255 - mask_addr[i]);
		
	}


	printf("-3 %d.%d.%d.%d\n", broad_addr[0], broad_addr[1], broad_addr[2], broad_addr[3]);
}

int task4(int ip_1[5], int ip_2[4]){

	int i, net_addr1[4], net_addr2[4], mask_addr[4]={0};

	for(i = 0; i < ip_1[4] ; i++)
		if(mask_addr[0] < 255)
			mask_addr[0] = (mask_addr[0]>>1) | 128;
		else 
			if (mask_addr[1] < 255)
				mask_addr[1] = (mask_addr[1]>>1) | 128;
			else
				if(mask_addr[2] < 255)
					mask_addr[2] = (mask_addr[2]>>1) | 128;
				else
					if(mask_addr[3] < 255)
						mask_addr[3] = (mask_addr[3]>>1) | 128;

	for(i = 0; i < 4; i++){
		net_addr1[i] = ip_1[i];
		net_addr2[i] = ip_2[i];
	}


	for(i = 0; i < 4; i++){
		net_addr1[i] &= mask_addr[i];
		net_addr2[i] &= mask_addr[i];
	}

	for(i = 0; i < 4; i++)
		if(net_addr1[i] != net_addr2[i])
			break;

	if(i == 4)
		return 1;
	else
		return 0;		
}

void task5(int ip_1[4], int ip_2[4]) {

	printf("-5 %X.%X.%X.%X", ip_1[0], ip_1[1], ip_1[2], ip_1[3]);
	printf(" %o.%o.%o.%o\n", ip_2[0], ip_2[1], ip_2[2], ip_2[3]);
}

void task6(int num, int networks[1024][5], int ip_2[4]) {

	int i, j, net_addr1[4], net_addr2[4], mask_addr[4];

	printf("-6 ");

	for(i = 0; i < 4; i++)
		net_addr2[i] = ip_2[i];

	for(i = 0; i < num; i++) {

		mask_addr[0]=0, mask_addr[1]=0, mask_addr[2]=0, mask_addr[3]=0;

		for(j = 0; j < networks[i][4]; j++)
			if(mask_addr[0] < 255)
				mask_addr[0] = (mask_addr[0]>>1) | 128;
			else 
				if (mask_addr[1] < 255)
					mask_addr[1] = (mask_addr[1]>>1) | 128;
				else
					if(mask_addr[2] < 255)
						mask_addr[2] = (mask_addr[2]>>1) | 128;
					else
						if(mask_addr[3] < 255)
							mask_addr[3] = (mask_addr[3]>>1) | 128;
		for(j = 0; j < 4; j++) {
			net_addr1[j] = networks[i][j];
			net_addr1[j] &= mask_addr[j];
			net_addr2[j] &= mask_addr[j];				
		}

		for(j = 0; j < 4; j++)
			if(net_addr1[j] != net_addr2[j])
				break;

		if(j == 4)
			printf("%d ", i+1);

		for(j = 0; j < 4; j++)
			net_addr2[j]=ip_2[j];
		
		}

	printf("\n");	

	}



void binary(int x) {

	int i, k;

	for(i = 7; i >= 0; i--) {
		k = x >> i;

		if(k & 1)
			printf("1");
		else
			printf("0");

	}

}

void task7(int ip_1[4], int ip_2[4]) {

	int i;

	printf("-7 ");

	for(i = 0; i < 4; i++)
		if(i == 3) {
			binary(ip_1[3]);
			printf(" ");
		}
		else {
			binary(ip_1[i]);
			printf(".");
		}
			

	for(i = 0; i < 4; i++)
		if(i == 3) {
			binary(ip_2[3]);
			printf(" \n");
		}
		else {
			binary(ip_2[i]);
			printf(".");
		}

}

void task8(int ip_1[4], int ip_2[4]) {

	int j;

	printf("-8 ");

	for(j = 0; j < 4; j++) {
		int x = ip_1[j];
		int i = 0;
		char buffer[32];

		while(x > 0) {

			int digit = x % 32;
			if(digit < 10)
				buffer[i++] =  digit + '0';
			else 
				buffer[i++] = 'A' + (digit - 10);
		x /= 32;	
		}

		if(i == 0)
			printf("0");
		else 
			for(i = i - 1;i >= 0; i--) {
				printf("%c", buffer[i]);		
		}

		if(j == 3)
			printf(" ");
		else
			printf(".");
	}

	for(j = 0; j < 4; j++) {
		int x = ip_2[j];
		int i = 0;
		char buffer[32];

		while(x > 0) {

			int digit = x % 32;
			if(digit < 10)
				buffer[i++] =  digit + '0';
			else 
				buffer[i++] = 'A' + (digit - 10);
		x /= 32;	
		}

		if(i == 0)
			printf("0");
		else
			for(i = i - 1;i >= 0; i--) {
				printf("%c", buffer[i]);			
			}

		if(j == 3)
			printf(" \n");
		else
			printf(".");
	}
}

int main(void)
{
	int datasets, num, ip_1[5], ip_2[4], networks[1024][5];

	scanf("%d\n", &datasets);

	int i, j;

	for(i = 0; i < datasets; i++) {
		//START READING
		scanf("%d.%d.%d.%d/%d", &ip_1[0], &ip_1[1], &ip_1[2], &ip_1[3], &ip_1[4]);
		scanf("%d.%d.%d.%d", &ip_2[0], &ip_2[1], &ip_2[2], &ip_2[3]);
		scanf("%d", &num);

		for(j = 0; j < num; j++){ 
			scanf("%d.%d.%d.%d/%d", &networks[j][0], &networks[j][1], &networks[j][2], &networks[j][3], &networks[j][4]);
			printf("\n");
		}
		//END READING

		printf("%d\n", i+1);

		task0(ip_1);

		task1(ip_1[4]);

		task2(ip_1);

		task3(ip_1);

		int net_addr2[4];

		if(task4(ip_1, ip_2))
			printf("-4 da\n");
		else
			printf("-4 nu\n");

		task5(ip_1, ip_2);

		if(task4(ip_1, ip_2))
			printf("-6 0\n");
		else
			task6(num, networks, ip_2);


		task7(ip_1, ip_2);

		task8(ip_1, ip_2);

		




		









	}
	return 0;
}