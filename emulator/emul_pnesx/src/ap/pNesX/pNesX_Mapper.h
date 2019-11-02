/*===================================================================*/
/*                                                                   */
/*  pNesX_Mapper.h : pNesX Mapper Function                           */
/*                                                                   */
/*  1999/11/03  Racoon  New preparation                              */
/*                                                                   */
/*===================================================================*/

#ifndef PNESX_MAPPER_H_INCLUDED
#define PNESX_MAPPER_H_INCLUDED

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include "pNesX_Types.h"

/*-------------------------------------------------------------------*/
/*  Macros                                                           */
/*-------------------------------------------------------------------*/

/* The address of 8Kbytes unit of the ROM */
#define ROMPAGE(a)     &ROM[ (a) * 0x2000 ]
/* From behind the ROM, the address of 8kbytes unit */
#define ROMLASTPAGE(a) &ROM[ NesHeader.byRomSize * 0x4000 - ( (a) + 1 ) * 0x2000 ]

/* The address of 1Kbytes unit of the VROM */
#define VROMPAGE(a)    &VROM[ (a) * 0x400 ]

/*-------------------------------------------------------------------*/
/*  Table of Mapper initialize function                              */
/*-------------------------------------------------------------------*/

struct MapperTable_tag
{
  int nMapperNo;
  void (*pMapperInit)();
};

extern struct MapperTable_tag MapperTable[];

/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/

void Map0_Init();
void Map0_Write( WORD wAddr, BYTE byData );
void Map0_VSync();
void Map0_HSync();

void Map1_Init();
void Map1_Write( WORD wAddr, BYTE byData );

void Map2_Init();
void Map2_Write( WORD wAddr, BYTE byData );

void Map3_Init();
void Map3_Write( WORD wAddr, BYTE byData );

void Map4_Init();
void Map4_Write( WORD wAddr, BYTE byData );
void Map4_VSync();
void Map4_HSync();

#endif /* !PNESX_MAPPER_H_INCLUDED */


