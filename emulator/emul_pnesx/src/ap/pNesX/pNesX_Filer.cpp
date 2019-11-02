/*===================================================================*/
/*                                                                   */
/*  pNesX_Filer.cpp : ROM selection screen                           */
/*                                                                   */
/*  2016/1/20  Racoon                                                */
/*                                                                   */
/*===================================================================*/

#include "ap.h"
#include "pNesX.h"

extern void pNesX_PadState( DWORD *pdwPad1, DWORD *pdwPad2, DWORD *pdwSystem );
extern const unsigned char pNesX_logo[];
extern const unsigned char Racoon_logo[];

//#define LOGO_BACKCOLOR 0x86fd
#define LOGO_BACKCOLOR blue

#define FILER_OFFSET_X    (0)
#define FILER_OFFSET_Y    (0)


#define FOLDER_FILE_MAX 100
#define LIST_ROWS 8
#define LIST_TOP  40
#define LIST_LEFT 4

int DispTop; // Top of the list
int SelIdx; // Cursor index
int FileCount; // Number of files in the folder

// file information
typedef struct tagLISTITEM{
    char filename[33];
    bool isDir;
    long size;
} LISTITEM;

// folder file list
LISTITEM *flist;

char szRomFolder[256];
char szPrevFolder[256];
char szRomFilename[33];
char szRomPath[256];

/*-------------------------------------------------------------------*/
/*  Get file extension                                               */
/*-------------------------------------------------------------------*/
char *get_ext(char *filename)
{
    int i;
    for (i = strlen(filename) - 1; i >= 0; --i) {
        if (filename[i] == '.') {
            return &filename[i + 1];
        }
    }
    return NULL;
}

/*-------------------------------------------------------------------*/
/*  Get file path                                                    */
/*-------------------------------------------------------------------*/
char *get_filepath(char *path, char *filename)
{
    static char filepath[256];

    if (strcmp(path, PNESX_ROOT_DIR) != 0)
    {
        filepath[0] = 0;
        return filepath;
    }
    
    int len = strlen(path);
    strcpy(filepath, filename);

    return filepath;
}

/*-------------------------------------------------------------------*/
/*  Get file list in the folder                                     */
/*-------------------------------------------------------------------*/
int get_filelist(char *path)
{
#if 0
    DIR *dir = opendir(path);
    if ( dir == NULL ) {
        return 0;
    }

    dirent *entry;
    FILINFO fi;
    fi.lfname = NULL;
    fi.lfsize = 0;
    int i = 0;

    // not root
    if (strcmp(path, "/sd") != 0) {
        strcpy(flist[i].filename, "..");
        flist[i].isDir = true;
        i++;
    }

    // folder
    while ((entry = readdir(dir)) != NULL && i < FOLDER_FILE_MAX) {
        if (f_stat(get_filepath(path, entry->d_name), &fi) == FR_OK) {
            int len = strlen(entry->d_name);
            len = len > 32 ? 32 : len;
            if (entry->d_name[0] == '.') continue;

            memcpy(flist[i].filename, entry->d_name, len);
            flist[i].filename[len] = 0;
            flist[i].isDir = fi.fattrib & AM_DIR;
            flist[i].size = fi.fsize;
            ++i;
        }
    }

    closedir(dir);

    return i;
#else

  FRESULT res;
  DIR dir;
  UINT i = 0;
  static FILINFO fno;

  /*
  // not root
  if (strcmp(path, "/sd") != 0) {
      strcpy(flist[i].filename, "..");
      flist[i].isDir = true;
      i++;
  }
  */

  res = f_opendir(&dir, path);                       /* Open the directory */
  if (res == FR_OK)
  {
    for (;;)
    {
      res = f_readdir(&dir, &fno);                   /* Read a directory item */

      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
      if (fno.fattrib & AM_DIR)
      {                    /* It is a directory */
        continue;
      }
      else
      {                                       /* It is a file. */
        int len = strlen(fno.fname);
        len = len > 32 ? 32 : len;
        memcpy(flist[i].filename, fno.fname, len);
        flist[i].filename[len] = 0;
        flist[i].isDir = false;
        flist[i].size = fno.fsize;
        ++i;
      }
    }
    f_closedir(&dir);
  }

  return i;
#endif

}


/*-------------------------------------------------------------------*/
/*  Get file information string                                      */
/*-------------------------------------------------------------------*/
char* filedata(int idx)
{
    static char disptext[41];
    
    if (flist[idx].isDir) {
        sprintf(disptext, "%-16s  <DIR>", flist[idx].filename);
    } else {
        sprintf(disptext, "%-16s", flist[idx].filename);
    }

    return disptext;
}

/*-------------------------------------------------------------------*/
/*  Draw file list                                                   */
/*-------------------------------------------------------------------*/
void draw_list()
{
#if 0
    int i,y;

    for (y = 0, i = DispTop; y < LIST_ROWS && i < FileCount; ++y, ++i) {
        tft_text( LIST_LEFT, LIST_TOP + y * 8, filedata(i), TFT_WHITE, TFT_BLACK );
    }

    if (y < LIST_ROWS) {
        tft_boxfill( 0, LIST_TOP + y * 8, 319, LIST_TOP + LIST_ROWS * 8 - 1, TFT_BLACK);
    }
#else
  int i,y;

  for (y = 0, i = DispTop; y < LIST_ROWS && i < FileCount; ++y, ++i)
  {
    //tft_text( LIST_LEFT, LIST_TOP + y * 8, filedata(i), TFT_WHITE, TFT_BLACK );
    lcdPrintf( LIST_LEFT, LIST_TOP + y * 16, white, "%s", filedata(i));
  }

  if (y < LIST_ROWS)
  {
      //tft_boxfill( 0, LIST_TOP + y * 8, 319, LIST_TOP + LIST_ROWS * 8 - 1, TFT_BLACK);
    //lcdDrawFillRect( 0, LIST_TOP + y * 16, 319, LIST_ROWS * 16 - 1, black);
  }
#endif
}

/*-------------------------------------------------------------------*/
/*  Draw cursor                                                      */
/*-------------------------------------------------------------------*/
void draw_cursor(bool clear = false)
{
#if 0
    int y = SelIdx - DispTop;
    if (clear) {
        tft_box( LIST_LEFT - 1, LIST_TOP + y * 8 - 1, LIST_LEFT + 39 * 8, LIST_TOP + y * 8 + 7, TFT_BLACK);
        tft_text( LIST_LEFT, LIST_TOP + y * 8, filedata(SelIdx), TFT_WHITE, TFT_BLACK );
    } else {
        tft_box( LIST_LEFT - 1, LIST_TOP + y * 8 - 1, LIST_LEFT + 39 * 8, LIST_TOP + y * 8 + 7, TFT_YELLOW);
        tft_text( LIST_LEFT, LIST_TOP + y * 8, filedata(SelIdx), TFT_BLACK, TFT_YELLOW );
    }
#else

  int y = SelIdx - DispTop;
  if (clear)
  {
    lcdDrawFillRect( LIST_LEFT - 1, LIST_TOP + y * 16 - 1, HW_LCD_WIDTH-LIST_LEFT*2, 15, black);
    lcdPrintf(LIST_LEFT, LIST_TOP + y * 16, white, "%s", filedata(SelIdx));
  } else
  {
    lcdDrawFillRect( LIST_LEFT - 1, LIST_TOP + y * 16 - 1, HW_LCD_WIDTH-LIST_LEFT*2, 15, yellow);
    lcdPrintf( LIST_LEFT, LIST_TOP + y * 16, black, "%s", filedata(SelIdx));
  }
#endif
}

/*-------------------------------------------------------------------*/
/*  Draw back ground                                                 */
/*-------------------------------------------------------------------*/
void draw_frame()
{
#if 0
        tft_boxfill( 0, 0, 319, 31, LOGO_BACKCOLOR);
        tft_boxfill(0, 32, 319, 223, TFT_BLACK);
        tft_boxfill( 0, 224, 319, 239, LOGO_BACKCOLOR);
        
        draw_bmp_4bpp(pNesX_logo, 8, 6);
        tft_text( 200, 225, "L2:Emu<->Menu", TFT_BLACK, LOGO_BACKCOLOR );
        tft_text( 200, 233, "R1+R2:Reset", TFT_BLACK, LOGO_BACKCOLOR );
        draw_bmp_4bpp(Racoon_logo, 0, 224);
#endif

  lcdClear(black);


  lcdDrawFillRect(0, 0, HW_LCD_WIDTH, 31, LOGO_BACKCOLOR);
  lcdDrawFillRect(0, HW_LCD_HEIGHT-31, HW_LCD_WIDTH, 31, LOGO_BACKCOLOR);

  lcdPrintf((HW_LCD_WIDTH - 8*8)/2, 0, yellow, "OROCABOY");
}

/*-------------------------------------------------------------------*/
/*  Dialog window                                                    */
/*-------------------------------------------------------------------*/
void dialog(char *msg)
{
  if (msg)
  {
    lcdDrawFillRect(0, 100, lcdGetWidth(), 40, darkgray);

    uint16_t w = 8, h = 8;

    lcdPrintf((lcdGetWidth()-w)/2, 100 + (40-h)/2, white, "%s", msg);
  }
  else
  {
    //lcdDrawFillRect(0, 100, lcdGetWidth(), 40, black);
    lcdDrawFillRect(0, 100, lcdGetWidth(), 40, white);
  }
}

/*-------------------------------------------------------------------*/
/*  Rom selection                                                    */
/*-------------------------------------------------------------------*/
//  return code:
//   0 : File selected
//   1 : Return to Emu
//   2 : Reset
//   -1: Error
int pNesX_Filer()
{
  DWORD dwPad1, dwPad2, dwSystem;
  DWORD prev_dwPad1 = 0;

  bool loadFolder;
  int keyrepeat = 1;

  lcdClear(black);
  lcdUpdateDraw();

  lcdSetDoubleBuffer(false);
  lcdClear(black);
  lcdUpdateDraw();


  // Draw back ground
  draw_frame();


  // init
  flist = (LISTITEM *)malloc(sizeof(LISTITEM) * FOLDER_FILE_MAX);
  if (flist == NULL)
  {
      return -1;
  }

  loadFolder = true;
  int ret;

  while(1)
  {
    
    if (loadFolder)
    {
        // Drawing of the files in the folder
        FileCount = get_filelist(szRomFolder);

        if (FileCount > 0) {
            if (strcmp(szRomFolder, szPrevFolder) != 0)
            {
                DispTop = 0;
                SelIdx = 0;
                strcpy( szPrevFolder, szRomFolder);
            }
            draw_list();
            draw_cursor();
        }
        loadFolder = false;
    }

    pNesX_PadState( &dwPad1, &dwPad2, &dwSystem );
    
    if (dwPad1 != prev_dwPad1)
    {
        keyrepeat = 0;
    }

    if (keyrepeat == 0 || keyrepeat >= 10) {
        // Down
        if (dwPad1 & 0x20) {
            if (SelIdx < FileCount - 1) {
                // clear cursor
                draw_cursor(true);

                if (SelIdx - DispTop == LIST_ROWS - 1) {
                    DispTop++;
                    draw_list();
                }

                // draw new cursor
                SelIdx++;
                draw_cursor();
            }
        }

        // Up
        if (dwPad1 & 0x10) {
            if (SelIdx > 0) {
                // clear cursor
                draw_cursor(true);

                if (SelIdx - DispTop == 0) {
                    DispTop--;
                    draw_list();
                }

                // draw new cursor
                SelIdx--;
                draw_cursor();
            }
        }

        // Select
        if (dwPad1 & 0x01) {
            // clear cursor
            draw_cursor(true);


            if (flist[SelIdx].isDir) {
                // folder
                if (strcmp(flist[SelIdx].filename, "..") != 0) {
                    sprintf(szRomFolder, "%s/%s", szRomFolder, flist[SelIdx].filename);
                } else {
                    // upper folder
                    char *upper = strrchr(szRomFolder, '/');
                    if (upper) {
                        *upper = 0;
                    }
                }

                loadFolder = true;
            } else {
                char *ext = get_ext(flist[SelIdx].filename);

                if (ext != NULL && strcasecmp(ext, "nes") == 0)
                {
                    strcpy(szRomFilename, flist[SelIdx].filename);


                    strcpy(szRomPath, get_filepath(szRomFolder, szRomFilename));
                    
                    ret = 0;

                    break;
                }
                else
                {
                    dialog((char *)"not a rom file!!");
                    wait(1);
                    dialog(NULL);
                    draw_list();
                    draw_cursor();
                }
            }
        }

        /*
        // Return to Emu
        if (dwSystem & 0x01) {
            if ( szRomFilename[0] != 0 ) {
                do {
                    wait(0.08);
                    pNesX_PadState( &dwPad1, &dwPad2, &dwSystem );

                } while (dwSystem & 0x01);

                ret = 1;
                break;
            }
        }

        // Reset
        if (dwSystem & 0x02) {
            if ( szRomFilename[0] != 0 ) {
                do {
                    wait(0.08);
                    pNesX_PadState( &dwPad1, &dwPad2, &dwSystem );
                } while (dwSystem & 0x02);

                ret = 2;
                break;
            }
        }
        */
    }

    keyrepeat++;

    prev_dwPad1 = dwPad1;

    //wait(0.08);
    delay(20);
  }

  // release memory
  free(flist);

  return ret;
}

