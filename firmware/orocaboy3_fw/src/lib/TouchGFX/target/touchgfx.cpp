#include <touchgfx/hal/HAL.hpp>
#include <touchgfx/hal/BoardConfiguration.hpp>
#include <touchgfx/canvas_widget_renderer/CanvasWidgetRenderer.hpp>

using namespace touchgfx;

/* Kernel includes. */
#include "hw.h"




/**
 * Define the FreeRTOS task priorities and stack sizes
 */
#define configGUI_TASK_PRIORITY                 ( tskIDLE_PRIORITY + 3 )

#define configGUI_TASK_STK_SIZE                 ( 4096 )

#define CANVAS_BUFFER_SIZE (3600)

static void GUITask(void* params)
{
  touchgfx::HAL::getInstance()->taskEntry();
}

int touchgfxMain(void)
{
  hw_init();
  touchgfx_init();


  /**
   * IMPORTANT NOTICE!
   *
   * The GUI task stack size might need to be increased if creating a new application.
   * The current value of 950 in this template is enough for the examples distributed with
   * TouchGFX, but is often insufficient for larger applications.
   * For reference, the touchgfx_2014 demo uses a stacksize of 2000.
   * If you experience inexplicable hard faults with your custom application, try increasing the
   * stack size.
   *
   * Also note that in FreeRTOS, stack size is in words. So a stack size of 950 actually consumes
   * 3800 bytes of memory. The configTOTAL_HEAP_SIZE define in platform/os/FreeRTOSConfig.h configures
   * the total amount of memory available for FreeRTOS, including task stacks. This value is expressed
   * in BYTES, not words. So, with a stack size of 950, your total heap should be at least 4K.
   */

  // Setup the CanvasWidgetRenderer. ONLY needed if you use CanvasWidgets
  // in your application. The CANVAS_BUFFER_SIZE can be adjusted to match
  // your needs in performance vs. RAM usage. Read more on this in the
  // TouchGFX Manual.
  static uint8_t canvasBuffer[CANVAS_BUFFER_SIZE];
  CanvasWidgetRenderer::setupBuffer(canvasBuffer, CANVAS_BUFFER_SIZE);

  xTaskCreate(GUITask, "GUITask",
              configGUI_TASK_STK_SIZE,
              NULL,
              configGUI_TASK_PRIORITY,
              NULL);

  gpioPinWrite(_PIN_GPIO_LCD_BK_EN, _DEF_HIGH);

  return 0;
}

void touchgfxDisable(void)
{
  touchgfx::HAL::getInstance()->disableInterrupts();
}
