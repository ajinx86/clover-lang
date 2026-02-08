#ifndef CL_DIAGNOSTIC_H_
#define CL_DIAGNOSTIC_H_

#include "cl-core.h"
#include "cl-source.h"

#define diag_note(loc,msg,args...)    __cl_diag(CL_DIAG_NOTE,loc,msg,##args)
#define diag_info(loc,msg,args...)    __cl_diag(CL_DIAG_INFO,loc,msg,##args)
#define diag_warning(loc,msg,args...) __cl_diag(CL_DIAG_WARNING,loc,msg,##args)
#define diag_error(loc,msg,args...)   __cl_diag(CL_DIAG_ERROR,loc,msg,##args)


CL_ENUM(DiagType) {
    CL_DIAG_NOTE,   /* doesn't print code snippet */
    CL_DIAG_INFO,
    CL_DIAG_WARNING,
    CL_DIAG_ERROR,
    __CL_DIAG_MAX
};


CL_TYPE(DiagLocation) {
    uint32_t offset;
    uint32_t line_offset;
    uint32_t length;
    uint32_t line_length;

    uint32_t line;
    uint32_t column;

    uint32_t caret;

    Source *src;
};


void __cl_diag(DiagType type, DiagLocation loc, str_t msg, ...);

#endif /* CL_DIAGNOSTIC_H_ */
