/*
 * ap.c
 *
 *  Created on: 2018. 8. 25.
 *      Author: Baram
 */




#include "ap.h"
#include "util.h"
#include <unistd.h>



#define FLASH_TAG_SIZE      0x400



flash_tag_t    fw_tag;
flash_ver_t    fw_ver;



int32_t getFileSize(char *file_name);



void apInit(void)
{

}

void apMain(int argc, char *argv[])
{
  bool ret;
  char *port_name;
  char *file_name;
  char *file_type;
  int baud;
  int file_size;
  uint8_t  block_buf[FLASH_TX_BLOCK_LENGTH];
  uint8_t  version_str[128];
  uint8_t  board_str[128];
  static FILE *fp;
  uint8_t errcode;
  uint32_t time_pre;
  bool flash_write_done;
  uint32_t addr;
  size_t readbytes;
  uint32_t percent;
  uint32_t len;
  uint16_t flash_crc;
  uint32_t pre_percent = 100;
  uint32_t file_addr = 0;
  uint32_t start_addr = 0;
  uint32_t file_run = 0;

  uint32_t flash_begin;
  uint32_t flash_end;

  setbuf(stdout, NULL);


  if (argc != 7)
  {
    printf("orocaboy3_loader.exe com1 115200 type[fw:image] 0x8040000 file_name run[0:1]\n");
    return;
  }

  port_name = (char *)argv[ 1 ];
  baud      = strtol( argv[ 2 ], NULL, 10 );
  file_type = (char *)argv[ 3 ];
  start_addr= (uint32_t)strtol( argv[ 4 ], NULL, 0 );
  file_name = (char *)argv[ 5 ];
  file_run  = (uint32_t)strtol( argv[ 6 ], NULL, 0 );


  file_size = getFileSize(file_name);

  file_addr = start_addr + FLASH_TAG_SIZE;

  printf("downloader...\n\n");

  printf("file open\n");
  printf("  file name \t: %s \n", file_name);
  printf("  file size \t: %d B\n", file_size);
  printf("  file addr \t: 0x%X\n", file_addr);
  printf("  file ver  \t: %s\n", (char *)fw_ver.version_str);
  printf("  file board\t: %s\n", (char *)fw_ver.board_str);
  printf("  file magic\t: 0x%X\n", fw_ver.magic_number);


  if (fw_ver.magic_number != 0x5555AAAA)
  {
    printf("MagicNumber Fail, Wrong Image.\n");
    return;
  }

  printf("port open \t: %s\n", port_name);

  ret = bootInit(_DEF_UART2, port_name, baud);
  if (ret == true)
  {
    printf("bootInit \t: OK\n");
  }
  else
  {
    printf("bootInit  Fail\n");
    return;
  }


  fw_tag.magic_number = 0x5555AAAA;
  fw_tag.flash_start  = file_addr;
  fw_tag.flash_length = file_size;


  flash_begin = start_addr;
  flash_end   = start_addr + file_size + FLASH_TAG_SIZE;
  flash_crc   = 0;

  while(1)
  {
    //-- 보드 이름 확인
    //
    errcode = bootCmdReadBootName(board_str);
    if (errcode == OK)
    {
      printf("boot name \t: %s\n", board_str);
    }
    else
    {
      printf("bootCmdReadBoardName faild \n");
      break;
    }


    //-- 버전 확인
    //
    errcode = bootCmdReadBootVersion(version_str);
    if (errcode == OK)
    {
      printf("boot version \t: %s\n", &version_str[0]);
    }
    else
    {
      printf("bootCmdReadBootVersion faild \n");
      break;
    }

    errcode = bootCmdReadFirmVersion(version_str);
    if (errcode == OK)
    {
      printf("firm version \t: %s\n", &version_str[0]);
    }
    else
    {
      printf("bootCmdReadFirmVersion faild \n");
      break;
    }

    /*
    if (strcmp((const char *)board_str, (const char *)info.board_str) != 0)
    {
      printf("board name is not equal. \n");
      break;
    }
    */

    //-- Flash Erase
    //
    printf("erase fw...\n");
    time_pre = millis();
    errcode = bootCmdFlashErase(flash_begin, flash_end - flash_begin);
    if (errcode == OK)
    {
      printf("erase fw ret \t: OK (%d ms)\n", millis()-time_pre);
    }
    else
    {
      printf("bootCmdFlashEraseFw faild : %d\n", errcode);
      break;
    }


    //-- Flash Write
    //
    if( ( fp = fopen( file_name, "rb" ) ) == NULL )
    {
      fprintf( stderr, "Unable to open %s\n", file_name );
      exit(1);
    }


    flash_write_done = false;
    addr = file_addr;
    time_pre = millis();
    while(1)
    {
      if( !feof( fp ) )
      {
        readbytes = fread( block_buf, 1, FLASH_TX_BLOCK_LENGTH, fp );
        percent = (addr+readbytes-file_addr)*100/file_size;

        if ((percent%10) == 0 && percent != pre_percent)
        {
          if (percent == 0)
          {
            printf("flash fw \t: %d%% ", percent);
          }
          else
          {
            printf("%d%% ", percent);
          }
          pre_percent = percent;
        }
      }
      else
      {
        break;
      }

      if( readbytes == 0 )
      {
        break;
      }
      else
      {
        len = readbytes;
      }


      for (int retry=0; retry<3; retry++)
      {
        errcode = bootCmdFlashWrite(addr, block_buf, len);
        if( errcode == OK )
        {
          for (int i=0; i<len; i++)
          {
            utilUpdateCrc(&flash_crc, block_buf[i]);
          }
          break;
        }
        else
        {
          printf("bootCmdFlashWrite Fail \t: %d \n", errcode);
        }
      }
      if( errcode != OK )
      {
        break;
      }

      addr += len;

      if ((addr-file_addr) == file_size)
      {
        flash_write_done = true;
        break;
      }
    }
    fclose(fp);

    printf("\r\n");

    if( errcode != OK || flash_write_done == false )
    {
      printf("flash fw fail \t: %d\n", errcode);
      break;
    }
    else
    {
      printf("flash fw ret \t: OK (%d ms) \n", millis()-time_pre);
    }


    if( errcode != OK || flash_write_done == false )
    {
      printf("Download \t: Fail\n");
      return;
    }
    else
    {
      printf("Download \t: OK\n");


      fw_tag.flash_crc = flash_crc;

      printf("Tag CRC \t: 0x%X\r\n", fw_tag.flash_crc);

      errcode = bootCmdFlashWrite(flash_begin, (uint8_t *)&fw_tag, sizeof(flash_tag_t));
      if( errcode == OK )
      {
        printf("Tag Write \t: OK\n");
      }
      else
      {
        printf("Tag Write Fail \t: %d \n", errcode);
        break;
      }
    }


    errcode = bootCmdJumpToFw();
    if (errcode == OK)
    {
        printf("jump to fw \t: OK\n");
    }
    else
    {
      printf("jump to fw fail : %d\n", errcode);
    }

    break;
  }


  uartClose(_DEF_UART2);
}

int32_t getFileSize(char *file_name)
{
  int32_t ret = -1;
  static FILE *fp;
  uint8_t block_buf[FLASH_TAG_SIZE];

  if( ( fp = fopen( file_name, "rb" ) ) == NULL )
  {
    fprintf( stderr, "Unable to open %s\n", file_name );
    exit( 1 );
  }
  else
  {
    fread( block_buf, 1, FLASH_TAG_SIZE, fp ); // boot
    fread( (void *)&fw_ver, 1, sizeof(flash_ver_t), fp );

    fseek( fp, 0, SEEK_END );
    ret = ftell( fp );
    fseek( fp, 0, SEEK_SET );
    fclose(fp);
  }

  return ret;
}
