/*
 * slot.c
 *
 *  Created on: 2019. 11. 4.
 *      Author: HanCheol Cho
 */




#include "slot.h"
#include "util.h"
#include "cmdif.h"
#include "gpio.h"
#include "qspi.h"
#include "flash.h"
#include "fatfs/fatfs.h"



#ifdef _USE_HW_SLOT


#define FLASH_TAG_SIZE    1024



typedef struct
{
  char     file_name[256];
  uint32_t file_size;

  flash_tag_t tag;
} slot_file_t;



static flash_tag_t slot_fw_tag;
static char slot_path[256];
static char slot_file_name[256];



static bool slotVerifyFwCrc(uint32_t addr);
static void slotCmdif(void);



bool slotInit(void)
{

  cmdifAdd("slot", slotCmdif);

  return true;
}

bool slotRunFromFlash(uint8_t slot_index)
{
  uint32_t addr_fw;
  uint32_t addr_run;
  uint32_t pre_time;
  uint32_t slot_size = 512*1024;
  flash_tag_t  *p_fw_tag;

  addr_fw  = QSPI_FW_ADDR(slot_index);


  p_fw_tag = (flash_tag_t *)addr_fw;


  if (p_fw_tag->magic_number != FLASH_MAGIC_NUMBER)
  {
    logPrintf("fw magic number \t: Fail\r\n");
    return false;
  }

  slot_size = p_fw_tag->tag_flash_length + p_fw_tag->tag_length;
  if (p_fw_tag->addr_tag == addr_fw)
  {
    addr_run = addr_fw;
  }
  else
  {
    pre_time = millis();
    addr_run = p_fw_tag->addr_tag;
    memcpy((void *)addr_run, (void *)addr_fw, slot_size);

    logPrintf("copy_fw   \t\t: %dms, %dKB\n", (int)(millis()-pre_time), (int)slot_size/1024);
  }

  if (slotVerifyFwCrc(addr_run) == true)
  {
    logPrintf("fw crc    \t\t: OK\n");
  }
  else
  {
    logPrintf("fw crc    \t\t: Fail\n");
    return false;
  }


  logPrintf("addr_fw   \t\t: 0x%X\n", (int)addr_fw);
  logPrintf("addr_run  \t\t: 0x%X\n", (int)addr_run);


  slotJumpToFw(addr_run + 1024);


  return true;
}

bool slotDelFromFlash(uint8_t slot_index)
{
  bool ret;
  uint32_t addr_fw;
  flash_tag_t  *p_fw_tag;


  logPrintf("\nslotRunFromFlash.. \n");

  addr_fw  = QSPI_FW_ADDR(slot_index);


  p_fw_tag = (flash_tag_t *)addr_fw;


  if (p_fw_tag->magic_number != FLASH_MAGIC_NUMBER)
  {
    logPrintf("slot %d empty\r\n", slot_index);
    return false;
  }

  qspiInit();
  delay(10);

  if (flashErase(addr_fw, 1024) == true)
  {
    logPrintf("slot erase  \t\t: OK\n");
    ret = true;
  }
  else
  {
    logPrintf("slot erase  \t\t: Fail\n");
    ret = false;
  }

  qspiEnableMemoryMappedMode();

  return ret;
}

bool slotIsAvailable(uint8_t slot_index)
{
  flash_tag_t  *p_fw_tag;


  p_fw_tag = (flash_tag_t *)QSPI_FW_ADDR(slot_index);


  if (p_fw_tag->magic_number != FLASH_MAGIC_NUMBER)
  {
    return false;
  }

  return true;
}

bool slotRun(uint8_t slot_index)
{
  if (slotRunFromFolder(slot_index) != true)
  {
    slotRunFromFlash(slot_index);
  }

  return false;
}

bool slotRunFromFile(const char *file_name)
{
  return false;
}

bool slotGetFile(uint8_t slot_index, slot_file_t *p_file)
{

  FRESULT res;
  DIR dir;
  FILINFO fno;
  FIL file;
  UINT len;


  sprintf(slot_path, "/slot/%d", slot_index);


  res = f_opendir(&dir, slot_path);                  /* Open the directory */
  if (res == FR_OK)
  {
    for (;;)
    {
      res = f_readdir(&dir, &fno);                   /* Read a directory item */
      if (res != FR_OK || fno.fname[0] == 0) break;  /* Break on error or end of dir */
      if (fno.fattrib & AM_DIR)
      {

      }
      else
      {
        sprintf(slot_file_name, "/slot/%d/%s", slot_index, fno.fname);

        res = f_open(&file, slot_file_name, FA_OPEN_EXISTING | FA_READ);
        if (res == FR_OK)
        {
          f_read(&file, (void *)&slot_fw_tag, sizeof(slot_fw_tag), &len);

          if (slot_fw_tag.magic_number == 0x5555AAAA || slot_fw_tag.magic_number == 0xAAAA5555)
          {
            logPrintf("file      \t\t: %s\n", slot_file_name);
            logPrintf("file info \t\t: %s %s\n", slot_fw_tag.version_str, slot_fw_tag.name_str);

            strcpy(p_file->file_name, slot_file_name);

            p_file->tag = slot_fw_tag;
            p_file->file_size = f_size(&file);
          }
          f_close(&file);
          break;
        }
      }
    }
    f_closedir(&dir);
  }

  return true;
}

bool slotRunFromFolder(uint8_t slot_index)
{
  uint32_t addr_fw;
  uint32_t addr_run;
  uint32_t pre_time;
  uint32_t slot_size;
  flash_tag_t  *p_fw_tag;
  slot_file_t slot_file;

  FRESULT res;
  FIL file;
  UINT len;

  bool ret = false;


  logPrintf("\nslotRunFromFolader.. \n");


  if (slotGetFile(slot_index, &slot_file) == true)
  {
    p_fw_tag = &slot_file.tag;

    addr_fw  = p_fw_tag->addr_tag;
    addr_run = p_fw_tag->addr_tag;

    res = f_open(&file, slot_file.file_name, FA_OPEN_EXISTING | FA_READ);
    if (res == FR_OK)
    {
      slot_size = slot_file.file_size;

      pre_time = millis();
      f_read(&file, (void *)addr_run, slot_file.file_size, &len);
      logPrintf("copy_fw   \t\t: %dms, %dKB\n", (int)(millis()-pre_time), (int)slot_size/1024);
      f_close(&file);

      p_fw_tag = (flash_tag_t *)addr_run;

      if (p_fw_tag->magic_number == 0xAAAA5555 && strcmp((char *)p_fw_tag->board_str, "OROCABOY3") == 0)
      {
        ret = true;
      }

      if (p_fw_tag->magic_number == 0x5555AAAA && strcmp((char *)p_fw_tag->board_str, "OROCABOY3") == 0)
      {
        if (slotVerifyFwCrc(addr_run) == true)
        {
          logPrintf("fw crc    \t\t: OK\n");
          ret = true;
        }
        else
        {
          logPrintf("fw crc    \t\t: Fail\n");
          return false;
        }
      }
    }
  }


  if (ret == true)
  {
    logPrintf("addr_fw   \t\t: 0x%X\n", (int)addr_fw);
    logPrintf("addr_run  \t\t: 0x%X\n", (int)addr_run);


    slotJumpToFw(addr_run + 1024);
  }

  return ret;
}



bool slotVerifyFwCrc(uint32_t addr)
{
  uint32_t i;
  uint8_t *p_data;
  uint16_t fw_crc;
  flash_tag_t  *p_fw_tag = (flash_tag_t *)addr;


  if (p_fw_tag->magic_number != FLASH_MAGIC_NUMBER)
  {
    logPrintf("fw magic number \t: Fail\r\n");
    return false;
  }

  p_data = (uint8_t *)p_fw_tag->tag_flash_start;

  fw_crc = 0;

  for (i=0; i<p_fw_tag->tag_flash_length; i++)
  {
    utilUpdateCrc(&fw_crc, p_data[i]);
  }

  if (fw_crc == p_fw_tag->tag_flash_crc)
  {
    return true;
  }
  else
  {
    return false;
  }
}

void slotJumpToFw(uint32_t addr)
{
  void (**jump_func)(void) = (void (**)(void))(addr + 4);

  // LCD Off;
  //
  gpioPinWrite(_PIN_GPIO_LCD_BK_EN, _DEF_LOW);

  bspDeInit();

  SysTick->CTRL = 0;
  __set_CONTROL(0x00);
  __set_MSP(*(__IO uint32_t*)addr);
  SCB->VTOR = addr;



  (*jump_func)();
}









void slotCmdif(void)
{
  bool ret = true;
  uint8_t ch;
  uint32_t i;
  flash_tag_t  *p_fw_tag;


  if (cmdifGetParamCnt() == 1)
  {
    if(cmdifHasString("list", 0) == true)
    {
      for (i=0; i<HW_SLOT_MAX_CH; i++)
      {
        p_fw_tag = (flash_tag_t *)QSPI_FW_ADDR(i);

        if (p_fw_tag->magic_number == FLASH_MAGIC_NUMBER)
        {
          cmdifPrintf("%02d 0x%X : %s \t%s %dKB\r\n", i, 0x90000000+0x200000*i, p_fw_tag->name_str, p_fw_tag->version_str, p_fw_tag->tag_flash_length/1024);
        }
        else
        {
          cmdifPrintf("%02d 0x%X : \r\n", i, 0x90000000+0x200000*i);
        }
      }
      cmdifPrintf("\r");
    }
    else
    {
      ret = false;
    }
  }
  else if (cmdifGetParamCnt() == 2)
  {
    ch = (uint8_t)cmdifGetParam(1);

    ch = constrain(ch, 0, HW_SLOT_MAX_CH-1);

    if (cmdifHasString("run", 0) == true)
    {
      slotRunFromFlash(ch);
    }
    else if (cmdifHasString("del", 0) == true)
    {
      slotDelFromFlash(ch);
    }
    else
    {
      ret = false;
    }
  }
  else
  {
    ret = false;
  }


  if (ret == false)
  {
    cmdifPrintf( "slot list\n");
    cmdifPrintf( "slot run 0~%d\n", HW_SLOT_MAX_CH-1);
    cmdifPrintf( "slot del 0~%d\n", HW_SLOT_MAX_CH-1);
  }
}



#endif
