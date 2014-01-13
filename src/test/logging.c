#include "logging.h"

static int _LOG_LEVEL = _LOG_DEFAULT;
static FILE *_LOG_FD = NULL;

inline int log_level(int level)
{
	if(level != 0) _LOG_LEVEL = level;
	return _LOG_LEVEL;
}

inline FILE* log_fd(FILE *fd)
{
	if(fd != NULL) _LOG_FD = fd;
	return _LOG_FD;
}

inline void log_str(int level, const char *file, const int line, const char
		*level_str, const char *format, ...)
{
	if(_LOG_LEVEL > level)
		return;

	if(_LOG_FD == NULL)
		_LOG_FD = stdout;

	time_t t = time(NULL);
	struct tm *tm = localtime(&t);

	if(_LOG_FILELINE)
	{
		fprintf(_LOG_FD, "%d-%02d-%02d %02d:%02d:%02d [%s] %s(%d) ",
				(tm->tm_year) + 1900, (tm->tm_mon) + 1, tm->tm_mday, tm->tm_hour,
				tm->tm_min, tm->tm_sec, level_str, file, line);
	}else
	{
		fprintf(_LOG_FD, "%d-%02d-%02d %02d:%02d:%02d [%s] ", (tm->tm_year) +
				1900, (tm->tm_mon) + 1, tm->tm_mday, tm->tm_hour, tm->tm_min,
				tm->tm_sec, level_str);
	}

	va_list argptr;
	va_start(argptr, format);
	vfprintf(_LOG_FD, format, argptr);
	va_end(argptr);
	fprintf(_LOG_FD, "\n");
	fflush(_LOG_FD);
}

inline void log_nstr(int level, const char *file, const int line, const char
		*level_str, const char *format, ...)
{
	if(_LOG_LEVEL > level)
		return;

	if(_LOG_FD == NULL)
		_LOG_FD = stdout;

	time_t t = time(NULL);
	struct tm *tm = localtime(&t);

	if(_LOG_FILELINE)
	{
		fprintf(_LOG_FD, "%d-%02d-%02d %02d:%02d:%02d [%s] %s(%d) ",
				(tm->tm_year) + 1900, (tm->tm_mon) + 1, tm->tm_mday, tm->tm_hour,
				tm->tm_min, tm->tm_sec, level_str, file, line);
	}else
	{
		fprintf(_LOG_FD, "%d-%02d-%02d %02d:%02d:%02d [%s] ", (tm->tm_year) +
				1900, (tm->tm_mon) + 1, tm->tm_mday, tm->tm_hour, tm->tm_min,
				tm->tm_sec, level_str);
	}

	va_list argptr;
	va_start(argptr, format);
	vfprintf(_LOG_FD, format, argptr);
	va_end(argptr);
	fflush(_LOG_FD);
}

inline int log_level_compare(int cur_level, int cmp_level)
{
    return cur_level <= cmp_level ? 1 : 0;
}

inline int log_debug_enable()
{
    return log_level_compare(log_level(0), _LOG_DEBUG);
}

inline int log_info_enable()
{
    return log_level_compare(log_level(0), _LOG_INFO);
}

inline int log_warn_enable()
{
    return log_level_compare(log_level(0), _LOG_WARN);
}

inline int log_err_enable()
{
    return log_level_compare(log_level(0), _LOG_ERR);
}

inline int log_msg_enable()
{
    return 1;
}
