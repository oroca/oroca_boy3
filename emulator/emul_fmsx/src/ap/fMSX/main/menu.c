/*
 * The MIT License
 *
 * Copyright 2018 Schuemi.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "hw.h"

#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include "ugui.h"
#include <string.h>
#include <sys/stat.h>
#include "MSX.h"
#include "EMULib.h"
#include "minini.h"
//#include "files.h"
#include "menu.h"
#include "fmsx_main.h"


#define MAX_OBJECTS           6


#define MENU_TEXTBOX_ID       1
#define FILES_TEXTBOX_ID      2
#define MSG_TEXTBOX_ID        3

#define FILES_MAX_ROWS        20
#define FILES_MAX_LENGTH      31
#define FILES_HEADER_LENGTH   27
#define FILES_MAX_LENGTH_NAME 256




typedef unsigned short pixelp;


pixelp* pixelBuffer;

UG_GUI   gui;
UG_WINDOW window;
UG_TEXTBOX menuTextBox;
UG_OBJECT obj_buff_wnd_1[MAX_OBJECTS];

UG_WINDOW fileWindow;
UG_TEXTBOX fileTextBox;
UG_OBJECT obj_buff_wnd_file[MAX_OBJECTS];

UG_WINDOW msgWindow;
UG_TEXTBOX msgTextBox;
UG_OBJECT obj_buff_wnd_msg[MAX_OBJECTS];


int m_width = WIDTH_OVERLAY;
int m_height = HEIGHT_OVERLAY;
int currentSelectedItem = 0;
char* menuTxt;
//bool keyPressed;
int lastPressedKey;
int lastSelectedItem = 0;
int holdDownCounter = 0;
char* selectedFile;
char* lastOpenedFileFullPath;

int position = 0;
int selPosition = 0;

#define MENU_ITEMS 13

static const struct {
    const char* menuItem[MENU_ITEMS];
} menuItems = {
    "Load file\n",
   "Save state\n",
   "\n",
   "Eject cartridge\n",
   "Eject disks\n",
   "\n",
   "fMSX Menu\n",
   "Reset\n",
   "\n",
   "Audio output\n",
   "\n",
   "\n",
   "About\n",


};



int DrawuGui(uint16_t* uGuiMenu, int y)
{
  while(lcdDrawAvailable() == false)
  {
    delay(1);
  }

  uint16_t *p_buf = lcdGetFrameBuffer();

  memcpy(p_buf, uGuiMenu, WIDTH_OVERLAY * HEIGHT_OVERLAY * 2);

  lcdRequestDraw();

  return 0;
}


void pixelSet(UG_S16 ul_x,UG_S16 ul_y, UG_COLOR ul_color)
{
  if (ul_x > m_width || ul_y > m_height) return;
  pixelBuffer[ul_x + (ul_y*m_width)] = ul_color;
}

void window_callback( UG_MESSAGE* msg ) {
    
}

void fmsxGUI_initMenu()
{
  pixelBuffer = (pixelp*)memMalloc(m_width*m_height*sizeof(pixelp));
  memset(pixelBuffer, 0, m_width*m_height*sizeof(pixelp));

  selectedFile = (char*)memMalloc(FILES_MAX_LENGTH_NAME+1);
  lastOpenedFileFullPath = (char*)malloc(1024);
  lastOpenedFileFullPath[0] = 0;

  menuTxt = (char*)memMalloc(1000);

  UG_Init(&gui,pixelSet,m_width, m_height);

  UG_WindowCreate ( &window , obj_buff_wnd_1 , MAX_OBJECTS, window_callback);
  UG_WindowResize(&window, 10, 10, 310, 220);
  UG_WindowSetTitleTextFont ( &window , &FONT_8X8 ) ;
  UG_WindowSetTitleText ( &window , "OROCABOY fMSX" ) ;
  UG_WindowSetBackColor( &window , C_GRAY );
  UG_TextboxCreate(&window, &menuTextBox, MENU_TEXTBOX_ID, 10, 10, 290, 160);
  UG_TextboxSetAlignment(&window, MENU_TEXTBOX_ID, ALIGN_TOP_LEFT);
  UG_TextboxSetForeColor(&window, MENU_TEXTBOX_ID, C_BLACK);
  UG_TextboxSetFont ( &window , MENU_TEXTBOX_ID, &FONT_8X8 ) ;

  
  UG_TextboxShow(&window, MENU_TEXTBOX_ID);
  UG_WindowShow(&window);
}

int fmsxGUI_getKey()
{
  int key_num = -1;
  bool key_pressed;

  key_pressed = false;

  for (int i = 0; i < BUTTON_MAX_CH; i++)
  {
    if (buttonGetPressed(i))
    {
      key_pressed = true;
      key_num = i;
    }
  }


  if (key_pressed && lastPressedKey != -1)
  {
    holdDownCounter++;
    delay(100);

    if (holdDownCounter > 200)
    {
      return lastPressedKey;
    }
    return -1;
  }
  holdDownCounter = 0;
    
  if (!key_pressed)
  {
    lastPressedKey = -1;
  }
  else
  {
    lastPressedKey = key_num;
    delay(100);
  }

  return lastPressedKey;
}

int fmsxGUI_getKeyBlock()
{
  int key;

  do
  {
    delay(1);
    key = fmsxGUI_getKey();
  }while (key == -1);
  return key;
}


const char* fmsxGUI_chooseFile(const char *Ext)
{
#if 0
   DIR *D;
   struct dirent *DP;
   struct stat ST;
   

   char* Buf;
   int BufSize = 256;
   char* txtFiles;
   char* shownFiles[FILES_MAX_ROWS];
   
   
   txtFiles = memMalloc(FILES_MAX_ROWS*(FILES_MAX_LENGTH + 5) + 1);
   for (int i = 0; i < FILES_MAX_ROWS; i++) {
       shownFiles[i] = memMalloc(FILES_MAX_LENGTH_NAME+1);
   }
   
   Buf = (char*)memMalloc(256);
   char* txtFilesPosition;
   
   
   
   int i, r, s, fileCount;

   
   bool isDir[FILES_MAX_ROWS];
   bool switchedPage = true;
   
   UG_WindowCreate ( &fileWindow , obj_buff_wnd_file , MAX_OBJECTS, window_callback);
   UG_WindowResize(&fileWindow, 20, 20, 300, 210);
   UG_WindowSetTitleTextFont ( &fileWindow , &FONT_8X8 ) ;
   
   if(!getcwd(Buf,BufSize-2)) strncpy(Buf,"Choose File", 256);
   txtFilesPosition = Buf;
   if (strlen(txtFilesPosition) > FILES_HEADER_LENGTH) txtFilesPosition += strlen(txtFilesPosition) - FILES_HEADER_LENGTH;
  
   
   
   UG_WindowSetTitleText ( &fileWindow , txtFilesPosition ) ;
   UG_WindowSetBackColor( &fileWindow , C_GRAY );
   UG_TextboxCreate(&fileWindow, &fileTextBox, FILES_TEXTBOX_ID, 1, 1, 274, 80); 
   
   UG_TextboxSetAlignment(&fileWindow, FILES_TEXTBOX_ID, ALIGN_TOP_LEFT);
   UG_TextboxSetForeColor(&fileWindow, FILES_TEXTBOX_ID, C_BLACK);
   UG_TextboxSetFont ( &fileWindow , FILES_TEXTBOX_ID, &FONT_8X8 ) ;
   UG_TextboxShow(&fileWindow, FILES_TEXTBOX_ID);
   UG_WindowShow(&fileWindow);
   
   int keyNumPressed;
   
   if((D=_opendir("."))){

        
        fileCount = 0;
        // count how many files we have here
        for (_rewinddir(D); (DP=_readdir(D));){
            if (DP->d_type==DT_DIR || hasExt(DP->d_name, Ext)) {fileCount++;}
        }
       
        do {
            
            txtFilesPosition = txtFiles;
         
         // read a new Page
         if (switchedPage){
            // first, go to the position
            _rewinddir(D);
            for (s=0; s < position && (DP=_readdir(D)); s++){
                if (DP->d_type!=DT_DIR && !hasExt(DP->d_name, Ext)) {s--; continue;}
            }
            // than read the next FILES_MAX_ROWS files
            for(i=0;(i < FILES_MAX_ROWS && (DP=_readdir(D)));i++) {
               isDir[i] = false;
               if (DP->d_type==DT_DIR) isDir[i] = true;
               if (isDir[i] == false && !hasExt(DP->d_name, Ext)) {i--; continue;}
               strncpy(shownFiles[i],DP->d_name,FILES_MAX_LENGTH_NAME);
            }
            switchedPage = false;
            if (i == 0) {
                // nothing on this page, go to the first page, if we are not already
                if (position >= FILES_MAX_ROWS) {
                    position = 0;
                    selPosition = 0;
                    switchedPage = true;
                    continue;
                }
                
            }
        } 
       
        /// Draw the TextBox
        r = i;
        for(i=0;i<r;i++) {
           if (selPosition == i + position) *txtFilesPosition = 16;  else *txtFilesPosition = 0x20;
           txtFilesPosition++;
           if (isDir[i]) *txtFilesPosition = 0xfd; else   *txtFilesPosition = 0xfe;
           txtFilesPosition++;*txtFilesPosition = 0x20;txtFilesPosition++;
           strncpy(txtFilesPosition, shownFiles[i], FILES_MAX_LENGTH);
           if (strlen(shownFiles[i]) < FILES_MAX_LENGTH) txtFilesPosition += strlen(shownFiles[i]); else txtFilesPosition += FILES_MAX_LENGTH;
           *txtFilesPosition = '\n';
           txtFilesPosition++;
          
        }
        
        
        *txtFilesPosition = 0;
         
        UG_WindowShow(&fileWindow); // force a window update
        UG_TextboxSetText( &fileWindow , FILES_TEXTBOX_ID, txtFiles);
        
         UG_Update();
         DrawuGui(pixelBuffer, 0);
         keyNumPressed = odroidFmsxGUI_getKey_block();
         
         if (keyNumPressed == ODROID_INPUT_RIGHT) selPosition+= FILES_MAX_ROWS;
         if (keyNumPressed == ODROID_INPUT_LEFT)  selPosition-= FILES_MAX_ROWS;
         if (keyNumPressed == ODROID_INPUT_DOWN)  selPosition++;
         if (keyNumPressed == ODROID_INPUT_UP)    selPosition--;
         
         
         if (selPosition < 0) {
             // go to the last page
             selPosition = fileCount -1;
             position = FILES_MAX_ROWS * (int)(fileCount / FILES_MAX_ROWS);
             if (position == fileCount) position -= FILES_MAX_ROWS;
             switchedPage = true;
             
         }
         if (i != FILES_MAX_ROWS && selPosition >= position + i) { 
             // go to the first page
             selPosition = 0; position = 0;  switchedPage = true;
         }
         
         if (selPosition >= FILES_MAX_ROWS + position){ 
             // go to next page
             position += FILES_MAX_ROWS; selPosition = position;  switchedPage = true;
         }
         
         if (selPosition < position){ 
             // go to previous page
             position -= FILES_MAX_ROWS; 
             if (position < 0) position = 0;
             selPosition = position + FILES_MAX_ROWS - 1; 
             switchedPage = true;
         }
         if (selPosition - position >= 0 && selPosition - position < FILES_MAX_ROWS && keyNumPressed == ODROID_INPUT_A && isDir[selPosition - position]) {
            free(txtFiles);
            free(Buf);
            UG_TextboxDelete(&fileWindow, FILES_TEXTBOX_ID);
            UG_WindowDelete(&fileWindow);
            chdir(shownFiles[selPosition - position]);
            selPosition = 0;
            for (int i = 0; i < FILES_MAX_ROWS; i++) free(shownFiles[i]);
            closedir(D);
            return odroidFmsxGUI_chooseFile(Ext);
         }
        if(keyNumPressed == ODROID_INPUT_B) {
            free(txtFiles);
            free(Buf);
            UG_TextboxDelete(&fileWindow, FILES_TEXTBOX_ID);
            UG_WindowDelete(&fileWindow);
            chdir("..");
            selPosition = 0;
            for (int i = 0; i < FILES_MAX_ROWS; i++) free(shownFiles[i]);
            closedir(D);
            return odroidFmsxGUI_chooseFile(Ext);
            
        }
         

        } while(keyNumPressed != ODROID_INPUT_A && keyNumPressed != ODROID_INPUT_MENU);
        
        
        closedir(D);
    }
   if(keyNumPressed == ODROID_INPUT_A) {
       strncpy(selectedFile, shownFiles[selPosition - position], FILES_MAX_LENGTH_NAME);
   }
   
   free(txtFiles);
   free(Buf);
   for (int i = 0; i < FILES_MAX_ROWS; i++) free(shownFiles[i]);
   UG_TextboxDelete(&fileWindow, FILES_TEXTBOX_ID);
   UG_WindowDelete(&fileWindow);
   
   if(keyNumPressed == ODROID_INPUT_A) {
            return selectedFile;
   }
#endif
   return NULL;
}
void fmsxGUI_selectMenuItem(int item)
{
  char* menuPos = menuTxt;

  for (int i = 0; i < MENU_ITEMS; i++)
  {
    if (item == i) *menuPos = 16;  else *menuPos = 0x20;
    menuPos++;*menuPos = 0x20;menuPos++;
    int len = strlen(menuItems.menuItem[i]);
    memcpy(menuPos, menuItems.menuItem[i], len);
    menuPos += len;
  }
  *menuPos = 0;
  UG_TextboxSetText( &window , MENU_TEXTBOX_ID, menuTxt);
  UG_TextboxSetAlignment(&window , MENU_TEXTBOX_ID,ALIGN_H_LEFT|ALIGN_V_CENTER);
}

void fmsxGUI_setLastLoadedFile(const char* file)
{
  if (file != NULL)
      strncpy(lastOpenedFileFullPath, file, 1024);
  else
      lastOpenedFileFullPath[0] = 0;
}

// msg Box: max 33 letters in one row!

void fmsxGUI_msgBox(const char* title, const char* msg, char waitKey)
{
  int rows = 1;
  const char* p = msg;
  while(*p++ != 0)
  {
    if (*p == '\n') rows++;
  }

  // if there is a old instance
  UG_TextboxDelete(&msgWindow, MSG_TEXTBOX_ID);
  UG_WindowDelete(&msgWindow);


  UG_WindowCreate ( &msgWindow , obj_buff_wnd_file , MAX_OBJECTS, window_callback);
  UG_WindowResize(&msgWindow, 20, 20, 300, 54 + (rows*8));



  UG_WindowSetTitleTextFont ( &msgWindow , &FONT_8X8 ) ;
  UG_WindowSetTitleText ( &msgWindow , (char *)title ) ;
  UG_WindowSetBackColor( &msgWindow , C_GRAY );


  UG_TextboxCreate(&msgWindow, &msgTextBox, MSG_TEXTBOX_ID, 6, 6, 274, 12+rows*8);

  UG_TextboxSetAlignment(&msgWindow, MSG_TEXTBOX_ID, ALIGN_TOP_LEFT);
  UG_TextboxSetForeColor(&msgWindow, MSG_TEXTBOX_ID, C_BLACK);
  UG_TextboxSetFont ( &msgWindow , MSG_TEXTBOX_ID, &FONT_8X8 ) ;
  UG_TextboxShow(&msgWindow, MSG_TEXTBOX_ID);
  UG_WindowShow(&msgWindow);

  UG_TextboxSetText( &msgWindow , MSG_TEXTBOX_ID, (char *)msg);
  
  
  UG_Update();
  DrawuGui(pixelBuffer, 0);

  if (waitKey && fmsxGUI_getKeyBlock()){}
}

char saveState(const char* fileName)
{
  char res = 1;
#if 0
  if (! fileName)
  {
    char* stateFileName = (char*)memMalloc(1024);
    char* stateFileNameF = (char*)memMalloc(1024);
    strncpy(stateFileName, lastOpenedFileFullPath, 1024);
    cutExtension(stateFileName);
    snprintf(stateFileNameF, 1024, "%s.sta", stateFileName);
    if (!SaveSTA(stateFileNameF)){
        res = 0;
    }
    memFree(stateFileName);
    memFree(stateFileNameF);
  } else
  {
    SaveSTA(fileName);
  }
#endif
  return res;
}

MENU_ACTION fmsxGUI_showMenu()
{
   
  char stopMenu = 0;
  fmsxGUI_selectMenuItem(currentSelectedItem);
  MENU_ACTION ret = MENU_ACTION_NONE;



  // wait until the menu button is not pressed anymore
  int keyPressed;
  do
  {
    keyPressed = fmsxGUI_getKey();
  }while (keyPressed == _DEF_HW_BTN_SELECT);
   
   
  /// now listen for another button

  do
  {
    int c = 0;
    char buf[3];

    UG_WindowShow(&window); // force a window update
    UG_Update();
    DrawuGui(pixelBuffer, 0);

    keyPressed = fmsxGUI_getKeyBlock();

    if (keyPressed == _DEF_HW_BTN_DOWN) c = 1;
    if (keyPressed == _DEF_HW_BTN_UP) c = -1;

    currentSelectedItem += c;

    if (currentSelectedItem < 0)currentSelectedItem = MENU_ITEMS - 1;
    if (currentSelectedItem >= MENU_ITEMS)currentSelectedItem = 0;

    if (menuItems.menuItem[currentSelectedItem][0] == '\n') currentSelectedItem += c;


    if (lastSelectedItem != currentSelectedItem) {
       lastSelectedItem = currentSelectedItem;
       fmsxGUI_selectMenuItem(currentSelectedItem);
    }

    if (keyPressed == _DEF_HW_BTN_A)
    {
      const char* lastSelectedFile = NULL;
      switch(currentSelectedItem)
      {
        ///////////// Load File ///////////////////
        case 0:
#if 0
          lastSelectedFile = fmsxGUI_chooseFile(".rom\0.mx1\0.mx2\0.dsk\0.cas\0\0");
          fmsxGUI_msgBox("Please wait...", "Please wait while loading", 0);
          if (lastSelectedFile != NULL)
          {
            if (LoadFile(lastSelectedFile))
            {
              getFullPath(lastOpenedFileFullPath, lastSelectedFile, 1024);
              ini_puts("FMSX", "LASTGAME", lastOpenedFileFullPath, FMSX_CONFIG_FILE);
              loadKeyMappingFromGame(lastSelectedFile);
            }
            else
            {
              lastOpenedFileFullPath[0] = 0;
            }
            stopMenu = true;
          }
#endif
          break;

        //////////////// Save State ////////////////////
        case 1:
          if (lastOpenedFileFullPath[0] != 0)
          {
            fmsxGUI_msgBox("Please wait...", "Please wait while saving", 0);
            if (!saveState(NULL))
            {
              fmsxGUI_msgBox("Error", "Could not save state", 1);
            }
          }
          stopMenu = true;
          break;

        //////////////// Eject Cardridge ////////////////////
        case 3:
#if 0
          for(int J=0;J<MAXSLOTS;++J) LoadCart(0,J,0);
          ini_puts("FMSX", "LASTGAME", "", FMSX_CONFIG_FILE);
          LoadKeyMapping("/sd/odroid/data/msx/config.ini");
          stopMenu = true;
#endif
          break;

        case 4:
#if 0
          for(int J=0;J<MAXDRIVES;++J) ChangeDisk(J,0);
          ini_puts("FMSX", "LASTGAME", "", FMSX_CONFIG_FILE);
          stopMenu = true;
#endif
          break;

        case 6:
#if 0
          MenuMSX();
          stopMenu = true;
#endif
          break;

        case 7:
#if 0
          ResetMSX(Mode,RAMPages,VRAMPages);
          stopMenu = true;
#endif
          break;

        case 9:
#if 0
          ini_gets("FMSX", "DAC", "0", buf, 3, FMSX_CONFIG_FILE);
          if (! atoi(buf))
          {
            ini_puts("FMSX", "DAC", "1", FMSX_CONFIG_FILE);
            fmsxGUI_msgBox("Audio", " Audio is now: DAC\n\nPlease turn the Device OFF and\nON again to take effect.", 1);
          }
          else
          {
            ini_puts("FMSX", "DAC", "0", FMSX_CONFIG_FILE);
            fmsxGUI_msgBox("Audio", " Audio is now: Speaker\n\nPlease turn the Device OFF and\nON again to take effect.", 1);
          }
          ret = MENU_ACTION_CHANGED_AUDIO_TYPE;
#endif
          break;

        case 12:
          fmsxGUI_msgBox("About", " \nfMSX\n\n by Marat Fayzullin\n\n ported by Jan P. Schuemann\n\nThanks to the ODROID-GO community\n\nHave fun!\n ", 1);
          break;
       };
    }
    if (keyPressed == _DEF_HW_BTN_SELECT) stopMenu = true;

  }while (!stopMenu);

  return ret;
    
}


// Key handling for the old menu


/** WaitKeyOrMouse() *****************************************/
/** Wait for a key or a mouse button to be pressed. Returns **/
/** the same result as GetMouse(). If no mouse buttons      **/
/** reported to be pressed, call GetKey() to fetch a key.   **/
/*************************************************************/

unsigned int WaitKeyOrMouse(void)
{
 int key = fmsxGUI_getKeyBlock();
 switch(key)
 {
     case _DEF_HW_BTN_RIGHT: return CON_RIGHT;
     case _DEF_HW_BTN_LEFT: return CON_LEFT;
     case _DEF_HW_BTN_UP: return CON_UP;
     case _DEF_HW_BTN_DOWN: return CON_DOWN;
     case _DEF_HW_BTN_A: return CON_OK;
     default: return 0;
 }
}
unsigned int GetKey(void) {
   
    return 0;
}
unsigned int WaitKey(void) {
    return WaitKeyOrMouse();
}

