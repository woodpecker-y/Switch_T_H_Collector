/*
* 进程描述：
*     此进程处理充电过程，使用脉冲充电方式， 检测超级电容电压及电压计算。并且切换
* 供电方式。
*/
#include "adf.h"
#include "sysparams.h"
//#include "disp.h"
//#include "rtc.h"
#include "LCD.h"
#include "board.h"

// 进程头文件引用
#include "task_report.h"
#include "task_sample.h"
#include "task_disp.h"
#include "task_key.h"
#include "task_super_capacitor.h"

typedef enum _SuperCapacitorState{
    E_SC_STATE_IDLE = 0,            // 空闲状态
    E_SC_STATE_COLLECT,              // 完成状态
    E_SC_STATE_FAST_RECHARGE            // 充电状态
}SuperCapacitorState;

//充电模式
typedef enum _SuperCapacitorRechargeMode{
    E_SC_RECHARGE_50MS = 0,//无
    E_SC_RECHARGE_125MS,
    E_SC_RECHARGE_250MS,
    E_SC_RECHARGE_375MS,
    E_SC_RECHARGE_500MS
}_SuperCapacitorRechargeMode;

//充电模式
typedef enum _SuperCapacitorRechargeSpeed{
    E_SC_RECHARGE_SPEED_10MS = 0,//无
    E_SC_RECHARGE_SPEED_20MS,
    E_SC_RECHARGE_SPEED_30MS,
    E_SC_RECHARGE_SPEED_40MS,
    E_SC_RECHARGE_SPEED_50MS
}_SuperCapacitorRechargeSpeed;



SuperCapacitorHandler s_super_capacitor_handler;

//void task_super_capacitor_init(u8 have_lights_on)
//{
//    s_super_capacitor_handler.recharge_switch = 0;
//    //    if (have_lights_on == 1)
//    //    {
//    s_super_capacitor_handler.recharge_max_counter = 2;
//    //    }
//    //    else
//    //    {
//    //        s_super_capacitor_handler.recharge_max_counter = SUPER_CAPACITOR_RECHARGE_IDLE_COUNTER;
//    //    }
//}


// 系统判断是否可以充电
static s8 recharge_state(void)
{
    // 切换电平为高，启动充电。只有在进程空闲的时候才开始充电，并且任何一个进程执行完成之后会有1s的延时然后再开始充电
    if(  task_report_get_state() == E_REPORT_IDLE    && \
         task_sample_get_state() == E_COLLECT_IDLE   && \
         task_key_get_state()    == KEY_STS_IDLE     && \
         task_disp_get_state()   == E_TASK_DISP_IDLE && \
         s_super_capacitor_handler.delay_recharge_count <= 0 )      // 显示进程
    {
        return 0;
    }
    else
    {
        return -1;
    }
}

// 重置充电延时
void reset_delay_recharge_count(void)
{
    s_super_capacitor_handler.delay_recharge_count = UNIT_SECOND;//任何任务只要执行完成则重置一次充电延时
}

// 超级电容充电初始化
void task_super_capacitor_init(void)
{
    s_super_capacitor_handler.recharge_count = E_SC_RECHARGE_250MS;
    s_super_capacitor_handler.recharge_mode = E_SUPERCAPACITOR_MODE_NORMAL;     //上电默认是正常模式
    reset_delay_recharge_count();//重置充电延时

    return;
}



// 进程主函数
void task_super_capacitor_proc(void)
{
    u16 super_capacitor_v_time = 0;     // 检测电容电压计时总时间
    u16 before_super_capacitor_voltage;                 // 检测上次超级电容电压

    // 充电计数器
    s_super_capacitor_handler.recharge_counter++;

    if(s_super_capacitor_handler.delay_recharge_count > 0)
    {
        s_super_capacitor_handler.delay_recharge_count--;
    }


        if(s_super_capacitor_handler.recharge_counter >= s_super_capacitor_handler.recharge_count)
        {
            s_super_capacitor_handler.recharge_counter = 0;

            if(recharge_state() == 0)
            {
                GPIO_SetBits(GPIOB, GPIO_Pin_6);
                if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
                {
                    switch(s_super_capacitor_handler.recharge_speed)
                    {
                    case E_SC_RECHARGE_SPEED_10MS:
                        delay_ms(10);
                        break;

                    #if 0
                    case E_SC_RECHARGE_SPEED_20MS:
                        delay_ms(20);
                        break;
                    #endif

                    #if 0
                    case E_SC_RECHARGE_SPEED_30MS:
                        delay_ms(30);
                        break;
                    #endif

                    #if 0
                    case E_SC_RECHARGE_SPEED_40MS:
                        delay_ms(40);
                        break;
                    #endif

                    case E_SC_RECHARGE_SPEED_50MS:
                        delay_ms(50);
                        break;
                    default:
                        break;
                    }
                }
                GPIO_ResetBits(GPIOB, GPIO_Pin_6);
            }
        }

    switch(s_super_capacitor_handler.state)
    {
    case E_SC_STATE_IDLE:        // 空闲状态
        if(task_report_get_state() > E_REPORT_READY || s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
            super_capacitor_v_time = NB_SUPER_CAPACITOR_COLLECT_COUNTER;        //在设备联网或者上传数据的时候5s检测一次电容电量
        else
            super_capacitor_v_time = NORMO_SUPER_CAPACITOR_COLLECT_COUNTER;     //在设备联网或者上传数据的时候15s检测一次电容电量

        s_super_capacitor_handler.sample_counter++;
        if (s_super_capacitor_handler.sample_counter >= super_capacitor_v_time)
        {
            // 恢复计数器
            s_super_capacitor_handler.sample_counter = 0;
            // 切换状态
            s_super_capacitor_handler.state = E_SC_STATE_COLLECT;
        }
        break;
    case E_SC_STATE_COLLECT:        //电容电压采集

        // 采集超级电容电压
        before_super_capacitor_voltage = g_run_params.super_capacitor_voltage;     // 更新超级电容历史电压值
        g_run_params.super_capacitor_voltage = task_super_capacitor_read_voltage();             // 单位mV

        // 关灯的情况下清除水滴显示
        if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
        {
            if(g_sys_params.light_sts[E_LIGHT_1] == E_LIGHT_OFF || g_sys_params.light_sts[E_LIGHT_2] == E_LIGHT_OFF)
                LCD_DisplayMARK(Mark_RH, CLEAR);                                    //水滴
        }

        // 判断是否在充电,水滴闪烁显示用
        if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
        {
            if((g_run_params.super_capacitor_voltage-before_super_capacitor_voltage)>0 || g_run_params.super_capacitor_voltage == 0)
                g_run_params.recharge_state = TRUE;
            else
                g_run_params.recharge_state = FALSE;
        }
        else
        {
           g_run_params.recharge_state = FALSE;
        }

        // 切换面板供电方式
        if(g_run_params.super_capacitor_voltage >= SUPER_CAPACITOR_FULL)     // >=4900 判断检测电压是否充满 如果充满则供电电源切换为电容供电
            power_selector_capacitor();            // 切换为超级电容供电
        if(g_run_params.super_capacitor_voltage <= SUPER_CAPACITOR_LOSS)     // <=3600 判断检测电压是否亏电 如果亏电则供电电源切换为模块供电
            power_selector_module();               //切换为单火线取电模块供电

        // 关闭快充模式
        if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)
        {
            if(g_run_params.super_capacitor_voltage >= 5000)
            {
                s_super_capacitor_handler.hold_recharge_count++;
                if(s_super_capacitor_handler.hold_recharge_count>=10)
                {
                    s_super_capacitor_handler.hold_recharge_count = 0;
                    task_super_capacitor_normal_triggered();                        // 切换为正常模式

                    g_run_params.recharge_state = FALSE;                            // 关闭水滴闪烁
                    //if(g_sys_params.factory_flag <= FACTORY_NUM){
                            LCD_DisplayMARK(Mark_RH, CLEAR);                        // 清除水滴
                    //}
                    task_key_init();                                                // 快速模式下充满电关灯
                }
            }
        }

        // 切换充电频率 判断电容电压
        if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_NORMAL)
        {
            if(g_run_params.super_capacitor_voltage >= 3600)
            {
                s_super_capacitor_handler.recharge_count = E_SC_RECHARGE_125MS;
            }
            else if(g_run_params.super_capacitor_voltage < 3600)
            {
                s_super_capacitor_handler.recharge_count = E_SC_RECHARGE_250MS;
            }
        }
        else if(s_super_capacitor_handler.recharge_mode == E_SUPERCAPACITOR_MODE_FAST)//快充使用125ms模式
        {
            s_super_capacitor_handler.recharge_count = E_SC_RECHARGE_125MS;
        }

#if 1
        if(g_run_params.super_capacitor_voltage<=3000)
            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_10MS;

//        else if(g_run_params.super_capacitor_voltage>3000 && g_run_params.super_capacitor_voltage<4000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_50MS;

        else if(g_run_params.super_capacitor_voltage>3000 )
            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_50MS;
#endif

//#if 0
//        if(g_run_params.super_capacitor_voltage<=2000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_10MS;
//        else if(g_run_params.super_capacitor_voltage>2000 && g_run_params.super_capacitor_voltage<3000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_20MS;
//        else if(g_run_params.super_capacitor_voltage>3000 && g_run_params.super_capacitor_voltage<4000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_30MS;
//        else if(g_run_params.super_capacitor_voltage>4000 )
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_50MS;
//#endif

//#if 0
//        if(g_run_params.super_capacitor_voltage<=1000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_10MS;
//        else if(g_run_params.super_capacitor_voltage>1000 && g_run_params.super_capacitor_voltage<2000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_20MS;
//        else if(g_run_params.super_capacitor_voltage>2000 && g_run_params.super_capacitor_voltage<3000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_30MS;
//        else if(g_run_params.super_capacitor_voltage>3000 && g_run_params.super_capacitor_voltage<4000)
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_40MS;
//        else if(g_run_params.super_capacitor_voltage>4000 )
//            s_super_capacitor_handler.recharge_speed = E_SC_RECHARGE_SPEED_50MS;
//#endif

        //printf("g_run_params.super_capacitor_voltage = %d\r\n", g_run_params.super_capacitor_voltage);

        // 切换状态
        s_super_capacitor_handler.state = E_SC_STATE_IDLE;
        break;
//    case E_SC_STATE_FAST_RECHARGE:
//        if(s_super_capacitor_handler.exit_fast_recharge_timer >= 1000)
//            GPIO_ResetBits(GPIOB, GPIO_Pin_6);
//        else
//            GPIO_SetBits(GPIOB, GPIO_Pin_6);
//
//        break;
    default:
        s_super_capacitor_handler.state = E_SC_STATE_IDLE;
        break;
    }
}

// 获取充电状态
u8 task_super_capacitor_get_mode(void)
{
    return s_super_capacitor_handler.state;
}

//void task_super_capacitor_ctrl(u8 enable)
//{
//    s_super_capacitor_handler.recharge_switch = enable;
//    s_super_capacitor_handler.recharge_counter = 0;
//}

// 快速充电模式
void task_super_capacitor_fast_triggered(void)
{
    s_super_capacitor_handler.recharge_mode = E_SUPERCAPACITOR_MODE_FAST;
}

// 正常充电模式
void task_super_capacitor_normal_triggered(void)
{
    s_super_capacitor_handler.recharge_mode = E_SUPERCAPACITOR_MODE_NORMAL;
}


// 采集电容器中的电压值
u16 task_super_capacitor_read_voltage(void)
{
//    u16 super_capacitor_voltage = 0;
//
//    // 初始化超级电容采样
//    adc_power_on(SUPER_CAPACITOR_RECHARGE);
//    adc_init(SUPER_CAPACITOR_RECHARGE);
//
//    delay_ms(5);
//
//    // 对超级电容采样
//    super_capacitor_voltage = adc_sample(SUPER_CAPACITOR_RECHARGE);
//    //super_capacitor_voltage = 3038;
//
//    // 电压转换
//    super_capacitor_voltage = super_capacitor_voltage / 62;
//
//    // 关闭释放超级电容采样电路
//    adc_close(SUPER_CAPACITOR_RECHARGE);
//    adc_power_off(SUPER_CAPACITOR_RECHARGE);

    float adc_value = 0;

    // 初始化超级电容采样
    adc_power_on(SUPER_CAPACITOR_RECHARGE);
    adc_init(SUPER_CAPACITOR_RECHARGE);
    // 对超级电容电压采样
    adc_value = adc_sample(SUPER_CAPACITOR_RECHARGE);
    //printf("adc_value = %f\r\n",adc_value);

    adc_close(SUPER_CAPACITOR_RECHARGE);            // 关闭释放超级电容采样引脚
    adc_power_off(SUPER_CAPACITOR_RECHARGE);        // 关闭电容电压采集ADC

    // 电压计算
    //adc_value = adc_value*6600/4096;
    adc_value = adc_value*6200/4096;
    //g_run_params.super_capacitor_voltage = (u16)adc_value;  //单位mV
    //printf("g_run_params.super_capacitor_voltage = %d\r\n",g_run_params.super_capacitor_voltage);

    return (u16)adc_value;
}

u8 super_capacitor_electric_quantity(u16 voltage)
{
    u8 voltage_percent = 0;

    voltage_percent = voltage/50;       //超级电容电量百分比

    if(voltage_percent>100)
    {
        voltage_percent = 100;
    }

    return voltage_percent;
}




