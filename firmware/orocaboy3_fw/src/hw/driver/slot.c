/*
 * slot.c
 *
 *  Created on: 2019. 11. 4.
 *      Author: HanCheol Cho
 */




#include "slot.h"
#include "util.h"
#include "cmdif.h"


#ifdef _USE_HW_SLOT



#define SLOT_SIZE   2048*1024



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

  addr_fw  = QSPI_FW_ADDR(slot_index);
  //addr_run = SDRAM_ADDR_FW;
  addr_run = 0x24000000;

  pre_time = millis();
  memcpy((void *)addr_run, (void *)addr_fw, slot_size);

  logPrintf("copy_fw   \t\t: %dms\n", (int)(millis()-pre_time));

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

bool slotRunFromFile(const char *file_name)
{
  return true;
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


  bspDeInit();


  __set_CONTROL(0x00);
  __set_MSP(*(__IO uint32_t*)addr);
  SCB->VTOR = addr;
  //portDISABLE_INTERRUPTS();

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
          cmdifPrintf("%d : %s \t%s %dKB\r\n", i, p_fw_tag->name_str, p_fw_tag->version_str, p_fw_tag->tag_flash_length/1024);
        }
        else
        {
          cmdifPrintf("%d : empty\r\n", i);
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
  }
}



#endif
