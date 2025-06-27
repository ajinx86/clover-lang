#ifndef CLOVER_LOG_H_
#define CLOVER_LOG_H_

#include <clover/base.hpp>

#define CLV_LOG_NONE        (0)
#define CLV_LOG_FORMAT      (1)
#define CLV_LOG_NEWLINE     (2)
#define CLV_LOG_ALL         (CLV_LOG_FORMAT | CLV_LOG_NEWLINE)

#define CLV_DEBUG           (0)
#define CLV_INFO            (1)
#define CLV_WARNING         (2)
#define CLV_ERROR           (3)

#define clv_log(level,msg,args...)  _clv_log0 (__FILE__, __LINE__, level, CLV_LOG_ALL, msg, ## args)
#define clv_nlog(level,msg,args...) _clv_log0 (__FILE__, __LINE__, level, CLV_LOG_FORMAT, msg, ## args)
#define clv_xlog(level,msg,args...) _clv_log0 (__FILE__, __LINE__, level, CLV_LOG_NONE, msg, ## args)

#define clv_debug(msg,args...)      clv_log (CLV_DEBUG, msg, ## args)
#define clv_info(msg,args...)       clv_log (CLV_INFO, msg, ## args)
#define clv_warning(msg,args...)    clv_log (CLV_WARNING, msg, ## args)
#define clv_error(msg,args...)      clv_log (CLV_ERROR, msg, ## args)

void _clv_log0 (clv_str file, int lineno, int level, int mode, clv_str msg, ...);

bool clv_log_debug ();

#endif /* CLOVER_LOG_H_ */
