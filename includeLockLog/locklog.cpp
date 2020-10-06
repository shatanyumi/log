//
// Created by hss on 2020/10/5.
//

#include <unistd.h>
#include <sys/stat.h>
#include <string.h>
#include "locklog.h"
#include <stdarg.h>

__thread char locklog::m_buffer[thread_buffer_size];
locklog::locklog() {
    m_system_level = log_info;
    fp = nullptr;
    m_is_sync = false;
    m_is_append = true;
    m_file_location[0] = '\0';
    pthread_mutex_init(&m_mutex, nullptr);
}

locklog::~locklog() {
    log_close();
}

void locklog::set_log_level(log_level level) {
    m_system_level = level;
}

log_level locklog::get_log_level() {
    return m_system_level;
}

bool locklog::log_close() {
    if (nullptr == fp)
        return false;
    fflush(fp);
    fclose(fp);
    fp = nullptr;
    return true;
}

bool locklog::log_init(log_level level, const char *filelocation, bool append, bool issync) {
    if (nullptr != fp)
        return false;
    m_system_level = level;
    m_is_append = append;
    m_is_sync = issync;

    if (strlen(filelocation) >= (sizeof(m_file_location) - 1)) {
        fprintf(stderr, "the path of log file is too long:%zu limit:%lu\n", strlen(filelocation),
                sizeof(m_file_location) - 1);
        exit(0);
    }

    strncpy(m_file_location, filelocation, sizeof(m_file_location));
    m_file_location[sizeof(m_file_location) - 1] = '\0';

    if ('\0' == m_file_location[0]) {
        fp = stdout;
        fprintf(stderr, "all information will put to stderr!\n");
        return false;
    }

    fp = fopen(m_file_location, append ? "a" : "w");
    if (fp == nullptr) {
        fprintf(stderr, "cannot open log file ,file location is %s!\n", m_file_location);
        exit(0);
    }
    //IO line buffer
    setvbuf(fp, (char *) nullptr, _IOLBF, 0);
    fprintf(stderr, "now all the information will got to the file %s\n", m_file_location);
    return false;
}

bool locklog::log(log_level level, char *logformat, ...) {
    int _size;
    int pre_strlen = 0;

    char *star = m_buffer;
    pre_strlen = pre_make_string(star, level);
    star += pre_strlen;

    va_list args;
    va_start(args, logformat);
    _size = vsnprintf(star, thread_buffer_size - pre_strlen, logformat, args);
    va_end(args);

    if (nullptr == fp)
        fprintf(stderr, "%s", m_buffer);
    else
        write(m_buffer, pre_strlen + _size);
    return true;
}

const char *locklog::log_level_to_string(log_level level) {
    switch (level) {
        case log_debug:
            return "DEBUG";
        case log_info:
            return "INFO";
        case log_trace:
            return "TRACE";
        case log_error:
            return "ERROR";
        case log_warning:
            return "WARNING";
        default:
            return "XXX";
    }
}


int locklog::pre_make_string(char *t_buffer, log_level level) {
    get_now_time();
    return snprintf(t_buffer, thread_buffer_size, "%s : %02d-%02d %02d:%02d:%02d ",
                    log_level_to_string(level),
                    vtm.tm_mon + 1, vtm.tm_mday, vtm.tm_hour, vtm.tm_min, vtm.tm_sec);
}

bool locklog::write(char *p_buffer, int len) {
    if (access(m_file_location, W_OK) != 0) {
        /// mutex to protect the file !
        pthread_mutex_lock(&m_mutex);
        if (access(m_file_location, W_OK) != 0) {
            log_close();
            log_init(m_system_level, m_file_location, m_is_append, m_is_sync);
        }
        pthread_mutex_unlock(&m_mutex);
    }

    if (fwrite(p_buffer, len, 1, fp)) {
        if (m_is_sync)
            fflush(fp);
        *p_buffer = '\0';
    } else {
        int x = errno;
        fprintf(stderr, "Failed to write to log file .  errno:%s  message:%s\n", strerror(x), p_buffer);
        return false;
    }
    return true;
}

bool locklog::dir_init(log_level level, const char *process_module_name, const char *process_log_dir) {
    if (access(process_log_dir, 00) == -1) {
        if (mkdir(process_log_dir, S_IREAD | S_IWRITE) < 0)
            fprintf(stderr, "create folder failed\n");
    }
    char location_str[log_path_len];
    snprintf(location_str, log_path_len, "%s/%s.log", process_log_dir, process_module_name);
    log_init(level, location_str);
    return true;
}

void locklog::clock_cycle() {

}

void locklog::get_now_time() {
    now = time(&now);
    localtime_r(&now, &vtm);
}


