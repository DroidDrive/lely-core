/*!\file
 * This header file is part of the utilities library; it contains the diagnostic
 * declarations.
 *
 * \copyright 2016 Lely Industries N.V.
 *
 * \author J. S. Seldenthuis <jseldenthuis@lely.com>
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

#ifndef LELY_UTIL_DIAG_H
#define LELY_UTIL_DIAG_H

#include <lely/util/errnum.h>

#include <stdarg.h>
#include <stddef.h>

//! A location in a text file.
struct floc {
	//! The name of the file.
	const char *filename;
	//! The line number (starting from 1).
	int line;
	//! The column number (starting from 1).
	int column;
};

//! The severity of a diagnostic message.
enum diag_severity {
	//! A debug message.
	DIAG_DEBUG,
	//! An informational message.
	DIAG_INFO,
	//! A warning.
	DIAG_WARNING,
	//! An error.
	DIAG_ERROR,
	//! A fatal error, which SHOULD result in program termination.
	DIAG_FATAL
};

#ifdef __cplusplus
extern "C" {
#endif

/*!
 * The function type of a handler for diag().
 *
 * \param handle   the extra argument specified to diag_set_handler().
 * \param severity the severity of the message.
 * \param errc     the native error code.
 * \param format   a printf-style format string.
 * \param ap       the list with arguments to be printed according to \a format.
 */
typedef void diag_handler_t(void *handle, enum diag_severity severity,
		errc_t errc, const char *format, va_list ap)
		__format_printf(4, 0);

/*!
 * The function type of a handler for diag_at().
 *
 * \param handle   the extra argument specified to diag_at_set_handler().
 * \param severity the severity of the message.
 * \param errc     the native error code.
 * \param at       a pointer the location in a text file (can be NULL).
 * \param format   a printf-style format string.
 * \param ap       the list with arguments to be printed according to \a format.
 */
typedef void diag_at_handler_t(void *handle, enum diag_severity severity,
		errc_t errc, const struct floc *at, const char *format,
		va_list ap) __format_printf(5, 0);

/*!
 * Increments a file location with a text string. This function assumes a tab
 * stop of 8 spaces.
 *
 * \param at a pointer to a file location.
 * \param s  a pointer to the string to be parsed.
 *
 * \see floc_strninc().
 */
LELY_UTIL_EXTERN void floc_strinc(struct floc *at, const char *s);

/*!
 * Increments a file location with a text string. This function is similar to
 * floc_strinc(), except that it only considers the first \a n characters of the
 * string.
 */
LELY_UTIL_EXTERN void floc_strninc(struct floc *at, const char *s, size_t n);

/*!
 * Prints a file location to a string buffer. The filename, line and column are
 * separated by a colon. If the line or column is zero, it is omitted from the
 * output. This allows the user to print a partial location if, for example, the
 * column is unknown or meaningless.
 *
 * \param s  the address of the output buffer. If \a s is not NULL, at most
 *           `n - 1` characters are written, plus a terminating null byte.
 * \param n  the size (in bytes) of the buffer at \a s. If \a n is zero, nothing
 *           is written.
 * \param at a pointer the location in a text file.
 *
 * \returns the number of characters that would have been written had the
 * buffer been sufficiently large, not counting the terminating null byte, or a
 * negative number on error. In the latter case, the error number is stored in
 * `errno`.
 */
LELY_UTIL_EXTERN int snprintf_floc(char *s, size_t n, const struct floc *at);

/*!
 * Retrieves the handler function for diag().
 *
 * \param phandler the address at which to store a pointer to the handler
 *                 function (can be NULL).
 * \param phandle  the address at which to store the pointer to the extra
 *                 argument for the handler function (can be NULL).
 *
 * \see diag_set_handler()
 */
LELY_UTIL_EXTERN void diag_get_handler(diag_handler_t **phandler,
		void **phandle);

/*!
 * Sets the handler function for diag().
 *
 * \param handler a pointer to the new handler function.
 * \param handle  an optional pointer to an extra argument for the handler
 *                function (can be NULL).
 *
 * \see diag_get_handler()
 */
LELY_UTIL_EXTERN void diag_set_handler(diag_handler_t *handler,
		void *handle);

/*!
 * Retrieves the handler function for diag_at().
 *
 * \param phandler the address at which to store a pointer to the handler
 *                 function (can be NULL).
 * \param phandle  the address at which to store the pointer to the extra
 *                 argument for the handler function (can be NULL).
 *
 * \see diag_at_set_handler()
 */
LELY_UTIL_EXTERN void diag_at_get_handler(diag_at_handler_t **phandler,
		void **phandle);

/*!
 * Sets the handler function for diag_at().
 *
 * \param handler a pointer to the new handler function.
 * \param handle  an optional pointer to an extra argument for the handler
 *                function (can be NULL).
 *
 * \see diag_at_get_handler()
 */
LELY_UTIL_EXTERN void diag_at_set_handler(diag_at_handler_t *handler,
		void *handle);

/*!
 * Emits a diagnostic message. This function SHOULD print the severity of the
 * diagnostic followed by the user-specified message. If the specified severity
 * is #DIAG_FATAL, the program SHOULD be terminated after printing the message.
 *
 * The default error handler can be changed with diag_set_handler(), in which
 * case the new handler is responsible for terminating the program in case of an
 * error with severity #DIAG_FATAL.
 *
 * \param severity the severity of the message.
 * \param errc     the native error code generated by a system call or library
 *                 function, given by get_errc(). If errc is non-zero, a text
 *                 string describing the error SHOULD be printed after the
 *                 user-specified message.
 * \param format   a printf-style format string.
 * \param ...      an optional list of arguments to be printed according to
 *                 \a format.
 *
 * \see diag_set_handler(), diag_at()
 */
LELY_UTIL_EXTERN void diag(enum diag_severity severity, errc_t errc,
		const char *format, ...) __format_printf(3, 4);

/*!
 * Emits a diagnostic message. This function is equivalent to diag(), except
 * that it accepts a `va_list` instead of a variable number of arguments.
 *
 * \see diag()
 */
LELY_UTIL_EXTERN void vdiag(enum diag_severity severity, errc_t errc,
		const char *format, va_list ap) __format_printf(3, 0);

/*!
 * Emits a diagnostic message occurring at a location in a text file. This
 * function SHOULD print the location in a text file, where the diagnostic
 * presumably occurred, followed by the severity and a user-specified message.
 * If the specified severity is #DIAG_FATAL, the program SHOULD be terminated
 * after printing the message.
 *
 * The default error handler can be changed with diag_at_set_handler(), in which
 * case the new handler is responsible for terminating the program in case of an
 * error with severity #DIAG_FATAL.
 *
 * \param severity the severity of the message.
 * \param errc     the native error code generated by a system call or library
 *                 function, given by get_errc(). If errc is non-zero, a text
 *                 string describing the error will be printed after the
 *                 user-specified message.
 * \param at       a pointer the location in a text file (can be NULL). If \a at
 *                 is not NULL, the location SHOULD be printed before the
 *                 severity.
 * \param format   a printf-style format string.
 * \param ...      an optional list of arguments to be printed according to
 *                 \a format.
 *
 * \see diag_at_set_handler(), diag()
 */
LELY_UTIL_EXTERN void diag_at(enum diag_severity severity, errc_t errc,
		const struct floc *at, const char *format, ...)
		__format_printf(4, 5);

/*!
 * Emits a diagnostic message occurring at a location in a text file. This
 * function is equivalent to diag_at(), except that it accepts a `va_list`
 * instead of a variable number of arguments.
 *
 * \see diag_at()
 */
LELY_UTIL_EXTERN void vdiag_at(enum diag_severity severity, errc_t errc,
		const struct floc *at, const char *format, va_list ap)
		__format_printf(4, 0);

/*!
 * The default diag() handler. This function prints the string generated by
 * vsnprintf_diag() to `stderr`. If \a severity equals #DIAG_FATAL, this
 * function calls `exit(EXIT_FAILURE)` instead of returning.
 */
LELY_UTIL_EXTERN void default_diag_handler(void *handle,
		enum diag_severity severity, errc_t errc, const char *format,
		va_list ap) __format_printf(4, 0);

/*!
 * The default diag_at() handler. This function is similar to
 * default_diag_handler(), except that it prints the string generated by
 * vsnprintf_diag_at().
 */
LELY_UTIL_EXTERN void default_diag_at_handler(void *handle,
		enum diag_severity severity, errc_t errc, const struct floc *at,
		const char *format, va_list ap) __format_printf(5, 0);

/*!
 * The diag() handler used for command-line programs. This function prints the
 * string at \a handle, which SHOULD point to the name of the program (see
 * cmdname()), and then calls default_diag_handler().
 */
LELY_UTIL_EXTERN void cmd_diag_handler(void *handle,
		enum diag_severity severity, errc_t errc, const char *format,
		va_list ap) __format_printf(4, 0);

/*!
 * The diag() handler for daemons. On Windows this function is equivalent to
 * dialog_diag_handler(), on other platforms to syslog_diag_handler().
 */
LELY_UTIL_EXTERN void daemon_diag_handler(void *handle,
		enum diag_severity severity, errc_t errc, const char *format,
		va_list ap) __format_printf(4, 0);

/*!
 * The diag_at() handler for daemons. This function is similar to
 * daemon_diag_handler(), except that it prints the string generated by
 * vsnprintf_diag_at().
 */
LELY_UTIL_EXTERN void daemon_diag_at_handler(void *handle,
		enum diag_severity severity, errc_t errc, const struct floc *at,
		const char *format, va_list ap) __format_printf(5, 0);

/*!
 * The diag() handler for dialog boxes. It expects \a handle to point to the
 * caption and prints the string generated by vsnprintf_diag() as the message.
 * Note that this function is only available on Windows.
 */
LELY_UTIL_EXTERN void dialog_diag_handler(void *handle,
		enum diag_severity severity, errc_t errc, const char *format,
		va_list ap) __format_printf(4, 0);

/*!
 * The diag_at() handler for dialog boxes. This function is similar to
 * dialog_diag_handler(), except that it prints the string generated by
 * vsnprintf_diag_at().
 */
LELY_UTIL_EXTERN void dialog_diag_at_handler(void *handle,
		enum diag_severity severity, errc_t errc, const struct floc *at,
		const char *format, va_list ap) __format_printf(5, 0);

/*!
 * The diag() handler for log files. This function is similar to
 * default_diag_handler(), except that it prepends each message with a
 * RFC-2822-compliant timestamp.
 */
LELY_UTIL_EXTERN void log_diag_handler(void *handle,
		enum diag_severity severity, errc_t errc, const char *format,
		va_list ap) __format_printf(4, 0);

/*!
 * The diag_at() handler used for log-files. This function is similar to
 * default_diag_at_handler(), except that it prepends each message with a
 * RFC-2822-compliant timestamp.
 */
LELY_UTIL_EXTERN void log_diag_at_handler(void *handle,
		enum diag_severity severity, errc_t errc, const struct floc *at,
		const char *format, va_list ap) __format_printf(5, 0);

/*!
 * The diag() handler used for the system logging facilities. On POSIX platforms
 * this function invokes syslog(); on other platforms this function is
 * equivalent to log_diag_handler().
 */
LELY_UTIL_EXTERN void syslog_diag_handler(void *handle,
		enum diag_severity severity, errc_t errc, const char *format,
		va_list ap) __format_printf(4, 0);

/*!
 * The diag_at() handler used for the system logging facilities. On POSIX
 * platforms this function invokes syslog(); on other platforms this function is
 * equivalent to log_diag_at_handler().
 */
LELY_UTIL_EXTERN void syslog_diag_at_handler(void *handle,
		enum diag_severity severity, errc_t errc, const struct floc *at,
		const char *format, va_list ap) __format_printf(5, 0);

/*!
 * Prints a diagnostic message to a string buffer. This function prints the
 * severity of the message (except in the case of #DIAG_INFO), followed by a
 * colon and the user-specified message.  If \a errc is non-zero, the message is
 * followed by another colon and the result of errc2str().
 *
 * \param s        the address of the output buffer. If \a s is not NULL, at
 *                 most `n - 1` characters are written, plus a terminating null
 *                 byte.
 * \param n        the size (in bytes) of the buffer at \a s. If \a n is zero,
 *                 nothing is written.
 * \param severity the severity of the message.
 * \param errc     the native error code generated by a system call or library
 *                 function, given by get_errc(). If errc is non-zero, a text
 *                 string describing the error will be printed after the
 *                 user-specified message.
 * \param format   a printf-style format string.
 * \param ap       the list of arguments to be printed according to \a format.
 *
 * \returns the number of characters that would have been written had the
 * buffer been sufficiently large, not counting the terminating null byte, or a
 * negative number on error. In the latter case, the error number is stored in
 * `errno`.
 *
 * \see vsnprintf_diag_at()
 */
LELY_UTIL_EXTERN int vsnprintf_diag(char *s, size_t n,
		enum diag_severity severity, errc_t errc, const char *format,
		va_list ap) __format_printf(5, 0);

/*!
 * Equivalent to vsnprintf_diag(), except that it allocates a string large
 * enough to hold the output, including the terminating null byte.
 *
 * \param ps       the address of a value which, on success, contains a pointer
 *                 to the allocated string. This pointer SHOULD be passed to
 *                 `free()` to release the allocated storage.
 * \param severity the severity of the message.
 * \param errc     the native error code generated by a system call or library
 *                 function, given by get_errc(). If errc is non-zero, a text
 *                 string describing the error will be printed after the
 *                 user-specified message.
 * \param format   a printf-style format string.
 * \param ap       the list of arguments to be printed according to \a format.
 *
 * \returns the number of characters written, not counting the terminating null
 * byte, or a negative number on error. In the latter case, the error number is
 * stored in `errno`.
 *
 * \see vasprintf_diag_at()
 */
LELY_UTIL_EXTERN int vasprintf_diag(char **ps, enum diag_severity severity,
		errc_t errc, const char *format, va_list ap)
		__format_printf(4, 0);

/*!
 * Prints a diagnostic message occurring at a location in a text file to a
 * string buffer. This function prints the location in a text file, where the
 * diagnostic presumably occurred, followed a colon and the severity of the
 * message (except in the case of #DIAG_INFO), followed by a colon and the
 * user-specified message. If \a errc is non-zero, the message is followed by
 * another colon and the result of errc2str().
 *
 * \param s        the address of the output buffer. If \a s is not NULL, at
 *                 most `n - 1` characters are written, plus a terminating null
 *                 byte.
 * \param n        the size (in bytes) of the buffer at \a s. If \a n is zero,
 *                 nothing is written.
 * \param severity the severity of the message.
 * \param errc     the native error code generated by a system call or library
 *                 function, given by get_errc(). If errc is non-zero, a text
 *                 string describing the error will be printed after the
 *                 user-specified message.
 * \param at       a pointer the location in a text file (can be NULL). If \a at
 *                 is not NULL, the location will be printed before the
 *                 severity according to snprintf_floc().
 * \param format   a printf-style format string.
 * \param ap       the list of arguments to be printed according to \a format.
 *
 * \returns the number of characters that would have been written had the
 * buffer been sufficiently large, not counting the terminating null byte, or a
 * negative number on error. In the latter case, the error number is stored in
 * `errno`.
 *
 * \see vsnprintf_diag()
 */
LELY_UTIL_EXTERN int vsnprintf_diag_at(char *s, size_t n,
		enum diag_severity severity, errc_t errc, const struct floc *at,
		const char *format, va_list ap) __format_printf(6, 0);

/*!
 * Equivalent to vsnprintf_diag_at(), except that it allocates a string large
 * enough to hold the output, including the terminating null byte.
 *
 * \param ps       the address of a value which, on success, contains a pointer
 *                 to the allocated string. This pointer SHOULD be passed to
 *                 `free()` to release the allocated storage.
 * \param severity the severity of the message.
 * \param errc     the native error code generated by a system call or library
 *                 function, given by get_errc(). If errc is non-zero, a text
 *                 string describing the error will be printed after the
 *                 user-specified message.
 * \param at       a pointer the location in a text file (can be NULL). If \a at
 *                 is not NULL, the location will be printed before the
 *                 severity according to snprintf_floc().
 * \param format   a printf-style format string.
 * \param ap       the list of arguments to be printed according to \a format.
 *
 * \returns the number of characters written, not counting the terminating null
 * byte, or a negative number on error. In the latter case, the error number is
 * stored in `errno`.
 *
 * \see vasprintf_diag()
 */
LELY_UTIL_EXTERN int vasprintf_diag_at(char **ps, enum diag_severity severity,
		errc_t errc, const struct floc *at, const char *format,
		va_list ap) __format_printf(5, 0);

/*!
 * Extracts the command name from a path. This function returns a pointer to the
 * first character after the last separator ('/' or '\\') in \a path.
 */
LELY_UTIL_EXTERN const char *cmdname(const char *path);

#ifdef __cplusplus
}
#endif

#endif

