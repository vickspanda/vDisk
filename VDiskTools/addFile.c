#include<stdio.h>
#include<string.h>
#include"helper-tools/helper.c"
#include"helper-tools/encode.c"
#include"helper-tools/vDisk.c"
#include"helper-tools/deFrag.c"

/* 
 * This is the c File which will Add Files to given virtual Disk
 */


/* addFile is the Function Responsible for Addition of the FIle in the Virtual Disk */
int addFile(const char *vDisk, const char *inputFileName)
{
	/* File Pointers , one for countOfFiles, Second for encoded seq of sizes of files, third for file's names and Fourth for File's Data*/
	FILE *countPointer = fopen(vDisk,"r+");

	FILE *metaDataPointer = fopen(vDisk,"r+");

	FILE *fileDataPointer = fopen(vDisk,"r+");

	FILE *inputFile = fopen(inputFileName,"r");



	union decStore fileSize = getSize(inputFileName);
	unsigned char *out, countOfSeqBits, sizeOfOut;
	unsigned int countOfFiles;
	
	countOfFiles = getNoOfFilesInDisk(countPointer);
	// getNoOfFilesInDisk returns count of Existing Files in the Virtual Disk
	 
	out = (unsigned char *)calloc(11,sizeof(unsigned char));

	countOfSeqBits = getEncodedSeqOfFileSize(out,fileSize);
	/* getEncodedSeqOfFileSize returns the count Of bits of Encoded Seq Of file Size passed in it and prepares the encoded Sequence */

	sizeOfOut = bitsToBytes(countOfSeqBits);
	/* bitsToBytes is the Function which takes Number of Bits and convert it into Number of Bytes */
	

	/*
	* storeMetaData is the Function which stores the encoded Seq present along with respective file Name in *out in the virtual Disk by 
	* seting the file pointer and writing the content
	*/
	storeMetaData(metaDataPointer, out, sizeOfOut, countOfFiles, inputFileName);


	/*
	* storeFileData is the Function which stores the data of *inputFile in the virtual Disk by 	
	* seting the file pointer and writing the content
	*/
	storeFileData(fileDataPointer, metaDataPointer, countOfFiles, fileSize, inputFile);


	/* updateNoOfFiles function updates the count and set it as 2nd Argument passed in it */
	updateNoOfFilesInDisk(countPointer,countOfFiles+1);

	free(out);

	/* CLosing All File Pointers */
	fclose(countPointer);
		
	fclose(metaDataPointer);

	fclose(fileDataPointer);

	fclose(inputFile);

	printf("Success : %s Added Successfully !!!\n",inputFileName);
						
	return 0;

}


void makeDecisionAndAddFile(const char* vDisk, const char* inputFileName)
{
	/* File Pointers , one for countOfFiles, Second for encoded seq of sizes of files, third for file's names and Fourth for File's Data*/
	FILE *countPointer = fopen(vDisk,"r+");

	FILE *metaDataPointer = fopen(vDisk,"r+");

	union decStore fileSize = getSize(inputFileName);

	unsigned int countOfFiles;
	
	countOfFiles = getNoOfFilesInDisk(countPointer);
	// getNoOfFilesInDisk returns count of Existing Files in the Virtual Disk


	/*
	 * searchInFiles returns the Index of the File in the Virtual Disk if exists,
	 * if not Found, then it returns -1
	 */
	if(searchInFiles(metaDataPointer, countOfFiles, inputFileName)>=0)
		printf("Error : %s Already Exists !!!\n",inputFileName);
	else
	{
		if(canBeAdded(vDisk, fileSize.num, metaDataPointer, countOfFiles, inputFileName))
		{
			fclose(countPointer);
		
			fclose(metaDataPointer);

			addFile(vDisk,inputFileName);

		}
		else if(canBeAddedInHoles(vDisk, fileSize.num, metaDataPointer, countOfFiles, inputFileName))
		{
			fclose(countPointer);
		
			fclose(metaDataPointer);

			int index = defragmentor(vDisk,fileSize);

			addFileAtIndex(vDisk,fileSize,inputFileName,index);
		}
		else if(canBeAddedInHolesWithFreeSpace(vDisk, fileSize.num, metaDataPointer, countOfFiles, inputFileName))
		{
			fclose(countPointer);
		
			fclose(metaDataPointer);

			printf("This may take time ... Please wait for a while ...\n");
			hardCompaction(vDisk);

			addFile(vDisk,inputFileName);
		}
		else
			printf("Error : Sufficient Space Not Available to Store %s !!!\n",inputFileName);
	}
}

/* It is the Main Function for vdAddFile Operation which deals with all kinds of Validations for Input */
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
	
	while(i!=(argc-2))
	{
		/* FILE Pointer for the file to be Added in Read Mode */
		FILE * file = fopen(argv[i+2],"r");
		if(!file)
		{
			perror("Error ");
		}
		else
		{
			/* CLosing File Pointers */
        	fclose(file);
			makeDecisionAndAddFile(argv[1],argv[i+2]);
		}
		i++;	

		/* addFile is the Function Responsible for Addition of the FIle in the Virtual Disk */
	}

	return 0;
}
