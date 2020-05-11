#ifndef _TASK_DISP_H
#define _TASK_DISP_H

#include "sysparams.h"

typedef enum {
    E_TASK_DISP_IDLE = 0,     //空闲状态
    E_TASK_DISP_MAIN_MENU,    //刷新屏幕
    E_TASK_DISP_ANT,           //刷新信号值

    E_TASK_DISP_ANT_IDLE,
    E_TASK_DISP_ANT_TWINKLE,

    E_TASK_DISP_LOOP_IDLE,
    E_TASK_DISP_LOOP_MARK,
    E_TASK_DISP_LOOP_NUM
}TASK_DISP_STATE;


typedef struct _TASK_DISP_HANDLER{
    u8          sts;            //状态
    u16         disp_timer;     //刷新定时器
    bool       temp_humi_flag; //循环刷新标志

    u8          sts_ant;
    u16         disp_ant_timer;
    bool        ant_flag;      //信号闪烁标志
    u16         twinkle_num;

    bool        recharge_flag;  //充电闪烁标志



    bool        loop_disp;
    u16          loop_timer;
    u8          loop_disp_mark;
    u8          loop_maek_num;
}TASK_DISP_HANDLER;

/* 显示的主进程 */
void task_disp_proc();
// 显示初始化
void task_disp_init(TASK_DISP_STATE sts);

u8 task_disp_get_state();

void task_disp_triggered(TASK_DISP_STATE sts);

u8 task_disp_calc_ant_level(u8 rssi);

#endif