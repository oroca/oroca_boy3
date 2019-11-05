/*===================================================================*/
/*                                                                   */
/*  pNesX.h : NES Emulator for PSX                                   */
/*                                                                   */
/*  1999/11/03  Racoon  New preparation                              */
/*                                                                   */
/*===================================================================*/

#ifndef PNESX_H_INCLUDED
#define PNESX_H_INCLUDED

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include "pNesX_Types.h"
#include "hw.h"



#define PNESX_ROOT_DIR    "/pnesx"

/*-------------------------------------------------------------------*/
/*  NES resources                                                    */
/*-------------------------------------------------------------------*/

#define RAM_SIZE     0x2000
#define SRAM_SIZE    0x2000
#define PPURAM_SIZE  0x4000
#define SPRRAM_SIZE  256

/* RAM */
extern BYTE RAM[];

/* SRAM */
extern BYTE SRAM[];

/* ROM */
extern BYTE *ROM;

/* ROM BANK ( 8Kb * 4 ) */
extern BYTE *ROMBANK0;
extern BYTE *ROMBANK1;
extern BYTE *ROMBANK2;
extern BYTE *ROMBANK3;

/*-------------------------------------------------------------------*/
/*  PPU resources                                                    */
/*-------------------------------------------------------------------*/

/* PPU RAM */
extern BYTE PPURAM[];

/* VROM */
extern BYTE *VROM;

/* PPU BANK ( 1Kb * 16 ) */
extern BYTE *PPUBANK[];

#define NAME_TABLE0  8
#define NAME_TABLE1  9
#define NAME_TABLE2  10
#define NAME_TABLE3  11

#define NAME_TABLE_V_MASK 2
#define NAME_TABLE_H_MASK 1

/* Sprite RAM */
extern BYTE SPRRAM[];

#define SPR_Y    0
#define SPR_CHR  1
#define SPR_ATTR 2
#define SPR_X    3
#define SPR_ATTR_COLOR  0x3
#define SPR_ATTR_V_FLIP 0x80
#define SPR_ATTR_H_FLIP 0x40
#define SPR_ATTR_PRI    0x20

/* PPU Register */
extern BYTE PPU_R0;
extern BYTE PPU_R1;
extern BYTE PPU_R2;
extern BYTE PPU_R3;
extern BYTE PPU_R7;

extern BYTE PPU_Scr_V;
extern BYTE PPU_Scr_V_Next;
extern BYTE PPU_Scr_V_Byte;
extern BYTE PPU_Scr_V_Byte_Next;
extern BYTE PPU_Scr_V_Bit;
extern BYTE PPU_Scr_V_Bit_Next;

extern BYTE PPU_Scr_H;
extern BYTE PPU_Scr_H_Next;
extern BYTE PPU_Scr_H_Byte;
extern BYTE PPU_Scr_H_Byte_Next;
extern BYTE PPU_Scr_H_Bit;
extern BYTE PPU_Scr_H_Bit_Next;

extern BYTE PPU_Latch_Flag;
extern WORD PPU_Addr;
extern WORD PPU_Increment;

#define R0_NMI_VB      0x80
#define R0_NMI_SP      0x40
#define R0_SP_SIZE     0x20
#define R0_BG_ADDR     0x10
#define R0_SP_ADDR     0x08
#define R0_INC_ADDR    0x04
#define R0_NAME_ADDR   0x03

#define R1_BACKCOLOR   0xe0
#define R1_SHOW_SP     0x10
#define R1_SHOW_SCR    0x08
#define R1_CLIP_SP     0x04
#define R1_CLIP_BG     0x02
#define R1_MONOCHROME  0x01

#define R2_IN_VBLANK   0x80
#define R2_HIT_SP      0x40
#define R2_MAX_SP      0x20
#define R2_WRITE_FLAG  0x10

#define SCAN_TOP_OFF_SCREEN_START       0 
#define SCAN_ON_SCREEN_START            8
#define SCAN_BOTTOM_OFF_SCREEN_START  232
#define SCAN_POSTRENDER_START          240
#define SCAN_VBLANK_START             245
#define SCAN_VBLANK_END               261

#define SCAN_APU_CLK1   45      // 240Hz
#define SCAN_APU_CLK2   110     // 240Hz 120Hz
#define SCAN_APU_CLK3   176     // 240Hz
#define SCAN_APU_CLK4   241     // 240Hz 120Hz 60Hz

/* Current Scanline */
extern WORD PPU_Scanline;

/* Scanline Table */
extern BYTE PPU_ScanTable[];

/* Name Table Bank */
extern BYTE PPU_NameTableBank;

/* BG Base Address */
extern BYTE *PPU_BG_Base;

/* Sprite Base Address */
extern BYTE *PPU_SP_Base;

/* Sprite Height */
extern WORD PPU_SP_Height;

/* NES display size */
#define NES_DISP_WIDTH      256
#define NES_DISP_HEIGHT     224

/*-------------------------------------------------------------------*/
/*  Display and Others resouces                                      */
/*-------------------------------------------------------------------*/

/* Frame Skip */
extern WORD FrameSkip;
extern WORD FrameCnt;

//extern WORD DoubleFrame[ 2 ][ NES_DISP_WIDTH * NES_DISP_HEIGHT ];
//extern WORD DoubleFrame[ 2 ][ NES_DISP_WIDTH ];
extern WORD *WorkFrame;
extern WORD WorkFrameIdx;

extern BYTE ChrBuf[];

extern BYTE ChrBufUpdate;

extern WORD PalTable[];

/*-------------------------------------------------------------------*/
/*  APU and Pad resources                                            */
/*-------------------------------------------------------------------*/

extern BYTE APU_Reg[];

extern DWORD PAD1_Latch;
extern DWORD PAD2_Latch;
extern DWORD PAD_System;
extern DWORD PAD1_Bit;
extern DWORD PAD2_Bit;

#define PAD_SYS_QUIT   1
#define PAD_SYS_OK     2
#define PAD_SYS_CANCEL 4
#define PAD_SYS_UP     8
#define PAD_SYS_DOWN   0x10
#define PAD_SYS_LEFT   0x20
#define PAD_SYS_RIGHT  0x40

#define PAD_PUSH(a,b)  ( ( (a) & (b) ) != 0 )

/*-------------------------------------------------------------------*/
/*  Mapper Function                                                  */
/*-------------------------------------------------------------------*/

/* Initialize Mapper */
extern void (*MapperInit)();
/* Write to Mapper */
extern void (*MapperWrite)( WORD wAddr, BYTE byData );
/* Callback at VSync */
extern void (*MapperVSync)();
/* Callback at HSync */
extern void (*MapperHSync)();

/*-------------------------------------------------------------------*/
/*  ROM information                                                  */
/*-------------------------------------------------------------------*/

/* .nes File Header */
struct NesHeader_tag
{
  BYTE byID[ 4 ];
  BYTE byRomSize;
  BYTE byVRomSize;
  BYTE byInfo1;
  BYTE byInfo2;
  BYTE byReserve[ 8 ];
};

/* .nes File Header */
extern struct NesHeader_tag NesHeader;

/* Mapper No. */
extern BYTE MapperNo;

/* Other */
extern BYTE ROM_Mirroring;
extern BYTE ROM_SRAM;
extern BYTE ROM_Trainer;
extern BYTE ROM_FourScr;

/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/

/* Initialize pNesX */
void pNesX_Init();

/* Completion treatment */
void pNesX_Fin();

/* Load a cassette */
int pNesX_Load( const char *pszFileName );

/* Reset pNesX */
int pNesX_Reset();

/* Initialize PPU */
void pNesX_SetupPPU();

/* Set up a Mirroring of Name Table */
void pNesX_Mirroring( int nType );

/* The main loop of pNesX */ 
void pNesX_Main();

/* The loop of emulation */
void pNesX_Cycle();

/* A function in H-Sync */
int pNesX_HSync();

/* Render a scanline */
void pNesX_DrawLine();

/* Get a position of scanline hits sprite #0 */
void pNesX_GetSprHitY();

/* Develop character data */
void pNesX_SetupChr();

/* Apu */
void ApuInit();
void ApuWrite(WORD wAddr, BYTE byData);
BYTE ApuRead(WORD wAddr);
void pNesX_ApuClk_240Hz();
void pNesX_ApuClk_120Hz();
void pNesX_ApuClk_60Hz();

void wait(float data);

#endif /* !PNESX_H_INCLUDED */


