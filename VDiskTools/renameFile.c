#include<stdio.h>
#include<string.h>
#include"helper-tools/helper.c"
#include"helper-tools/encode.c"
#include"helper-tools/vDisk.c"

/* 
 * This is the c File which Delete the File present in given virtual Disk
 */
 
/* This Function Updates the Seq Part of the File from the Virtual Disk, if After Renaming, size of new encoded Seq of Size is greater than Size of previous Encoded Sequence */
void replaceSeqToRight(FILE *leftSeqPointer,FILE *rightSeqPointer,unsigned int bytes)
{
	for(int i=0; i < bytes; i++)
	{
		fputc(fgetc(leftSeqPointer),rightSeqPointer);
		fseek(leftSeqPointer,-2,SEEK_CUR);
		fseek(rightSeqPointer,-2,SEEK_CUR);
	}
	fseek(rightSeqPointer,1,SEEK_CUR);
	fseek(leftSeqPointer,1,SEEK_CUR);
}

/* This Function Updates the Seq Part of the File from the Virtual Disk, if After Renaming, size of new encoded Seq of Size is lesser than Size of previous Encoded Sequence */
void replaceSeqToLeft(FILE *leftSeqPointer,FILE *rightSeqPointer,unsigned int bytes)
{
	for(unsigned int i=0; i<bytes; i++)
		fputc(fgetc(rightSeqPointer),leftSeqPointer);
}


/* This Function Updates the File Name in the Virtual Disk */
int renameFile(FILE *metaDataPointer, const char newName[])
{
	int i = 0;
	while(newName[i] !='\0')
		fputc(newName[i++],metaDataPointer);
	return i;
}

/* 
 * This Function Shifts the Seq Part of the File from the index of that file which needs to be renamed, 
 * Only if After Renaming, size of new encoded Seq of File Size is greater than or lesser than Size of previous Encoded Sequence of File Size
 * and updates the new encoded seq on the Virtual Disk of the File which needs to be renamed (Also if size does not change)
 */
void shiftEncodedSeq(FILE *rightSeqPointer, FILE *leftSeqPointer, const char * oldFileName, const char * newFileName, unsigned int countOfFiles, int index)
{
	unsigned char bytes;
		
	fseek(rightSeqPointer,4,SEEK_SET);
	fseek(leftSeqPointer,4,SEEK_SET);
	
	int oldFileSize = strlen(oldFileName), newFileSize = strlen(newFileName);
	
	// if size of new encoded Seq of Size is lesser than Size of previous Encoded Sequence 
	if(newFileSize < oldFileSize)
	{
		for(int i=0;i<index;i++){
			fseek(rightSeqPointer,calCountOfBytesSeq(decode(leftSeqPointer)),SEEK_CUR);
			if(fgetc(leftSeqPointer)==1)
				skipName(leftSeqPointer);
			if(fgetc(rightSeqPointer)==1)
				skipName(rightSeqPointer);

		}
		fseek(rightSeqPointer,calCountOfBytesSeq(decode(leftSeqPointer)),SEEK_CUR);
		if(fgetc(leftSeqPointer)==1)
		{
			if(fgetc(rightSeqPointer)==1)
				skipName(rightSeqPointer);
			renameFile(leftSeqPointer,newFileName);
			fputc('\0',leftSeqPointer);
		}
		
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
	// if size of new encoded Seq of Size is greater than Size of previous Encoded Sequence 
	else if(newFileSize > oldFileSize)
	{
		int metaDataSize[countOfFiles];
		for(int i=0;i<countOfFiles;i++)
		{
			metaDataSize[i] = calCountOfBytesSeq(decode(rightSeqPointer));
			fseek(leftSeqPointer,metaDataSize[i],SEEK_CUR);
			
			if(i==index)
				bytes = metaDataSize[i];

			if(fgetc(rightSeqPointer)==1)
				metaDataSize[i] = metaDataSize[i] + getLengthOfFileName(rightSeqPointer) + 1;
			else
				metaDataSize[i] = metaDataSize[i] + 1;
			
			if(fgetc(leftSeqPointer)==1)
				skipName(leftSeqPointer);
		}
			
		fseek(rightSeqPointer,newFileSize-oldFileSize,SEEK_CUR);
		
		
		for(unsigned int i=countOfFiles-1; i > index; i--){
			fseek(leftSeqPointer,-1,SEEK_CUR);
			fseek(rightSeqPointer,-1,SEEK_CUR);
			replaceSeqToRight(leftSeqPointer,rightSeqPointer,metaDataSize[i]);
		}

		fseek(leftSeqPointer,4,SEEK_SET);
		for(int i = 0; i < index; i++)
			fseek(leftSeqPointer,metaDataSize[i],SEEK_CUR);

		fseek(leftSeqPointer,bytes,SEEK_CUR);

		if(fgetc(leftSeqPointer)==1)
		{
			renameFile(leftSeqPointer,newFileName);
			fputc('\0',leftSeqPointer);
		}	
	}
	
}



/* It is the Main Function for vdDelete Operation which deals with all kinds of Validations for Input */
int main(int argc, char *argv[])
{
	/* Validating Number of Arguments */
	if(argc!=4)
	{
		printf("Error : Invalid Number of Arguments Passed!!!\n");
		return 0;
	}

	/* Validating whether the Disk passed is Binary File or Not */
	if(isBinaryFile(argv[1])==0)
		return 0;

	unsigned int countOfFiles;

	unsigned long int offset = 0;
	
	// Getting Length of new and old file names
	int oldFileNameSize = strlen(argv[2]), newFileNameSize = strlen(argv[3]);
	
	char extension1[oldFileNameSize], extension2[newFileNameSize];
	
	// Getting Extensions for both File Names
	getFileExtension(argv[2], extension1);
	
	getFileExtension(argv[3], extension2);
	
	/* Three File Pointers , one for countOfFiles, Second for encoded seq of sizes of files and third for file's names*/
	
	
	FILE *countPointer = fopen(argv[1],"r+");

	FILE *metaDataPointer = fopen(argv[1],"r+");
	
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
	// Checking Whether new File Name already exists or not
	int index = searchInFiles(metaDataPointer, countOfFiles, argv[3]);
	if(index!=-1)
		printf("Error : %s File Name Already Exists in %s !!!\n",argv[3],argv[1]);
	else{
		// Checking Whether Old File Name already exists or not
		index = searchInFiles(metaDataPointer, countOfFiles, argv[2]);
		if(index==-1)
		printf("Error : %s File Does Not Exists in %s !!!\n",argv[2],argv[1]);
		else{
			// Checking Whether Extensions of both File is same or not
			if (strcmp(extension1, extension2) != 0)
			{
				printf("Error : Extensions of Old File Name and New File Name are Not Same\n");
				return 0;
			}
			
			// Checking Whether both File Names are same or not
			if (strcmp(argv[2], argv[3]) == 0)
			{
				printf("Error : Old File Name and New File Name are Same\n");
				return 0;
			}
			
			// If size of old and new file name is same
			if(oldFileNameSize==newFileNameSize)
				renameFile(metaDataPointer, argv[3]);
			// If size of old and new file name are different
			else if( oldFileNameSize < newFileNameSize )
				shiftEncodedSeq(leftSeqPointer, rightSeqPointer, argv[2], argv[3], countOfFiles, index);
			else
				shiftEncodedSeq(leftSeqPointer, rightSeqPointer, argv[2], argv[3], countOfFiles, index);
			
			
			/* CLosing All File Pointers */
			fclose(countPointer);
				
			fclose(metaDataPointer);
			
			fclose(rightSeqPointer);
					
			fclose(leftSeqPointer);
			
			printf("Success : %s File Renameded to %s Successfully !!!\n",argv[2],argv[3]);
		}
	}

	return 0;
}
