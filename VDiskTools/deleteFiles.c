#include<stdio.h>
#include<string.h>
#include"helper-tools/helper.c"
#include"helper-tools/encode.c"
#include"helper-tools/vDisk.c"

/* 
 * This is the c File which Delete the File present in given virtual Disk
 */
 
 
/* This Function Updates the Seq Part of the File from the Virtual Disk, if After Renaming, size of new encoded Seq of Size is lesser than Size of previous Encoded Sequence */
void replaceSeqToLeft(FILE *leftSeqPointer,FILE *rightSeqPointer,unsigned char bytes)
{
	char ch;
	for(unsigned int i=0; i<bytes; i++)
		fputc(fgetc(rightSeqPointer),leftSeqPointer);
}



/* deleteFileData Function Delete the File from the Virtual Disk */
void deleteFile(FILE *leftSeqPointer,FILE *rightSeqPointer, const char *fileName, int index, unsigned int countOfFiles)
{
	unsigned char bytes;
		
	fseek(rightSeqPointer,4,SEEK_SET);
	fseek(leftSeqPointer,4,SEEK_SET);
	
	for(int i=0;i<index;i++){
			fseek(rightSeqPointer,calCountOfBytesSeq(decode(leftSeqPointer)),SEEK_CUR);
			if(fgetc(leftSeqPointer)==1)
				skipName(leftSeqPointer);
			if(fgetc(rightSeqPointer)==1)
				skipName(rightSeqPointer);

		}
		fseek(rightSeqPointer,calCountOfBytesSeq(decode(leftSeqPointer)),SEEK_CUR);
		if(fgetc(leftSeqPointer)==1)
			fputc('\0',rightSeqPointer);

		fseek(rightSeqPointer,strlen(fileName)+1,SEEK_CUR);
		
		for(unsigned int i=index+1; i < countOfFiles; i++){
			bytes = calCountOfBytesSeq(decode(rightSeqPointer));
			fseek(rightSeqPointer,-bytes,SEEK_CUR);
			replaceSeqToLeft(leftSeqPointer,rightSeqPointer,bytes);

			int ch = fgetc(rightSeqPointer);
			fputc(ch,leftSeqPointer);

			if(ch==1)
			{
				bytes = getLengthOfFileName(rightSeqPointer);
				fseek(rightSeqPointer,-bytes,SEEK_CUR);
				replaceSeqToLeft(leftSeqPointer,rightSeqPointer,bytes);
			}

		}
}


/* It is the Main Function for vdDelete Operation which deals with all kinds of Validations for Input */
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

	
	while(i!=(argc-2))
	{
		/* Three File Pointers , one for countOfFiles, Second for encoded seq of sizes of files and third for file's names*/
	
	
		FILE *countPointer = fopen(argv[1],"r+");

		FILE *metaDataPointer = fopen(argv[1],"r");
	
		FILE *leftSeqPointer = fopen(argv[1],"r+");
			
		FILE *rightSeqPointer = fopen(argv[1],"r+");
		
		
		countOfFiles = getNoOfFilesInDisk(countPointer);
		// getNoOfFilesInDisk returns count of Existing Files in the Virtual Disk
		
		if(countOfFiles == getNoOfHolesInDisk(metaDataPointer,countOfFiles))
		{
			printf("Disk is Empty !!!\n");
			return 0;
		}
	

		
		// searchInFiles returns the Index of the File in the Virtual Disk if exists, else it will return -1
		int index = searchInFiles(metaDataPointer, countOfFiles, argv[i+2]);
		if(index==-1)
			printf("Error : %s File Does Not Exists in %s !!!\n",argv[i+2],argv[1]);
		else{
			
			deleteFile(leftSeqPointer,rightSeqPointer, argv[i+2],index, countOfFiles);

			/* CLosing All File Pointers */
			fclose(countPointer);
		
			fclose(metaDataPointer);
			
			fclose(rightSeqPointer);
			
			fclose(leftSeqPointer);
			
			printf("Success : %s File Deleted Successfully !!!\n",argv[i+2]);

			mergeConsecutiveHoles(argv[1]);
		}
		i++;
	}
	
	/* 
	 * deleteFileData Function writes the content for the retrieval of the File
	 */
	 
	 
	
	return 0;
}
