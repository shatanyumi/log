//
// Created by hss on 2020/10/6.
//

/*
 * how to use this
 * copy this to the main function and run it!
 */
#include "includeLockLog/locklog.h"
#include <iostream>

int main(){
    locklog lock_log;
    lock_log.dir_init(log_info,"main.c","../logfile/includeLockLogTest");
    lock_log.log(log_info,"%s [value:%d] [xx:%d]\n","test",0,10);
    return 0;
}