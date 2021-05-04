/* config.h.  Generated from config.h.in by configure.  */
/* config.h.in.  Generated from configure.ac by autoheader.  */

/* Define to 1 if you have the <bluetooth/bluetooth.h> header file. */
/* #undef HAVE_BLUETOOTH_BLUETOOTH_H */

/* Define to 1 if you have the <bluetooth/rfcomm.h> header file. */
/* #undef HAVE_BLUETOOTH_RFCOMM_H */

/* Define to 1 if you have the <cantype.h> header file. */
/* #undef HAVE_CANTYPE_H */

/* Define to 1 if CAN_RAW_FD_FRAMES is defined. */
#define HAVE_CAN_RAW_FD_FRAMES 1

/* define if the compiler supports basic C++11 syntax */
#define HAVE_CXX11 1

/* Define to 1 if you have the <dlfcn.h> header file. */
#define HAVE_DLFCN_H 1

/* Define to 1 if you have the <ifaddrs.h> header file. */
#define HAVE_IFADDRS_H 1

/* Define to 1 if you have the <inttypes.h> header file. */
#define HAVE_INTTYPES_H 1

/* Define to 1 if you have the <linux/can/error.h> header file. */
#define HAVE_LINUX_CAN_ERROR_H 1

/* Define to 1 if you have the <linux/can.h> header file. */
#define HAVE_LINUX_CAN_H 1

/* Define to 1 if you have the <linux/can/netlink.h> header file. */
#define HAVE_LINUX_CAN_NETLINK_H 1

/* Define to 1 if you have the <linux/can/raw.h> header file. */
#define HAVE_LINUX_CAN_RAW_H 1

/* Define to 1 if you have the <linux/rtnetlink.h> header file. */
#define HAVE_LINUX_RTNETLINK_H 1

/* Define to 1 if you have the <memory.h> header file. */
#define HAVE_MEMORY_H 1

/* Have PTHREAD_PRIO_INHERIT. */
/* #undef HAVE_PTHREAD_PRIO_INHERIT */

/* Define to 1 if you have the <stdint.h> header file. */
#define HAVE_STDINT_H 1

/* Define to 1 if you have the <stdlib.h> header file. */
#define HAVE_STDLIB_H 1

/* Define to 1 if you have the <strings.h> header file. */
#define HAVE_STRINGS_H 1

/* Define to 1 if you have the <string.h> header file. */
#define HAVE_STRING_H 1

/* Define to 1 if you have the <sys/epoll.h> header file. */
#define HAVE_SYS_EPOLL_H 1

/* Define to 1 if you have the <sys/ioctl.h> header file. */
#define HAVE_SYS_IOCTL_H 1

/* Define to 1 if you have the <sys/stat.h> header file. */
#define HAVE_SYS_STAT_H 1

/* Define to 1 if you have the <sys/types.h> header file. */
#define HAVE_SYS_TYPES_H 1

/* Define to 1 if you have the <unistd.h> header file. */
#define HAVE_UNISTD_H 1

/* kikass13 */
/* DO SOME STUFF */
/* ########################################################################## */
#define LELY_KEEP_CPP_API_STUFF 1
#define LELY_NO_DCF_VIA_FILESYSTEM 1
#define LELY_NO_CO_DCF_RESTORE 1
#define LELY_IO_USER_CAN_RXLEN 32
#define LELY_NO_MALLOC 1
#define LELY_NO_CO_OBJ_NAME 1
//#define LELY_NO_CO_OBJ_UPLOAD 1
#define LELY_NO_CO_OBJ_LIMITS 1
#define LELY_NO_THREADS 1
/* ########################################################################## */

/* Define to 1 if you have the IXXAT VCI SDK. */
/* #undef LELY_HAVE_IXXAT */

/* Define to 1 if you have SocketCAN. */
#define LELY_HAVE_SOCKET_CAN 1

/* Define to 1 if you have the <valgrind/valgrind.h> header file. */
/* #undef LELY_HAVE_VALGRIND */

/* Define to 1 if you have the IXXAT VCI SDK. */
/* #undef LELY_HAVE_VCI */

/* Define to 1 if CAN FD support is disabled. */
/* #undef LELY_NO_CANFD */

/* Define to 1 if C++ CANopen application master support is disabled. */
/* #undef LELY_NO_COAPP_MASTER */

/* Define to 1 if C++ CANopen application slave support is disabled. */
/* #undef LELY_NO_COAPP_SLAVE */

/* Define to 1 if Client-SDO support is disabled. */
/* #undef LELY_NO_CO_CSDO */

/* Define to 1 if EDS/DCF support is disabled. */
/* #undef LELY_NO_CO_DCF */

/* Define to 1 if concise DCF of the application parameters is not (re)stored.
   */
/* #undef LELY_NO_CO_DCF_RESTORE */

/* Define to 1 if emergency (EMCY) object support is disabled. */
/* #undef LELY_NO_CO_EMCY */

/* Define to 1 if gateway support is disabled. */
/* #undef LELY_NO_CO_GW */

/* Define to 1 if ASCII gateway support is disabled. */
/* #undef LELY_NO_CO_GW_TXT */

/* Define to 1 if Layer Setting Services (LSS) and protocols support is
   disabled. */
/* #undef LELY_NO_CO_LSS */

/* Define to 1 if master support is disabled. */
/* #undef LELY_NO_CO_MASTER */

/* Define to 1 if node guarding support is disabled. */
/* #undef LELY_NO_CO_NG */

/* Define to 1 if NMT boot slave support is disabled. */
/* #undef LELY_NO_CO_NMT_BOOT */

/* Define to 1 if NMT configuration request support is disabled. */
/* #undef LELY_NO_CO_NMT_CFG */

/* Define to 1 if default values are disabled in the object dictionary. */
/* #undef LELY_NO_CO_OBJ_DEFAULT */

/* Define to 1 if UploadFile/DownloadFile support is disabled for the object
   dictionary. */
/* #undef LELY_NO_CO_OBJ_FILE */

/* Define to 1 if minimum/maximum values are disabled in the object
   dictionary. */
/* #undef LELY_NO_CO_OBJ_LIMITS */

/* Define to 1 if names are disabled in the object dictionary. */
/* #undef LELY_NO_CO_OBJ_NAME */

/* Define to 1 if custom upload indication functions are disabled in the
   object dictionary. */
/* #undef LELY_NO_CO_OBJ_UPLOAD */

/* Define to 1 if Receive-PDO support is disabled. */
/* #undef LELY_NO_CO_RPDO */

/* Define to 1 if static device description support is disabled. */
/* #undef LELY_NO_CO_SDEV */

/* Define to 1 if synchronization (SYNC) object support is disabled. */
/* #undef LELY_NO_CO_SYNC */

/* Define to 1 if time stamp (TIME) object support is disabled. */
/* #undef LELY_NO_CO_TIME */

/* Define to 1 if Transmit-PDO support is disabled. */
/* #undef LELY_NO_CO_TPDO */

/* Define to 1 if Wireless Transmission Media (WTM) support is disabled. */
/* #undef LELY_NO_CO_WTM */

/* Define to 1 if C++ support is disabled. */
/* #undef LELY_NO_CXX */

/* Define to 1 if daemon support is disabled. */
/* #undef LELY_NO_DAEMON */

/* Define to 1 if diagnostic functions are disabled. */
/* #undef LELY_NO_DIAG */

/* Define to 1 if errno is disabled. */
/* #undef LELY_NO_ERRNO */

/* Define to 1 if a freestanding instead of a hosted environment is used. */
/* #undef LELY_NO_HOSTED */

/* Define to 1 if dynamic memory allocation is disabled. */
/* #undef LELY_NO_MALLOC */

/* Define to 1 if Realtime Extensions are disabled. */
/* #undef LELY_NO_RT */

/* Define to 1 if standard I/O is disabled. */
/* #undef LELY_NO_STDIO */

/* Define to 1 if multithreading support is disabled. */
/* #undef LELY_NO_THREADS */

/* Define to the sub-directory where libtool stores uninstalled libraries. */
#define LT_OBJDIR ".libs/"

/* Name of package */
#define PACKAGE "lely-core"

/* Define to the address where bug reports for this package should be sent. */
#define PACKAGE_BUGREPORT ""

/* Define to the full name of this package. */
#define PACKAGE_NAME "Lely core libraries"

/* Define to the full name and version of this package. */
#define PACKAGE_STRING "Lely core libraries 3.0.2"

/* Define to the one symbol short name of this package. */
#define PACKAGE_TARNAME "lely-core"

/* Define to the home page for this package. */
#define PACKAGE_URL ""

/* Define to the version of this package. */
#define PACKAGE_VERSION "3.0.2"

/* Define to necessary symbol if this constant uses a non-standard name on
   your system. */
/* #undef PTHREAD_CREATE_JOINABLE */

/* Define to 1 if you have the ANSI C header files. */
#define STDC_HEADERS 1

/* Enable extensions on AIX 3, Interix.  */
#ifndef _ALL_SOURCE
# define _ALL_SOURCE 1
#endif
/* Enable GNU extensions on systems that have them.  */
#ifndef _GNU_SOURCE
# define _GNU_SOURCE 1
#endif
/* Enable threading extensions on Solaris.  */
#ifndef _POSIX_PTHREAD_SEMANTICS
# define _POSIX_PTHREAD_SEMANTICS 1
#endif
/* Enable extensions on HP NonStop.  */
#ifndef _TANDEM_SOURCE
# define _TANDEM_SOURCE 1
#endif
/* Enable general extensions on Solaris.  */
#ifndef __EXTENSIONS__
# define __EXTENSIONS__ 1
#endif


/* Version number of package */
#define VERSION "3.0.2"

/* Define to 1 if on MINIX. */
/* #undef _MINIX */

/* Define to 2 if the system does not provide POSIX.1 features except with
   this defined. */
/* #undef _POSIX_1_SOURCE */

/* Define to 1 if you need to in order for `stat' and other things to work. */
/* #undef _POSIX_SOURCE */
