/*
 * LibBoy.h
 *
 *  Created on: 2019. 11. 7.
 *      Author: HanCheol Cho
 */

#ifndef SRC_AP_FMSX_MAIN_PORT_LIBBOY_H_
#define SRC_AP_FMSX_MAIN_PORT_LIBBOY_H_




#ifdef __cplusplus
extern "C" {
#endif

#define SND_CHANNELS    16     /* Number of sound channels   */
#define SND_BITS        8
#define SND_BUFSIZE     (1<<SND_BITS)


/** PIXEL() **************************************************/
/** Unix may use multiple pixel formats.                    **/
/*************************************************************/
#define PIXEL(R,G,B)  (pixel)(((31*(R)/255)<<11)|((63*(G)/255)<<5)|(31*(B)/255))
#define PIXEL2MONO(P) (522*(((P)&31)+(((P)>>5)&63)+(((P)>>11)&31))>>8)
#define RMASK 0xF800
#define GMASK 0x07E0
#define BMASK 0x001F



//////////////////// VIDEO /////////////////////////////

#define WIDTH           256
#define HEIGHT          216 // normally the msx has 212 lines in PAL, 4 more to send the bytes faster over SPI to the display

#define WIDTH_OVERLAY   320
#define HEIGHT_OVERLAY  240

#define MSX_DISPLAY_X   ((WIDTH_OVERLAY-WIDTH)/2)
#define MSX_DISPLAY_Y   ((HEIGHT_OVERLAY-212)/2)

#define Black           0x0000


int InitBoy(const char *Title,int Width,int Height);
void TrashBoy(void);

unsigned int InitAudio(unsigned int Rate,unsigned int Latency);
void TrashAudio(void);
int PauseAudio(int Switch);


#ifdef __cplusplus
}
#endif



#endif /* SRC_AP_FMSX_MAIN_PORT_LIBBOY_H_ */
