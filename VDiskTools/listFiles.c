#include<stdio.h>
#include<string.h>
#include"helper-tools/helper.c"
#include"helper-tools/encode.c"
#include"helper-tools/vDisk.c"

/* 
 * This is the c File which will lists All Files Existing in given virtual Disk
 */


/* printFileName displays the file Name, Present at the Current Position of the File Name Pointer */
int printFileName(FILE *fileNamePointer)
{
	char ch;
	while((ch=fgetc(fileNamePointer))!='\0')
		printf("%c",ch);
	printf("\n");
}

/* listFileNames displays all the Files Exists in the Virtual Disk by Setting the Pointer At the End of the File Name, Then at start and pass it
 * to printFileName Function
 */
int listFileNames(FILE *metaDataPointer, unsigned int countOfFiles)
{
	unsigned long long int curFileSize;
	fseek(metaDataPointer,4,SEEK_SET);
	for(int i = 0; i < countOfFiles; i++){
		curFileSize = decode(metaDataPointer);
		if(fgetc(metaDataPointer)==1)
			printFileName(metaDataPointer);
	}
}

/* It is the Main Function for vdls Operation which deals with all kinds of Validations for Input */
int main(int argc, char *argv[])
{
	/* Validating Number of Arguments */
	if(argc!=2)
	{
		printf("Error : Invalid Number of Arguments Passed!!!\n");
		return 0;
	}
	
	/* Validating whether the Disk passed is Binary File or Not */
	if(isBinaryFile(argv[1])==0)
		return 0;
	
	/* File Pointers for file's names*/
	FILE *countPointer = fopen(argv[1],"r");
	
	FILE *metaDataPointer = fopen(argv[1],"r");
	
	unsigned int countOfFiles;
	
	countOfFiles = getNoOfFilesInDisk(countPointer);
	// getNoOfFilesInDisk returns count of Existing Files in the Virtual Disk
	
	if(countOfFiles == getNoOfHolesInDisk(metaDataPointer,countOfFiles))
	{
		printf("Disk is Empty !!!\n");
	}
	else{
		printf("Disk Name:\t%s\nFiles In Disk are:\n",argv[1]);
		
		// listFileNames displays all the Files Exists in the Virtual Disk
		listFileNames(metaDataPointer, countOfFiles);
	}
	
	/* CLosing All File Pointers */
	
	fclose(metaDataPointer);
	
	return 0;
	
}
