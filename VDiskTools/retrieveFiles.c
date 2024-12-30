#include<stdio.h>
#include<string.h>
#include"helper-tools/helper.c"
#include"helper-tools/encode.c"
#include"helper-tools/vDisk.c"

/* 
 * This is the c File which will make a copy of File present in given virtual Disk in our current Working Directory
 */


// writeFile Function writes the Data from the Virtual Disk to the External File for the retrieval of the existing File in the Virtual Disk
void writeFile(FILE *fileDataPointer, const char *fileName, int index, FILE *metaDataPointer)
{
	fseek(metaDataPointer,4,SEEK_SET);
	fseek(fileDataPointer,0,SEEK_END);
	FILE *getFile = fopen(fileName,"w");
	unsigned long long int curFileSize;
	
	for(int i=0; i<=index; i++)
	{
		curFileSize = decode(metaDataPointer);
		fseek(fileDataPointer,-curFileSize,SEEK_CUR);
		if(fgetc(metaDataPointer)==1)
			skipName(metaDataPointer);
	}

	for(int i = 0; i<curFileSize; i++)
		fputc(fgetc(fileDataPointer),getFile);

	fseek(metaDataPointer,0,SEEK_SET);
	fclose(getFile);
	printf("Success : %s Retrieved Successfully !!!\n",fileName);
}

/* It is the Main Function for vdGet Operation which deals with all kinds of Validations for Input */
int main(int argc, char *argv[])
{
	int i=0;
	/* Validating Number of Arguments */
	if(argc<3)
	{
		printf("Error : Invalid Number of Arguments Passed!!!\n");
		return 0;
	}

	/* Validating whether the Disk passed is Binary File or Not */
	if(isBinaryFile(argv[1])==0)
		return 0;

	unsigned int countOfFiles;
	
	/* Three File Pointers , one for countOfFiles, Second for encoded seq of sizes of files and third for file's names*/
	FILE *countPointer = fopen(argv[1],"r");
	
	FILE *metaDataPointer = fopen(argv[1],"r");

	FILE *fileDataPointer = fopen(argv[1],"r");
	
	countOfFiles = getNoOfFilesInDisk(countPointer);
	// getNoOfFilesInDisk returns count of Existing Files in the Virtual Disk
	
	if(countOfFiles == getNoOfHolesInDisk(metaDataPointer,countOfFiles))
	{
		printf("Disk is Empty !!!\n");
		return 0;
	}
	
	while(i!=(argc-2))
	{
		// searchInFiles returns the Index of the File in the Virtual Disk if exists, else it will return -1
		int index = searchInFiles(metaDataPointer, countOfFiles, argv[i+2]);
		if(index==-1)
			printf("Error : %s File Does Not Exists in %s !!!\n",argv[i+2],argv[1]);
		else
			writeFile(fileDataPointer,argv[i+2],index, metaDataPointer);
		i++;
	}
	
	/* 
	 * writeFile Function writes the content for the retrieval of the File
	 */
	 
	 /* CLosing All File Pointers */
	fclose(countPointer);
	
	fclose(metaDataPointer);
	
	fclose(fileDataPointer);
	
	return 0;
}
