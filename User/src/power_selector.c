#include "adf.h"
#include "power_selector.h"

/*
    电源供电方式切换模式函数，切换5V模块供电还是电容器供电
*/

//电源选择引脚初始化
void power_selector_init(void)
{
    gpio_init(power_selector_cfg.port, power_selector_cfg.pin, GPIO_Mode_Out_PP_High_Fast);
}

//// 硬件V1.1
////供电方式切换为单火线取5V电模块
//void power_selector_module(void)
//{
//    gpio_set_high(power_selector_cfg.port, power_selector_cfg.pin);
//}
//
////供电方式切换为超级电容
//void power_selector_capacitor(void)
//{
//    gpio_set_low(power_selector_cfg.port, power_selector_cfg.pin);
//}


// 硬件V1.2  初次上电超级电容没有电压，选择电源模块供电则提供单片机充电，当超级电容充满电之后选择超级电容供电。
//供电方式切换为单火线取5V电模块
void power_selector_module(void)
{
    gpio_set_low(power_selector_cfg.port, power_selector_cfg.pin);
}

//供电方式切换为超级电容
void power_selector_capacitor(void)
{
    gpio_set_high(power_selector_cfg.port, power_selector_cfg.pin);
}
