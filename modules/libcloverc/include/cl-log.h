#ifndef CL_LOG_H_
#define CL_LOG_H_

#include "cl-core.h"
#include "cl-annotation.h"

#ifndef CL_LOG_SCOPE
#define CL_LOG_SCOPE NULL
#endif

#ifndef DEBUG
#define cl_debug(msg,args...)
#else
#define cl_debug(msg,args...)   __cl_log(CL_LOG_DEBUG,CL_LOG_SCOPE,msg,##args)
#endif /* DEBUG */

#define cl_info(msg,args...)    __cl_log(CL_LOG_INFO,CL_LOG_SCOPE,msg,##args)
#define cl_warning(msg,args...) __cl_log(CL_LOG_WARNING,CL_LOG_SCOPE,msg,##args)
#define cl_error(msg,args...)   __cl_log(CL_LOG_ERROR,CL_LOG_SCOPE,msg,##args)
#define cl_fatal(msg,args...)   __cl_log(CL_LOG_FATAL,CL_LOG_SCOPE,msg,##args)


CL_ENUM(LogLevel) {
    CL_LOG_DEBUG,
    CL_LOG_INFO,
    CL_LOG_WARNING,
    CL_LOG_ERROR,
    CL_LOG_FATAL,
    __CL_LOG_MAX
};


void __cl_log (LogLevel level, __Nullable str_t scope, str_t msg, ...)
    __Format(3, 4);

#endif /* CL_LOG_H_ */
