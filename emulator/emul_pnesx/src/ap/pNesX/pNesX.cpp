/*===================================================================*/
/*                                                                   */
/*  pNesX.cpp : NES Emulator for PSX                                 */
/*                                                                   */
/*  1999/11/03  Racoon  New preparation                              */
/*  2016/ 1/20  Racoon  Some optimization.                           */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------
 * File List :
 *
 * [NES Hardware]
 *   pNesX.cpp
 *   pNesX.h
 *   K6502_rw.h
 *   pNesX_Apu.cpp (Added)
 *
 * [Mapper function]
 *   pNesX_Mapper.cpp
 *   pNesX_Mapper.h
 *
 * [The function which depends on a system]
 *   pNesX_System_ooo.cpp (ooo is a system name. psx, win, Nucleo...)
 *   pNesX_System.h
 *
 * [CPU]
 *   K6502.cpp
 *   K6502.h
 *
 * [Others]
 *   pNesX_Types.h
 *
 --------------------------------------------------------------------*/

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include "ap.h"
#include "pNesX.h"
#include "pNesX_System.h"
#include "pNesX_Mapper.h"
#include "K6502.h"

/*-------------------------------------------------------------------*/
/*  NES resources                                                    */
/*-------------------------------------------------------------------*/

/* RAM */
BYTE RAM[ RAM_SIZE ];

/* SRAM */
BYTE SRAM[ SRAM_SIZE ];

/* ROM */
BYTE *ROM;

/* ROM BANK ( 8Kb * 4 ) */
BYTE *ROMBANK0;
BYTE *ROMBANK1;
BYTE *ROMBANK2;
BYTE *ROMBANK3;

/*-------------------------------------------------------------------*/
/*  PPU resources                                                    */
/*-------------------------------------------------------------------*/

/* PPU RAM */
BYTE PPURAM[ PPURAM_SIZE ];

/* VROM */
BYTE *VROM;

/* PPU BANK ( 1Kb * 16 ) */
BYTE *PPUBANK[ 16 ];

/* Sprite RAM */
BYTE SPRRAM[ SPRRAM_SIZE ];

/* PPU Register */
BYTE PPU_R0;
BYTE PPU_R1;
BYTE PPU_R2;
BYTE PPU_R3;
BYTE PPU_R7;

/* Flag for PPU Address and Scroll Latch */
BYTE PPU_Latch_Flag;

/* Vertical scroll value */
BYTE PPU_Scr_V;
BYTE PPU_Scr_V_Next;
BYTE PPU_Scr_V_Byte;
BYTE PPU_Scr_V_Byte_Next;
BYTE PPU_Scr_V_Bit;
BYTE PPU_Scr_V_Bit_Next;

/* Horizontal scroll value */
BYTE PPU_Scr_H;
BYTE PPU_Scr_H_Next;
BYTE PPU_Scr_H_Byte;
BYTE PPU_Scr_H_Byte_Next;
BYTE PPU_Scr_H_Bit;
BYTE PPU_Scr_H_Bit_Next;

/* PPU Address */
WORD PPU_Addr;

/* The increase value of the PPU Address */
WORD PPU_Increment;

/* Current Scanline */
WORD PPU_Scanline;

/* Scanline Table */
//BYTE PPU_ScanTable[ 263 ];

/* Name Table Bank */
BYTE PPU_NameTableBank;

/* BG Base Address */
BYTE *PPU_BG_Base;

/* Sprite Base Address */
BYTE *PPU_SP_Base;

/* Sprite Height */
WORD PPU_SP_Height;

/* Sprite #0 Scanline Hit Position */
int SpriteHitPos;

/*-------------------------------------------------------------------*/
/*  Display and Others resouces                                      */
/*-------------------------------------------------------------------*/

/* Frame Skip */
WORD FrameSkip;
WORD FrameCnt;
bool FrameDraw;

/* Display Line Buffer */
WORD LineData[2][ NES_DISP_WIDTH ];
WORD *pDrawData;
int LineDataIdx;

/* Character Buffer */
BYTE ChrBuf[ 256 * 2 * 8 * 8 ];

/* Update flag for ChrBuf */
BYTE ChrBufUpdate;

/* Palette Table */
WORD PalTable[ 32 ];

/* Table for Mirroring */
const BYTE PPU_MirrorTable[][ 4 ] =
{
  { NAME_TABLE0, NAME_TABLE0, NAME_TABLE1, NAME_TABLE1 },
  { NAME_TABLE0, NAME_TABLE1, NAME_TABLE0, NAME_TABLE1 },
  { NAME_TABLE1, NAME_TABLE1, NAME_TABLE1, NAME_TABLE1 },
  { NAME_TABLE0, NAME_TABLE0, NAME_TABLE0, NAME_TABLE0 }
};

/*-------------------------------------------------------------------*/
/*  APU and Pad resources                                            */
/*-------------------------------------------------------------------*/

/* APU Register */
BYTE APU_Reg[ 0x18 ];

/* Pad data */
DWORD PAD1_Latch;
DWORD PAD2_Latch;
DWORD PAD_System;
DWORD PAD1_Bit;
DWORD PAD2_Bit;

/*-------------------------------------------------------------------*/
/*  Mapper Function                                                  */
/*-------------------------------------------------------------------*/

/* Initialize Mapper */
void (*MapperInit)();
/* Write to Mapper */
void (*MapperWrite)( WORD wAddr, BYTE byData );
/* Callback at VSync */
void (*MapperVSync)();
/* Callback at HSync */
void (*MapperHSync)();

/*-------------------------------------------------------------------*/
/*  ROM information                                                  */
/*-------------------------------------------------------------------*/

/* .nes File Header */
struct NesHeader_tag NesHeader;

/* Mapper Number */
BYTE MapperNo;

/* Mirroring 0:Horizontal 1:Vertical */
BYTE ROM_Mirroring;
/* It has SRAM */
BYTE ROM_SRAM;
/* It has Trainer */
BYTE ROM_Trainer;
/* Four screen VRAM  */
BYTE ROM_FourScr;

/*===================================================================*/
/*                                                                   */
/*                pNesX_Init() : Initialize pNesX                    */
/*                                                                   */
/*===================================================================*/
void pNesX_Init()
{
/*
 *  Initialize pNesX
 *
 *  Remarks
 *    Initialize K6502 and Scanline Table.
 */

  // Initialize 6502
  K6502_Init();
}

/*===================================================================*/
/*                                                                   */
/*                pNesX_Fin() : Completion treatment                 */
/*                                                                   */
/*===================================================================*/
void pNesX_Fin()
{
/*
 *  Completion treatment
 *
 *  Remarks
 *    Release resources
 */

  // Release a memory for ROM
  pNesX_ReleaseRom();
}

/*===================================================================*/
/*                                                                   */
/*                  pNesX_Load() : Load a cassette                   */
/*                                                                   */
/*===================================================================*/
int pNesX_Load( const char *pszFileName )
{
/*
 *  Load a cassette
 *
 *  Parameters
 *    const char *pszFileName            (Read)
 *      File name of ROM image
 *
 *  Return values
 *     0 : It was finished normally.
 *    -1 : An error occurred.
 *
 *  Remarks
 *    Read a ROM image in the memory. 
 *    Reset pNesX.
 */

  // Release a memory for ROM
  pNesX_ReleaseRom();

  // Read a ROM image in the memory
  if ( pNesX_ReadRom( pszFileName ) < 0 )
    return -1;

  // Reset pNesX
  if ( pNesX_Reset() < 0 )
    return -1;

  // Successful
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*                 pNesX_Reset() : Reset pNesX                       */
/*                                                                   */
/*===================================================================*/
int pNesX_Reset()
{
/*
 *  Reset pNesX
 *
 *  Return values
 *     0 : Normally
 *    -1 : Non support mapper
 *
 *  Remarks
 *    Initialize Resources, PPU and Mapper.
 *    Reset CPU.
 */

  int nIdx;

  /*-------------------------------------------------------------------*/
  /*  Get information on the cassette                                  */
  /*-------------------------------------------------------------------*/

  // Get Mapper Number
  MapperNo = NesHeader.byInfo1 >> 4;

  // Check bit counts of Mapper No.
  for ( nIdx = 4; nIdx < 8 && NesHeader.byReserve[ nIdx ] == 0; ++nIdx )
    ;

  if ( nIdx == 8 )
  {
    // Mapper Number is 8bits
    MapperNo |= ( NesHeader.byInfo2 & 0xf0 );
  }

  // Get information on the ROM
  ROM_Mirroring = NesHeader.byInfo1 & 1;
  ROM_SRAM = NesHeader.byInfo1 & 2;
  ROM_Trainer = NesHeader.byInfo1 & 4;
  ROM_FourScr = NesHeader.byInfo1 & 8;

  /*-------------------------------------------------------------------*/
  /*  Initialize resources                                             */
  /*-------------------------------------------------------------------*/

  // Clear RAM
  pNesX_MemorySet( RAM, 0, sizeof RAM );

  // Reset frame skip and frame count
  FrameSkip = 0; // 0:full 1:half 2:2/3 3:1/3
  FrameCnt = 0;
  
 
  // Reset update flag of ChrBuf
  ChrBufUpdate = 0xff;

  // Reset palette table
  pNesX_MemorySet( PalTable, 0, sizeof PalTable );

  // Reset APU register
  pNesX_MemorySet( APU_Reg, 0, sizeof APU_Reg );

  // Reset joypad
  PAD1_Latch = PAD2_Latch = PAD_System = 0;
  PAD1_Bit = PAD2_Bit = 0;

  /*-------------------------------------------------------------------*/
  /*  Initialize PPU                                                   */
  /*-------------------------------------------------------------------*/

  pNesX_SetupPPU();

  /*-------------------------------------------------------------------*/
  /*  Initialize Mapper                                                */
  /*-------------------------------------------------------------------*/

  // Get Mapper Table Index
  for ( nIdx = 0; MapperTable[ nIdx ].nMapperNo != -1; ++nIdx )
  {
    if ( MapperTable[ nIdx ].nMapperNo == MapperNo )
      break;
  }

  if ( MapperTable[ nIdx ].nMapperNo == -1 )
  {
    // Non support mapper
    return -1;
  }

  // Set up a mapper initialization function
  MapperTable[ nIdx ].pMapperInit();

  /*-------------------------------------------------------------------*/
  /*  Reset CPU                                                        */
  /*-------------------------------------------------------------------*/

  K6502_Reset();
  
  // Successful
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*                pNesX_SetupPPU() : Initialize PPU                  */
/*                                                                   */
/*===================================================================*/
void pNesX_SetupPPU()
{
/*
 *  Initialize PPU
 *
 */
  int nPage;

  // Clear PPU and Sprite Memory
  pNesX_MemorySet( PPURAM, 0, sizeof PPURAM );
  pNesX_MemorySet( SPRRAM, 0, sizeof SPRRAM );

  // Reset PPU Register
  PPU_R0 = PPU_R1 = PPU_R2 = PPU_R3 = PPU_R7 = 0;

  // Reset latch flag
  PPU_Latch_Flag = 0;

  // Reset Scroll values
  PPU_Scr_V = PPU_Scr_V_Next = PPU_Scr_V_Byte = PPU_Scr_V_Byte_Next = PPU_Scr_V_Bit = PPU_Scr_V_Bit_Next = 0;
  PPU_Scr_H = PPU_Scr_H_Next = PPU_Scr_H_Byte = PPU_Scr_H_Byte_Next = PPU_Scr_H_Bit = PPU_Scr_H_Bit_Next = 0;

  // Reset PPU address
  PPU_Addr = 0;

  // Reset scanline
  PPU_Scanline = 0;

  // Reset hit position of sprite #0 
  SpriteHitPos = 0;

  // Reset information on PPU_R0
  PPU_Increment = 1;
  PPU_NameTableBank = NAME_TABLE0;
  PPU_BG_Base = ChrBuf;
  PPU_SP_Base = ChrBuf + 256 * 64;
  PPU_SP_Height = 8;

  // Reset PPU banks
  for ( nPage = 0; nPage < 16; ++nPage )
    PPUBANK[ nPage ] = &PPURAM[ nPage * 0x400 ];

  /* Mirroring of Name Table */
  pNesX_Mirroring( ROM_Mirroring );
}

/*===================================================================*/
/*                                                                   */
/*       pNesX_Mirroring() : Set up a Mirroring of Name Table        */
/*                                                                   */
/*===================================================================*/
void pNesX_Mirroring( int nType )
{
/*
 *  Set up a Mirroring of Name Table
 *
 *  Parameters
 *    int nType          (Read)
 *      Mirroring Type
 *        0 : Horizontal
 *        1 : Vertical
 *        2 : One Screen 0x2000
 *        3 : One Screen 0x2400
 */

  PPUBANK[ NAME_TABLE0 ] = &PPURAM[ PPU_MirrorTable[ nType ][ 0 ] * 0x400 ];
  PPUBANK[ NAME_TABLE1 ] = &PPURAM[ PPU_MirrorTable[ nType ][ 1 ] * 0x400 ];
  PPUBANK[ NAME_TABLE2 ] = &PPURAM[ PPU_MirrorTable[ nType ][ 2 ] * 0x400 ];
  PPUBANK[ NAME_TABLE3 ] = &PPURAM[ PPU_MirrorTable[ nType ][ 3 ] * 0x400 ];
}

/*===================================================================*/
/*                                                                   */
/*              pNesX_Main() : The main loop of pNesX                */
/*                                                                   */
/*===================================================================*/
void pNesX_Main()
{
/*
 *  The main loop of pNesX
 *
 */

  // Initialize pNesX
  pNesX_Init();

  // Main loop
  while ( 1 )
  {
    /*-------------------------------------------------------------------*/
    /*  To the menu screen                                               */
    /*-------------------------------------------------------------------*/
    if ( pNesX_Menu() == -1 )
    {
      break;  // Quit
    }
    
    /*-------------------------------------------------------------------*/
    /*  Start a NES emulation                                            */
    /*-------------------------------------------------------------------*/
    pNesX_Cycle();
  }

  // Completion treatment
  pNesX_Fin();
}

/*===================================================================*/
/*                                                                   */
/*              pNesX_Cycle() : The loop of emulation                */
/*                                                                   */
/*===================================================================*/
void pNesX_Cycle()
{
/*
 *  The loop of emulation
 *
 */
  lcdSetDoubleBuffer(true);
  lcdClear(black);
  lcdUpdateDraw();


  // Emulation loop
  for (;;)
  {
    // Execute instructions
    K6502_Step( 40 );

    // Set a flag if a scanning line is a hit in the sprite #0
    if ( ( SPRRAM[ SPR_Y ] + SpriteHitPos ) == PPU_Scanline &&
         PPU_Scanline >= SCAN_ON_SCREEN_START && PPU_Scanline < SCAN_BOTTOM_OFF_SCREEN_START )
         //PPU_ScanTable[ PPU_Scanline ] == SCAN_ON_SCREEN )
    {
      // Set a sprite hit flag
      PPU_R2 |= R2_HIT_SP;

      // NMI is required if there is necessity
      if ( ( PPU_R0 & R0_NMI_SP ) && ( PPU_R1 & R1_SHOW_SP ) )
        NMI_REQ;
    }
    
    // Execute instructions
    K6502_Step( 75 );

    // A mapper function in H-Sync
    MapperHSync();

    // A function in H-Sync
    if ( pNesX_HSync() == -1 )
      return;  // To the menu screen
  }
}

/*===================================================================*/
/*                                                                   */
/*              pNesX_HSync() : A function in H-Sync                 */
/*                                                                   */
/*===================================================================*/
int pNesX_HSync()
{
/*
 *  A function in H-Sync
 *
 *  Return values
 *    0 : Normally
 *   -1 : Exit an emulation
 */

  /*-------------------------------------------------------------------*/
  /*  Render a scanline                                                */
  /*-------------------------------------------------------------------*/
  if ( FrameDraw &&
       PPU_Scanline >= SCAN_ON_SCREEN_START && PPU_Scanline < SCAN_BOTTOM_OFF_SCREEN_START )
  {
    pNesX_DrawLine();
    pNesX_TransmitLinedata();
  }

  /*-------------------------------------------------------------------*/
  /*  Set new scroll values                                            */
  /*-------------------------------------------------------------------*/
  PPU_Scr_V      = PPU_Scr_V_Next;
  PPU_Scr_V_Byte = PPU_Scr_V_Byte_Next;
  PPU_Scr_V_Bit  = PPU_Scr_V_Bit_Next;

  PPU_Scr_H      = PPU_Scr_H_Next;
  PPU_Scr_H_Byte = PPU_Scr_H_Byte_Next;
  PPU_Scr_H_Bit  = PPU_Scr_H_Bit_Next;

  /*-------------------------------------------------------------------*/
  /*  Next Scanline                                                    */
  /*-------------------------------------------------------------------*/
  PPU_Scanline = ( PPU_Scanline == SCAN_VBLANK_END ) ? 0 : PPU_Scanline + 1;

  /*-------------------------------------------------------------------*/
  /*  Operation in the specific scanning line                          */
  /*-------------------------------------------------------------------*/
  switch ( PPU_Scanline )
  {
    case SCAN_ON_SCREEN_START:
      // Reset a PPU status
      PPU_R2 = 0;

      // Set up a character data
      if ( NesHeader.byVRomSize == 0 && FrameDraw )
        pNesX_SetupChr();

      // Get position of sprite #0
      pNesX_GetSprHitY();
      break;

    case SCAN_VBLANK_START:
      if (FrameDraw)
      {
        pNesX_LoadFrame();
      }
      // Frame skip
      FrameCnt = (++FrameCnt) % 6;
      switch (FrameSkip)
      {
        case 0: // full
          FrameDraw = true;
          break;
          
        case 1: // 1/2
          FrameDraw = FrameCnt & 1;
          break;
          
        case 2: // 2/3
          FrameDraw = !(FrameCnt == 2 || FrameCnt == 5);
          break;
          
        case 3: // 1/3
          FrameDraw = FrameCnt == 0 || FrameCnt == 3;  
      } 
        
      //pNesX_LoadFrame();
      if (lcdDrawAvailable() == false)
      {
        FrameDraw = false;
      }
      
      // Set a V-Blank flag
      PPU_R2 = R2_IN_VBLANK;

      // Reset latch flag
      PPU_Latch_Flag = 0;

      // A mapper function in V-Sync
      MapperVSync();

      // Get the condition of the joypad
      pNesX_PadState( &PAD1_Latch, &PAD2_Latch, &PAD_System );
      
      // NMI on V-Blank
      if ( PPU_R0 & R0_NMI_VB )
        NMI_REQ;

      // Exit an emulation if a QUIT button is pushed
      static DWORD quitWait;
      if (PAD_PUSH( PAD_System, PAD_SYS_QUIT ))
      {
        quitWait++;
      }
      else
      {
        if (quitWait > 0)
        {
          quitWait = 0;
          return -1;  // Exit an emulation
        }
        quitWait = 0;
      }
      break;
      
    case SCAN_APU_CLK1: // 240Hz
    case SCAN_APU_CLK3: // 240Hz    
      pNesX_ApuClk_240Hz();
      break;

    case SCAN_APU_CLK2: // 240Hz 120Hz
      pNesX_ApuClk_240Hz();
      pNesX_ApuClk_120Hz();
      break;

    case SCAN_APU_CLK4: // 240Hz 120Hz 60Hz
      pNesX_ApuClk_240Hz();
      pNesX_ApuClk_120Hz();
      pNesX_ApuClk_60Hz();   
      break;
        
  }

  // Successful
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*              pNesX_DrawLine() : Render a scanline                 */
/*                                                                   */
/*===================================================================*/


void pNesX_DrawLine()
{
/*
 *  Render a scanline
 *
 */
  int nX;
  int nY;
  int nY4;
  int nYBit;
  WORD *pPalTbl;
  BYTE *pAttrBase;
  WORD *pPoint;
  int nNameTable;
  BYTE *pbyNameTable;
  BYTE *pbyChrData;
  BYTE *pSPRRAM;
  int nAttr;
  int nSprCnt;
  int nIdx;
  BYTE bySprCol;
  bool bMask[ NES_DISP_WIDTH ];
  bool *pMask;
  
  // Pointer to the render position
  pPoint = LineData[LineDataIdx];
  LineDataIdx = LineDataIdx == 0 ? 1 : 0;
  pDrawData = pPoint;
  
  // Clear a scanline if screen is off
  if ( !( PPU_R1 & R1_SHOW_SCR ) )
  {
    pNesX_MemorySet( pPoint, 0, NES_DISP_WIDTH << 1 );
    return;
  }

  nNameTable = PPU_NameTableBank;

  nY = PPU_Scr_V_Byte + ( PPU_Scanline >> 3 );

  nYBit = PPU_Scr_V_Bit + ( PPU_Scanline & 7 );

  if ( nYBit > 7 )
  {
    ++nY;
    nYBit &= 7;
  }
  nYBit <<= 3;

  if ( nY > 29 )
  {
    // Next NameTable (An up-down direction)
    nNameTable ^= NAME_TABLE_V_MASK;
    nY -= 30;
  }

  nX = PPU_Scr_H_Byte;

  nY4 = ( ( nY & 2 ) << 1 );

  pMask = bMask;
  
  /*-------------------------------------------------------------------*/
  /*  Rendering of the block of the left end                           */
  /*-------------------------------------------------------------------*/

  pbyNameTable = PPUBANK[ nNameTable ] + nY * 32 + nX;
  pbyChrData = PPU_BG_Base + ( *pbyNameTable << 6 ) + nYBit;
  pAttrBase = PPUBANK[ nNameTable ] + 0x3c0 + ( nY / 4 ) * 8;
  pPalTbl =  &PalTable[ ( ( ( pAttrBase[ nX >> 2 ] >> ( ( nX & 2 ) + nY4 ) ) & 3 ) << 2 ) ];

  for ( nIdx = PPU_Scr_H_Bit; nIdx < 8; ++nIdx )
  {
    *( pPoint++ ) = pPalTbl[ pbyChrData[ nIdx ] ];
    *pMask++ = pbyChrData[ nIdx ] == 0;
  }

  ++nX;
  ++pbyNameTable;

  /*-------------------------------------------------------------------*/
  /*  Rendering of the left table                                      */
  /*-------------------------------------------------------------------*/

  for ( ; nX < 32; ++nX )
  {
    pbyChrData = PPU_BG_Base + ( *pbyNameTable << 6 ) + nYBit;
    pPalTbl = &PalTable[ ( ( ( pAttrBase[ nX >> 2 ] >> ( ( nX & 2 ) + nY4 ) ) & 3 ) << 2 ) ];

    *pPoint++ = pPalTbl[ pbyChrData[ 0 ] ]; 
    *pPoint++ = pPalTbl[ pbyChrData[ 1 ] ];
    *pPoint++ = pPalTbl[ pbyChrData[ 2 ] ];
    *pPoint++ = pPalTbl[ pbyChrData[ 3 ] ];
    *pPoint++ = pPalTbl[ pbyChrData[ 4 ] ];
    *pPoint++ = pPalTbl[ pbyChrData[ 5 ] ];
    *pPoint++ = pPalTbl[ pbyChrData[ 6 ] ];
    *pPoint++ = pPalTbl[ pbyChrData[ 7 ] ];

    *pMask++ = pbyChrData[ 0 ] == 0;
    *pMask++ = pbyChrData[ 1 ] == 0;
    *pMask++ = pbyChrData[ 2 ] == 0;
    *pMask++ = pbyChrData[ 3 ] == 0;
    *pMask++ = pbyChrData[ 4 ] == 0;
    *pMask++ = pbyChrData[ 5 ] == 0;
    *pMask++ = pbyChrData[ 6 ] == 0;
    *pMask++ = pbyChrData[ 7 ] == 0;

    ++pbyNameTable;
  }

  // Holizontal Mirror
  nNameTable ^= NAME_TABLE_H_MASK;

  pbyNameTable = PPUBANK[ nNameTable ] + nY * 32;
  pAttrBase = PPUBANK[ nNameTable ] + 0x3c0 + ( nY / 4 ) * 8;

  /*-------------------------------------------------------------------*/
  /*  Rendering of the right table                                     */
  /*-------------------------------------------------------------------*/

  for ( nX = 0; nX < PPU_Scr_H_Byte; ++nX )
  {
    pbyChrData = PPU_BG_Base + ( *pbyNameTable << 6 ) + nYBit;
    pPalTbl = &PalTable[ ( ( ( pAttrBase[ nX >> 2 ] >> ( ( nX & 2 ) + nY4 ) ) & 3 ) << 2 ) ];

    *pPoint++ = pPalTbl[ pbyChrData[ 0 ] ]; 
    *pPoint++ = pPalTbl[ pbyChrData[ 1 ] ];
    *pPoint++ = pPalTbl[ pbyChrData[ 2 ] ];
    *pPoint++ = pPalTbl[ pbyChrData[ 3 ] ];
    *pPoint++ = pPalTbl[ pbyChrData[ 4 ] ];
    *pPoint++ = pPalTbl[ pbyChrData[ 5 ] ];
    *pPoint++ = pPalTbl[ pbyChrData[ 6 ] ];
    *pPoint++ = pPalTbl[ pbyChrData[ 7 ] ];

    *pMask++ = pbyChrData[ 0 ] == 0;
    *pMask++ = pbyChrData[ 1 ] == 0;
    *pMask++ = pbyChrData[ 2 ] == 0;
    *pMask++ = pbyChrData[ 3 ] == 0;
    *pMask++ = pbyChrData[ 4 ] == 0;
    *pMask++ = pbyChrData[ 5 ] == 0;
    *pMask++ = pbyChrData[ 6 ] == 0;
    *pMask++ = pbyChrData[ 7 ] == 0;

    ++pbyNameTable;
  }

  /*-------------------------------------------------------------------*/
  /*  Rendering of the block of the right end                          */
  /*-------------------------------------------------------------------*/

  pbyChrData = PPU_BG_Base + ( *pbyNameTable << 6 ) + nYBit;
  pPalTbl = &PalTable[ ( ( ( pAttrBase[ nX >> 2 ] >> ( ( nX & 2 ) + nY4 ) ) & 3 ) << 2 ) ];
  for ( nIdx = 0; nIdx < PPU_Scr_H_Bit; ++nIdx )
  {
    pPoint[ nIdx ] = pPalTbl[ pbyChrData[ nIdx ] ];
    *pMask++ = pbyChrData[ nIdx ] == 0;
  }

  /*-------------------------------------------------------------------*/
  /*  Render a sprite                                                  */
  /*-------------------------------------------------------------------*/

  if ( PPU_R1 & R1_SHOW_SP )
  {
    // Reset Scanline Sprite Count
    PPU_R2 &= ~R2_MAX_SP;

    // Render a sprite to the sprite buffer
    pPoint = pDrawData;
    nSprCnt = 0;
    for ( pSPRRAM = SPRRAM + ( 63 << 2 ); pSPRRAM >= SPRRAM &&
                                          nSprCnt < 8; pSPRRAM -= 4 )
    {
      nY = pSPRRAM[ SPR_Y ] + 1;
      if ( nY > PPU_Scanline || nY + PPU_SP_Height <= PPU_Scanline )
        continue;  // Next sprite

     /*-------------------------------------------------------------------*/
     /*  A sprite in scanning line                                        */
     /*-------------------------------------------------------------------*/

      // Holizontal Sprite Count +1
      ++nSprCnt;
      
      nAttr = pSPRRAM[ SPR_ATTR ];
      nYBit = PPU_Scanline - nY;
      nYBit = ( nAttr & SPR_ATTR_V_FLIP ) ? ( PPU_SP_Height - nYBit - 1 ) << 3 : nYBit << 3;

      if ( PPU_R0 & R0_SP_SIZE )
      {
        // Sprite size 8x16
        if ( pSPRRAM[ SPR_CHR ] & 1 )
        {
          pbyChrData = ChrBuf + 256 * 64 + ( ( pSPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + nYBit;
        }
        else
        {
          pbyChrData = ChrBuf + ( ( pSPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + nYBit;
        }
      }
      else
      {
        // Sprite size 8x8
        pbyChrData = PPU_SP_Base + ( pSPRRAM[ SPR_CHR ] << 6 ) + nYBit;
      }

      bool bSprFront = !(nAttr & SPR_ATTR_PRI);
      bySprCol = (( nAttr & SPR_ATTR_COLOR ) << 2) + 0x10;
      nX = pSPRRAM[ SPR_X ];

      if ( nAttr & SPR_ATTR_H_FLIP )
      {
        // Horizontal flip
        if ( pbyChrData[ 0 ] && nX < 249 && (bSprFront || bMask[nX + 7]) )
          pPoint[ nX + 7 ] = PalTable[ bySprCol | pbyChrData[ 0 ] ];
        if ( pbyChrData[ 1 ] && nX < 250 && (bSprFront || bMask[nX + 6]) )
          pPoint[ nX + 6 ] = PalTable[ bySprCol | pbyChrData[ 1 ] ];
        if ( pbyChrData[ 2 ] && nX < 251 && (bSprFront || bMask[nX + 5]) )
          pPoint[ nX + 5 ] = PalTable[ bySprCol | pbyChrData[ 2 ] ];
        if ( pbyChrData[ 3 ] && nX < 252 && (bSprFront || bMask[nX + 4]) )
          pPoint[ nX + 4 ] = PalTable[ bySprCol | pbyChrData[ 3 ] ];
        if ( pbyChrData[ 4 ] && nX < 253 && (bSprFront || bMask[nX + 3]) )
          pPoint[ nX + 3 ] = PalTable[ bySprCol | pbyChrData[ 4 ] ];
        if ( pbyChrData[ 5 ] && nX < 254 && (bSprFront || bMask[nX + 2]) )
          pPoint[ nX + 2 ] = PalTable[ bySprCol | pbyChrData[ 5 ] ];
        if ( pbyChrData[ 6 ] && nX < 255 && (bSprFront || bMask[nX + 1]) )
          pPoint[ nX + 1 ] = PalTable[ bySprCol | pbyChrData[ 6 ] ];
        if ( pbyChrData[ 7 ] && (bSprFront || bMask[nX]) )
          pPoint[ nX ] = PalTable[ bySprCol | pbyChrData[ 7 ] ];
      }
      else
      {
        // Non flip
        if ( pbyChrData[ 0 ] && (bSprFront || bMask[nX]) )
          pPoint[ nX ]     = PalTable[ bySprCol | pbyChrData[ 0 ] ];
        if ( pbyChrData[ 1 ] && nX < 255 && (bSprFront || bMask[nX + 1]) )
          pPoint[ nX + 1 ] = PalTable[ bySprCol | pbyChrData[ 1 ] ];
        if ( pbyChrData[ 2 ] && nX < 254 && (bSprFront || bMask[nX + 2]) )
          pPoint[ nX + 2 ] = PalTable[ bySprCol | pbyChrData[ 2 ] ];
        if ( pbyChrData[ 3 ] && nX < 253 && (bSprFront || bMask[nX + 3]) )
          pPoint[ nX + 3 ] = PalTable[ bySprCol | pbyChrData[ 3 ] ];
        if ( pbyChrData[ 4 ] && nX < 252 && (bSprFront || bMask[nX + 4]) )
          pPoint[ nX + 4 ] = PalTable[ bySprCol | pbyChrData[ 4 ] ];
        if ( pbyChrData[ 5 ] && nX < 251 && (bSprFront || bMask[nX + 5]) )
          pPoint[ nX + 5 ] = PalTable[ bySprCol | pbyChrData[ 5 ] ];
        if ( pbyChrData[ 6 ] && nX < 250 && (bSprFront || bMask[nX + 6]) )
          pPoint[ nX + 6 ] = PalTable[ bySprCol | pbyChrData[ 6 ] ];
        if ( pbyChrData[ 7 ] && nX < 249 && (bSprFront || bMask[nX + 7]) )
          pPoint[ nX + 7 ] = PalTable[ bySprCol | pbyChrData[ 7 ] ];
      }
    }

    if ( nSprCnt == 8 )
      PPU_R2 |= R2_MAX_SP;  // Set a flag of maximum sprites on scanline
  }
}

/*===================================================================*/
/*                                                                   */
/*   pNesX_GetSprHitY() : Get a position of scanline hits sprite #0  */
/*                                                                   */
/*===================================================================*/
void pNesX_GetSprHitY()
{
/*
 * Get a position of scanline hits sprite #0
 *
 */

  int nYBit;
  DWORD *pdwChrData;
  int nOff;

  if ( SPRRAM[ SPR_ATTR ] & SPR_ATTR_V_FLIP )
  {
    // Vertical flip
    nYBit = ( PPU_SP_Height - 1 ) << 3;
    nOff = -2;
  }
  else
  {
    // Non flip
    nYBit = 0;
    nOff = 2;
  }

  if ( PPU_R0 & R0_SP_SIZE )
  {
    // Sprite size 8x16
    if ( SPRRAM[ SPR_CHR ] & 1 )
    {
      pdwChrData = (DWORD *)( ChrBuf + 256 * 64 + ( ( SPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + nYBit );
    }
    else
    {
      pdwChrData = (DWORD * )( ChrBuf + ( ( SPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + nYBit );
    } 
  }
  else
  {
    // Sprite size 8x8
    pdwChrData = (DWORD *)( PPU_SP_Base + ( SPRRAM[ SPR_CHR ] << 6 ) + nYBit );
  }

  for ( SpriteHitPos = 1; SpriteHitPos < PPU_SP_Height + 1; ++SpriteHitPos )
  {
    if ( pdwChrData[ 0 ] | pdwChrData[ 1 ] )
      return;  // Scanline hits sprite #0

    pdwChrData += nOff;
  }

  // Scanline didn't hit sprite #0
  SpriteHitPos = SCAN_VBLANK_END;
}

/*===================================================================*/
/*                                                                   */
/*            pNesX_SetupChr() : Develop character data              */
/*                                                                   */
/*===================================================================*/
void pNesX_SetupChr()
{
/*
 *  Develop character data
 *
 */

  BYTE *pbyBGData;
  BYTE byData1;
  BYTE byData2;
  int nIdx;
  int nY;
  int nOff;
  static BYTE *pbyPrevBank[ 8 ];
  int nBank;

  for ( nBank = 0; nBank < 8; ++nBank )
  {
    if ( pbyPrevBank[ nBank ] == PPUBANK[ nBank ] && !( ( ChrBufUpdate >> nBank ) & 1 ) )
      continue;  // Next bank

    /*-------------------------------------------------------------------*/
    /*  An address is different from the last time                       */
    /*    or                                                             */
    /*  An update flag is being set                                      */
    /*-------------------------------------------------------------------*/

    for ( nIdx = 0; nIdx < 64; ++nIdx )
    {
      nOff = ( nBank << 12 ) + ( nIdx << 6 );

      for ( nY = 0; nY < 8; ++nY )
      {
        pbyBGData = PPUBANK[ nBank ] + ( nIdx << 4 ) + nY;

        byData1 = ( ( pbyBGData[ 0 ] >> 1 ) & 0x55 ) | ( pbyBGData[ 8 ] & 0xAA );
        byData2 = ( pbyBGData[ 0 ] & 0x55 ) | ( ( pbyBGData[ 8 ] << 1 ) & 0xAA );

        ChrBuf[ nOff++ ] = ( byData1 >> 6 ) & 3;
        ChrBuf[ nOff++ ] = ( byData2 >> 6 ) & 3;
        ChrBuf[ nOff++ ] = ( byData1 >> 4 ) & 3;
        ChrBuf[ nOff++ ] = ( byData2 >> 4 ) & 3;
        ChrBuf[ nOff++ ] = ( byData1 >> 2 ) & 3;
        ChrBuf[ nOff++ ] = ( byData2 >> 2 ) & 3;
        ChrBuf[ nOff++ ] = byData1 & 3;
        ChrBuf[ nOff++ ] = byData2 & 3;
      }
    }
    // Keep this address
    pbyPrevBank[ nBank ] = PPUBANK[ nBank ];
  }

  // Reset update flag
  ChrBufUpdate = 0;
}

