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


#ifdef _USE_HW_SLOT



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

  // LCD Off;
  //
  gpioPinWrite(_PIN_GPIO_LCD_BK_EN, _DEF_LOW);

  bspDeInit();

  //portDISABLE_INTERRUPTS();
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
