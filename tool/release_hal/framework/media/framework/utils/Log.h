#ifndef ASR_FP_MM_LOG_H
#define ASR_FP_MM_LOG_H

#ifndef LOG_TAG
#define LOG_TAG __FILE__
#endif

#define LOG_SEVERITY_V 1
#define LOG_SEVERITY_D 2
#define LOG_SEVERITY_I 3
#define LOG_SEVERITY_W 4
#define LOG_SEVERITY_E 5

#ifndef LOG_SEVERITY
#define LOG_SEVERITY LOG_SEVERITY_V
#endif

#ifndef LOG_NDEBUG
#ifdef NDEBUG
#define LOG_NDEBUG 1
#else
#define LOG_NDEBUG 0
#endif
#endif

#include "ui_log_api.h"

#ifdef CONFIG_RELEASE_WITH_NO_ASR_UI

#define LOG_PRINT_IF fatal_printf

#if LOG_NDEBUG
#define ALOGV(x, ...)
#define ALOGD(x, ...)
#else
#define ALOGV(x, ...) {if (LOG_SEVERITY <= LOG_SEVERITY_V) LOG_PRINT_IF("MMI:%s [%s]: " x "\n","V", LOG_TAG, ##__VA_ARGS__);}
#define ALOGD(x, ...) {if (LOG_SEVERITY <= LOG_SEVERITY_D) LOG_PRINT_IF("MMI:%s [%s]: " x "\n","D", LOG_TAG, ##__VA_ARGS__);}
#endif

#define ALOGI(x, ...) {if (LOG_SEVERITY <= LOG_SEVERITY_I) LOG_PRINT_IF("MMI:%s [%s]: " x "\n","I", LOG_TAG, ##__VA_ARGS__);}
#define ALOGW(x, ...) {if (LOG_SEVERITY <= LOG_SEVERITY_W) LOG_PRINT_IF("MMI:%s [%s]: " x "\n","W", LOG_TAG, ##__VA_ARGS__);}
#define ALOGE(x, ...) {if (LOG_SEVERITY <= LOG_SEVERITY_E) LOG_PRINT_IF("MMI:%s [%s]: " x "\n","E", LOG_TAG, ##__VA_ARGS__);}

#else

#define LOG_PRINT_IF raw_uart_log

#if LOG_NDEBUG
#define ALOGV(x, ...)
#define ALOGD(x, ...)
#else
#define ALOGV(x, ...) {if (LOG_SEVERITY <= LOG_SEVERITY_V) LOG_PRINT_IF(" %s [%s]: " x "\n","V", LOG_TAG, ##__VA_ARGS__);}
#define ALOGD(x, ...) {if (LOG_SEVERITY <= LOG_SEVERITY_D) LOG_PRINT_IF(" %s [%s]: " x "\n","D", LOG_TAG, ##__VA_ARGS__);}
#endif

#define ALOGI(x, ...) {if (LOG_SEVERITY <= LOG_SEVERITY_I) LOG_PRINT_IF(" %s [%s]: " x "\n","I", LOG_TAG, ##__VA_ARGS__);}
#define ALOGW(x, ...) {if (LOG_SEVERITY <= LOG_SEVERITY_W) LOG_PRINT_IF(" %s [%s]: " x "\n","W", LOG_TAG, ##__VA_ARGS__);}
#define ALOGE(x, ...) {if (LOG_SEVERITY <= LOG_SEVERITY_E) LOG_PRINT_IF(" %s [%s]: " x "\n","E", LOG_TAG, ##__VA_ARGS__);}

#endif

#endif
