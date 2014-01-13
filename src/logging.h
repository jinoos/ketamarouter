#ifndef _LOGGING_H_
#define _LOGGING_H_

#include <stdio.h>
#include <time.h>
#include <stdarg.h>

#include <stdio.h>
#include <time.h>
#include <stdarg.h>
#include <inttypes.h>

#define _LOG_DEBUG  1 
#define _LOG_INFO   2
#define _LOG_WARN   3
#define _LOG_ERR	4
#define _LOG_MSG    5
#define _LOG_DEFAULT _LOG_WARN

#define _LOG_SDEBUG   "DEBUG"
#define _LOG_SINFO    "INFO "
#define _LOG_SWARN    "WARN "
#define _LOG_SERR     "ERROR"
#define _LOG_SMSG     "MSG  "

#ifdef LOG_FILELINE
#define	_LOG_FILELINE	1
#else
#define _LOG_FILELINE	0
#endif

#define log_debug(F, ...)	{log_str(_LOG_DEBUG, __FILE__, __LINE__, _LOG_SDEBUG, F, ##__VA_ARGS__);}
#define log_info(F, ...)	{log_str(_LOG_INFO, __FILE__, __LINE__, _LOG_SINFO, F, ##__VA_ARGS__);}
#define log_warn(F, ...)	{log_str(_LOG_WARN, __FILE__, __LINE__, _LOG_SWARN, F, ##__VA_ARGS__);}
#define log_err(F, ...)		{log_str(_LOG_ERR, __FILE__, __LINE__, _LOG_SERR, F, ##__VA_ARGS__);}
#define log_msg(F, ...)		{log_str(_LOG_MSG, __FILE__, __LINE__, _LOG_SMSG, F, ##__VA_ARGS__);}

// No new line
#define log_ndebug(F, ...)	{log_nstr(_LOG_DEBUG, __FILE__, __LINE__, _LOG_SDEBUG, F, ##__VA_ARGS__);}
#define log_ninfo(F, ...)	{log_nstr(_LOG_INFO, __FILE__, __LINE__, _LOG_SINFO, F, ##__VA_ARGS__);}
#define log_nwarn(F, ...)	{log_nstr(_LOG_WARN, __FILE__, __LINE__, _LOG_SWARN, F, ##__VA_ARGS__);}
#define log_nerr(F, ...)	{log_nstr(_LOG_ERR, __FILE__, __LINE__, _LOG_SERR, F, ##__VA_ARGS__);}
#define log_nmsg(F, ...)	{log_nstr(_LOG_MSG, __FILE__, __LINE__, _LOG_SMSG, F, ##__VA_ARGS__);}

inline void log_str(int level, const char *file, const int line, const char *level_str, const char *format, ...);
inline int log_level(int level);
inline FILE* log_fd(FILE *fd);

inline int log_debug_enable();
inline int log_info_enable();
inline int log_warn_enable();
inline int log_err_enable();
inline int log_msg_enable();

#endif // _LOGGING_H_
