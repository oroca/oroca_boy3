/*
 * files.h
 *
 *  Created on: 2019. 11. 6.
 *      Author: HanCheol Cho
 */

#ifndef SRC_AP_FMSX_MAIN_FILES_H_
#define SRC_AP_FMSX_MAIN_FILES_H_



#ifdef __cplusplus
 extern "C" {
#endif


#include "hw_def.h"


int initFiles();
char* cutExtension(char* file);
const char* getFileName(const char* file);
char* getPath(char* file);
char* getFullPath(char* buffer, const char* fileName, int bufferLength);
bool hasExt(const char *file, const char *Ext);

//DIR* _opendir(const char* name);
//void _rewinddir(DIR* pdir);
//struct dirent* _readdir(DIR* pdir);
int _stat( const char *__restrict __path, struct stat *__restrict __sbuf );
FILE* _fopen(const char *__restrict _name, const char *__restrict _type);
//void _seekdir(DIR* pdir, long loc);
//long _telldir(DIR* pdir);
int _fclose(FILE* file);
size_t _fread(_PTR __restrict p, size_t _size, size_t _n, FILE *__restrict f);
size_t _fwrite(const _PTR __restrict p , size_t _size, size_t _n, FILE * f);
//int _closedir(DIR* pdir);

void _rewind(FILE* f);
int _fscanf(FILE *__restrict f, const char *__restrict c, ...);
int _fprintf(FILE *__restrict f, const char *__restrict c, ...);
int _fgetc(FILE * f);
int _fputc(int i, FILE * f);
int _fputs(const char *__restrict c, FILE *__restrict f);
char * _fgets(char *__restrict c, int i, FILE *__restrict f);
int _fseek(FILE * f, long a, int b);
long _ftell( FILE * f);




#ifdef __cplusplus
 }
#endif



#endif /* SRC_AP_FMSX_MAIN_FILES_H_ */
