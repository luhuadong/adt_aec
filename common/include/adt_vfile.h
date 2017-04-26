#ifndef _VFILE_H
#define _VFILE_H
#include <stdio.h>
typedef struct
{
	long int NWords;
	short int *pStart;
	long int WordIndex;
	short int ReadDispositionFlag;
	FILE *WriteFileHandle;	//if doing writes, this points to the FILE * handle, otherwise zero
}		VFile_t;
typedef struct
{
	char FileName[100];
	short int *FileArray;
}		FileSystem_t;

void VF_initFileSystem(int MaxFiles);
void VF_closeFileSystem();
void VF_addFile(char *pFName, short int *pFileArray);
VFile_t *VF_open_array(short int *pFileArray, char *Disposition);
VFile_t *VF_open(char *pFileName, char *Disposition);
void VF_close(VFile_t *pFile);
int VF_read_words(short int *pBuff, int WordCount, VFile_t *pFile);
int VF_write_words(short int *pBuff, int WordCount, VFile_t *pFile);	// Not implemented
int VF_feof(VFile_t *pFile);

#ifdef USE_VFILE
#define VFILE VFile_t
#define VOPEN(name,disposition) VF_open(name,disposition)
#define VCLOSE(handle) VF_close(handle)
#define VREAD_WORDS(buffer, length, file) VF_read_words(buffer, length, file)
#define VWRITE_WORDS(buffer, length, file) VF_write_words(buffer, length, file)
#define VFEOF(file) VF_feof(file)
extern void MyVF_init(int);
#define VFINIT(nfiles) MyVF_init(nfiles)
#else
#define VFILE FILE
#define VOPEN(name, disposition) fopen(name, disposition)
#define VCLOSE(handle) fclose(handle)
#define VREAD_WORDS(buffer, length, file) fread(buffer, sizeof(short int), length, file)
#define VWRITE_WORDS(buffer, length, file) fwrite(buffer, sizeof(short int), length, file)
#define VFEOF(file) feof(file)
#define VFINIT(nfiles)
#endif
#endif //_VFILE_H
