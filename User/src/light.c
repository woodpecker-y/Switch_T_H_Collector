#include "adf.h"
#include "light.h"
#include "led.h"

void light_load_config(PortTypeDef *light_config, int cnt)
{
    // Light Init,高电平有效，需要反向
    //led_load_config(light_config, cnt);
}

void light_init(void)
{
    GPIO_Init(GPIOB, GPIO_Pin_2, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(GPIOB, GPIO_Pin_3, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(GPIOD, GPIO_Pin_5, GPIO_Mode_Out_PP_Low_Fast);
    GPIO_Init(GPIOD, GPIO_Pin_6, GPIO_Mode_Out_PP_Low_Fast);
}

void light_off(u8 idx)
{
    if(idx == E_LIGHT_2)
    {
        GPIO_SetBits  (GPIOB, GPIO_Pin_3);
        GPIO_ResetBits(GPIOB, GPIO_Pin_2);
        delay_ms(35);
        GPIO_ResetBits(GPIOB, GPIO_Pin_3);
        GPIO_ResetBits(GPIOB, GPIO_Pin_2);
    }
    else if(idx == E_LIGHT_1)
    {
        GPIO_SetBits  (GPIOD, GPIO_Pin_6);
        GPIO_ResetBits(GPIOD, GPIO_Pin_5);
        delay_ms(35);
        GPIO_ResetBits(GPIOD, GPIO_Pin_6);
        GPIO_ResetBits(GPIOD, GPIO_Pin_5);
    }
}

void light_on(u8 idx)
{
    if(idx == E_LIGHT_2)
    {
        GPIO_SetBits  (GPIOB, GPIO_Pin_2);
        GPIO_ResetBits(GPIOB, GPIO_Pin_3);
        delay_ms(35);
        GPIO_ResetBits(GPIOB, GPIO_Pin_2);
        GPIO_ResetBits(GPIOB, GPIO_Pin_3);
    }
    else if(idx == E_LIGHT_1)
    {
        GPIO_SetBits  (GPIOD, GPIO_Pin_5);
        GPIO_ResetBits(GPIOD, GPIO_Pin_6);
        delay_ms(35);
        GPIO_ResetBits(GPIOD, GPIO_Pin_5);
        GPIO_ResetBits(GPIOD, GPIO_Pin_6);
    }
}


