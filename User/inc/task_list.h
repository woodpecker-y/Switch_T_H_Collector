#ifndef _TASK_LIST_H
#define _TASK_LIST_H
// 进程头文件引用
#include "task_report.h"
#include "task_sample.h"
#include "task_disp.h"
#include "task_key.h"
#include "task_super_capacitor.h"
//#include "task_pwm.h"

TaskItem_t task_list[] = {
    {1, task_key_proc},
    {2, task_sample_proc},
    {3, task_super_capacitor_proc},
    {4, task_report_proc},
    {5, task_disp_proc}
};

#endif