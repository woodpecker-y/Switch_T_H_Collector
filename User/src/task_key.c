/*
* 进程描述：
* 此进程执行按键操作（开关灯操作），并且检测外部中断，在开灯后两秒检测中断触发次数
*/

#include "adf.h"
#include "sysparams.h"
#include "board.h"
#include "LCD.h"
#include "task_key.h"
//#include "task_disp.h"
#include "key.h"
#include "light.h"
#include "task_super_capacitor.h"
#include "task_report.h"

u8 task_key_dealwith(void);

/*! \brief key status */
static KeyHandler s_key_handler;

/*! \brief
*       key process
*/
void task_key_proc(void)
{
    if(s_key_handler.key_comb_counter>0)
    {
        s_key_handler.key_comb_timer++;
    }
    else
    {
        s_key_handler.key_comb_timer = 0;
        s_key_handler.key_comb_counter = 0;
    }

    if(s_key_handler.key_comb_timer > 7*UNIT_SECOND)
    {
        s_key_handler.key_comb_timer = 0;
        s_key_handler.key_comb_counter = 0;
    }

    switch(s_key_handler.sts)
    {
    case KEY_STS_IDLE:        // 按键空闲状态
        break;

    case KEY_STS_PUSH:        // 判断是否为合法按键
        if (TRUE == key_check_valid(s_key_handler.key_val))
        {
            s_key_handler.sts = KEY_STS_VALID;
        }
        else
        {
            s_key_handler.sts = KEY_STS_IDLE;
        }
        break;

    case KEY_STS_VALID:        // 检测按键释放
        // 只有在没有联网并且开灯时候才有长按
        if(s_report_handler.connected_state == 0 && g_sys_params.light_sts[E_LIGHT_1] == E_LIGHT_ON && g_sys_params.light_sts[E_LIGHT_2] == E_LIGHT_ON)
        {
            if (TRUE == key_release(s_key_handler.key_val))
                s_key_handler.sts = KEY_STS_DEAL;
        }
        else
        {
            s_key_handler.sts = KEY_STS_DEAL;
        }

        break;

    case KEY_STS_DEAL:        // 按键处理
        #ifdef LIGHT_DEBUG
        printf("dealwith\r\n");
        #endif

        task_key_dealwith();   //按键处理函数

        if(s_key_handler.key_val == KEY_IDLE)
            s_key_handler.sts = KEY_STS_IDLE;
        else
            s_key_handler.sts = KEY_STS_FEEDBACK;

        //s_key_handler.feedback_timer = 0;
        //delay_ms(25);
        break;
    case KEY_STS_FEEDBACK:      // 反馈处理 主要检测开关打开之后检测是否 接上负载了,和存储状态
//        s_key_handler.feedback_timer++;
//        if (s_key_handler.feedback_timer >= MAX_FEEDBACK_TIMER)
//        {
////            #ifdef LIGHT_DEBUG
////            printf("counter:%d \r\n", s_key_handler.pluse_counter_light);
////            #endif
//
//            //exti_close_by_index(E_LIGHT_1);     // 关闭中断检测
//
//            // L1和L2均无灯负载处理
//            if (s_key_handler.pluse_counter_light < MAX_VAILD_PLUSE_COUNT) // 2秒内检测到中断大于20次则说明有负载，如果小于20次则说明没有负载灯全部关闭  20
//            {
//                // 关闭L1和L2
//                light_off(E_LIGHT_1);
//                light_off(E_LIGHT_2);
//                //g_sys_params.light_sts[E_LIGHT_1] = E_LIGHT_OFF;
//                //g_sys_params.light_sts[E_LIGHT_2] = E_LIGHT_OFF;
//
//                // 仅在关灯的情况下开启充电
//                //task_super_capacitor_ctrl(1);
//
//            }
//            else
//            {
//                // 仅在开灯的情况下关闭充电
//                //task_super_capacitor_ctrl(0);
//            }
//
//            // 保存灯的状态
//            //sysparams_write(E_ADDR_OFFSET_LIGHT1_STATE, g_sys_params.light_sts, 2);
//            s_key_handler.sts = KEY_STS_IDLE;
//        }
        reset_delay_recharge_count();//重置充电延时
        s_key_handler.sts = KEY_STS_IDLE;
        break;

    default:
        break;
    }
}

/*! \brief
*       按键进程初始化 key task init
*/
void task_key_init(void)
{
//    // 关闭L1和L2
//    light_off(E_LIGHT_1);
//    light_off(E_LIGHT_2);
    //g_sys_params.light_sts[E_LIGHT_1] = E_LIGHT_OFF;
    //g_sys_params.light_sts[E_LIGHT_2] = E_LIGHT_OFF;
//
//    // 保存灯的状态
//    sysparams_write(E_ADDR_OFFSET_LIGHT1_STATE, g_sys_params.light_sts, 2);

    memset(&s_key_handler, 0, sizeof(s_key_handler));
}

/*! \brief
*       按键进程触发函数 key triggered, start key process
* \param key_code[IN]       - key value
*/
void task_key_triggered(u8 key_code)
{
    s_key_handler.key_val = key_code;
    s_key_handler.sts = KEY_STS_PUSH;
}

/*! \brief
*      按键触发开始按键处理  key triggered, start key process
* \param light1_state       - 第一路灯的状态 light1_state
* \param light2_state       - 第二路灯的状态 light2_state
*/
void task_key_triggered_manual(u8 light1_state, u8 light2_state)
{
    if (light1_state == E_LIGHT_ON)
    {
        light_on(E_LIGHT_1);
        g_sys_params.light_sts[E_LIGHT_1] = E_LIGHT_ON;
    }

    if (light2_state == E_LIGHT_ON)
    {
        light_on(E_LIGHT_2);
        g_sys_params.light_sts[E_LIGHT_2] = E_LIGHT_ON;
    }

    //if (light2_state == 1 || light1_state == 1)
    {
        //delay_ms(8);
        //exti_init_by_index(E_LIGHT_1);
        s_key_handler.pluse_counter_light = 0;

        //s_key_handler.feedback_timer = 0;
        s_key_handler.sts = KEY_STS_FEEDBACK;
    }
}

/*! \brief
*       查新按键进程状态
* \return
*       key task run state
*/
u8 task_key_get_state(void)
{
    return s_key_handler.sts;
}

// 按键处理函数
u8 task_key_dealwith(void)
{
//    if(g_sys_params.factory_flag > FACTORY_NUM && s_key_handler.key_val == KEY_COMB_1)
//        s_key_handler.key_val = KEY_IDLE;s_report_handler.connected_state==0

    if(s_report_handler.connected_state==1 && s_key_handler.key_val == KEY_COMB_1)
        s_key_handler.key_val = KEY_IDLE;

    switch(s_key_handler.key_val)
    {
    case KEY_1:             // 按键开关1
        if (g_sys_params.light_sts[E_LIGHT_1] == E_LIGHT_OFF)
        {
            light_on(E_LIGHT_1);
            g_sys_params.light_sts[E_LIGHT_1] = E_LIGHT_ON;

            #ifdef LIGHT_DEBUG
            printf("light1 on\r\n");
            #endif
        }
        else
        {
            light_off(E_LIGHT_1);
            g_sys_params.light_sts[E_LIGHT_1] = E_LIGHT_OFF;

            if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
                LCD_DisplayMARK(Mark_RH, CLEAR);                                    //水滴
            task_super_capacitor_normal_triggered();

            g_run_params.recharge_state = FALSE;
            #ifdef LIGHT_DEBUG
            printf("light1 off\r\n");
            #endif
        }

        //delay_ms(8);
        //exti_init_by_index(E_LIGHT_1);      // 初始化检测开关 中断引脚
        //s_key_handler.pluse_counter_light = 0;
        break;
    case KEY_2:             // 按键开关2
        if (g_sys_params.light_sts[E_LIGHT_2] == E_LIGHT_OFF)
        {
            light_on(E_LIGHT_2);
            g_sys_params.light_sts[E_LIGHT_2] = E_LIGHT_ON;

            #ifdef LIGHT_DEBUG
            printf("light2 on\r\n");
            #endif
        }
        else
        {
            light_off(E_LIGHT_2);
            g_sys_params.light_sts[E_LIGHT_2] = E_LIGHT_OFF;

            if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
                LCD_DisplayMARK(Mark_RH, CLEAR);                                    //水滴
            task_super_capacitor_normal_triggered();

            g_run_params.recharge_state = FALSE;
            #ifdef LIGHT_DEBUG
            printf("light2 off\r\n");
            #endif
        }

        //delay_ms(8);
        //exti_init_by_index(E_LIGHT_1);      // 初始化检测开关 中断引脚
        //s_key_handler.pluse_counter_light = 0;
        break;
    case KEY_COMB_1:        // 按键组合键
        //if(g_run_params.key_comb_1)
        //    g_run_params.key_comb_1 = 0;
        //else
        //    g_run_params.key_comb_1 = 1;
        s_key_handler.key_comb_counter++;

        if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
        {
            LCD_Display_H(g_run_params.humi, WRITE);//显示湿度数据
//            // 显示电压
            LCD_DisplayMARK(Mark_PT,CLEAR);                                     // 小数点
            LCD_DisplayMARK(Mark_T, CLEAR);                                     // 温度计
//
            LCD_DisplayMARK(Mark_DC, CLEAR);                                    // C 摄氏度标志
            LCD_DisplayMARK(Mark_RH, CLEAR);                                    // 水滴
            LCD_DisplayMARK(Mark_PC, CLEAR);                                    // 百分号
            LCD_WE_NUM(g_run_params.super_capacitor_voltage/1000,1,WRITE);	    // 温度十位
            LCD_WE_NUM((g_run_params.super_capacitor_voltage/100)%10,2,WRITE);	// 温度个位
            LCD_WE_NUM((g_run_params.super_capacitor_voltage/10)%10,3,WRITE);	// 温度小数位
//
        }

        if( g_sys_params.light_sts[E_LIGHT_1] == E_LIGHT_ON && g_sys_params.light_sts[E_LIGHT_2] == E_LIGHT_ON)
        {
            if(s_key_handler.key_comb_counter >= 5 && s_key_handler.key_comb_timer <= 7*UNIT_SECOND)
            {
                LCD_Display_H(g_run_params.humi, WRITE);//显示湿度数据
                // 显示电压
                LCD_DisplayMARK(Mark_PT,CLEAR);                                     // 小数点
                LCD_DisplayMARK(Mark_T, CLEAR);                                     // 温度计

                LCD_DisplayMARK(Mark_DC, CLEAR);                                    // C 摄氏度标志
                LCD_DisplayMARK(Mark_RH, CLEAR);                                    // 水滴
                LCD_DisplayMARK(Mark_PC, CLEAR);                                    // 百分号
                LCD_WE_NUM(g_run_params.super_capacitor_voltage/1000,1,WRITE);	    // 温度十位
                LCD_WE_NUM((g_run_params.super_capacitor_voltage/100)%10,2,WRITE);	// 温度个位
                LCD_WE_NUM((g_run_params.super_capacitor_voltage/10)%10,3,WRITE);	// 温度小数位

                s_key_handler.key_comb_timer = 0;
                s_key_handler.key_comb_counter = 0;
                task_super_capacitor_fast_triggered();
            }
        }

        break;
    default:
#ifdef LIGHT_DEBUG
            printf("comb key\r\n");
#endif
        return 1;
        break;
    }

    return 1;
}

void task_key_set_pluse_counter(void)
{
    //s_key_handler.pluse_counter_light++;
}