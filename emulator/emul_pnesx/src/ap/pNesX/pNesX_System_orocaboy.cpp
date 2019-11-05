/*===================================================================*/
/*                                                                   */
/*  pNesX_System_Nucleo.cpp : The function which depends on a system */
/*                         (for Nucleo F446RE)                       */
/*                                                                   */
/*  2016/1/20  Racoon                                                */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Include files                                                    */
/*-------------------------------------------------------------------*/

#include "ap.h"

//#define PS_GAMEPAD


#include "pNesX.h"
#include "pNesX_System.h"

extern int pNesX_Filer();
extern void ApuMute(bool mute);





extern WORD LineData[][256];
extern WORD *pDrawData;

#define SHOW_FPS

#if defined(SHOW_FPS)
uint32_t fps_pre_time;
int fps = 0;
int dmawait;
#endif

/*-------------------------------------------------------------------*/
/*  ROM image file information                                       */
/*-------------------------------------------------------------------*/

extern char szRomFolder[];
extern char szRomFilename[];
extern char szRomPath[];
    
char szRomName[ 256 ];
char szSaveName[ 256 ];
int nSRAM_SaveFlag;

// Palette data
/*
const WORD NesPalette[ 64 ] =
{
    0xef7b,0x1f00,0x1700,0x5741,0x1090,0x04a8,0x80a8,0xa088,0x8051,0xc003,0x4003,0xc002,0x0b02,0x0000,0x0000,0x0000,
    0xf7bd,0xdf03,0xdf02,0x3f6a,0x19d8,0x0be0,0xc0f9,0xe2e2,0xe0ab,0xc005,0x4005,0x4805,0x5104,0x0000,0x0000,0x0000,
    0xdfff,0xff3d,0x5f6c,0xdf9b,0xdffb,0xd3fa,0xcbfb,0x08fd,0xc0fd,0xc3bf,0xca5e,0xd35f,0x5b07,0xcf7b,0x0000,0x0000,
    0xffff,0x3fa7,0xdfbd,0xdfdd,0xdffd,0x38fd,0x96f6,0x15ff,0xcffe,0xcfdf,0xd7bf,0xdbbf,0xff07,0xdffe,0x0000,0x0000
};
*/
const WORD NesPalette[ 64 ] =
{
    0x7bef,0x001f,0x0017,0x4157,0x9010,0xa804,0xa880,0x88a0,0x5180,0x03c0,0x0340,0x02c0,0x020b,0x0000,0x0000,0x0000,
    0xbdf7,0x03df,0x02df,0x6a3f,0xd819,0xe00b,0xf9c0,0xe2e2,0xabe0,0x05c0,0x0540,0x0548,0x0451,0x0000,0x0000,0x0000,
    0xffdf,0x3dff,0x6c5f,0x9bdf,0xfbdf,0xfad3,0xfbcb,0xfd08,0xfdc0,0xbfc3,0x5eca,0x5fd3,0x075b,0x7bcf,0x0000,0x0000,
    0xffff,0xa73f,0xbddf,0xdddf,0xfddf,0xfd38,0xf696,0xff15,0xfecf,0xdfcf,0xbfd7,0xbfdb,0x07ff,0xfedf,0x0000,0x0000
};

/*-------------------------------------------------------------------*/
/*  Function prototypes                                              */
/*-------------------------------------------------------------------*/

int LoadSRAM();
int SaveSRAM();

/*===================================================================*/
/*                                                                   */
/*           LoadSRAM() : Load a SRAM                                */
/*                                                                   */
/*===================================================================*/
int LoadSRAM()
{
/*
 *  Load a SRAM
 *
 *  Return values
 *     0 : Normally
 *    -1 : SRAM data couldn't be read
 */

  return 0;
}

/*===================================================================*/
/*                                                                   */
/*           SaveSRAM() : Save a SRAM                                */
/*                                                                   */
/*===================================================================*/
int SaveSRAM()
{
/*
 *  Save a SRAM
 *
 *  Return values
 *     0 : Normally
 *    -1 : SRAM data couldn't be written
 */


  // Successful
  return 0;
}


/*===================================================================*/
/*                                                                   */
/*                  pNesX_Menu() : Menu screen                       */
/*                                                                   */
/*===================================================================*/
int pNesX_Menu()
{
/*
 *  Menu screen
 *
 *  Return values
 *     0 : Normally
 *    -1 : Exit pNesX
 */

#if defined(SHOW_FPS)

#endif

    ApuMute(true);

    switch (pNesX_Filer())
    {
        case 0:  // Selected a file
            if ( pNesX_Load( szRomPath ) == 0 )
            {
                // Set a ROM image name
                strcpy( szRomName, szRomFilename );
        
                // Load SRAM
                LoadSRAM();
            }
            break;
            
        case 1:  // Return to Emu
            break;
            
        case 2:  // Reset
            if ( szRomName[ 0 ] != 0 )
            {
              // Reset pNesX
              pNesX_Reset();
            }                
        
            break;
            
        case -1:  // Error
            printf("Filer Error\r\n");
            while(1);
    }

    //tft_clear(TFT_BLACK);
    //tft_set_window(32, 8, NES_DISP_WIDTH + 32 - 1, NES_DISP_HEIGHT + 8 - 1);
    
    ApuMute(false);

#if defined(SHOW_FPS)
    fps_pre_time = millis();
#endif

  return 0;
}

/*===================================================================*/
/*                                                                   */
/*               pNesX_ReadRom() : Read ROM image file               */
/*                                                                   */
/*===================================================================*/
int pNesX_ReadRom( const char *pszFileName )
{
#if 0
/*
 *  Read ROM image file
 *
 *  Parameters
 *    const char *pszFileName          (Read)
 *
 *  Return values
 *     0 : Normally
 *    -1 : Error
 */
    FileHandle* file;
    
    /* Open ROM file */
    file = sd->open(pszFileName, O_RDONLY);
    if ( file == NULL )
    {
        printf("open error\r\n");
        return -1;
    }
    
    /* Read ROM Header */
    file->read( &NesHeader, sizeof NesHeader);
    if ( memcmp( NesHeader.byID, "NES\x1a", 4 ) != 0 )
    {
        /* not .nes file */
        file->close();
        return -1;
    }

    /* Clear SRAM */
    memset( SRAM, 0, SRAM_SIZE );

    /* If trainer presents Read Triner at 0x7000-0x71ff */
    if ( NesHeader.byInfo1 & 4 )
    {
        printf("Read Trainer\r\n");
        file->read( &SRAM[ 0x1000 ], 512 );
    }

    printf("RomSize: %d\r\n", NesHeader.byRomSize);
    /* Allocate Memory for ROM Image */
    ROM = (BYTE *)malloc( NesHeader.byRomSize * 0x4000 );
    printf("ROM addr:%x\r\n", ROM);

    /* Read ROM Image */
    file->read( ROM, 0x4000 * NesHeader.byRomSize );

    if ( NesHeader.byVRomSize > 0 )
    {
        /* Allocate Memory for VROM Image */
        VROM = (BYTE *)malloc( NesHeader.byVRomSize * 0x2000 );
        printf("VROM addr:%x\r\n", VROM);

        /* Read VROM Image */
        file->read( VROM, 0x2000 * NesHeader.byVRomSize );
    }

    /* File close */
    file->close();
#else
    FRESULT fr;          /* FatFs function common result code */
    FIL file;            /* File objects */
    UINT br, bw;         /* File read/write count */


    sprintf(szRomName, "%s/%s", szRomFolder, pszFileName);

    /* Open ROM file */
    fr = f_open(&file, szRomName, FA_READ);
    if (fr)
    {
      return -1;
    }

    /* Read ROM Header */
    fr = f_read(&file, &NesHeader, sizeof NesHeader, &br);
    if ( memcmp( NesHeader.byID, "NES\x1a", 4 ) != 0 )
    {
        /* not .nes file */
        f_close(&file);
        return -1;
    }

    /* Clear SRAM */
    memset( SRAM, 0, SRAM_SIZE );

    /* If trainer presents Read Triner at 0x7000-0x71ff */
    if ( NesHeader.byInfo1 & 4 )
    {
        printf("Read Trainer\r\n");
        f_read(&file, &SRAM[ 0x1000 ], 512, &br);
    }

    printf("RomSize: %d\r\n", NesHeader.byRomSize);
    /* Allocate Memory for ROM Image */
    ROM = (BYTE *)memMalloc( NesHeader.byRomSize * 0x4000 );
    printf("ROM addr:%x\r\n", (int)ROM);

    /* Read ROM Image */
    f_read(&file, ROM, 0x4000 * NesHeader.byRomSize, &br);

    if ( NesHeader.byVRomSize > 0 )
    {
        /* Allocate Memory for VROM Image */
        VROM = (BYTE *)memMalloc( NesHeader.byVRomSize * 0x2000 );
        printf("VROM addr:%x\r\n", (int)VROM);

        /* Read VROM Image */
        f_read(&file, VROM, 0x2000 * NesHeader.byVRomSize, &br);
    }

    /* File close */
    f_close(&file);

#endif
    /* Successful */
    return 0;
}

/*===================================================================*/
/*                                                                   */
/*           pNesX_ReleaseRom() : Release a memory for ROM           */
/*                                                                   */
/*===================================================================*/
void pNesX_ReleaseRom()
{
/*
 *  Release a memory for ROM
 *
 */
  if ( ROM )
  {
    memFree( ROM );
    ROM = NULL;
  }

  if ( VROM )
  {
    memFree( VROM );
    VROM = NULL;
  }
}

/*===================================================================*/
/*                                                                   */
/*      pNesX_LoadFrame() :                                          */
/*           Transfer the contents of work frame on the screen       */
/*                                                                   */
/*===================================================================*/
bool refresh_draw = false;
void pNesX_LoadFrame()
{
/*
 *  Transfer the contents of work frame on the screen
 *
 */
#if defined(SHOW_FPS)
    fps++;
    
    if (millis()-fps_pre_time >= 1000)
    {
        printf("%d %d %d\r\n", fps, millis()-fps_pre_time, dmawait);
        fps = 0;
        fps_pre_time = millis();
    }
    dmawait = 0;
#endif

    refresh_draw = true;
    lcdRequestDraw();
}

extern "C" void drvLcdCopyLineBuffer(uint16_t x_pos, uint16_t y_pos, uint8_t *p_data, uint32_t length);

void pNesX_TransmitLinedata()
{
#if 0
  while (SpiHandle.State != HAL_SPI_STATE_READY)
  {
#if defined(SHOW_FPS)
    dmawait++;
#endif
  }
  //HAL_SPI_Transmit_DMA(&SpiHandle, (uint8_t *)pDrawData, 256 * 2);
#endif

  /*
  if (refresh_draw == true)
  {
    uint16_t *p_data = (uint16_t *)pDrawData;
    for(int i=0; i<256; i++)
    {
      //gb.display.drawPixel(72+i, PPU_Scanline, (Color)p_data[i]);
    }
  }
  //drvLcdCopyLineBuffer(0, PPU_Scanline, (uint8_t *)p_data, 256*2);
  */

  if (refresh_draw == true)
  {
    uint16_t *p_data = (uint16_t *)pDrawData;
    uint16_t *p_buf = lcdGetFrameBuffer();

    for(int i=0; i<256; i++)
    {
      p_buf[PPU_Scanline * HW_LCD_WIDTH + i + 32] = p_data[i];
    }
  }
}

#if !defined(PS_GAMEPAD)
const BYTE UsbPadTable[] = {0x10, 0x90, 0x80, 0xa0, 0x20, 0x60, 0x40, 0x50}; 
#endif

extern void ApuAdjustVolume(int8_t step);

/*===================================================================*/
/*                                                                   */
/*             pNesX_PadState() : Get a joypad state                 */
/*                                                                   */
/*===================================================================*/
void pNesX_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem )
{
/*
 *  Get a joypad state
 *
 *  Parameters
 *    DWORD *pdwPad1                   (Write)
 *      Joypad 1 State  R L D U St Se B A 
 *
 *    DWORD *pdwPad2                   (Write)
 *      Joypad 2 State
 *
 *    DWORD *pdwSystem                 (Write)
 *      Input for pNesX
 *
 */
#if 0
#if !defined(PS_GAMEPAD)
  *pdwPad1 = ((pad->report[4] & 0x20) >> 5) |
             ((pad->report[4] & 0x10) >> 3) |
             ((pad->report[5] & 0x30) >> 2);

  if (!(pad->report[4] & 8))
  {
      *pdwPad1 |= UsbPadTable[pad->report[4] & 7];
  }
  
  *pdwPad1 = *pdwPad1 | ( *pdwPad1 << 8 );
    
  *pdwPad2 = 0;
  *pdwSystem = ((pad->report[5] & 0x4) >> 2) |
               (((pad->report[5] & 0xa) == 0xa) << 1);
                   
  USBHost::poll();
#else

    unsigned short pad1, pad2;
    pspad_read(&pad1, &pad2);
                               // R L D U St Se B A 
 
    // SE -- -- ST U R D L       L2 R2 L1 R1 TR O X SQ
    *pdwPad1 = ((pad1 & 0x0400) >> 3) |
               ((pad1 & 0x0100) >> 2) |
               ((pad1 & 0x0200) >> 4) |
               ((pad1 & 0x0800) >> 7) |
               ((pad1 & 0x1000) >> 9) |
               ((pad1 & 0x8000) >> 13) |
               ((pad1 & 1) << 1) |
               ((pad1 & 2) >> 1);

    *pdwPad1 = *pdwPad1 | ( *pdwPad1 << 8 );

    *pdwPad2 = 0;
    
    *pdwSystem = ((pad1 & 0x80) >> 7) |
                 (((pad1 & 0x50) == 0x50) << 1);
    
#endif
#else
    uint8_t data = 0;

    if (buttonGetPressed(_DEF_HW_BTN_A))
    {
      data |= (1<<0);
    }
    if (buttonGetPressed(_DEF_HW_BTN_B))
    {
      data |= (1<<1);
    }
    if (buttonGetPressed(_DEF_HW_BTN_SELECT))
    {
      data |= (1<<3);
    }
    if (buttonGetPressed(_DEF_HW_BTN_START))
    {
      data |= (1<<2);
    }
    if (buttonGetPressed(_DEF_HW_BTN_LEFT))
    {
      data |= (1<<6);
    }
    if (buttonGetPressed(_DEF_HW_BTN_RIGHT))
    {
      data |= (1<<7);
    }
    if (buttonGetPressed(_DEF_HW_BTN_UP))
    {
      data |= (1<<4);
    }
    if (buttonGetPressed(_DEF_HW_BTN_DOWN))
    {
      data |= (1<<5);
    }

#if 0
    if (gb.buttons.released(Button::c))
    {
      ApuAdjustVolume(-1);
    }
    if (gb.buttons.released(Button::d))
    {
      ApuAdjustVolume(+1);
    }
#endif

    *pdwPad1 = data | ( data << 8 );
    *pdwPad2 = 0;

    *pdwSystem = buttonGetPressedTime(0);
#endif

}

/*===================================================================*/
/*                                                                   */
/*             pNesX_MemoryCopy() : memcpy                           */
/*                                                                   */
/*===================================================================*/
void *pNesX_MemoryCopy( void *dest, const void *src, int count )
{
/*
 *  memcpy
 *
 *  Parameters
 *    void *dest                       (Write)
 *      Points to the starting address of the copied block to destination
 *
 *    const void *src                  (Read)
 *      Points to the starting address of the block of memory to copy
 *
 *    int count                        (Read)
 *      Specifies the size, in bytes, of the block of memory to copy
 *
 *  Return values
 *    Pointer of destination
 */

  memcpy(dest, src, count );
  return dest;
}

/*===================================================================*/
/*                                                                   */
/*             pNesX_MemorySet() :                                   */
/*                                                                   */
/*===================================================================*/
void *pNesX_MemorySet( void *dest, int c, int count )
{
/*
 *  memset
 *
 *  Parameters
 *    void *dest                       (Write)
 *      Points to the starting address of the block of memory to fill
 *
 *    int c                            (Read)
 *      Specifies the byte value with which to fill the memory block
 *
 *    int count                        (Read)
 *      Specifies the size, in bytes, of the block of memory to fill
 *
 *  Return values
 *    Pointer of destination
 */

  memset(dest, c, count); 
  return dest;
}

void wait(float data)
{
  uint32_t delay_data;

  delay_data = data * 1000.;
  delay(delay_data);
}

/*===================================================================*/
/*                                                                   */
/*                DebugPrint() : Print debug message                 */
/*                                                                   */
/*===================================================================*/
void pNesX_DebugPrint( char *pszMsg )
{
}

/*===================================================================*/
/*                                                                   */
/*                pNesX Initialise                                   */
/*                                                                   */
/*===================================================================*/
int pnesxMain()
{
	// SD card Initialize

  strcpy(szRomFolder, PNESX_ROOT_DIR);


  // Apu Initialize
  ApuInit();
    
  /*-------------------------------------------------------------------*/
  /*  Start pNesX                                                      */
  /*-------------------------------------------------------------------*/

  pNesX_Main();
}


