/*
 * ap.cpp
 *
 *  Created on: 2019. 6. 14.
 *      Author: HanCheol Cho
 */




#include "ap.h"
#include "lvgl.h"
#include "lvgl/porting/lv_port_disp.h"
#include "lvgl/porting/lv_port_indev.h"



extern uint32_t _flash_tag_addr;
extern uint32_t _flash_fw_addr;



__attribute__((section(".tag"))) flash_tag_t fw_tag =
    {
     // fw info
     //
     0xAAAA5555,        // magic_number
     "V191201R1",       // version_str
     "OROCABOY3",       // board_str
     "LittlevGL",       // name
     __DATE__,
     __TIME__,
     (uint32_t)&_flash_tag_addr,
     (uint32_t)&_flash_fw_addr,


     // tag info
     //
    };




extern "C"
{
void gnuboyMain (void);

}

static void threadEmul(void const *argument);


void apInit(void)
{
  uartOpen(_DEF_UART1, 57600);
  uartOpen(_DEF_UART2, 57600);
  cmdifOpen(_DEF_UART1, 57600);



  osThreadDef(threadEmul, threadEmul, _HW_DEF_RTOS_THREAD_PRI_EMUL, 0, _HW_DEF_RTOS_THREAD_MEM_EMUL*3);
  if (osThreadCreate(osThread(threadEmul), NULL) != NULL)
  {
    logPrintf("threadEmul \t\t: OK\r\n");
  }
  else
  {
    logPrintf("threadEmul \t\t: Fail\r\n");
    while(1);
  }

}

void apMain(void)
{
  uint32_t pre_time;

  while(1)
  {
    cmdifMain();

    if (millis()-pre_time >= 500)
    {
      pre_time = millis();
      ledToggle(_DEF_LED1);
    }
    osThreadYield();

    batteryUpdate();
    joypadUpdate();
    osdUpdate();
  }
}


void lv_test_stress_1(void);
lv_obj_t * terminal_create(void);
lv_group_t *lv_test_group_1(void);

static void threadEmul(void const *argument)
{
  UNUSED(argument);


  lv_init();
  lv_port_disp_init();
  lv_port_indev_init();

  lv_obj_t * btn = lv_btn_create(lv_scr_act(), NULL);     /*Add a button the current screen*/
  lv_obj_set_pos(btn, 10, 10);                            /*Set its position*/
  lv_obj_set_size(btn, 100, 50);                          /*Set its size*/
  //lv_obj_set_event_cb(btn, btn_event_cb);                 /*Assign a callback to the button*/

  lv_obj_t * label = lv_label_create(btn, NULL);          /*Add a label to the button*/
  lv_label_set_text(label, "Button");                     /*Set the labels text*/

  lv_test_stress_1();
  //lv_test_group_1();

  while(1)
  {
    lv_task_handler();
    delay(1);
  }
}



#if 1

static void obj_mem_leak_tester(lv_task_t *);
static void alloc_free_tester(lv_task_t *);
static void mem_monitor(lv_task_t *);

/**********************
 *  STATIC VARIABLES
 **********************/
static lv_obj_t * all_obj_h;
static lv_obj_t * alloc_label;
static lv_obj_t * alloc_ta;
static const char * mbox_btns[] = {"Ok", "Cancel", ""};


/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create and delete a lot of objects and animations.
 */
void lv_test_stress_1(void)
{
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    lv_task_create(obj_mem_leak_tester, 200, LV_TASK_PRIO_MID, NULL);
    lv_task_create(mem_monitor, 500, LV_TASK_PRIO_MID, NULL);
    lv_task_create(alloc_free_tester, 100, LV_TASK_PRIO_MID, NULL);

    /* Holder for all object types */
    all_obj_h = lv_obj_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_set_size(all_obj_h, hres / 2, vres);
    lv_obj_set_style(all_obj_h, &lv_style_pretty);

    alloc_ta = lv_ta_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_align(alloc_ta, all_obj_h, LV_ALIGN_OUT_RIGHT_TOP, 10, 10);
    lv_obj_set_height(alloc_ta, vres / 4);

    alloc_label = lv_label_create(lv_disp_get_scr_act(NULL), NULL);
    lv_obj_align(alloc_label, alloc_ta, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 20);


    /*Add style animation to the ta*/
    static lv_style_t ta_style;
    lv_style_copy(&ta_style, &lv_style_pretty);
    lv_ta_set_style(alloc_ta, LV_TA_STYLE_BG, &ta_style);


    lv_anim_t sa;
    lv_style_anim_init(&sa);
    lv_style_anim_set_styles(&sa, &ta_style, &lv_style_pretty, &lv_style_pretty_color);
    lv_style_anim_set_time(&sa, 500, 500);
    lv_style_anim_set_playback(&sa, 500);
    lv_style_anim_set_repeat(&sa, 500);
    lv_style_anim_create(&sa);
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

static void mem_monitor(lv_task_t * param)
{
    (void) param;    /*Unused*/

#if LV_EX_PRINTF
    lv_mem_monitor_t mon;
    lv_mem_monitor(&mon);
    printf("used: %6d (%3d %%), frag: %3d %%, biggest free: %6d\n", (int)mon.total_size - mon.free_size,
           mon.used_pct,
           mon.frag_pct,
           (int)mon.free_biggest_size);
#endif
}

static void obj_mem_leak_tester(lv_task_t * param)
{
    (void) param;    /*Unused*/
    static const lv_color_t needle_colors[1] = {LV_COLOR_RED};  /*For gauge*/

    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    static int16_t state = 0;
    lv_obj_t * obj;
    static lv_obj_t * page;

    lv_anim_t a;
    a.path_cb = lv_anim_path_linear;
    a.ready_cb = NULL;
    a.act_time = 0;
    a.time = 500;
    a.playback = 0;
    a.repeat = 0;
    a.playback_pause = 100;
    a.repeat_pause = 100;

    switch(state) {
        case 0:
            obj = lv_obj_create(all_obj_h, NULL);
            lv_obj_set_pos(obj, 10, 5);
            a.playback = 1;
            a.repeat = 1;
            a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_x;
            a.var = obj;
            a.start = 10 ;
            a.end = 100 ;
            lv_anim_create(&a);
            break;
        case 1:
            obj = lv_btn_create(all_obj_h, NULL);
            lv_obj_set_pos(obj, 60, 5);
            a.playback = 0;
            a.repeat = 1;
            a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_x;
            a.var = obj;
            a.start = 150 ;
            a.end = 200 ;
            lv_anim_create(&a);
            obj = lv_label_create(obj, NULL);
            lv_label_set_text(obj, "Button");
            break;
        case 2:     /*Page tests container too*/
            page = lv_page_create(all_obj_h, NULL);
            lv_obj_set_pos(page, 10, 60);
            lv_obj_set_size(page, lv_obj_get_width(all_obj_h) - (20), 3 * vres / 4);
            lv_page_set_scrl_layout(page, LV_LAYOUT_PRETTY);
            break;
        case 3:
            obj = lv_label_create(page, NULL);
            lv_label_set_text(obj, "Label");
            break;

        case 5:
            obj = lv_cb_create(page, NULL);
            lv_cb_set_text(obj, "Check box");
            break;
        case 7:             /*Switch tests bar and slider memory leak too*/
            obj = lv_sw_create(page, NULL);
            lv_sw_on(obj, LV_ANIM_OFF);
            break;
        case 8:     /*Kb tests butm too*/
            obj = lv_kb_create(all_obj_h, NULL);
            lv_obj_set_size(obj, hres / 3, vres / 5);
            lv_obj_set_pos(obj, 30, 90);
            a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_y;
            a.var = obj;
            a.start = LV_VER_RES ;
            a.end = lv_obj_get_y(obj);
            a.time = 200;
            lv_anim_create(&a);
            break;

        case 9: /*Roller test ddlist too*/
            obj = lv_roller_create(page, NULL);
            lv_roller_set_options(obj, "One\nTwo\nThree", false);
            lv_roller_set_anim_time(obj, 300);
            lv_roller_set_selected(obj, 2, true);
            break;
        case 10: /*Gauge test lmeter too*/
            obj = lv_gauge_create(page, NULL);
            lv_gauge_set_needle_count(obj, 1, needle_colors);
            lv_gauge_set_value(obj, 1, 30);
            break;
        case 15: /*Wait a little to see the previous results*/
            obj = lv_list_create(all_obj_h, NULL);
            lv_obj_set_pos(obj, 40, 50);
            lv_list_add_btn(obj, LV_SYMBOL_OK, "List 1");
            lv_list_add_btn(obj, LV_SYMBOL_OK, "List 2");
            lv_list_add_btn(obj, LV_SYMBOL_OK, "List 3");
            lv_list_add_btn(obj, LV_SYMBOL_OK, "List 4");
            lv_list_add_btn(obj, LV_SYMBOL_OK, "List 5");
            lv_list_add_btn(obj, LV_SYMBOL_OK, "List 6");
            a.exec_cb = (lv_anim_exec_xcb_t)lv_obj_set_height;
            a.var = obj;
            a.start = 0;
            a.end = lv_obj_get_height(obj);
            a.time = 5000;
            lv_anim_create(&a);
            break;
        case 16:
            obj = lv_win_create(all_obj_h, NULL);
            lv_win_add_btn(obj, LV_SYMBOL_CLOSE);
            lv_win_add_btn(obj, LV_SYMBOL_OK);
            lv_win_set_style(obj, LV_WIN_STYLE_BG, &lv_style_pretty);
            lv_obj_set_size(obj, hres / 3, vres / 3);
            lv_obj_set_pos(obj, 20, 100);
            break;
        case 17:
            obj = lv_tabview_create(all_obj_h, NULL);
            lv_tabview_add_tab(obj, "tab1");
            lv_tabview_add_tab(obj, "tab2");
            lv_tabview_add_tab(obj, "tab3");
            lv_tabview_set_style(obj, LV_TABVIEW_STYLE_BG, &lv_style_pretty);
            lv_obj_set_size(obj, hres / 3, vres / 3);
            lv_obj_set_pos(obj, 50, 140);
            break;
        case 18:
            obj = lv_mbox_create(all_obj_h, NULL);
            lv_obj_set_width(obj, hres / 4);
            lv_mbox_set_text(obj, "message");
            lv_mbox_add_btns(obj, mbox_btns); /*Set 3 times to test btnm add memory leasks*/
            lv_mbox_add_btns(obj, mbox_btns);
            lv_mbox_add_btns(obj, mbox_btns);
            lv_mbox_set_anim_time(obj, 300);
            lv_mbox_start_auto_close(obj, 500);
            break;

        /*Delete object from the page*/
        case 20:
            obj = lv_obj_get_child(lv_page_get_scrl(page), NULL);
            if(obj) lv_obj_del(obj);
            else  {
                state = 24;
            }
            break;
        case 21:
            obj = lv_obj_get_child_back(lv_page_get_scrl(page), NULL);       /*Delete from the end too to be more random*/
            if(obj) {
                lv_obj_del(obj);
                state -= 2;     /*Go back to delete state*/
            } else {
                state = 24;
            }
            break;
        /*Remove object from 'all_obj_h'*/
        case 25:
            obj = lv_obj_get_child(all_obj_h, NULL);
            if(obj) lv_obj_del(obj);
            else state = 29;
            break;
        case 26:
            obj = lv_obj_get_child_back(all_obj_h, NULL);       /*Delete from the end too to be more random*/
            if(obj) {
                lv_obj_del(obj);
                state -= 2;     /*Go back to delete state*/
            } else state = 29;
            break;

        case 30:
            state = -1;
            break;
        default:
            break;
    }

    state ++;
}


/**
 * Test alloc and free by settings the text of a label and instering text to a text area
 */
static void alloc_free_tester(lv_task_t * param)
{
    (void) param;    /*Unused*/

    static int16_t state = 0;

    switch(state) {
        case 0:
            lv_label_set_text(alloc_label, "a");
            break;

        case 1:
            lv_ta_set_text(alloc_ta, "Initilal");
            break;

        case 2:
            lv_label_set_text(alloc_label, "long long long\nlong long long");
            break;

        case 3:
            lv_label_set_text(alloc_label, "b");
            break;

        case 6:    /*Wait to be random*/
            lv_ta_set_cursor_pos(alloc_ta, 0);
            lv_ta_add_text(alloc_ta, "aaaaaaaaaaaaaaaaaaaaaaaaaaa!\n");
            break;

        case 7:
            lv_label_set_text(alloc_label, "medium");
            break;

        case 8:
            lv_label_set_text(alloc_label, "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n"
                              "very very long very very long\n");
            break;

        case 10:       /*Wait to be random*/
            lv_label_set_text(alloc_label, "some text");
            break;

        case 11:
            lv_ta_set_cursor_pos(alloc_ta, 20);
            lv_ta_del_char(alloc_ta);
            lv_ta_del_char(alloc_ta);
            lv_ta_del_char(alloc_ta);
            lv_ta_del_char(alloc_ta);
            lv_ta_del_char(alloc_ta);
            lv_ta_del_char(alloc_ta);
            lv_ta_del_char(alloc_ta);
            lv_ta_del_char(alloc_ta);
            lv_ta_del_char(alloc_ta);
            lv_ta_del_char(alloc_ta);
            lv_ta_del_char(alloc_ta);
            break;

        case 12:
            lv_label_set_text(alloc_label, LV_SYMBOL_DIRECTORY);
            break;

        case 16:     /*Wait to be random*/
            lv_label_set_text(alloc_label, "A dummy sentence nothing else");
            break;

        case 17:
            lv_ta_set_cursor_pos(alloc_ta, 5);
            lv_ta_add_char(alloc_ta, 'A');
            lv_ta_add_char(alloc_ta, 'A');
            lv_ta_add_char(alloc_ta, 'A');
            lv_ta_add_char(alloc_ta, 'A');
            lv_ta_add_char(alloc_ta, 'A');
            break;

        case 18:
            lv_label_set_text(alloc_label, "ok");
            break;

        case 20:   /*Wait to be random*/
            lv_label_set_text(alloc_label, LV_SYMBOL_FILE);
            break;
        case 21:
            lv_label_set_text(alloc_label, "c");
            break;
        case 22:
            lv_ta_set_cursor_pos(alloc_ta, 20);
            lv_ta_add_text(alloc_ta, "Ú");
            lv_ta_add_text(alloc_ta, "Ú");
            lv_ta_add_text(alloc_ta, "Ú");
            lv_ta_add_text(alloc_ta, "Ú");
            lv_ta_add_text(alloc_ta, "Ú");
            break;

        case 23:
            lv_label_set_text(alloc_label, "ÁaÁaaÁÁaaaÁÁÁaaaaÁÁÁÁ");
            break;

        case 25:
            lv_ta_set_text(alloc_ta, "");
            break;

        case 26:
            lv_label_set_text(alloc_label, "");
            break;

        case 28:
            state = -1 ;
            break;
        default:
            break;
    }

    state ++;
}
#else



/*To emulate some keys on the window header*/
static bool win_btn_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data);
static void win_btn_event_handler(lv_obj_t * btn, lv_event_t event);

static void group_focus_cb(lv_group_t * group);

static void general_event_handler(lv_obj_t * obj, lv_event_t event);

/**********************
 *  STATIC VARIABLES
 **********************/
static uint32_t last_key;
static lv_indev_state_t last_key_state = LV_INDEV_STATE_REL;
static lv_group_t * g;
static lv_obj_t * win;

/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

/**
 * Create base groups to test their functionalities
 */
lv_group_t *lv_test_group_1(void)
{
    lv_coord_t hres = lv_disp_get_hor_res(NULL);
    lv_coord_t vres = lv_disp_get_ver_res(NULL);

    g = lv_group_create();
    lv_group_set_focus_cb(g, group_focus_cb);

    /*A keyboard will be simulated*/
    lv_indev_drv_t sim_kb_drv;
    lv_indev_drv_init(&sim_kb_drv);
    sim_kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    sim_kb_drv.read_cb = win_btn_read;
    lv_indev_t * win_kb_indev = lv_indev_drv_register(&sim_kb_drv);
    lv_indev_set_group(win_kb_indev, g);

#if LV_EX_KEYBOARD
    lv_indev_drv_t real_kb_drv;
    lv_indev_drv_init(&real_kb_drv);
    real_kb_drv.type = LV_INDEV_TYPE_KEYPAD;
    real_kb_drv.read_cb = keyboard_read;
    lv_indev_t * real_kb_indev = lv_indev_drv_register(&real_kb_drv);
    lv_indev_set_group(real_kb_indev, g);
#endif

#if LV_EX_MOUSEWHEEL
    lv_indev_drv_t enc_drv;
    lv_indev_drv_init(&enc_drv);
    enc_drv.type = LV_INDEV_TYPE_ENCODER;
    enc_drv.read_cb = mousewheel_read;
    lv_indev_t * enc_indev = lv_indev_drv_register(&enc_drv);
    lv_indev_set_group(enc_indev, g);
#endif

    /*Create a window to hold all the objects*/
    static lv_style_t win_style;
    lv_style_copy(&win_style, &lv_style_transp);
    win_style.body.padding.left= LV_DPI / 6;
    win_style.body.padding.right = LV_DPI / 6;
    win_style.body.padding.top = LV_DPI / 6;
    win_style.body.padding.bottom = LV_DPI / 6;
    win_style.body.padding.inner = LV_DPI / 6;

    win = lv_win_create(lv_disp_get_scr_act(NULL), NULL);
    lv_win_set_title(win, "Group test");
    lv_page_set_scrl_layout(lv_win_get_content(win), LV_LAYOUT_PRETTY);
    lv_win_set_style(win, LV_WIN_STYLE_CONTENT, &win_style);
    lv_group_add_obj(g, lv_win_get_content(win));
    lv_obj_set_event_cb(lv_win_get_content(win), general_event_handler);

    lv_obj_t * win_btn = lv_win_add_btn(win, LV_SYMBOL_RIGHT);
    lv_obj_set_protect(win_btn, LV_PROTECT_CLICK_FOCUS);
    lv_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = lv_win_add_btn(win, LV_SYMBOL_NEXT);
    lv_obj_set_protect(win_btn, LV_PROTECT_CLICK_FOCUS);
    lv_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = lv_win_add_btn(win, LV_SYMBOL_OK);
    lv_obj_set_protect(win_btn, LV_PROTECT_CLICK_FOCUS);
    lv_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = lv_win_add_btn(win, LV_SYMBOL_PREV);
    lv_obj_set_protect(win_btn, LV_PROTECT_CLICK_FOCUS);
    lv_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = lv_win_add_btn(win, LV_SYMBOL_LEFT);
    lv_obj_set_protect(win_btn, LV_PROTECT_CLICK_FOCUS);
    lv_obj_set_event_cb(win_btn, win_btn_event_handler);

    win_btn = lv_win_add_btn(win, LV_SYMBOL_DUMMY"a");
    lv_obj_set_protect(win_btn, LV_PROTECT_CLICK_FOCUS);
    lv_obj_set_event_cb(win_btn, win_btn_event_handler);

    lv_obj_t * obj;


    obj = lv_spinbox_create(win, NULL);
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_spinbox_set_digit_format(obj, 5, 2);
    lv_group_add_obj(g, obj);

    obj = lv_btn_create(win, NULL);
    lv_group_add_obj(g, obj);
    lv_btn_set_toggle(obj, true);
    lv_obj_set_event_cb(obj, general_event_handler);
    obj = lv_label_create(obj, NULL);
    lv_label_set_text(obj, "Button");

    //LV_IMG_DECLARE(imgbtn_img_1);
    //LV_IMG_DECLARE(imgbtn_img_2);
    obj = lv_imgbtn_create(win, NULL);
    //lv_imgbtn_set_src(obj, LV_BTN_STATE_REL, &imgbtn_img_1);
    //lv_imgbtn_set_src(obj, LV_BTN_STATE_PR, &imgbtn_img_2);

    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    obj = lv_cb_create(win, NULL);
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    obj = lv_slider_create(win, NULL);
    lv_slider_set_range(obj, 0, 10);
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    obj = lv_sw_create(win, NULL);
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    obj = lv_ddlist_create(win, NULL);
    lv_ddlist_set_options(obj, "Item1\nItem2\nItem3\nItem4\nItem5\nItem6");
    lv_ddlist_set_fix_height(obj, LV_DPI);
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    obj = lv_roller_create(win, NULL);
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    lv_obj_t * ta = lv_ta_create(win, NULL);
    lv_ta_set_cursor_type(ta, LV_CURSOR_BLOCK);
    lv_obj_set_event_cb(ta, general_event_handler);
    lv_group_add_obj(g, ta);

    obj = lv_kb_create(win, NULL);
    lv_obj_set_size(obj, hres - LV_DPI, vres / 2);
    lv_kb_set_ta(obj, ta);
    lv_kb_set_cursor_manage(obj, true);
    lv_group_add_obj(g, obj);

    static const char * mbox_btns[] = {"Yes", "No", ""};
    obj = lv_mbox_create(win, NULL);
    lv_mbox_add_btns(obj, mbox_btns);
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    obj = lv_list_create(win, NULL);
    lv_obj_set_event_cb(obj, general_event_handler);
    const char * list_txts[] = {"File 1", "File 2", "File 3", "File 4", "File 5", "File 6", ""};

    uint32_t i;
    for(i = 0; list_txts[i][0] != '\0'; i++) {
        lv_obj_t * b;
        b = lv_list_add_btn(obj, LV_SYMBOL_FILE, list_txts[i]);
        lv_obj_set_event_cb(b, general_event_handler);
    }

    lv_group_add_obj(g, obj);

    obj = lv_page_create(win, NULL);
    lv_obj_set_size(obj, 2 * LV_DPI, LV_DPI);
    lv_group_add_obj(g, obj);

    obj = lv_label_create(obj, NULL);
    lv_label_set_text(obj, "I'm a page\nwith a long \ntext.\n\n"
                      "You can try \nto scroll me\nwith UP and DOWN\nbuttons.");
    lv_label_set_align(obj, LV_LABEL_ALIGN_CENTER);
    lv_obj_align(obj, NULL, LV_ALIGN_CENTER, 0, 0);

    obj = lv_tabview_create(win, NULL);
    lv_obj_set_size(obj, hres / 2, vres / 2);
    lv_obj_t * t1 = lv_tabview_add_tab(obj, "Tab 1");
    lv_obj_t * t2 = lv_tabview_add_tab(obj, "Tab 2");
    lv_obj_set_event_cb(obj, general_event_handler);
    lv_group_add_obj(g, obj);

    obj = lv_label_create(t1, NULL);
    lv_label_set_text(obj, "This is the content\nof the first tab");

    obj = lv_label_create(t2, NULL);
    lv_label_set_text(obj, "This is the content\nof the second tab");
    return g;
}


/**********************
 *   STATIC FUNCTIONS
 **********************/

/**
 * Read function for the input device which emulates keys on the window header
 * @param indev_drv pointer to the related input device driver
 * @param data store the last key and its state here
 * @return false because the reading in not buffered
 */
static bool win_btn_read(lv_indev_drv_t * indev_drv, lv_indev_data_t * data)
{
    (void) indev_drv;      /*Unused*/

    data->state = last_key_state;
    data->key = last_key;

    return false;
}

/**
 * Called when a control button on the window header is released to change the key state to RELEASED
 * @param btn pointer t to a button on the window header
 * @return LV_RES_OK  because the button is not deleted
 */
static void win_btn_event_handler(lv_obj_t * btn, lv_event_t event)
{
    (void) btn; /*Unused*/

    uint32_t key = 0;

    lv_obj_t * label = lv_obj_get_child(btn, NULL);
    const char * txt = lv_label_get_text(label);

    if(strcmp(txt, LV_SYMBOL_PREV) == 0) key = LV_KEY_PREV;
    else if(strcmp(txt, LV_SYMBOL_NEXT) == 0) key = LV_KEY_NEXT;
    else if(strcmp(txt, LV_SYMBOL_LEFT) == 0) key = LV_KEY_LEFT;
    else if(strcmp(txt, LV_SYMBOL_RIGHT) == 0) key = LV_KEY_RIGHT;
    else if(strcmp(txt, LV_SYMBOL_OK) == 0) key = LV_KEY_ENTER;
    else key = 'a';

    switch(event) {
        case LV_EVENT_PRESSED:
            last_key_state = LV_INDEV_STATE_PR;
            last_key = key;
            break;

        case LV_EVENT_CLICKED:
        case LV_EVENT_PRESS_LOST:
            last_key_state = LV_INDEV_STATE_REL;
            last_key = 0;
            break;
        default:
            break;
    }
}


static void group_focus_cb(lv_group_t * group)
{
    lv_obj_t * f = lv_group_get_focused(group);
    if(f != win) lv_win_focus(win, f, LV_ANIM_ON);
}

static void general_event_handler(lv_obj_t * obj, lv_event_t event)
{
    (void) obj; /*Unused*/

#if LV_EX_PRINTF
    switch(event) {
        case LV_EVENT_PRESSED:
            printf("Pressed\n");
            break;

        case LV_EVENT_SHORT_CLICKED:
            printf("Short clicked\n");
            break;

        case LV_EVENT_CLICKED:
            printf("Clicked\n");
            break;

        case LV_EVENT_LONG_PRESSED:
            printf("Long press\n");
            break;

        case LV_EVENT_LONG_PRESSED_REPEAT:
            printf("Long press repeat\n");
            break;

        case LV_EVENT_VALUE_CHANGED:
            printf("Value changed: %s\n", lv_event_get_data() ? (const char *)lv_event_get_data() : "");
            break;

        case LV_EVENT_RELEASED:
            printf("Released\n");
            break;

        case LV_EVENT_DRAG_BEGIN:
            printf("Drag begin\n");
            break;

        case LV_EVENT_DRAG_END:
            printf("Drag end\n");
            break;

        case LV_EVENT_DRAG_THROW_BEGIN:
            printf("Drag throw begin\n");
            break;

        case LV_EVENT_FOCUSED:
            printf("Focused\n");
            break;
        case LV_EVENT_DEFOCUSED:
            printf("Defocused\n");
            break;
        default:
            break;
    }
#endif
}

#endif
