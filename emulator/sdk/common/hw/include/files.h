/*
 * files.h
 *
 *  Created on: 2019. 11. 10.
 *      Author: Baram
 */

#ifndef SRC_COMMON_HW_INCLUDE_FILES_H_
#define SRC_COMMON_HW_INCLUDE_FILES_H_


#ifdef __cplusplus
extern "C" {
#endif


#include "hw_def.h"

#ifdef _USE_HW_I2C


FILE  *ob_fopen(const char *filename, const char *mode);
int    ob_fclose(FILE *stream);
size_t ob_fread(void *ptr, size_t size, size_t count, FILE *stream);
size_t ob_fwrite(const void *ptr, size_t size, size_t count, FILE *stream);
int    ob_fflush(FILE *stream);
int    ob_feof(FILE *stream);
int    ob_fseek(FILE *stream, long offset, int whence);
long   ob_ftell(FILE *stream);

#endif


#ifdef __cplusplus
}
#endif



#endif /* SRC_COMMON_HW_INCLUDE_FILES_H_ */
