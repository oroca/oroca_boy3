/*===================================================================*/
/*                                                                   */
/*  pNesX_System.h : The function which depends on a system          */
/*                                                                   */
/*  1999/11/03  Racoon  New preparation                              */
/*                                                                   */
/*===================================================================*/

#ifndef PNESX_SYSTEM_H_INCLUDED
#define PNESX_SYSTEM_H_INCLUDED

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include "pNesX_Types.h"

/*-------------------------------------------------------------------*/
/*  Palette data                                                     */
/*-------------------------------------------------------------------*/
extern const WORD NesPalette[];

/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/

/* Menu screen */
int pNesX_Menu();

/* Read ROM image file */
int pNesX_ReadRom( const char *pszFileName );

/* Release a memory for ROM */
void pNesX_ReleaseRom();

/* Transfer the contents of work frame on the screen */
void pNesX_LoadFrame();
void pNesX_TransmitLinedata();

/* Get a joypad state */
void pNesX_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem );

/* memcpy */
void *pNesX_MemoryCopy( void *dest, const void *src, int count );

/* memset */
void *pNesX_MemorySet( void *dest, int c, int count );

/* Print debug message */
void pNesX_DebugPrint( char *pszMsg );

#endif /* !PNESX_SYSTEM_H_INCLUDED */


