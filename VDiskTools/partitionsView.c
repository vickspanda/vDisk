#include<stdio.h>
#include<string.h>
#include"helper-tools/helper.c"
#include"helper-tools/encode.c"
#include"helper-tools/vDisk.c"
#include"helper-tools/deFrag.c"


/* It is the Program to Print the Information of the Partitions created for Storing the Files or Holes */
void main(int argc, char *argv[])
{
    FILE * countPointer = fopen(argv[1],"r+");

    unsigned int countOfFiles = getNoOfFilesInDisk(countPointer);

    FILE * leftMetaPointer = fopen(argv[1],"r");

    unsigned int countOfHoles = getNoOfHolesInDisk(leftMetaPointer,countOfFiles);

    unsigned long long fileSizeArray[countOfFiles];

    int fileNameSizeArray[countOfFiles], metaArray[countOfFiles], holesArray[countOfHoles];

    createMetaTable(leftMetaPointer,fileSizeArray,fileNameSizeArray,metaArray,countOfFiles);

    genHolesArray(fileNameSizeArray, holesArray,countOfFiles);

    printf("Index No.:\t");
    
    for(int i = 0; i < countOfFiles; i++)
        printf("%d\t",i);
    
    printf("\nFile Size:\t");
    
    printSizeArray(fileSizeArray,countOfFiles);

    printf("\nIs Hole ?:\t");
    
    for(int i = 0; i < countOfFiles; i++){
        if(fileNameSizeArray[i]==1)
            printf("Y\t");
        else
            printf("N\t");
    }

    printf("\nName Size:\t");
    
    printArray(fileNameSizeArray,countOfFiles);
    
    printf("\nSeq Size:\t");
    
    printArray(metaArray,countOfFiles);
    
    printf("\nHoles Index:\t");
    
    printArray(holesArray,countOfHoles);
    
    printf("\n");

    fclose(countPointer);
    
    fclose(leftMetaPointer);
    
}
