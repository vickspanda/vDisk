#include<stdio.h>
#include<string.h>
#include"helper-tools/helper.c"
#include"helper-tools/encode.c"
#include"helper-tools/vDisk.c"

/* 
 * This is the c File which will Display the Details of the given virtual Disk
 * Such as Disk Size, Disk Name, Free Space and Used Space
 */


/* It is the Main Function for vdDetails Operation which deals with all kinds of Validations for Input */
int main(int argc, char *argv[])
{
	/* Validating Number of Arguments */
	if(argc!=2)
	{
		printf("Error: Invalid Number of Arguments Passed!!!\n");
		return 0;
	}
	
	/* Validating whether the Disk passed is Binary File or Not */
	if(isBinaryFile(argv[1])==0)
		return 0;
		
	union decStore diskSize = getSize(argv[1]);
	printf("Disk Name:\t%s\n",argv[1]);
	printf("Disk Size:\t%llu bytes\n",diskSize.num);
	
	/* Disk Pointer for Disk's Details*/
	FILE *diskPointer = fopen(argv[1],"r");
	
	FILE *encodedSeqPointer = fopen(argv[1],"r");
	
	unsigned int countOfFiles;
	
	countOfFiles = getNoOfFilesInDisk(diskPointer);
	// getNoOfFilesInDisk returns count of Existing Files in the Virtual Disk
	
	unsigned long long int usedSpace;
	
	fseek(diskPointer,0,SEEK_END);
		
	usedSpace = getUsedSpace(diskPointer, encodedSeqPointer, countOfFiles);	
	/*
	 * getUsedSpace Function returns the usedSpace in Virtual Disk whose file Pointer was provided as an input to it
	 */
	printf("Used Space:\t%llu bytes\n",usedSpace);
	
	/*
	 * getFreeSpace Function returns the Free Space available in Virtual Disk whose file Pointer was provided as an input to it
	 */
	printf("Free Space:\t%llu bytes\n",getFreeSpace(usedSpace ,argv[1]));
	
	/* CLosing Disk Pointer */
	fclose(diskPointer);
	
	return 0;
}