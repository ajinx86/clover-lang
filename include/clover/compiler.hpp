#ifndef CLOVER_COMPILER_H_
#define CLOVER_COMPILER_H_

#include <clover/base.hpp>
#include <clover/list.hpp>

bool clv_compile (clv_str _manifest, clv_list_t *files, clv_str output, bool debug);

#endif /* CLOVER_COMPILER_H_ */
