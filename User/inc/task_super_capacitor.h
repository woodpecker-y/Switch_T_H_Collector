#ifndef _TASK_SUPER_CAPACITOR_H
#define _TASK_SUPER_CAPACITOR_H

#define SUPER_CAPACITOR_FULL                 4800              // 判断超级电压充满电
#define SUPER_CAPACITOR_LOSS                 3600              // 判断超级电容亏电

#define NB_SUPER_CAPACITOR_COLLECT_COUNTER      5*UNIT_SECOND     //NB模块在工作的时候每秒检测一次ADC
#define NORMO_SUPER_CAPACITOR_COLLECT_COUNTER   15*UNIT_SECOND     //正常模式10s秒检测一次ADC

//#define SUPER_CAPACITOR_RECHARGE_IDLE_COUNTER   (100/RTC_FRE)

typedef enum _SuperCapacitorMode{
    E_SUPERCAPACITOR_MODE_NORMAL = 0,                           // 正常模式
    E_SUPERCAPACITOR_MODE_FAST,                                 // 快速充电模式
    E_SUPERCAPACITOR_MODE_DIRECT_CONNECTION                     //直接连接
}SuperCapacitorMode;

typedef struct _SuperCapacitorHandler{
    u8  state;                      // 状态
    u32 recharge_counter;           // 充电 计数器
    u32 sample_counter;             // 采集电压 计数器
    //u8  recharge_max_counter;       //
    u8  recharge_mode;              //
    u8  recharge_count;
    //u8  report_triggered;           //
    //u8  recharge_switch;            //
    //s32 exit_fast_recharge_timer;   //
    //u8  exit_fast_recharge_flag;    //
    s8  delay_recharge_count;
    u8  recharge_speed;                //快速充电，充电速度
    u8  hold_recharge_count;
}SuperCapacitorHandler;

extern SuperCapacitorHandler s_super_capacitor_handler;

//重置充电延时
void reset_delay_recharge_count(void);

void task_super_capacitor_init(void);

void task_super_capacitor_proc(void);

u16 task_super_capacitor_read_voltage(void);

void task_super_capacitor_fast_triggered(void);

void task_super_capacitor_normal_triggered(void);

void task_super_capacitor_ctrl(u8 enable);

u8 task_super_capacitor_get_mode(void);

u8 super_capacitor_electric_quantity(u16 voltage);

#endif