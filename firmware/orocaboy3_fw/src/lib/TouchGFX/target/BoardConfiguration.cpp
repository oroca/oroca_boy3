
#include <common/TouchGFXInit.hpp>
#include <touchgfx/hal/BoardConfiguration.hpp>
#include <touchgfx/hal/GPIO.hpp>
#include <platform/driver/lcd/LCD16bpp.hpp>
#include <platform/driver/lcd/LCD24bpp.hpp>

#include <STM32H7DMA.hpp>
#include <STM32H7HAL.hpp>

#include <STM32H750TouchController.hpp>
#include <STM32H7Instrumentation.hpp>
#include <BUTTON.hpp>

/***********************************************************
 ******         24 Bits Per Pixel Support            *******
 ***********************************************************
 *
 * The default bit depth of the framebuffer is 16bpp. If you want 24bpp support, define the symbol "USE_BPP" with a value
 * of "24", e.g. "USE_BPP=24". This symbol affects the following:
 *
 * 1. Type of TouchGFX LCD (16bpp vs 24bpp)
 * 2. Bit depth of the framebuffer(s)
 * 3. TFT controller configuration.
 *
 * WARNING: Remember to modify your image formats accordingly in app/config/. Please see the following knowledgebase article
 * for further details on how to choose and configure the appropriate image formats for your application:
 *
 * https://touchgfx.zendesk.com/hc/en-us/articles/206725849
 */

extern "C" {
#include "hw.h"

//LTDC_HandleTypeDef hltdc;
DMA2D_HandleTypeDef hdma2d;
}

#if !defined(USE_BPP) || USE_BPP==16
#define FRAMEBUFFER_SIZE  (ltdcWidth() * ltdcHeight() * 2)
#elif USE_BPP==24
#define FRAMEBUFFER_SIZE  (ltdcWidth() * ltdcHeight() * 3)
#else
#error Unknown USE_BPP
#endif

#define FB_BUFFER_NBR     3


static uint32_t frameBuf0 = ltdcGetBufferAddr(0);

extern "C" uint32_t LCD_GetXSize()
{
    return ltdcWidth();
}

extern "C" uint32_t LCD_GetYSize()
{
    return ltdcHeight();
}

/**
  * @brief  Initializes the LCD layers.
  * @param  LayerIndex: Layer foreground or background
  * @param  FB_Address: Layer frame buffer
  * @retval None
  */
static void LCD_LayerDefaultInit(uint16_t LayerIndex, uint32_t Address)
{
  ltdcLayerInit(LayerIndex, Address);
}


void OneTimeInitializeDefaultLayer()
{
  static bool intialized = false;
  if (!intialized)
  {
    intialized = true;
    LCD_LayerDefaultInit(0, frameBuf0);
  }
}

static uint8_t LCD_Init(void)
{

  return 0;
}

namespace touchgfx
{

void hw_init()
{
    __HAL_RCC_CRC_CLK_ENABLE();

    /* Disable FMC Bank1 to avoid speculative/cache accesses */
    //FMC_Bank1_R->BTCR[0] &= ~FMC_BCRx_MBKEN;

    GPIO::init();

    /* Initialize display */
    LCD_Init();
}

STM32H7DMA dma;
STM32H750TouchController tc;
STM32H7Instrumentation mcuInstr;
ButtonSampler btnCtrl;

#if !defined(USE_BPP) || USE_BPP==16
static LCD16bpp display;
static uint16_t bitDepth = 16;
#elif USE_BPP==24
static LCD24bpp display;
static uint16_t bitDepth = 24;
#else
#error Unknown USE_BPP
#endif


void touchgfx_init()
{
    uint16_t dispWidth  = LCD_GetXSize();
    uint16_t dispHeight = LCD_GetYSize();

    LCD_LayerDefaultInit(0, frameBuf0);


    HAL& hal = touchgfx_generic_init<STM32H7HAL>(dma, display, tc, dispWidth, dispHeight, 0, 0, 0);

    hal.setFrameBufferStartAddress((uint16_t*)frameBuf0, bitDepth, true, true);

    hal.setTouchSampleRate(1);
    hal.setFingerSize(1);
    hal.setButtonController(&btnCtrl);


    // By default frame rate compensation is off.
    // Enable frame rate compensation to smooth out animations in case there is periodic slow frame rates.
    hal.setFrameRateCompensation(false);

    // This platform can handle simultaneous DMA and TFT accesses to SDRAM, so disable lock to increase performance.
    hal.lockDMAToFrontPorch(false);

    mcuInstr.init();

    //Set MCU instrumentation and Load calculation
    hal.setMCUInstrumentation(&mcuInstr);
    hal.enableMCULoadCalculation(true);
}
}
