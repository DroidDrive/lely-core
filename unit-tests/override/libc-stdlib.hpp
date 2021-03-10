/**@file
 * This file is part of the CANopen Library Unit Test Suite.
 *
 * @copyright 2021 N7 Space Sp. z o.o.
 *
 * Unit Test Suite was developed under a programme of,
 * and funded by, the European Space Agency.
 *
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef LELY_OVERRIDE_STDLIB_STDIO_H_
#define LELY_OVERRIDE_STDLIB_STDIO_H_

#include "libc-defs.hpp"

#if HAVE_LIBC_OVERRIDE

#include <cstdlib>

#if (__GNUC__ != 7) && (__GNUC__ != 8)
/* Overriding does not work on GCC 7 and 8 with -fprintf-return-value
 * optimization enabled (default) when using shared libraries.
 */
#define HAVE_REALLOC_OVERRIDE 1
#endif

/**
 * Override parameters.
 */
namespace LibCOverride {
#if HAVE_REALLOC_OVERRIDE
/**
 * Number of valid calls to realloc().
 */
static int realloc_vc = Override::AllCallsValid;
#endif
}  // namespace LibCOverride

/* realloc() override */
#if HAVE_REALLOC_OVERRIDE
#ifdef __GNUC__
void* realloc(void* ptr, size_t new_size) __THROWNL;
#endif

void*
realloc(void* ptr, size_t new_size) __THROWNL {
  (void)ptr;
  (void)new_size;

  if (LibCOverride::realloc_vc == Override::NoneCallsValid) return nullptr;

  if (LibCOverride::realloc_vc > Override::NoneCallsValid)
    --LibCOverride::realloc_vc;

  void* ret = malloc(new_size);
  if (ret != nullptr) free(ptr);

  return ret;
}
#endif  // HAVE_REALLOC_OVERRIDE
/* end of realloc() override */

#endif  // HAVE_LIBC_OVERRIDE

#endif  // LELY_OVERRIDE_STDLIB_STDIO_H_
