#include <stddef.h>
#include <string.h>
#include <stdlib.h>

#include "def.h"
#include "hw.h"
#include "ff.h"


extern void lcdClear(uint32_t rgb_code);
extern bool lcdDrawAvailable(void);
extern void lcdRequestDraw(void);


#ifdef __cplusplus
extern "C" {
#endif


extern void *memMalloc(uint32_t size);
extern void  memFree(void *ptr);
extern void *memCalloc(size_t nmemb, size_t size);
extern void *memRealloc(void *ptr, size_t size);

extern bool buttonGetPressed(uint8_t ch);
extern uint32_t adcRead12(uint8_t ch);

extern void drvLcdCopyLineBuffer(uint16_t x_pos, uint16_t y_pos, uint8_t *p_data, uint32_t length);
extern void drvLcdBufferClear(uint32_t rgb_code);
extern uint8_t tsIsDetected(void);
extern bool drvAudioGetReady(void);
extern bool audioIsPlaying(void);
extern err_code_t audioPlay(uint16_t* p_buf, uint32_t size);
extern err_code_t audioStop(void);
extern err_code_t audioSetVol(uint8_t volume);

#ifdef __cplusplus
}
#endif
