#ifndef COMPILER_H_
#define COMPILER_H_

#include "cl-core.h"
#include "cl-bits.h"

#include "cl-vector.h"

bool cl_compile(Vector *files, str_t output_file);

#endif /* COMPILER_H_ */
