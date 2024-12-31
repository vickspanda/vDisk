#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>


int createDisk(FILE * file, int size)
{
	char put = 1;
	while(size>0)
	{
		fputc(put,file);
		size--;
	}	
}

/*
 * main executable for creating a Disk
 */

int main(int argc, char *argv[])
{
	int size;
	/* Validating Number of Arguments */
	if(argc != 3)
	{
		printf("Error : Invalid Number of Arguments\n");
		return 0;
	}
	
	/* Argument is open as file pointer in write mode */
	FILE * file = fopen(argv[1],"w");
	char * sizeChar = argv[2];

	/* If File is not opened then Error will be Displayed */
	if(!file){
		perror("Error ");
		return 0;
	}
	
	/* typecasting from string to int */
	size = atoi(sizeChar);
	/* calling to create virtual Disk */
	createDisk(file,size);
	/* Close file pointer */
	fclose(file);
}
