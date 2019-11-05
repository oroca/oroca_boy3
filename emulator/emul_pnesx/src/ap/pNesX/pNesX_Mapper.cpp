/*===================================================================*/
/*                                                                   */
/*  pNesX_Mapper.cpp : pNesX Mapper Function                         */
/*                                                                   */
/*  1999/11/03  Racoon  New preparation                              */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include "pNesX.h"
#include "pNesX_System.h"
#include "pNesX_Mapper.h"
#include "K6502.h"

/*-------------------------------------------------------------------*/
/*  Table of Mapper initialize function                              */
/*-------------------------------------------------------------------*/

struct MapperTable_tag MapperTable[] =
{
  { 0, Map0_Init },
  { 1, Map1_Init },
  { 2, Map2_Init },
  { 3, Map3_Init },
  { 4, Map4_Init },
  { -1, NULL }
};

/*===================================================================*/
/*                                                                   */
/*                            Mapper 0                               */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 0                                              */
/*-------------------------------------------------------------------*/
void Map0_Init()
{
  /* Initialize Mapper */
  MapperInit = Map0_Init;

  /* Write to Mapper */
  MapperWrite = Map0_Write;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map0_HSync;

  /* Set ROM Banks */
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMLASTPAGE( 1 );
  ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks */
  if ( NesHeader.byVRomSize > 0 )
  {
    for ( int nPage = 0; nPage < 8; ++nPage )
      PPUBANK[ nPage ] = &VROM[ nPage * 0x400 ];

    pNesX_SetupChr();
  }

  /* Set up wiring of the interrupt pin */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 0 Write Function                                          */
/*-------------------------------------------------------------------*/
void Map0_Write( WORD wAddr, BYTE byData )
{
/*
 *  Dummy Write to Mapper
 *
 */
}

/*-------------------------------------------------------------------*/
/*  Mapper 0 V-Sync Function                                         */
/*-------------------------------------------------------------------*/
void Map0_VSync()
{
/*
 *  Dummy Callback at VSync
 *
 */
}

/*-------------------------------------------------------------------*/
/*  Mapper 0 H-Sync Function                                         */
/*-------------------------------------------------------------------*/
void Map0_HSync()
{
/*
 *  Dummy Callback at HSync
 *
 */
}

/*===================================================================*/
/*                                                                   */
/*                            Mapper 1                               */
/*                                                                   */
/*===================================================================*/

BYTE Map1_Reg[ 4 ];

int Map1_Cnt;
BYTE Map1_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 1                                              */
/*-------------------------------------------------------------------*/
void Map1_Init()
{
  int nPage;

  /* Initialize Mapper */
  MapperInit = Map1_Init;

  /* Write to Mapper */
  MapperWrite = Map1_Write;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map0_HSync;

  pNesX_MemorySet( Map1_Reg, 0, sizeof Map1_Reg );
  Map1_Cnt = Map1_Latch = 0;

  /* Set ROM Banks */
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMLASTPAGE( 1 );
  ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU VROM Banks */
  if ( NesHeader.byVRomSize > 0 )
  {
    for ( nPage = 0; nPage < 8; ++nPage )
      PPUBANK[ nPage ] = &VROM[ nPage * 0x400 ];

    pNesX_SetupChr();
  }

  /* Set up wiring of the interrupt pin */
  K6502_Set_Int_Wiring( 1, 1 );
}

/*-------------------------------------------------------------------*/
/*  Mapper 1 Write Function                                          */
/*-------------------------------------------------------------------*/
void Map1_Write( WORD wAddr, BYTE byData )
{
/*
 * MMC1
 */

  int nReg;
  int nROMPos;
  int nBank;
  int nVBank;
  int nPage;

  nReg = ( wAddr >> 13 ) - 4;

  if ( byData & 0x80 )
  {
    // Reset
    Map1_Reg[ 0 ] |= 0xc;
    Map1_Cnt = 0;
    Map1_Latch = 0;
    return;
  }
  else
  {
    // Bit Data Set
    Map1_Latch |= ( ( byData & 1 ) << Map1_Cnt );
    ++Map1_Cnt;
  }
  
  if ( Map1_Cnt < 5 )
    return;

  // 5Bits Latched
  Map1_Reg[ nReg ] = ( nReg == 3 ) ? ( Map1_Latch & 0xf ) : Map1_Latch;
  Map1_Cnt = 0;
  Map1_Latch = 0;
  
  // Name Table Mirroring
  pNesX_Mirroring( ( Map1_Reg[ 0 ] & 3 ) ^ 3 );
    
  // Select Program ROM Bank

  // 256K ROM Position Selection
  nROMPos = 0;
  if ( NesHeader.byRomSize == 32 )
  {
    /* 512K Program ROM */
    // 0 (First 256K) or 32 (Second 256K)
    nROMPos = ( Map1_Reg[ 1 ] & 0x10 ) << 1;
  }
  else
  if ( NesHeader.byRomSize == 64 )
  {
    /* 1024K Program ROM */
    if ( Map1_Reg[ 0 ] & 0x10 )
    {
      // Acknowledge 256K selection register 1 
      nROMPos = ( ( Map1_Reg[ 1 ] & 0x10 ) << 1 ) |
               ( ( Map1_Reg[ 2 ] & 0x10 ) << 2 );
    }
    else 
    {
      // Ignore 256K selection register 1
      // 0 (First 256K) or 64 (Third 256K)
      nROMPos = ( Map1_Reg[ 1 ] & 0x10 ) << 2;
    }
  }
    
  // Set Program ROM Bank
  if ( !( Map1_Reg[ 0 ] & 8 ) )
  {
    // 32K ROM Bank
    /* Set ROM Banks */
    nBank = ( ( Map1_Reg[ 3 ] << 2 ) + nROMPos ) % ( NesHeader.byRomSize << 1 );

    ROMBANK0 = ROMPAGE( nBank );
    ROMBANK1 = ROMPAGE( nBank + 1 );
    ROMBANK2 = ROMPAGE( nBank + 2 );
    ROMBANK3 = ROMPAGE( nBank + 3 );
  }
  else
  if ( Map1_Reg[ 0 ] & 4 )
  {
    // 16K ROM Bank at 0x8000
    nBank = ( ( Map1_Reg[ 3 ] << 1 ) + nROMPos ) % ( NesHeader.byRomSize << 1 );
    ROMBANK0 = ROMPAGE( nBank );
    ROMBANK1 = ROMPAGE( nBank + 1 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );
  }
  else
  {
    // 16K ROM Bank at 0xc000
    nBank = ( ( Map1_Reg[ 3 ] << 1 ) + nROMPos ) % ( NesHeader.byRomSize << 1 );
    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( nBank );
    ROMBANK3 = ROMPAGE( nBank + 1 );
  }

  // Select PPU VROM Bank
  if ( NesHeader.byVRomSize > 0 )
  {
    if ( Map1_Reg[ 0 ] & 0x10 )
    {
      // 4K VROM Switching
      nVBank = Map1_Reg[ 1 ] % ( NesHeader.byVRomSize << 1 );
      for ( nPage = 0; nPage < 4; ++nPage )
        PPUBANK[ nPage ] = &VROM[ nVBank * 0x1000 + nPage * 0x400 ];

      nVBank = Map1_Reg[ 2 ] % ( NesHeader.byVRomSize << 1 );
      for ( nPage = 0; nPage < 4; ++nPage )
        PPUBANK[ nPage + 4 ] = &VROM[ nVBank * 0x1000 + nPage * 0x400 ];
    }
    else
    {
      // 8K VROM Switching
      nVBank = ( Map1_Reg[ 1 ] & 0xe ) % ( NesHeader.byVRomSize << 1 );
      for ( nPage = 0; nPage < 8; ++nPage )
        PPUBANK[ nPage ] = &VROM[ nVBank * 0x1000 + nPage * 0x400 ];
    }

    pNesX_SetupChr();
  }
}

/*===================================================================*/
/*                                                                   */
/*                        Mapper 2 (UNROM)                           */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 2                                              */
/*-------------------------------------------------------------------*/
void Map2_Init()
{
  /* Initialize Mapper */
  MapperInit = Map2_Init;

  /* Write to Mapper */
  MapperWrite = Map2_Write;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map0_HSync;

  /* Set ROM Banks */
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMLASTPAGE( 1 );
  ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set up wiring of the interrupt pin */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 2 Write Function                                          */
/*-------------------------------------------------------------------*/
void Map2_Write( WORD wAddr, BYTE byData )
{
  /* Set ROM Banks */
  byData %= NesHeader.byRomSize;
  byData <<= 1;

  ROMBANK0 = ROMPAGE( byData );
  ROMBANK1 = ROMPAGE( byData + 1 );
}


/*===================================================================*/
/*                                                                   */
/*                     Mapper 3 (VROM Switch)                        */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 3                                              */
/*-------------------------------------------------------------------*/
void Map3_Init()
{
  int nPage;

  /* Initialize Mapper */
  MapperInit = Map3_Init;

  /* Write to Mapper */
  MapperWrite = Map3_Write;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map0_HSync;

  /* Set ROM Banks */
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMLASTPAGE( 1 );
  ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks */
  if ( NesHeader.byVRomSize > 0 )
  {
    for ( nPage = 0; nPage < 8; ++nPage )
      PPUBANK[ nPage ] = &VROM[ nPage * 0x400 ];
    pNesX_SetupChr();
  }

  /* Set up wiring of the interrupt pin */
  /* "DragonQuest" doesn't run if IRQ isn't made to occur in CLI */
  K6502_Set_Int_Wiring( 1, 0 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 3 Write Function                                          */
/*-------------------------------------------------------------------*/
void Map3_Write( WORD wAddr, BYTE byData )
{
  int nPage;

  /* Set PPU Banks */
  byData %= NesHeader.byVRomSize;
  for ( nPage = 0; nPage < 8; ++nPage )
    PPUBANK[ nPage ] = &VROM[ byData * 0x2000 + nPage * 0x400 ];

  pNesX_SetupChr();
}

/*===================================================================*/
/*                                                                   */
/*                        Mapper 4 (MMC3)                            */
/*                                                                   */
/*===================================================================*/

BYTE Map4_VROM_Base;
BYTE Map4_ROM_Base;
BYTE Map4_Cmd;
BYTE Map4_Banks_Reg[ 8 ];
BYTE Map4_IRQ_Cnt;
BYTE Map4_IRQ_Set;
BYTE Map4_IRQ_Enable;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 4                                              */
/*-------------------------------------------------------------------*/
void Map4_Init()
{
  /* Initialize Mapper */
  MapperInit = Map4_Init;

  /* Write to Mapper */
  MapperWrite = Map4_Write;

  /* Callback at VSync */
  MapperVSync = Map4_VSync;

  /* Callback at HSync */
  MapperHSync = Map4_HSync;

  Map4_VROM_Base = Map4_ROM_Base = Map4_Cmd = Map4_IRQ_Cnt = Map4_IRQ_Set = Map4_IRQ_Enable = 0;

  Map4_Banks_Reg[ 0 ] = 0;
  Map4_Banks_Reg[ 1 ] = 2;
  Map4_Banks_Reg[ 2 ] = 4;
  Map4_Banks_Reg[ 3 ] = 5;
  Map4_Banks_Reg[ 4 ] = 6;
  Map4_Banks_Reg[ 5 ] = 7;
  Map4_Banks_Reg[ 6 ] = 0;
  Map4_Banks_Reg[ 7 ] = 1;

  Map4_Write( 0x8000, 0 );
  Map4_Write( 0x8001, 0 );

  /* Set up wiring of the interrupt pin */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 4 Write Function                                          */
/*-------------------------------------------------------------------*/
void Map4_Write( WORD wAddr, BYTE byData )
{
  WORD wMapAddr;

  wMapAddr = wAddr & 0xe001;

  switch ( wMapAddr )
  {
    case 0xa000:
      // Name Table Mirroring
      pNesX_Mirroring( ~byData & 1 );
      break;

    case 0xa001:
      break;

    case 0xc000:
      Map4_IRQ_Cnt = byData;
      break;

    case 0xc001:
      Map4_IRQ_Set = byData;
      break;

    case 0xe000:
      Map4_IRQ_Cnt = Map4_IRQ_Set;
      Map4_IRQ_Enable = 0;
      break;

    case 0xe001:
      if ( Map4_IRQ_Cnt > 0 )
        Map4_IRQ_Enable = 1;
      break;

    default:
      if ( wMapAddr == 0x8000 )
      {
        Map4_VROM_Base = byData >> 7;
        Map4_ROM_Base = ( byData >> 6 ) & 1;
        Map4_Cmd = byData & 7;
        return;
      }
      else
      {
        if ( Map4_Cmd >= 6 )
          byData %= ( NesHeader.byRomSize << 1 );
        else
        if ( NesHeader.byVRomSize > 0 )
        {
          if ( Map4_Cmd < 2 )
            byData = ( byData & 0xfe ) % ( NesHeader.byVRomSize << 3 );
          else
          if ( Map4_Cmd < 6 )
            byData %= ( NesHeader.byVRomSize << 3 );
        }

        Map4_Banks_Reg[ Map4_Cmd ] = byData;
      }

      /* Set VROM Banks */
      if ( NesHeader.byVRomSize > 0 )
      {
        if ( Map4_VROM_Base )
        {
          PPUBANK[ 0 ] = VROMPAGE( Map4_Banks_Reg[ 2 ] );
          PPUBANK[ 1 ] = VROMPAGE( Map4_Banks_Reg[ 3 ] );
          PPUBANK[ 2 ] = VROMPAGE( Map4_Banks_Reg[ 4 ] );
          PPUBANK[ 3 ] = VROMPAGE( Map4_Banks_Reg[ 5 ] );
          PPUBANK[ 4 ] = VROMPAGE( Map4_Banks_Reg[ 0 ] );
          PPUBANK[ 5 ] = VROMPAGE( Map4_Banks_Reg[ 0 ] + 1 );
          PPUBANK[ 6 ] = VROMPAGE( Map4_Banks_Reg[ 1 ] );
          PPUBANK[ 7 ] = VROMPAGE( Map4_Banks_Reg[ 1 ] + 1 );
        }
        else
        {
          PPUBANK[ 0 ] = VROMPAGE( Map4_Banks_Reg[ 0 ] );
          PPUBANK[ 1 ] = VROMPAGE( Map4_Banks_Reg[ 0 ] + 1 );
          PPUBANK[ 2 ] = VROMPAGE( Map4_Banks_Reg[ 1 ] );
          PPUBANK[ 3 ] = VROMPAGE( Map4_Banks_Reg[ 1 ] + 1 );
          PPUBANK[ 4 ] = VROMPAGE( Map4_Banks_Reg[ 2 ] );
          PPUBANK[ 5 ] = VROMPAGE( Map4_Banks_Reg[ 3 ] );
          PPUBANK[ 6 ] = VROMPAGE( Map4_Banks_Reg[ 4 ] );
          PPUBANK[ 7 ] = VROMPAGE( Map4_Banks_Reg[ 5 ] );
        }

        pNesX_SetupChr();
      }

      if ( Map4_ROM_Base )
      {
        ROMBANK0 = ROMLASTPAGE( 1 );
        ROMBANK1 = ROMPAGE( Map4_Banks_Reg[ 7 ] );
        ROMBANK2 = ROMPAGE( Map4_Banks_Reg[ 6 ] );
        ROMBANK3 = ROMLASTPAGE( 0 );
      }
      else
      {
        ROMBANK0 = ROMPAGE( Map4_Banks_Reg[ 6 ] );
        ROMBANK1 = ROMPAGE( Map4_Banks_Reg[ 7 ] );
        ROMBANK2 = ROMLASTPAGE( 1 );
        ROMBANK3 = ROMLASTPAGE( 0 );
      }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 4 V-Sync Function                                         */
/*-------------------------------------------------------------------*/
void Map4_VSync()
{
/*
 *  Callback at VSync
 *
 */
  Map4_IRQ_Cnt = Map4_IRQ_Set;
}

/*-------------------------------------------------------------------*/
/*  Mapper 4 H-Sync Function                                         */
/*-------------------------------------------------------------------*/
void Map4_HSync()
{
/*
 *  Callback at HSync
 *
 */

  if ( Map4_IRQ_Enable &&
       PPU_Scanline >= SCAN_ON_SCREEN_START && PPU_Scanline < SCAN_BOTTOM_OFF_SCREEN_START &&
       //PPU_ScanTable[ PPU_Scanline ] == SCAN_ON_SCREEN &&
       ( PPU_R1 & ( R1_SHOW_SCR | R1_SHOW_SP ) ) == ( R1_SHOW_SCR | R1_SHOW_SP ) &&
       Map4_IRQ_Cnt > 0 )
  {
    --Map4_IRQ_Cnt;
    if ( Map4_IRQ_Cnt == 0 )
    {
      Map4_IRQ_Cnt = Map4_IRQ_Set;
      IRQ_REQ;
    }
  }
}



