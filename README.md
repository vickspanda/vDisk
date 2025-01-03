# Virtual Disk File System

This project implements a **Virtual Disk File System** that allows users to create a virtual disk and perform file management operations such as adding, retrieving, deleting files, and more. All operations are done within the virtual disk created as a binary file. The file system includes efficient space management with automatic defragmentation to maintain contiguous file storage.

## Features

- **vdCreate**: Create a virtual disk of a specified size.
- **vdAddFile**: Add files to the virtual disk.
- **vdDetails**: View the details of the virtual disk, such as used and available space.
- **vdls**: List all files stored in the virtual disk.
- **vdGet**: Retrieve files from the virtual disk to the current directory.
- **vdRename**: Renames files from old name to new name in the virtual disk with automatic space defragmentation.
- **vdDelete**: Delete files from the virtual disk, leaving holes that can later be merged during file addition.
- **vdView**: View the information of the partitions created for storing the files or holes.

## Installation

1. Clone the repository:
   ```bash
   git clone https://github.com/vickspanda/vDisk.git
   cd vDisk
   ```

2. Compile the source code:
   ```bash
   make
   ```
   After compilation, a binary file named `myDisk` of size 4 MB will be created by default, which acts as the virtual disk. But you can create your own too, if you want to.

## Usage

### 1. Creating a Virtual Disk
To create a new virtual disk, run the following command:

```bash
./.vdCreate <disk-name> <size-in-MB>
```

Example:
```bash
./.vdCreate myDisk 10
```
This will create a binary file named `myDisk` of size 10 MB, which acts as the virtual disk.

### 2. Adding Files to the Virtual Disk
To add files to the virtual disk:

```bash
./.vdAddFile <your-disk> <file1> <file2> ...
```

Example:
```bash
./.vdAddFile myDisk file1.txt file2.pdf
```
This will add `file1.txt` and `file2.pdf` to `myDisk` unless they already exist. If the operation requires merging multiple holes in the disk, it will do so by shifting data to ensure the new file fits.

### 3. Viewing Disk Details
To see details such as disk name, disk size, free space, and used space:

```bash
./.vdDetails <your-disk>
```

Example:
```bash
./.vdDetails myDisk
```

### 4. Listing Files in the Virtual Disk
To list all files stored in your virtual disk:

```bash
./.vdls <your-disk>
```

Example:
```bash
./.vdls myDisk
```

### 5. Retrieving Files from the Virtual Disk
To retrieve files from your virtual disk to your current directory:

```bash
./.vdGet <your-disk> <file1> <file2> ...
```

Example:
```bash
./.vdGet myDisk file1.txt file2.pdf
```
This will copy `file1.txt` and `file2.pdf` from `myDisk` to your current working directory if they exist in `myDisk`.

### 6. Rename Files in the Virtual Disk
To rename files in the virtual disk:

```bash
./.vdRename <your-disk> <old-file-name> <new-file-name>
```

Example:
```bash
./.vdRename myDisk file1.txt newfile.txt
```
After rename, file1.txt will be named as newfile.txt

### 7. Deleting Files from the Virtual Disk
To delete files from the virtual disk:

```bash
./.vdDelete <your-disk> <file1> <file2> ...
```

Example:
```bash
./.vdDelete myDisk file1.txt
```
After deletion, the disk will have a "hole" in the space previously occupied by the deleted file. These holes are later merged during file addition operations to ensure efficient use of space.

### 8. View Partitions in the Virtual Disk
To view the information of the partitions created for storing the files or holes:

```bash
./.vdView <your-disk>
```

Example:
```bash
./.vdView myDisk
```
This will display details about partitions, including file locations and sizes, as well as holes present in the disk.

## Notes

- If a file already exists in the disk, it will not be added again during the `vdAddFile` operation.
- If a file already exists in the virtual disk with `new-file-name` or if the extension of `new-file-name` is different from `old-file-name`, then it will not allow the `vdRename` operation.
- During file addition, any existing holes in the disk are merged to accommodate the new file, and data is shifted as necessary to ensure proper space utilization.

