#ifndef _LOG_DATA_H
#define _LOG_DATA_H

#include "adf.h"

#define LOG_BASE_ADDR           85      //eeprom存储开始位置
#define LOG_MAX_RECORDS_COUNT   67      //72      //存储数据的最大数  //20分钟存储一条数据 最多存储一整天的数据

//STM8L152C6  1024 eeprom  起始存储位置是85 故1024-85=939  所以还剩下939个字节的数据，每个log_data 是 14个字节  故939/14=67  所以最大存储数据个数是67
//
typedef struct _LogData{
    s16         temp;                                           // 温度
    s16         humi;                                           // 湿度
    u8          rssi;                                           // 信号强度
    u8          st;                                             // 故障码
    u8          battery;                                        // 电池电量
    u8          sample_time[7];                                 // 采集时间
}LogData;

void log_write(LogData *log, u32 records_wr);

void log_read(LogData *log, u32 records_rd);

void log_clear_data(void);

extern LogData   g_last_log_data;

#endif