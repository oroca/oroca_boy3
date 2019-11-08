/*
 * esp32.c
 *
 *  Created on: 2019. 11. 8.
 *      Author: HanCheol Cho
 */
/*
 * pinMode(15, OUTPUT); // LED
   Serial1.begin(57600, SERIAL_8N1, 5, 17); // Serial Port
 *
 */



#include "esp32.h"
#include "gpio.h"
#include "uart.h"


#ifdef _USE_HW_ESP32


#define ESP_RESET_NONE    0
#define ESP_RESET_BOOT    1
#define ESP_RESET_RUN     2




static bool is_download_mode = false;

static uint8_t req_boot_reset = 0;
static bool req_set_baud   = false;

static uint8_t  uart_down_ch  = _DEF_UART3;
static uint8_t  uart_run_ch   = _DEF_UART4;
static uint8_t  usb_ch        = _DEF_UART2;
static uint32_t uart_baud_req = 115200;

bool esp32Init(void)
{
  //esp32DownloadMode(true);
  esp32Reset(false);

  return true;
}

void esp32Reset(bool boot)
{
  if (boot == true)
  {
    gpioPinWrite(_PIN_GPIO_ESP_RST, _DEF_HIGH);
    gpioPinWrite(_PIN_GPIO_ESP_BOOT,_DEF_LOW);
    delay(1);
    gpioPinWrite(_PIN_GPIO_ESP_RST, _DEF_LOW);
    delay(1);
    gpioPinWrite(_PIN_GPIO_ESP_RST, _DEF_HIGH);
  }
  else
  {
    gpioPinWrite(_PIN_GPIO_ESP_RST, _DEF_HIGH);
    gpioPinWrite(_PIN_GPIO_ESP_BOOT,_DEF_HIGH);
    delay(1);
    gpioPinWrite(_PIN_GPIO_ESP_RST, _DEF_LOW);
    delay(1);
    gpioPinWrite(_PIN_GPIO_ESP_RST, _DEF_HIGH);
  }
}

void esp32SetBaud(uint32_t baud)
{
  req_set_baud = true;
  uart_baud_req = baud;
}


void esp32RequestBoot(uint8_t param)
{

  if (param == 0x01)
  {
    is_download_mode = true;
    req_boot_reset = ESP_RESET_BOOT;
  }

  if (param == 0x02)
  {
    is_download_mode = false;
    req_boot_reset = ESP_RESET_RUN;
  }
}

void esp32DownloadMode(bool enable)
{
  is_download_mode = enable;
}

bool esp32IsDownloadMode(void)
{
  return is_download_mode;
}

void esp32Update(void)
{
  uint8_t data;


  if (req_boot_reset == ESP_RESET_BOOT)
  {
    req_boot_reset = ESP_RESET_NONE;
    esp32Reset(true);
  }
  if (req_boot_reset == ESP_RESET_RUN)
  {
    req_boot_reset = ESP_RESET_NONE;
    esp32Reset(false);
  }

  if (req_set_baud == true)
  {
    req_set_baud = false;

    if (is_download_mode == true)
    {
      if (uartGetBaud(uart_down_ch) != uart_baud_req)
      {
        uartOpen(uart_down_ch, uart_baud_req);
      }
    }
    else
    {
      if (uartGetBaud(uart_run_ch) != uart_baud_req)
      {
        uartOpen(uart_run_ch, uart_baud_req);
      }
    }
  }


  if (is_download_mode == true)
  {
    while(uartAvailable(uart_down_ch) > 0)
    {
      data = uartRead(uart_down_ch);
      uartPutch(usb_ch, data);
    }

    while(uartAvailable(usb_ch) > 0)
    {
      data = uartRead(usb_ch);
      uartPutch(uart_down_ch, data);
    }
  }
  else
  {
    while(uartAvailable(uart_run_ch) > 0)
    {
      data = uartRead(uart_run_ch);
      uartPutch(usb_ch, data);
    }

    while(uartAvailable(usb_ch) > 0)
    {
      data = uartRead(usb_ch);
      uartPutch(uart_run_ch, data);
    }
  }
}

#endif
