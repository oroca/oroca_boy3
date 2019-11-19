#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <signal.h>

#include "gnuboy.h"
#include "input.h"
#include "rc.h"
#include "loader.h"
#include "hw.h"
#include "Version"
#include "hw_def.h"





void doevents()
{
  ev_poll();

  pad_set(PAD_UP,    buttonGetPressed(_DEF_HW_BTN_UP));
  pad_set(PAD_RIGHT, buttonGetPressed(_DEF_HW_BTN_RIGHT));
  pad_set(PAD_DOWN,  buttonGetPressed(_DEF_HW_BTN_DOWN));
  pad_set(PAD_LEFT,  buttonGetPressed(_DEF_HW_BTN_LEFT));

  pad_set(PAD_SELECT, buttonGetPressed(_DEF_HW_BTN_SELECT));
  pad_set(PAD_START,  buttonGetPressed(_DEF_HW_BTN_START));

  pad_set(PAD_A, buttonGetPressed(_DEF_HW_BTN_A));
  pad_set(PAD_B, buttonGetPressed(_DEF_HW_BTN_B));
}




static void shutdown()
{
	vid_close();
	pcm_close();
}

void die(char *fmt, ...)
{
	va_list ap;

	printf("die\n");

	va_start(ap, fmt);
	vfprintf(stderr, fmt, ap);
	va_end(ap);
	exit(1);
}


void gnuboyMain(void)
{
	vid_preinit();

	init_exports();


	vid_init();
	pcm_init();

	
  rc_command("set rcpath /gnuboy");
  rc_command("set savedir /gnuboy/saves");


	//loader_init(rom);
	loader_init("/gnuboy/game.gb");
	
	emu_reset();
	emu_run();

  //rom_unload();
  //vid_close();
  //pcm_close();
}

