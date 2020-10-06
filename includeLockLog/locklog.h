//
// Created by hss on 2020/10/5.
//

#ifndef LOG_LOCKLOG_H
#define LOG_LOCKLOG_H

#include <iostream>
#include <stdio.h>
#include <pthread.h>

#define thread_buffer_size 1024*1024*5    // 5M
#define process_buffer_size 1024*1024*10    // 10M
#define log_path_len    250
#define log_module_len  32

enum log_level {
    log_debug,
    log_trace,
    log_info,
    log_warning,
    log_error
};

class locklog {
private:
    // log level
    enum log_level m_system_level;
    // file pointer
    FILE *fp;
    time_t now;
    struct tm vtm;
    bool m_is_sync;
    bool m_is_append;
    char m_file_location[log_path_len]{};
    pthread_mutex_t m_mutex;
    static __thread char m_buffer[thread_buffer_size];

public:
    locklog();

    ~locklog();

    void set_log_level(enum log_level level);

    log_level get_log_level();

    void clock_cycle();

    void get_now_time();

    bool log_close();

    bool log_init(log_level level, const char *filelocation, bool append = true, bool issync = false);

    bool log(log_level level, char *logformat, ...);

    bool dir_init(log_level level, const char *process_module_name, const char *process_log_dir);

    const char *log_level_to_string(log_level level);

    bool check_level(log_level level);

    int pre_make_string(char *t_buffer, log_level level);

    bool write(char *p_buffer, int len);
};


#endif //LOG_LOCKLOG_H
