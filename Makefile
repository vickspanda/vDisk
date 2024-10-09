final:
	gcc VDiskTools/createDisk.c -o vdCreate
	./vdCreate myDisk 4
	gcc VDiskTools/addFile.c -o vdAddFile
	gcc VDiskTools/diskProperties.c -o vdDetails
	gcc VDiskTools/listFiles.c -o vdls
	gcc VDiskTools/retrieveFiles.c -o vdGet
	gcc VDiskTools/deleteFiles.c -o vdDelete
	gcc VDiskTools/renameFile.c -o vdRename
