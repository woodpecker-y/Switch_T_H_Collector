#include "bsp.h"
#include "nb.h"
#include "board.h"

//////////////////////////////产品定义////////////////////////////////////
const char *manufacturer  = "Hebei fengyuan intelligent control technology co. LTD";
#if defined(HAVE_CT)
const char *product_model= "TH-NB-IoT-CT-Switch";
#elif defined(HAVE_CU)
const char *product_model= "TH-NB-IoT-CU-Switch";
#endif
const char *hw_ver = "1.0.0";
const char *sw_ver = "1.0.0.190824";

//////////////////////////////串口配置////////////////////////////////////
uint8_t com_global_buffer[MAX_COM_BUFF_SIZE+1] = {0};

///*! \brief Com Configure Definition */
//typedef struct _ComConfig{
//    COM          com;           /*!< \brief COM Object */
//    PortTypeDef  tx;            /*!< \brief COM Tx Pin */
//    PortTypeDef  rx;            /*!< \brief COM Rx Pin */
//    PortTypeDef  ctrl;          /*!< \brief COM Ctrl Pin, Only used with RS485 */
//    uint8_t*	   rcv_ptr;       /*!< \brief COM recv buffer poniter */
//    uint16_t	   max_rcv_size;  /*!< \brief COM recv buffer max size */
//}COMConfig;
COMConfig com_cfg[1] = {
    {COM1, {PORTA, PIN2}, {PORTA, PIN3}, {(Port)0, (Pin)0}, com_global_buffer+0, MAX_COM_BUFF_SIZE} /* 重要提示:此处必须留一个字节作为字符串的'\0'结尾，否则使用字符串函数时会溢出 */
};

//////////////////// light配置 //////////////////////////////
PortTypeDef light_config[2] = {
    {PORTB, PIN3},
    {PORTB, PIN2}
};

EXTIConfig light_check_config[1] = {
    {{PORTF, PIN0}, EXTI_Trigger_Falling}
};

//////////////////// KEY配置 //////////////////////////////
PortTypeDef key_config[KEY_CNT] = {
    {PORTB, PIN5},
    {PORTB, PIN4}
};


//////////////////// 超级电容电压检测及充电配置 //////////////////////////////
ADCConfig adc_cfg[1] = {
    {ADC1, {PORTB, PIN7}, {PORTC, PIN4}, E_LE_HIGH}
};

// 充电脚
PortTypeDef super_capacitor_recharge_cfg = {
    PORTB, PIN6
};


// 电源切换引脚 切换电容供电还是5V取电模块供电
PortTypeDef power_selector_cfg = {
    PORTB, PIN1
};

/////////////////// NB-IoT配置 //////////////////////////
///*! \brief NB Configure Definition */
//typedef struct _NBConfig{
//    COM          com;            /*!< \brief NB communication com */
//    PortTypeDef  reset;          /*!< \brief reset pin */
//    PortTypeDef  pwr;            /*!< \brief power pin */
//    u8           pwr_le;         /*!< \brief power level effective */
//}NBConfig;
NBConfig nb_config = {
    COM1, {PORTE, PIN7}, {PORTC, PIN7}, E_LE_HIGH        //E_LE_HIGH  ，  E_LE_LOW
};

/////////////////// 温湿度传感器配置 //////////////////////////
//typedef struct _SHT3xConfig{
//    PortTypeDef scl;
//    PortTypeDef sda;
//    PortTypeDef pwr;
//    u8          pwr_le;         /*!< \brief power level effective */
//}SHT3xConfig;
SHT3xConfig sht30_cfg = {
    {PORTC, PIN1}, {PORTC, PIN0}, {PORTD, PIN7}, E_LE_LOW
};

//////////////////// LCD配置 //////////////////////////////
//LCDPortConfig lcd_config = {
//    E_PWR_EXT,
//    {0xFF, 0x00, 0x00, 0x00, 0x00, 0x00}
//};

