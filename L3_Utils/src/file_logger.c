/*
 *     SocialLedge.com - Copyright (C) 2013
 *
 *     This file is part of free software framework for embedded processors.
 *     You can use it and/or distribute it as long as this copyright header
 *     remains unmodified.  The code is free for personal use and requires
 *     permission to use in a commercial product.
 *
 *      THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 *      OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 *      MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 *      I SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 *      CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 *     You can reach the author of this software at :
 *          p r e e t . w i k i @ g m a i l . c o m
 */

#include <stdlib.h>
#include <stdio.h>    // sprintf()
#include <string.h>   // strlen()
#include <stdarg.h>
#include <stdbool.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "file_logger.h"
#include "lpc_sys.h"
#include "rtc.h"
#include "ff.h"



static bool g_logger_initted = false;
static char * gp_file_buffer = NULL;                ///< Pointer to local buffer space before it is written to file
static QueueHandle_t g_write_buffer_queue = NULL;   ///< Log message pointers are written to this queue
static QueueHandle_t g_empty_buffer_queue = NULL;   ///< Log message pointers are available from this queue

static bool logger_write_to_file(const uint32_t bytes_to_write)
{
    FIL file;
    bool success = false;
    UINT bytes_written = 0;
    UINT bytes_to_write_dword = bytes_to_write;

    if (0 == bytes_to_write_dword) {
        success = true;
    }
    else if (FR_OK == f_open(&file, FILE_LOGGER_FILENAME, FA_OPEN_ALWAYS | FA_WRITE))
    {
        f_lseek(&file, f_size(&file));
        f_write(&file, gp_file_buffer, bytes_to_write, &bytes_to_write_dword);
        f_close(&file);
        success = (bytes_written == bytes_to_write);
    }

    return success;
}

/**
 * This is the actual logger FreeRTOS task responsible for:
 *      - Retrieve a log message written to the write queue
 *      - Copy it to our local buffer
 *      - If local buffer is full, write it to the file
 *      - Put-back the log message buffer to available queue
 */
static void logger_task(void *p)
{
    char * log_msg = NULL;
    uint32_t len = 0;
    uint32_t used_buffer = 0;
    uint32_t buffer_overflow_cnt = 0;

    while (1)
    {
        /* Receive the log message we wish to write to our buffer */
        log_msg = NULL;
        xQueueReceive(g_write_buffer_queue, &log_msg, OS_MS(1000 * FILE_LOGGER_FLUSH_TIMEOUT));

        /* If we receive NULL pointer, we assume it is to flush the data */
        if (NULL == log_msg) {
            /* We don't see the NULL pointer to the queue */
            logger_write_to_file(used_buffer);
            used_buffer = 0;
            continue;
        }

        /* else */
        len = strlen(log_msg);

        /* Find out how many bytes (if any) overflow the buffer */
        buffer_overflow_cnt = 0;
        if (len + used_buffer > FILE_LOGGER_BUFFER_SIZE) {
            buffer_overflow_cnt = (len + used_buffer) - FILE_LOGGER_BUFFER_SIZE;
        }

        /* Write the buffer up to the capacity */
        memcpy(gp_file_buffer + used_buffer, log_msg,
                buffer_overflow_cnt > 0 ? (FILE_LOGGER_BUFFER_SIZE - used_buffer) : len);
        used_buffer += len;

        /* If buffer is full, write it to the file */
        if (used_buffer >= FILE_LOGGER_BUFFER_SIZE) {
            logger_write_to_file(FILE_LOGGER_BUFFER_SIZE);
            used_buffer -= FILE_LOGGER_BUFFER_SIZE;
        }

        /* Write remaining message to the buffer (if any) */
        if (buffer_overflow_cnt > 0) {
            memcpy(gp_file_buffer, (log_msg + len - buffer_overflow_cnt), buffer_overflow_cnt);
        }

        /* Put the data pointer back to available buffer */
        xQueueSend(g_empty_buffer_queue, &log_msg, portMAX_DELAY);
    }
}

static void logger_init(void)
{
    uint32_t i = 0;
    char * ptr = NULL;

    /* Create the queues that keep track of the written buffers, and available buffers */
    g_write_buffer_queue = xQueueCreate(FILE_LOGGER_MSG_BUFFERS, sizeof(char*));
    g_empty_buffer_queue = xQueueCreate(FILE_LOGGER_MSG_BUFFERS, sizeof(char*));

    /* Create the buffer space we write the logged messages to (before we flush it to the file) */
    gp_file_buffer = (char*) malloc(FILE_LOGGER_BUFFER_SIZE);

    /* Create the actual buffers for log messages */
    for (i = 0; i < FILE_LOGGER_MSG_BUFFERS; i++)
    {
        ptr = (char*) malloc(FILE_LOGGER_MSG_MAX_LEN);
        xQueueSendFromISR(g_empty_buffer_queue, &ptr, NULL);
    }

    xTaskCreate(logger_task, "logger", FILE_LOGGER_STACK_SIZE, NULL, PRIORITY_LOW, NULL);
}

void logger_flush(void)
{
    char * null_ptr_to_flush = NULL;
    xQueueSend(g_write_buffer_queue, &null_ptr_to_flush, portMAX_DELAY);
}

void logger_log(logger_msg_t type, const char * filename, const char * func_name, unsigned line_num,
                const char * msg, ...)
{
    char * buffer = NULL;
    char * temp_ptr = NULL;
    uint32_t len = 0;
    const rtc_t time = rtc_gettime();
    const unsigned int uptime = sys_get_uptime_ms();

    /* This must match up with the logger_msg_t enumeration */
    const char * const type_str[] = { "invalid", "info", "warn", "error" };

    taskENTER_CRITICAL();
    if (!g_logger_initted)
    {
        g_logger_initted = true;
        logger_init();
    }
    taskEXIT_CRITICAL();

    // Find the back-slash or forward-slash to get filename only, not absolute or relative path
    if(0 != filename) {
        temp_ptr = strrchr(filename, '/');
        // If forward-slash not found, find back-slash
        if(0 == temp_ptr) temp_ptr = strrchr(filename, '\\');
        if(0 != temp_ptr) filename = temp_ptr+1;
    }
    else {
        filename = "";
    }

    if (0 == func_name) {
        func_name = "";
    }

    /* Get an available buffer to write the data to */
    xQueueReceive(g_empty_buffer_queue, &buffer, portMAX_DELAY);

    /* Write the header including time, filename, function name etc */
    len = sprintf(buffer, "%u/%u,%02d:%02d:%02d,%u,%s,%s,%s(),%u,",
                    (unsigned)time.month, (unsigned)time.day, (unsigned)time.hour,
                    (unsigned)time.min, (unsigned)time.sec, uptime,
                    type_str[type], filename, func_name, line_num
                    );

    /* Append actual user message */
    va_list args;
    va_start(args, msg);
    len += vsnprintf(buffer + len, FILE_LOGGER_MSG_MAX_LEN-len-1, msg, args);
    va_end(args);

    /* Append newline character */
    buffer[len] = '\n';
    buffer[++len] = '\0';

    /* Send the buffer to the queue for the logger task to write */
    xQueueSend(g_write_buffer_queue, &buffer, portMAX_DELAY);
}
