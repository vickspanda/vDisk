#include<stdio.h>
#include<ctype.h>
#include<stdlib.h>

/* 
 * This is the c File which will create a virtual Disk
 */


/* checkIfInt Function accepts an input which was passed via command line argument and returns 0 if it is not int and 1 if it is int */
int checkIfInt(char *input){
	/* 
	 * input is char array whether input is NULL or value at input's 0 index is '\0'
	 * if it is true return 0
	 */
        if (input == NULL || *input == '\0')
                return 0;

	/* 
	 * checking every index of input isdigit or not and for that we are incrementing input
	 * If it is not digit, return zero
	 */
        while (*input) {
                if (!isdigit(*input))
                        return 0;
        	input++;
        }

	/* Return 1, if all previous conditions are not True */
        return 1;
}

/* 
 * createDisk creates the Virtual Disk, which is in Binary Format,
 * accepts file pointer which is opened in write mode
 * it creates a File of x MBs where x is size passed by main fuction
 */
int createDisk(FILE * file, int size)
{
	char put = 0;
	size = (size * 1024 * 1024);
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
		printf("Error: Invalid Number of Arguments\n");
		return 0;
	}
	
	/* Argument is open as file pointer in write mode */
	FILE * file = fopen(argv[1],"w");
	char * sizeChar = argv[2];
	/* Checking if 3 argument is Integer or not */
	if(!checkIfInt(sizeChar)){
                        printf("Error: Invalid Input for Number of Breaks\n");
                        return 0;
        }

	/* If File is not opened then Error will be Displayed */
	if(!file){
		perror("Error: ");
		return 0;
	}
	
	/* typecasting from string to int */
	size = atoi(sizeChar);
	/* calling to create virtual Disk */
	createDisk(file,size);
	/* Close file pointer */
	fclose(file);
}
