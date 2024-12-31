final:
	gcc VDiskTools/createDisk.c -o vdCreate
	./.vdCreate myDisk 4 
	gcc VDiskTools/addFile.c -o vdAddFile
	gcc VDiskTools/diskProperties.c -o vdDetails
	gcc VDiskTools/partitionsView.c -o vdView
	gcc VDiskTools/createFile.c -o vdCreateFile
	gcc VDiskTools/listFiles.c -o vdls
	gcc VDiskTools/retrieveFiles.c -o vdGet
	gcc VDiskTools/renameFile.c -o vdRename
	gcc VDiskTools/deleteFiles.c -o vdDelete
	
