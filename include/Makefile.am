inc =

inc += lely/compat/sys/types.h
if !NO_CXX
inc += lely/compat/chrono.hpp
endif
inc += lely/compat/features.h
if !NO_CXX
inc += lely/compat/functional.hpp
endif
inc += lely/compat/stdatomic.h
inc += lely/compat/stddef.h
inc += lely/compat/stdint.h
if !NO_HOSTED
inc += lely/compat/stdio.h
inc += lely/compat/stdlib.h
endif
inc += lely/compat/string.h
inc += lely/compat/strings.h
if !NO_THREADS
inc += lely/compat/threads.h
endif
inc += lely/compat/time.h
if !NO_CXX
inc += lely/compat/type_traits.hpp
endif
inc += lely/compat/uchar.h
inc += lely/compat/unistd.h
if !NO_CXX
inc += lely/compat/utility.hpp
endif

inc += lely/tap/tap.h

if !ECSS_COMPLIANCE
inc += lely/util/bimap.h
inc += lely/util/coroutine.h
if !NO_CXX
inc += lely/util/coroutine.hpp
endif
inc += lely/util/sllist.h
inc += lely/util/spscring.h
endif # !ECSS_COMPLIANCE
inc += lely/util/def/type.def
inc += lely/util/bits.h
if !NO_MALLOC
inc += lely/util/bitset.h
endif
if !NO_CXX
inc += lely/util/chrono.hpp
endif
inc += lely/util/cmp.h
if !NO_MALLOC
inc += lely/util/config.h
endif
if !NO_DAEMON
inc += lely/util/daemon.h
endif
inc += lely/util/diag.h
inc += lely/util/dllist.h
inc += lely/util/endian.h
inc += lely/util/error.h
if !NO_CXX
inc += lely/util/error.hpp
endif
if !NO_MALLOC
inc += lely/util/fiber.h
if !NO_CXX
inc += lely/util/fiber.hpp
endif
endif # !NO_MALLOC
inc += lely/util/float.h
if !NO_STDIO
inc += lely/util/frbuf.h
inc += lely/util/fwbuf.h
endif
if !NO_CXX
inc += lely/util/invoker.hpp
endif
if !NO_STDIO
inc += lely/util/lex.h
endif
inc += lely/util/membuf.h
inc += lely/util/memory.h
if !NO_HOSTED
inc += lely/util/mkjmp.h
endif
if !NO_CXX
inc += lely/util/mutex.hpp
endif
inc += lely/util/pheap.h
if !NO_STDIO
inc += lely/util/print.h
endif
inc += lely/util/rbtree.h
if !NO_CXX
inc += lely/util/result.hpp
endif
if !NO_MALLOC
inc += lely/util/stop.h
if !NO_CXX
inc += lely/util/stop.hpp
endif
endif # !NO_MALLOC
inc += lely/util/time.h
inc += lely/util/ustring.h
inc += lely/util/util.h

inc += lely/can/buf.h
inc += lely/can/err.h
inc += lely/can/msg.h
inc += lely/can/net.h
if !NO_STDIO
if HAVE_SOCKET_CAN
inc += lely/can/socket.h
endif
if HAVE_VCI
inc += lely/can/vci.h
endif
endif # !NO_STDIO

if !NO_STDIO
inc += lely/io/addr.h
inc += lely/io/attr.h
inc += lely/io/can.h
inc += lely/io/file.h
inc += lely/io/if.h
inc += lely/io/io.h
inc += lely/io/pipe.h
inc += lely/io/poll.h
inc += lely/io/serial.h
inc += lely/io/sock.h
endif # !NO_STDIO

if !ECSS_COMPLIANCE
if !NO_CXX
inc += lely/ev/co_task.hpp
endif
inc += lely/ev/ev.h
inc += lely/ev/exec.h
if !NO_CXX
inc += lely/ev/exec.hpp
endif
inc += lely/ev/fiber_exec.h
if !NO_CXX
inc += lely/ev/fiber_exec.hpp
endif
inc += lely/ev/future.h
if !NO_CXX
inc += lely/ev/future.hpp
endif
inc += lely/ev/loop.h
if !NO_CXX
inc += lely/ev/loop.hpp
endif
inc += lely/ev/poll.h
if !NO_CXX
inc += lely/ev/poll.hpp
endif
inc += lely/ev/std_exec.h
inc += lely/ev/strand.h
if !NO_CXX
inc += lely/ev/strand.hpp
endif
inc += lely/ev/task.h
if !NO_CXX
inc += lely/ev/task.hpp
endif
inc += lely/ev/thrd_loop.h
if !NO_CXX
inc += lely/ev/thrd_loop.hpp
endif
endif # !ECSS_COMPLIANCE

if !ECSS_COMPLIANCE
inc += lely/io2/can/err.h
if !NO_CXX
inc += lely/io2/can/err.hpp
endif
inc += lely/io2/can/msg.h
if !NO_CXX
inc += lely/io2/can/msg.hpp
endif
if !NO_STDIO
if PLATFORM_LINUX
inc += lely/io2/linux/can.h
if !NO_CXX
inc += lely/io2/linux/can.hpp
endif
endif # PLATFORM_LINUX
if PLATFORM_POSIX
inc += lely/io2/posix/poll.h
if !NO_CXX
inc += lely/io2/posix/poll.hpp
endif
endif # PLATFORM_POSIX
endif # !NO_STDIO
inc += lely/io2/sys/clock.h
if !NO_CXX
inc += lely/io2/sys/clock.hpp
endif
inc += lely/io2/sys/io.h
if !NO_CXX
inc += lely/io2/sys/io.hpp
endif
if !NO_STDIO
inc += lely/io2/sys/sigset.h
if !NO_CXX
inc += lely/io2/sys/sigset.hpp
endif
inc += lely/io2/sys/timer.h
if !NO_CXX
inc += lely/io2/sys/timer.hpp
endif
endif # !NO_STDIO
inc += lely/io2/user/can.h
if !NO_CXX
inc += lely/io2/user/can.hpp
endif
inc += lely/io2/user/timer.h
if !NO_CXX
inc += lely/io2/user/timer.hpp
endif
if !NO_STDIO
if PLATFORM_WIN32
if HAVE_IXXAT
inc += lely/io2/win32/ixxat.h
if !NO_CXX
inc += lely/io2/win32/ixxat.hpp
endif
endif
inc += lely/io2/win32/poll.h
if !NO_CXX
inc += lely/io2/win32/poll.hpp
endif
endif # PLATFORM_WIN32
endif # !NO_STDIO
inc += lely/io2/can.h
if !NO_CXX
inc += lely/io2/can.hpp
endif
inc += lely/io2/can_net.h
if !NO_CXX
inc += lely/io2/can_net.hpp
endif
inc += lely/io2/can_rt.h
if !NO_CXX
inc += lely/io2/can_rt.hpp
endif
inc += lely/io2/clock.h
if !NO_CXX
inc += lely/io2/clock.hpp
inc += lely/io2/co_can.hpp
inc += lely/io2/co_can_rt.hpp
inc += lely/io2/co_sigset.hpp
inc += lely/io2/co_timer.hpp
inc += lely/io2/co_tqueue.hpp
endif
inc += lely/io2/ctx.h
if !NO_CXX
inc += lely/io2/ctx.hpp
endif
inc += lely/io2/dev.h
if !NO_CXX
inc += lely/io2/dev.hpp
endif
inc += lely/io2/event.h
if !NO_CXX
inc += lely/io2/event.hpp
endif
inc += lely/io2/io2.h
inc += lely/io2/sigset.h
if !NO_CXX
inc += lely/io2/sigset.hpp
endif
inc += lely/io2/timer.h
if !NO_CXX
inc += lely/io2/timer.hpp
endif
inc += lely/io2/tqueue.h
if !NO_CXX
inc += lely/io2/tqueue.hpp
endif
inc += lely/io2/vcan.h
if !NO_CXX
inc += lely/io2/vcan.hpp
endif
endif # !ECSS_COMPLIANCE

inc += lely/co/def/array.def
inc += lely/co/def/basic.def
inc += lely/co/def/time.def
inc += lely/co/def/type.def
inc += lely/co/detail/dev.h
inc += lely/co/detail/obj.h
inc += lely/co/co.h
inc += lely/co/crc.h
if !NO_CO_CSDO
inc += lely/co/csdo.h
endif
if !NO_CO_DCF
inc += lely/co/dcf.h
endif
inc += lely/co/dev.h
if !NO_CO_EMCY
inc += lely/co/emcy.h
endif
if !NO_CO_GW
inc += lely/co/gw.h
endif
if !NO_CO_GW_TXT
inc += lely/co/gw_txt.h
endif
if !NO_CO_LSS
inc += lely/co/lss.h
endif
inc += lely/co/nmt.h
inc += lely/co/obj.h
inc += lely/co/pdo.h
if !NO_CO_RPDO
inc += lely/co/rpdo.h
endif
if !NO_CO_SDEV
inc += lely/co/sdev.h
endif
inc += lely/co/sdo.h
inc += lely/co/ssdo.h
if !NO_CO_SYNC
inc += lely/co/sync.h
endif
if !NO_CO_TIME
inc += lely/co/time.h
endif
if !NO_CO_TPDO
inc += lely/co/tpdo.h
endif
inc += lely/co/type.h
inc += lely/co/val.h
if !NO_CO_WTM
inc += lely/co/wtm.h
endif

if !NO_CXX
if !NO_MALLOC
inc += lely/coapp/type_traits.hpp
inc += lely/coapp/device.hpp
if !NO_COAPP_MASTER
inc += lely/coapp/driver.hpp
inc += lely/coapp/fiber_driver.hpp
inc += lely/coapp/logical_driver.hpp
inc += lely/coapp/loop_driver.hpp
if !NO_CO_LSS
inc += lely/coapp/lss_master.hpp
endif
inc += lely/coapp/master.hpp
endif
inc += lely/coapp/node.hpp
inc += lely/coapp/sdo.hpp
inc += lely/coapp/sdo_error.hpp
if !NO_COAPP_SLAVE
inc += lely/coapp/slave.hpp
endif
endif # !NO_MALLOC
endif # !NO_CXX

nobase_include_HEADERS = $(inc)
