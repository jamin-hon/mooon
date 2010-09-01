/**
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * Author: jian yi, eyjian@qq.com
 */
#ifndef SYS_LOG_H
#define SYS_LOG_H
#include "sys/sys_config.h"
#define LOG_LINE_SIZE_MIN 256             /** ��־����С���� */
#define LOG_LINE_SIZE_MAX 65535           /** ��־����󳤶� */
#define DEFAULT_LOG_FILE_SIZE 104857600   /** Ĭ�ϵĵ�����־�ļ���С��100MB�� */
#define DEFAULT_LOG_FILE_BACKUP_NUMBER 10 /** Ĭ�ϵ���־�ļ����ݸ��� */
SYS_NAMESPACE_BEGIN

/** ������־���� */
typedef enum
{
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG = 1,
    LOG_LEVEL_INFO  = 2,
    LOG_LEVEL_WARN  = 3,
    LOG_LEVEL_ERROR = 4,
    LOG_LEVEL_FATAL = 5
}log_level_t;

/** ͨ����־����õ���־���������������������־�����򷵻�NULL */
extern const char* get_log_level_name(log_level_t log_level);

/**
  * ��־�ӿ�
  */
class ILogger
{
public:
    /** ���������������������α������澯 */
    virtual ~ILogger() {}
        
    virtual void enable_screen(bool enabled) = 0;
    virtual void enable_trace_log(bool enabled) = 0;
    virtual void enable_auto_newline(bool auto_newline) = 0;
    virtual void set_log_level(log_level_t log_level) = 0;
    virtual void set_single_filesize(uint32_t filesize) = 0;
    virtual void set_backup_number(uint16_t backup_number) = 0;

    virtual bool enabled_debug() = 0;
    virtual bool enabled_info() = 0;
    virtual bool enabled_warn() = 0;
    virtual bool enabled_error() = 0;
    virtual bool enabled_fatal() = 0;
    virtual bool enabled_trace() = 0;

    virtual void log_debug(const char* format, ...) = 0;
    virtual void log_info(const char* format, ...)  = 0;
    virtual void log_warn(const char* format, ...)  = 0;
    virtual void log_error(const char* format, ...) = 0;
    virtual void log_fatal(const char* format, ...) = 0;
    virtual void log_trace(const char* format, ...) = 0;
};

//////////////////////////////////////////////////////////////////////////
// ��־�꣬�����¼��־
extern ILogger* g_logger; // ֻ�����������Ƕ��壬���ܸ�ֵŶ��

#define MYLOG_DEBUG(format, ...) \
do { \
	if (NULL == sys::g_logger) \
		printf(format, ##__VA_ARGS__); \
	else if (sys::g_logger->enabled_debug()) \
		sys::g_logger->log_debug(format, ##__VA_ARGS__); \
} while(false)

#define MYLOG_INFO(format, ...) \
do { \
	if (NULL == sys::g_logger) \
		printf(format, ##__VA_ARGS__); \
	else if (sys::g_logger->enabled_info()) \
		sys::g_logger->log_info(format, ##__VA_ARGS__); \
} while(false)

#define MYLOG_WARN(format, ...) \
do { \
	if (NULL == sys::g_logger) \
		printf(format, ##__VA_ARGS__); \
	else if (sys::g_logger->enabled_warn()) \
		sys::g_logger->log_warn(format, ##__VA_ARGS__); \
} while(false)

#define MYLOG_ERROR(format, ...) \
do { \
	if (NULL == sys::g_logger) \
		printf(format, ##__VA_ARGS__); \
	else if (sys::g_logger->enabled_error()) \
		sys::g_logger->log_error(format, ##__VA_ARGS__); \
} while(false)

#define MYLOG_FATAL(format, ...) \
do { \
	if (NULL == sys::g_logger) \
		printf(format, ##__VA_ARGS__); \
	else if (sys::g_logger->enabled_fatal()) \
		sys::g_logger->log_fatal(format, ##__VA_ARGS__); \
} while(false)

#define MYLOG_TRACE(format, ...) \
do { \
	if (NULL == sys::g_logger) \
		printf(format, ##__VA_ARGS__); \
	else if (sys::g_logger->enabled_trace()) \
		sys::g_logger->log_trace(format, ##__VA_ARGS__); \
} while(false)

SYS_NAMESPACE_END
#endif // SYS_LOG_H