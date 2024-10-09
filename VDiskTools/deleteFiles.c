#include<stdio.h>
#include<string.h>
#include"helper-tools/helper.c"
#include"helper-tools/encode.c"
#include"helper-tools/vDisk.c"

/* 
 * This is the c File which Delete the File present in given virtual Disk
 */
 
 
/* This Function Removes the Seq Part of the File from the Virtual Disk*/
void replaceSeq(FILE *leftSeqPointer,FILE *rightSeqPointer,unsigned char bytes)
{
	char ch;
	for(unsigned int i=0; i<bytes; i++)
		fputc(fgetc(rightSeqPointer),leftSeqPointer);

}

/* This Function Deletes the Encoded Sequence Part of the File from the Virtual Disk*/
void deleteEncodedSeq(FILE *rightSeqPointer, FILE *leftSeqPointer, unsigned int countOfFiles, int index)
{
	unsigned char bytes;
	unsigned long long int curFileSize;
	fseek(rightSeqPointer,4,SEEK_SET);
	fseek(leftSeqPointer,4,SEEK_SET);
	
	for(unsigned int i=0; i<index; i++)
		curFileSize = decode(leftSeqPointer);

	for(unsigned int i=0; i<index+1; i++)
		curFileSize = decode(rightSeqPointer);

	for(unsigned int i=index+1; i < countOfFiles; i++){
		curFileSize = decode(rightSeqPointer);
		bytes = calCountOfBytesSeq(curFileSize);
		fseek(rightSeqPointer,-bytes,SEEK_CUR);
		replaceSeq(leftSeqPointer,rightSeqPointer,bytes);
	}
	
}


/* This Function Removes the Data Part of the File from the Virtual Disk*/
void compaction(FILE *leftFilePointer, FILE *rightFilePointer, unsigned int curFileSize)
{
	for(unsigned long long int i=0; i < curFileSize; i++)
	{
		fputc(fgetc(leftFilePointer),rightFilePointer);
		fseek(leftFilePointer,-2,SEEK_CUR);
		fseek(rightFilePointer,-2,SEEK_CUR);
	}
	fseek(leftFilePointer,1,SEEK_CUR);
	fseek(rightFilePointer,1,SEEK_CUR);
}


/* deleteFileData Function Delete the File Data from the End Portion of the Virtual Disk */
void deleteFileData(FILE *leftFilePointer,FILE *rightFilePointer, const char *fileName, int index, FILE *encodedSeqPointer, unsigned int countOfFiles)
{
	fseek(rightFilePointer,0,SEEK_END);
	fseek(encodedSeqPointer,4,SEEK_SET);
	unsigned long long int curFileSize;
	
	for(int i=0; i<=index; i++){
		curFileSize = decode(encodedSeqPointer);
		if(i<=(index-1)){
			fseek(rightFilePointer,-curFileSize,SEEK_CUR);
		}
	}
	
	for(int i=index+1; i < countOfFiles; i++){
		fseek(leftFilePointer,-1,SEEK_CUR);
		fseek(rightFilePointer,-1,SEEK_CUR);
		curFileSize = decode(encodedSeqPointer);
		compaction(leftFilePointer,rightFilePointer,curFileSize);
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

		FILE *encodedSeqPointer = fopen(argv[1],"r+");

		FILE *leftFilePointer = fopen(argv[1],"r");

		FILE *rightFilePointer = fopen(argv[1],"r+");
	
		FILE *leftSeqPointer = fopen(argv[1],"r+");
			
		FILE *rightSeqPointer = fopen(argv[1],"r+");
		
		
		countOfFiles = getNoOfFilesInDisk(countPointer);
		// getNoOfFilesInDisk returns count of Existing Files in the Virtual Disk
		
		
		if(countOfFiles == 0)
		{
			printf("Disk is Empty !!!\n");
			return 0;
		}
	

		
		// searchInFiles returns the Index of the File in the Virtual Disk if exists, else it will return -1
		int index = searchInFiles(leftFilePointer, encodedSeqPointer, countOfFiles, argv[i+2]);
		if(index==-1)
			printf("Error : %s File Does Not Exists in %s !!!\n",argv[i+2],argv[1]);
		else{
			
			deleteFileData(leftFilePointer,rightFilePointer, argv[i+2],index, encodedSeqPointer, countOfFiles);
			
			deleteEncodedSeq(leftSeqPointer, rightSeqPointer, countOfFiles, index);
			
			updateNoOfFilesInDisk(countPointer,countOfFiles-1);
			
			/* CLosing All File Pointers */
			fclose(countPointer);
		
			fclose(encodedSeqPointer);
	
			fclose(leftFilePointer);
	
			fclose(rightFilePointer);
			
			fclose(rightSeqPointer);
			
			fclose(leftSeqPointer);
			
			printf("Success : %s File Deleted Successfully !!!\n",argv[i+2]);
		}
		i++;
	}
	
	/* 
	 * deleteFileData Function writes the content for the retrieval of the File
	 */
	 
	 
	
	return 0;
}
