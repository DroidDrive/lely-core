/**@file
 * This file is part of the C11 and POSIX compatibility library.
 *
 * @see lely/compat/time.h
 *
 * @copyright 2013-2020 Lely Industries N.V.
 *
 * @author J. S. Seldenthuis <jseldenthuis@lely.com>
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

#include "compat.h"

#include <lely/compat/time.h>
#include <lely/compat/unistd.h>

#if !LELY_NO_RT

#if _WIN32 && !defined(__MINGW32__)

#include <errno.h>

int
nanosleep(const struct timespec *rqtp, struct timespec *rmtp)
{
	int errsv = clock_nanosleep(CLOCK_REALTIME, 0, rqtp, rmtp);
	if (errsv) {
		errno = errsv;
		return -1;
	}
	return 0;
}

unsigned
sleep(unsigned seconds)
{
	struct timespec rqtp = { seconds, 0 };
	struct timespec rmtp = { 0, 0 };
	int errsv = errno;
	if (nanosleep(&rqtp, &rmtp) == -1) {
		errno = errsv;
		return (unsigned)rmtp.tv_sec;
	}

	return 0;
}

#endif // _WIN32 && !__MINGW32__

#endif // !LELY_NO_RT
