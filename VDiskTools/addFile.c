#include<stdio.h>
#include<string.h>
#include"helper-tools/helper.c"
#include"helper-tools/encode.c"
#include"helper-tools/vDisk.c"

/* 
 * This is the c File which will Add Files to given virtual Disk
 */


/* addFile is the Function Responsible for Addition of the FIle in the Virtual Disk */
int addFile(const char *vDisk, FILE *inputFile, const char *inputFileName)
{
	union decStore fileSize = getSize(inputFileName);
	fileSize.num = fileSize.num + getSizeOfFileName(inputFileName);
	unsigned char *out, countOfSeqBits, sizeOfOut;
	unsigned int countOfFiles;
	

	/* Four File Pointers , one for countOfFiles, Second for encoded seq of sizes of files, third for file's names and Fourth for File's Data*/
	FILE *countPointer = fopen(vDisk,"r+");

	FILE *encodedSeqPointer = fopen(vDisk,"r+");

	FILE *fileDataPointer = fopen(vDisk,"r+");
	
	countOfFiles = getNoOfFilesInDisk(countPointer);
	// getNoOfFilesInDisk returns count of Existing Files in the Virtual Disk
	
	out = (unsigned char *)calloc(11,sizeof(unsigned char));

	countOfSeqBits = getEncodedSeqOfFileSize(out,fileSize);
	/* getEncodedSeqOfFileSize returns the count Of bits of Encoded Seq Of file Size passed in it and prepares the encoded Sequence */

	sizeOfOut = bitsToBytes(countOfSeqBits);
	/* bitsToBytes is the Function which takes Number of Bits and convert it into Number of Bytes */
	
	if(searchInFiles(fileDataPointer, encodedSeqPointer, countOfFiles, inputFileName)>=0)
	{
		printf("Error : %s Already Exists !!!\n",inputFileName);
		return 0;
	}
	/*
	 * searchInFiles returns the Index of the File in the Virtual Disk if exists,
	 * if not Found, then it returns -1
	 */
	 
	if(!canBeAdded(vDisk, fileSize.num, encodedSeqPointer, countOfFiles))
	{
		printf("Error : Sufficient Space Not Available to Store %s !!!\n",inputFileName);
		return 0;
	}
	
	/*
	 * canBeAdded Function returns true if Virtual Disk whose file Pointer was provided as an input to it
	 * have the Space to Add new File in it	
	 * else returns false
	 */
	 
	 /*
 	  * storeEncodedSeq is the Function which stores the encoded Seq present in *out in the virtual Disk by 
	  * seting the file pointer and writing the content
	  */
	storeEncodedSeq(encodedSeqPointer, out, sizeOfOut, countOfFiles);


	/*
	 * storeFileData is the Function which stores the data of *inputFile in the virtual Disk by 	
	 * seting the file pointer and writing the content
	 */
	storeFileData(fileDataPointer, encodedSeqPointer, countOfFiles, fileSize, inputFile, inputFileName);


	/* updateNoOfFiles function updates the count and set it as 2nd Argument passed in it */
	updateNoOfFilesInDisk(countPointer,countOfFiles+1);
	
	/* CLosing All File Pointers */
	fclose(countPointer);
	
	fclose(encodedSeqPointer);
	
	fclose(fileDataPointer);
	
	//fclose(fileNamePointer);
	
	printf("Success : %s Added Successfully !!!\n",inputFileName);
	
	return 0;

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
		}else
		{
			addFile(argv[1],file,argv[i+2]);
			
			/* CLosing All File Pointers */
        		fclose(file);
		}
		i++;
        	
	
		/* addFile is the Function Responsible for Addition of the FIle in the Virtual Disk */
	}
	
		
	return 0;
}
