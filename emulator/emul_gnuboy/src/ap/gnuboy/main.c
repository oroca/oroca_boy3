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
	int i;
	char *opt, *arg, *cmd, *s, *rom = 0;


	/* If we have special perms, drop them ASAP! */
	vid_preinit();

	init_exports();

	//s = strdup(argv[0]);


	sys_sanitize(s);
	sys_initpath(s);



	vid_init();
	pcm_init();

	rom = strdup(rom);
	sys_sanitize(rom);
	

  rc_command("set rcpath /gnuboy");
  rc_command("set savedir /gnuboy/saves");


	//loader_init(rom);
	loader_init("/gnuboy/game.gbc");
	
	emu_reset();
	emu_run();

  //rom_unload();
  //vid_close();
  //pcm_close();
}

