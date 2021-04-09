/**@file
 * This file is part of the CANopen Library Unit Test Suite.
 *
 * @copyright 2020-2021 N7 Space Sp. z o.o.
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <memory>

#include <CppUTest/TestHarness.h>

#include <lely/can/net.h>
#include <lely/co/csdo.h>
#include <lely/co/sdo.h>
#include <lely/co/type.h>
#include <lely/co/val.h>
#include <lely/util/endian.h>
#include <lely/util/membuf.h>

#include <libtest/allocators/default.hpp>
#include <libtest/allocators/limited.hpp>
#include <libtest/override/lelyco-val.hpp>
#include <libtest/tools/lely-unit-test.hpp>
#include <libtest/tools/lely-cpputest-ext.hpp>

#include "holder/dev.hpp"
#include "holder/obj.hpp"
#include "holder/array-init.hpp"

#ifndef LELY_MEMBUF_SIZE
/// The initial size (in bytes) of a memory buffer.
#define LELY_MEMBUF_SIZE 16
#endif

using sub_type = co_unsigned16_t;

TEST_GROUP(CO_CsdoInit) {
  const co_unsigned8_t CSDO_NUM = 0x01u;
  static const co_unsigned8_t DEV_ID = 0x01u;
  co_csdo_t* csdo = nullptr;
  co_dev_t* dev = nullptr;
  can_net_t* failing_net = nullptr;
  can_net_t* net = nullptr;
  std::unique_ptr<CoDevTHolder> dev_holder;
  Allocators::Default defaultAllocator;
  Allocators::Limited limitedAllocator;

  TEST_SETUP() {
    LelyUnitTest::DisableDiagnosticMessages();
    net = can_net_create(defaultAllocator.ToAllocT());
    CHECK(net != nullptr);

    limitedAllocator.LimitAllocationTo(can_net_sizeof());
    failing_net = can_net_create(limitedAllocator.ToAllocT());
    CHECK(failing_net != nullptr);

    dev_holder.reset(new CoDevTHolder(DEV_ID));
    dev = dev_holder->Get();
    CHECK(dev != nullptr);
  }

  TEST_TEARDOWN() {
    dev_holder.reset();
    can_net_destroy(net);
    can_net_destroy(failing_net);
  }
};

/// @name co_csdo_alignof()
///@{

/// \Given N/A
///
/// \When co_csdo_alignof() is called
///
/// \Then if \__MINGW32__ and !__MINGW64__, 4 is returned; else 8 is returned
///      \Calls _Alignof()
TEST(CO_CsdoInit, CoCsdoAlignof_Nominal) {
  const auto ret = co_csdo_alignof();

#if defined(__MINGW32__) && !defined(__MINGW64__)
  CHECK_EQUAL(4u, ret);
#else
  CHECK_EQUAL(8u, ret);
#endif
}

///@}

/// @name co_csdo_sizeof()
///@{

/// \Given N/A
///
/// \When co_csdo_sizeof() is called
///
/// \Then if LELY_NO_MALLOC or \__MINGW64__: 256 is returned;
///       else if \__MINGW32__ and !__MINGW64__: 108 is returned;
///       else: 248 is returned
TEST(CO_CsdoInit, CoCsdoSizeof_Nominal) {
  const auto ret = co_csdo_sizeof();

#if defined(LELY_NO_MALLOC) || defined(__MINGW64__)
  CHECK_EQUAL(256u, ret);
#else
#if defined(__MINGW32__) && !defined(__MINGW64__)
  CHECK_EQUAL(108u, ret);
#else
  CHECK_EQUAL(248u, ret);
#endif
#endif  // LELY_NO_MALLOC
}

///@}

/// @name co_csdo_create()
///@{

/// \Given a pointer to the device (co_dev_t)
///
/// \When co_csdo_create() is called with a pointer to the network (can_net_t)
///       with a failing allocator, the pointer to the device and a CSDO number
///       but CSDO allocation fails
///
/// \Then a null pointer is returned
///       \Calls co_csdo_alloc()
///       \Calls get_errc()
///       \Calls set_errc()
TEST(CO_CsdoInit, CoCsdoCreate_FailCsdoAlloc) {
  co_csdo_t* const csdo = co_csdo_create(failing_net, dev, CSDO_NUM);

  POINTERS_EQUAL(nullptr, csdo);
  CHECK_EQUAL(ERRNUM_INVAL, get_errnum());
}

/// \Given a pointer to the device (co_dev_t)
///
/// \When co_csdo_create() is called with a pointer to the network (can_net_t),
///       the pointer to the device and a CSDO number equal zero
///
/// \Then a null pointer is returned
///       \Calls co_csdo_alloc()
///       \Calls co_csdo_init()
///       \Calls get_errc()
///       \Calls co_csdo_free()
///       \Calls set_errc()
TEST(CO_CsdoInit, CoCsdoCreate_NumZero) {
  const co_unsigned8_t CSDO_NUM = 0;

  co_csdo_t* const csdo = co_csdo_create(net, dev, CSDO_NUM);

  POINTERS_EQUAL(nullptr, csdo);
  CHECK_EQUAL(ERRNUM_INVAL, get_errnum());
}

/// \Given a pointer to the device (co_dev_t)
///
/// \When co_csdo_create() is called with a pointer to the network (can_net_t),
///       the pointer to the device and a CSDO number higher than CO_NUM_SDOS
///
/// \Then a null pointer is returned
///       \Calls co_csdo_alloc()
///       \Calls co_csdo_init()
///       \Calls get_errc()
///       \Calls co_csdo_free()
///       \Calls set_errc()
TEST(CO_CsdoInit, CoCsdoCreate_NumTooHigh) {
  const co_unsigned8_t CSDO_NUM = CO_NUM_SDOS + 1u;

  co_csdo_t* const csdo = co_csdo_create(net, dev, CSDO_NUM);

  POINTERS_EQUAL(nullptr, csdo);
  CHECK_EQUAL(ERRNUM_INVAL, get_errnum());
}

/// \Given a pointer to the device (co_dev_t) containing 0x1280 object
///
/// \When co_csdo_create() is called with a pointer to the network (can_net_t),
///       the pointer to the device and a CSDO number
///
/// \Then a non-null pointer is returned, default values are set
///       \Calls co_csdo_alloc()
///       \Calls co_csdo_init()
TEST(CO_CsdoInit, CoCsdoCreate_WithObj1280) {
  std::unique_ptr<CoObjTHolder> obj1280(new CoObjTHolder(0x1280u));
  co_dev_insert_obj(dev, obj1280->Take());

  co_csdo_t* const csdo = co_csdo_create(net, dev, CSDO_NUM);

  CHECK(csdo != nullptr);
  POINTERS_EQUAL(dev, co_csdo_get_dev(csdo));
  POINTERS_EQUAL(net, co_csdo_get_net(csdo));
  CHECK_EQUAL(CSDO_NUM, co_csdo_get_num(csdo));
  POINTERS_EQUAL(can_net_get_alloc(net), co_csdo_get_alloc(csdo));
  const co_sdo_par* par = co_csdo_get_par(csdo);
  CHECK_EQUAL(3u, par->n);
  CHECK_EQUAL(DEV_ID, par->id);
  CHECK_EQUAL(0x580u + CSDO_NUM, par->cobid_res);
  CHECK_EQUAL(0x600u + CSDO_NUM, par->cobid_req);

  co_csdo_destroy(csdo);
}

/// \Given a pointer to the device (co_dev_t) without server parameter object
///
/// \When co_csdo_create() is called with a pointer to the network (can_net_t),
///       the pointer to the device and a CSDO number
///
/// \Then a null pointer is returned
///       \Calls co_csdo_alloc()
///       \Calls co_csdo_init()
///       \Calls get_errc()
///       \Calls co_csdo_free()
///       \Calls set_errc()
TEST(CO_CsdoInit, CoCsdoCreate_NoServerParameterObj) {
  co_csdo_t* const csdo = co_csdo_create(net, dev, CSDO_NUM);

  POINTERS_EQUAL(nullptr, csdo);
  CHECK_EQUAL(ERRNUM_INVAL, get_errnum());
}

/// \Given a pointer to the device (co_dev_t) containing 0x1280 object
///
/// \When co_csdo_create() is called with a pointer to the network (can_net_t)
///       with a failing allocator, the pointer to the device and a CSDO number
///       but can_recv_create() fails
///
/// \Then a null pointer is returned
///       \Calls co_csdo_alloc()
///       \Calls co_csdo_init()
///       \Calls get_errc()
///       \Calls co_csdo_free()
///       \Calls set_errc()
TEST(CO_CsdoInit, CoCsdoCreate_RecvCreateFail) {
  std::unique_ptr<CoObjTHolder> obj1280(new CoObjTHolder(0x1280u));
  co_dev_insert_obj(dev, obj1280->Take());

  limitedAllocator.LimitAllocationTo(co_csdo_sizeof());
  co_csdo_t* const csdo = co_csdo_create(failing_net, dev, CSDO_NUM);

  POINTERS_EQUAL(nullptr, csdo);
  CHECK_EQUAL(0, get_errc());
}

/// \Given a pointer to the device (co_dev_t) containing 0x1280 object
///
/// \When co_csdo_create() is called with a pointer to the network (can_net_t)
///       with a failing allocator, the pointer to the device and a CSDO number
///       but can_timer_create() fails
///
/// \Then a null pointer is returned
///       \Calls co_csdo_alloc()
///       \Calls co_csdo_init()
///       \Calls get_errc()
///       \Calls co_csdo_free()
///       \Calls set_errc()
TEST(CO_CsdoInit, CoCsdoCreate_TimerCreateFail) {
  std::unique_ptr<CoObjTHolder> obj1280(new CoObjTHolder(0x1280u));
  co_dev_insert_obj(dev, obj1280->Take());

  limitedAllocator.LimitAllocationTo(co_csdo_sizeof() + can_recv_sizeof());
  co_csdo_t* const csdo = co_csdo_create(failing_net, dev, CSDO_NUM);

  POINTERS_EQUAL(nullptr, csdo);
  CHECK_EQUAL(0, get_errc());
}

///@}

/// @name co_csdo_destroy()
///@{

/// \Given a null CSDO service pointer (co_csdo_t)
///
/// \When co_csdo_destroy() is called
///
/// \Then nothing is changed
TEST(CO_CsdoInit, CoCsdoDestroy_Nullptr) {
  co_csdo_t* const csdo = nullptr;

  co_csdo_destroy(csdo);
}

/// \Given a pointer to the CSDO service (co_csdo_t)
///
/// \When co_csdo_destroy() is called
///
/// \Then the CSDO is destroyed
///       \Calls co_ssdo_fini()
///       \Calls co_ssdo_free()
TEST(CO_CsdoInit, CoCsdoDestroy_Nominal) {
  std::unique_ptr<CoObjTHolder> obj1280(new CoObjTHolder(0x1280u));
  co_dev_insert_obj(dev, obj1280->Take());
  co_csdo_t* const csdo = co_csdo_create(net, dev, CSDO_NUM);
  CHECK(csdo != nullptr);

  co_csdo_destroy(csdo);
}

///@}

/// @name co_csdo_start()
///@{

/// \Given a pointer to the CSDO service (co_csdo_t)
///
/// \When co_csdo_start() is called
///
/// \Then 0 is returned, the service is not stopped, the service is idle
///       \Calls co_csdo_is_stopped()
///       \Calls co_csdo_enter()
///       \Calls co_csdo_update()
TEST(CO_CsdoInit, CoCsdoStart_NoDev) {
  co_csdo_t* const csdo = co_csdo_create(net, nullptr, CSDO_NUM);
  CHECK(csdo != nullptr);

  const auto ret = co_csdo_start(csdo);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(0, co_csdo_is_stopped(csdo));
  CHECK_EQUAL(1u, co_csdo_is_idle(csdo));

  co_csdo_destroy(csdo);
}

/// \Given a pointer to the started CSDO service (co_csdo_t)
///
/// \When co_csdo_start() is called
///
/// \Then 0 is returned, the service is not stopped, the service is idle
///       \Calls co_csdo_is_stopped()
TEST(CO_CsdoInit, CoCsdoStart_AlreadyStarted) {
  std::unique_ptr<CoObjTHolder> obj1280(new CoObjTHolder(0x1280u));
  co_dev_insert_obj(dev, obj1280->Take());
  co_csdo_t* const csdo = co_csdo_create(net, dev, CSDO_NUM);
  CHECK_EQUAL(0, co_csdo_start(csdo));

  const auto ret = co_csdo_start(csdo);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(0, co_csdo_is_stopped(csdo));
  CHECK_EQUAL(1u, co_csdo_is_idle(csdo));

  co_csdo_destroy(csdo);
}

/// \Given a pointer to the CSDO service (co_csdo_t) containing 0x1280 object
///
/// \When co_csdo_start() is called
///
/// \Then 0 is returned, the service is not stopped, the service is idle
///       \Calls co_csdo_is_stopped()
///       \Calls co_dev_find_obj()
///       \Calls co_obj_sizeof_val()
///       \Calls memcpy()
///       \Calls co_obj_addressof_val()
///       \Calls co_obj_set_dn_ind()
///       \Calls co_csdo_enter()
///       \Calls co_csdo_update()
TEST(CO_CsdoInit, CoCsdoStart_DefaultCSDO_WithObj1280) {
  std::unique_ptr<CoObjTHolder> obj1280(new CoObjTHolder(0x1280u));
  co_dev_insert_obj(dev, obj1280->Take());
  co_csdo_t* const csdo = co_csdo_create(net, dev, CSDO_NUM);

  const auto ret = co_csdo_start(csdo);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(0, co_csdo_is_stopped(csdo));
  CHECK_EQUAL(1u, co_csdo_is_idle(csdo));

  co_csdo_destroy(csdo);
}

///@}

/// @name co_csdo_stop()
///@{

/// \Given a pointer to the CSDO service (co_csdo_t) containing 0x1280 object
///
/// \When co_csdo_stop() is called
///
/// \Then the service is stopped
///       \Calls co_csdo_is_stopped()
TEST(CO_CsdoInit, CoCsdoStop_OnCreated) {
  std::unique_ptr<CoObjTHolder> obj1280(new CoObjTHolder(0x1280u));
  co_dev_insert_obj(dev, obj1280->Take());
  co_csdo_t* const csdo = co_csdo_create(net, dev, CSDO_NUM);
  CHECK(csdo != nullptr);

  co_csdo_stop(csdo);

  CHECK_EQUAL(1u, co_csdo_is_stopped(csdo));

  co_csdo_destroy(csdo);
}

/// \Given a pointer to the started CSDO service (co_csdo_t) containing 0x1280 object
///
/// \When co_csdo_stop() is called
///
/// \Then the service is stopped
///       \Calls co_csdo_is_stopped()
///       \Calls co_csdo_abort_req()
///       \Calls can_timer_stop()
///       \Calls can_recv_stop()
///       \Calls co_dev_find_obj()
///       \Calls co_obj_set_dn_ind()
///       \Calls co_csdo_enter()
TEST(CO_CsdoInit, CoCsdoStop_OnStarted) {
  std::unique_ptr<CoObjTHolder> obj1280(new CoObjTHolder(0x1280u));
  co_dev_insert_obj(dev, obj1280->Take());
  co_csdo_t* const csdo = co_csdo_create(net, dev, CSDO_NUM);
  CHECK(csdo != nullptr);
  CHECK_EQUAL(0, co_csdo_start(csdo));

  co_csdo_stop(csdo);

  CHECK_EQUAL(1u, co_csdo_is_stopped(csdo));

  co_csdo_destroy(csdo);
}

///@}

TEST_BASE(CO_CsdoBase) {
  TEST_BASE_SUPER(CO_CsdoBase);
  static const co_unsigned8_t CSDO_NUM = 0x01u;
  static const co_unsigned8_t DEV_ID = 0x01u;
  co_csdo_t* csdo = nullptr;
  co_dev_t* dev = nullptr;
  can_net_t* net = nullptr;
  std::unique_ptr<CoDevTHolder> dev_holder;
  Allocators::Default defaultAllocator;

  std::unique_ptr<CoObjTHolder> obj1280;

  void CreateObjInDev(std::unique_ptr<CoObjTHolder> & obj_holder,
                      co_unsigned16_t idx) {
    obj_holder.reset(new CoObjTHolder(idx));
    CHECK(obj_holder->Get() != nullptr);
    CHECK_EQUAL(0, co_dev_insert_obj(dev, obj_holder->Take()));
  }

  // obj 0x1280, sub 0x00 - highest sub-index supported
  void SetCli00HighestSubidxSupported(co_unsigned8_t subidx) {
    co_sub_t* const sub = co_dev_find_sub(dev, 0x1280u, 0x00u);
    if (sub != nullptr)
      co_sub_set_val_u8(sub, subidx);
    else
      obj1280->InsertAndSetSub(0x00u, CO_DEFTYPE_UNSIGNED8, subidx);
  }

  // obj 0x1280, sub 0x01 contains COB-ID client -> server
  void SetCli01CobidReq(co_unsigned32_t cobid) {
    co_sub_t* const sub = co_dev_find_sub(dev, 0x1280u, 0x01u);
    if (sub != nullptr)
      co_sub_set_val_u32(sub, cobid);
    else
      obj1280->InsertAndSetSub(0x01u, CO_DEFTYPE_UNSIGNED32, cobid);
  }

  // obj 0x1280, sub 0x02 contains COB-ID server -> client
  void SetCli02CobidRes(co_unsigned32_t cobid) {
    co_sub_t* const sub = co_dev_find_sub(dev, 0x1280u, 0x02u);
    if (sub != nullptr)
      co_sub_set_val_u32(sub, cobid);
    else
      obj1280->InsertAndSetSub(0x02u, CO_DEFTYPE_UNSIGNED32, cobid);
  }

  co_unsigned32_t GetCli01CobidReq() {
    return co_dev_get_val_u32(dev, 0x1280u, 0x01u);
  }

  co_unsigned32_t GetCli02CobidRes() {
    return co_dev_get_val_u32(dev, 0x1280u, 0x02u);
  }

  TEST_SETUP() {
    LelyUnitTest::DisableDiagnosticMessages();
    net = can_net_create(defaultAllocator.ToAllocT());
    CHECK(net != nullptr);

    dev_holder.reset(new CoDevTHolder(DEV_ID));
    dev = dev_holder->Get();
    CHECK(dev != nullptr);

    can_net_set_send_func(net, CanSend::func, nullptr);

    CreateObjInDev(obj1280, 0x1280u);
    SetCli00HighestSubidxSupported(0x02u);
    SetCli01CobidReq(0x600u + DEV_ID);
    SetCli02CobidRes(0x580u + DEV_ID);
    csdo = co_csdo_create(net, dev, CSDO_NUM);
    CHECK(csdo != nullptr);

    CoCsdoDnCon::Clear();
  }

  TEST_TEARDOWN() {
    co_csdo_destroy(csdo);

    dev_holder.reset();
    can_net_destroy(net);
  }
};

TEST_GROUP_BASE(CoCsdoSetGet, CO_CsdoBase) {
  int data = 0;  // clang-format fix

  static void co_csdo_ind_func(const co_csdo_t*, co_unsigned16_t,
                               co_unsigned8_t, size_t, size_t, void*) {}
};

/// @name co_csdo_get_net()
///@{

/// \Given a pointer to the CSDO service (co_csdo_t)
///
/// \When co_csdo_get_net() is called
///
/// \Then a pointer to the network (can_net_t) is returned
TEST(CoCsdoSetGet, CoCsdoGetNet_Nominal) {
  const auto* ret = co_csdo_get_net(csdo);

  POINTERS_EQUAL(net, ret);
}

///@}

/// @name co_csdo_get_dev()
///@{

/// \Given a pointer to the CSDO service (co_csdo_t)
///
/// \When co_csdo_get_dev() is called
///
/// \Then a pointer to the device (co_dev_t) is returned
TEST(CoCsdoSetGet, CoCsdoGetDev_Nominal) {
  const auto* ret = co_csdo_get_dev(csdo);

  POINTERS_EQUAL(dev, ret);
}

///@}

/// @name co_csdo_get_num()
///@{

/// \Given a pointer to the CSDO service (co_csdo_t)
///
/// \When co_csdo_get_num() is called
///
/// \Then the service's CSDO number is returned
TEST(CoCsdoSetGet, CoCsdoGetNum_Nominal) {
  const auto ret = co_csdo_get_num(csdo);

  CHECK_EQUAL(CSDO_NUM, ret);
}

///@}

/// @name co_csdo_get_par()
///@{

/// \Given a pointer to the CSDO service (co_csdo_t)
///
/// \When co_csdo_get_par() is called
///
/// \Then a pointer to the parameter object is returned
TEST(CoCsdoSetGet, CoCsdoGetPar_Nominal) {
  const auto* par = co_csdo_get_par(csdo);

  CHECK(par != nullptr);
  CHECK_EQUAL(3u, par->n);
  CHECK_EQUAL(CSDO_NUM, par->id);
  CHECK_EQUAL(0x580u + CSDO_NUM, par->cobid_res);
  CHECK_EQUAL(0x600u + CSDO_NUM, par->cobid_req);
}

///@}

/// @name co_csdo_get_dn_ind()
///@{

/// \Given a pointer to the CSDO service (co_csdo_t)
///
/// \When co_csdo_get_dn_ind() is called with a memory area to store the
///       results
///
/// \Then null pointers are returned
TEST(CoCsdoSetGet, CoCsdoGetDnInd_Nominal) {
  int data = 0;
  co_csdo_ind_t* pind = co_csdo_ind_func;
  void* pdata = &data;

  co_csdo_get_dn_ind(csdo, &pind, &pdata);

  POINTERS_EQUAL(nullptr, pind);
  POINTERS_EQUAL(nullptr, pdata);
}

/// \Given a pointer to the CSDO service (co_csdo_t)
///
/// \When co_csdo_get_dn_ind() is called with no memory area to store the
///       results
///
/// \Then nothing is changed
TEST(CoCsdoSetGet, CoCsdoGetDnInd_NoMemoryArea) {
  co_csdo_get_dn_ind(csdo, nullptr, nullptr);
}

///@}

/// @name co_csdo_set_dn_ind()
///@{

/// \Given a pointer to the CSDO service (co_csdo_t)
///
/// \When co_csdo_set_dn_ind() is called with a pointer to the function and
///       a pointer to data
///
/// \Then CSDO download indication function and user-specified data pointers
///       are set
TEST(CoCsdoSetGet, CoCsdoSetDnInd_Nominal) {
  int data = 0;

  co_csdo_set_dn_ind(csdo, co_csdo_ind_func, &data);

  co_csdo_ind_t* pind = nullptr;
  void* pdata = nullptr;
  co_csdo_get_dn_ind(csdo, &pind, &pdata);
  POINTERS_EQUAL(co_csdo_ind_func, pind);
  POINTERS_EQUAL(&data, pdata);
}

///@}

/// @name co_csdo_get_up_ind()
///@{

/// \Given a pointer to the CSDO service (co_csdo_t)
///
/// \When co_csdo_get_up_ind() is called with a memory area to store the results
///
/// \Then null pointers are returned
TEST(CoCsdoSetGet, CoCsdoGetUpInd_Nominal) {
  int data = 0;
  co_csdo_ind_t* pind = co_csdo_ind_func;
  void* pdata = &data;

  co_csdo_get_up_ind(csdo, &pind, &pdata);

  POINTERS_EQUAL(nullptr, pind);
  POINTERS_EQUAL(nullptr, pdata);
}

/// \Given a pointer to the CSDO service (co_csdo_t)
///
/// \When co_csdo_get_up_ind() is called with no memory to store the results
///
/// \Then nothing is changed
TEST(CoCsdoSetGet, CoCsdoGetUpInd_NoMemoryArea) {
  co_csdo_get_up_ind(csdo, nullptr, nullptr);
}

///@}

/// @name co_csdo_set_up_ind()
///@{

/// \Given a pointer to the CSDO service (co_csdo_t)
///
/// \When co_csdo_set_up_ind() is called with a pointer to the function and
///       a pointer to data
///
/// \Then CSDO upload indication function and user-specified data pointers
///       are set
TEST(CoCsdoSetGet, CoCsdoSetUpInd_Nominal) {
  int data = 0;

  co_csdo_set_up_ind(csdo, co_csdo_ind_func, &data);

  co_csdo_ind_t* pind = nullptr;
  void* pdata = nullptr;
  co_csdo_get_up_ind(csdo, &pind, &pdata);
  POINTERS_EQUAL(co_csdo_ind_func, pind);
  POINTERS_EQUAL(&data, pdata);
}

///@}

/// @name co_csdo_get_timeout()
///@{

/// \Given a pointer to the CSDO service (co_csdo_t)
///
/// \When co_csdo_get_timeout() is called
///
/// \Then default timeout value of zero is returned
TEST(CoCsdoSetGet, CoCsdoGetTimeout_Nominal) {
  const auto ret = co_csdo_get_timeout(csdo);

  CHECK_EQUAL(0u, ret);
}

///@}

/// @name co_csdo_set_timeout()
///@{

/// \Given a pointer to the CSDO service (co_csdo_t) with no timeout set
///
/// \When co_csdo_set_timeout() is called with a valid timeout value
///
/// \Then timeout is set
TEST(CoCsdoSetGet, CoCsdoSetTimeout_ValidTimeout) {
  co_csdo_set_timeout(csdo, 20);

  CHECK_EQUAL(20, co_csdo_get_timeout(csdo));
}

/// \Given a pointer to the CSDO service (co_csdo_t) with no timeout set
///
/// \When co_csdo_set_timeout() is called with an invalid timeout value
///
/// \Then timeout is not set
TEST(CoCsdoSetGet, CoCsdoSetTimeout_InvalidTimeout) {
  co_csdo_set_timeout(csdo, -1);

  CHECK_EQUAL(0, co_csdo_get_timeout(csdo));
}

/// \Given a pointer to the CSDO service (co_csdo_t) with a timeout set
///
/// \When co_csdo_set_timeout() is called with a zero timeout value
///
/// \Then timeout is disabled
TEST(CoCsdoSetGet, CoCsdoSetTimeout_DisableTimeout) {
  co_csdo_set_timeout(csdo, 1);

  co_csdo_set_timeout(csdo, 0);

  CHECK_EQUAL(0, co_csdo_get_timeout(csdo));
}

/// \Given a pointer to the CSDO service (co_csdo_t) with a timeout set
///
/// \When co_csdo_set_timeout() is called with a different timeout value
///
/// \Then timeout is updated
TEST(CoCsdoSetGet, CoCsdoSetTimeout_UpdateTimeout) {
  co_csdo_set_timeout(csdo, 1);

  co_csdo_set_timeout(csdo, 4);

  CHECK_EQUAL(4, co_csdo_get_timeout(csdo));
}

///@}

TEST_GROUP_BASE(CO_Csdo, CO_CsdoBase) {
  const co_unsigned16_t SUB_TYPE = CO_DEFTYPE_UNSIGNED16;

  static constexpr size_t ConciseDcfEntrySize(const size_t type_size) {
    return sizeof(co_unsigned16_t)    // index
           + sizeof(co_unsigned8_t)   // subidx
           + sizeof(co_unsigned32_t)  // data size of parameter
           + type_size;
  }

  static constexpr size_t ConciseDcfSize(size_t entries_size) {
    return sizeof(co_unsigned32_t) + entries_size;
  }

  const co_unsigned16_t IDX = 0x2020u;
  const co_unsigned8_t SUBIDX = 0x00u;
  const co_unsigned16_t INVALID_IDX = 0xffffu;
  const co_unsigned8_t INVALID_SUBIDX = 0xffu;
  const co_unsigned16_t ARR_IDX = 0x2021u;
  const co_unsigned16_t VAL = 0xabcdu;
  std::unique_ptr<CoObjTHolder> obj2020;

  static co_unsigned32_t co_sub_failing_dn_ind(co_sub_t*, co_sdo_req*,
                                               co_unsigned32_t, void*) {
    return CO_SDO_AC_ERROR;
  }

  void MembufCheck(const membuf* const mbuf, const char* const begin,
                   const size_t size) {
    POINTERS_EQUAL(begin, membuf_begin(mbuf));
    CHECK_EQUAL(size, membuf_size(mbuf));
  }

  TEST_SETUP() {
    TEST_BASE_SETUP();

    CreateObjInDev(obj2020, IDX);
    obj2020->InsertAndSetSub(SUBIDX, SUB_TYPE, sub_type(0));

    CoCsdoUpCon::Clear();
  }

  TEST_TEARDOWN() { TEST_BASE_TEARDOWN(); }
};

/// @name co_dev_dn_req()
///@{

/// \Given a pointer to the device (co_dev_t)
///
/// \When co_dev_dn_req() is called with an index of the existing object and a sub-index of
///       a non-existing sub-object, a pointer to a value, the length of the value
///       and a pointer to a download confirmation function
///
/// \Then 0 is returned, the confirmation function is called once with a null
///       pointer, an invalid index, an invalid sub-index, CO_SDO_AC_NO_OBJ and
///       a null pointer
TEST(CO_Csdo, CoDevDnReq_NoObj) {
  const auto ret = co_dev_dn_req(dev, INVALID_IDX, INVALID_SUBIDX, &VAL,
                                 sizeof(VAL), CoCsdoDnCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::num_called);
  CoCsdoDnCon::Check(nullptr, INVALID_IDX, INVALID_SUBIDX, CO_SDO_AC_NO_OBJ,
                     nullptr);
}

/// \Given a pointer to the device (co_dev_t)
///
/// \When co_dev_dn_req() is called with an index of the existing object and
///       a sub-index of a non-existing sub-object, a pointer to a value, the length of
///       the value and a download confirmation function
///
/// \Then 0 is returned, the confirmation function is called once with a null
///       pointer, an index, invalid sub-index, CO_SDO_AC_NO_SUB and
///       a null pointer
TEST(CO_Csdo, CoDevDnReq_NoSub) {
  const auto ret = co_dev_dn_req(dev, IDX, INVALID_SUBIDX, &VAL, sizeof(VAL),
                                 CoCsdoDnCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::num_called);
  CoCsdoDnCon::Check(nullptr, IDX, INVALID_SUBIDX, CO_SDO_AC_NO_SUB, nullptr);
}

/// \Given a pointer to the device (co_dev_t)
///
/// \When co_dev_dn_req() is called with an index and a sub-index of an existing
///       entry, a pointer to a value, the length of the value and no download
///       confirmation function
///
/// \Then 0 is returned, the requested value is set
TEST(CO_Csdo, CoDevDnReq_NoCsdoDnConFunc) {
  const auto ret =
      co_dev_dn_req(dev, IDX, SUBIDX, &VAL, sizeof(VAL), nullptr, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(VAL, co_dev_get_val_u16(dev, IDX, SUBIDX));
}

/// \Given a pointer to the device (co_dev_t)
///
/// \When co_dev_dn_req() is called with an index and a sub-index of an existing
///       entry, a pointer to a value, the length of the value and a download
///       confirmation function
///
/// \Then 0 is returned, the confirmation function is called once with a null
///       pointer, an index, a sub-index, 0 as the abort code and a null pointer
///       and the requested value is set
TEST(CO_Csdo, CoDevDnReq_Nominal) {
  const auto ret = co_dev_dn_req(dev, IDX, SUBIDX, &VAL, sizeof(VAL),
                                 CoCsdoDnCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::num_called);
  CoCsdoDnCon::Check(nullptr, IDX, SUBIDX, 0u, nullptr);
  CHECK_EQUAL(VAL, co_dev_get_val_u16(dev, IDX, SUBIDX));
}

///@}

/// @name co_dev_dn_val_req()
///@{

/// \Given a pointer to the device (co_dev_t)
///
/// \When co_dev_dn_val_req() is called with an index and a sub-index of
///       a non-existing entry, a pointer to a value, a type of the value,
///       no memory buffer and a download confirmation function
///
/// \Then 0 is returned, the confirmation function is called once with a null
///       pointer, an invalid index, an invalid sub-index, CO_SDO_AC_NO_OBJ and
///       a null pointer
TEST(CO_Csdo, CoDevDnValReq_NoObj) {
  const auto ret = co_dev_dn_val_req(dev, INVALID_IDX, INVALID_SUBIDX, SUB_TYPE,
                                     &VAL, nullptr, CoCsdoDnCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::num_called);
  CoCsdoDnCon::Check(nullptr, INVALID_IDX, INVALID_SUBIDX, CO_SDO_AC_NO_OBJ,
                     nullptr);
}

/// \Given a pointer to the device (co_dev_t)
///
/// \When co_dev_dn_req() is called with an index of the existing object and
///       a sub-index of a non-existing sub-object, a pointer to a value, a type
///       of the value, no memory buffer and a download confirmation function
///
/// \Then 0 is returned, the confirmation function is called once with a null
///       pointer, an index, invalid sub-index, CO_SDO_AC_NO_SUB and
///       a null pointer
TEST(CO_Csdo, CoDevDnValReq_NoSub) {
  const auto ret = co_dev_dn_val_req(dev, IDX, INVALID_SUBIDX, SUB_TYPE, &VAL,
                                     nullptr, CoCsdoDnCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::num_called);
  CoCsdoDnCon::Check(nullptr, IDX, INVALID_SUBIDX, CO_SDO_AC_NO_SUB, nullptr);
}

#if LELY_NO_MALLOC
/// \Given a pointer to the device (co_dev_t)
///
/// \When co_dev_dn_req() is called with an index and a sub-index of an existing
///       entry, a pointer to a value, 64-bit type, no memory buffer and
///       a download confirmation function
///
/// \Then 0 is returned, the confirmation function is called once with a null
///       pointer, an index, a sub-index, CO_SDO_AC_NO_MEM and a null pointer,
///       the requested value is not set
TEST(CO_Csdo, CoDevDnValReq_DnTooLong) {
  const uint_least64_t data = 0xffffffffu;
  membuf mbuf = MEMBUF_INIT;

  const auto ret = co_dev_dn_val_req(dev, IDX, SUBIDX, CO_DEFTYPE_UNSIGNED64,
                                     &data, &mbuf, CoCsdoDnCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::num_called);
  CoCsdoDnCon::Check(nullptr, IDX, SUBIDX, CO_SDO_AC_NO_MEM, nullptr);
  CHECK_EQUAL(0u, co_dev_get_val_u8(dev, IDX, SUBIDX));
}
#endif  // LELY_NO_MALLOC

/// \Given a pointer to the device (co_dev_t)
///
/// \When co_dev_dn_req() is called with an index and a sub-index of an existing
///       entry, a pointer to a value, a type of the value, no memory buffer and
///       no download confirmation function
///
/// \Then 0 is returned, the requested value is set.
TEST(CO_Csdo, CoDevDnValReq_NoCsdoDnConFunc) {
  const auto ret = co_dev_dn_val_req(dev, IDX, SUBIDX, SUB_TYPE, &VAL, nullptr,
                                     nullptr, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(VAL, co_dev_get_val_u16(dev, IDX, SUBIDX));
}

/// \Given a pointer to the device (co_dev_t)
///
/// \When co_dev_dn_req() is called with an index and a sub-index of an existing
///       entry, a pointer to a value, a type of the value, no memory buffer and
///       a download confirmation function
///
/// \Then 0 is returned, the confirmation function is called once with a null
///       pointer, an index, a sub-index, 0 as the abort code and a null pointer,
///       the requested value is set
TEST(CO_Csdo, CoDevDnValReq_Nominal) {
  const auto ret = co_dev_dn_val_req(dev, IDX, SUBIDX, SUB_TYPE, &VAL, nullptr,
                                     CoCsdoDnCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::num_called);
  CoCsdoDnCon::Check(nullptr, IDX, SUBIDX, 0u, nullptr);
  CHECK_EQUAL(VAL, co_dev_get_val_u16(dev, IDX, SUBIDX));
}

///@}

/// @name co_dev_dn_dcf_req()
///@{

/// \Given a pointer to the device (co_dev_t), a too short concise DCF buffer
///
/// \When co_dev_dn_dcf_req() is called with pointers to the beginning and the
///       end of the buffer and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called once with a null
///       pointer, 0, 0, CO_SDO_AC_TYPE_LEN_LO and a null pointer, the requested
///       value is not changed
TEST(CO_Csdo, CoDevDnDcfReq_ConciseBufTooShort) {
  const size_t CONCISE_DCF_SUB_TYPE_SIZE =
      ConciseDcfSize(ConciseDcfEntrySize(sizeof(sub_type)));
  uint_least8_t concise_dcf[CONCISE_DCF_SUB_TYPE_SIZE] = {0};
  for (size_t concise_buf_size = 3u;
       concise_buf_size < CONCISE_DCF_SUB_TYPE_SIZE - sizeof(sub_type);
       concise_buf_size++) {
    uint_least8_t* const begin = concise_dcf;
    uint_least8_t* const end = concise_dcf + concise_buf_size;
    CHECK_EQUAL(CONCISE_DCF_SUB_TYPE_SIZE,
                co_dev_write_dcf(dev, IDX, IDX, begin, end));

    const auto ret =
        co_dev_dn_dcf_req(dev, begin, end, CoCsdoDnCon::func, nullptr);

    CHECK_EQUAL(0, ret);
    CHECK_EQUAL(1u, CoCsdoDnCon::num_called);
    CoCsdoDnCon::Check(nullptr, 0, 0, CO_SDO_AC_TYPE_LEN_LO, nullptr);
    CHECK_EQUAL(0, co_dev_get_val_u16(dev, IDX, SUBIDX));

    CoCsdoDnCon::Clear();
  }
}

/// \Given a pointer to the device (co_dev_t), an invalid concise DCF buffer
///        that is too small for a declared entry value
///
/// \When co_dev_dn_dcf_req() is called with pointers to the beginning and the
///       end of the buffer and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called once with a null
///       pointer, an index, a sub-index, CO_SDO_AC_TYPE_LEN_LO and a null
///       pointer, the requested value is not changed
TEST(CO_Csdo, CoDevDnDcfReq_DatasizeMismatch) {
  const size_t CONCISE_DCF_SUB_TYPE_SIZE =
      ConciseDcfSize(ConciseDcfEntrySize(sizeof(sub_type)));
  uint_least8_t concise_dcf[CONCISE_DCF_SUB_TYPE_SIZE] = {0};

  obj2020->RemoveAndDestroyLastSub();
  obj2020->InsertAndSetSub(SUBIDX, SUB_TYPE, sub_type(0));

  uint_least8_t* const begin = concise_dcf;
  uint_least8_t* const end = concise_dcf + CONCISE_DCF_SUB_TYPE_SIZE;
  CHECK_EQUAL(CONCISE_DCF_SUB_TYPE_SIZE,
              co_dev_write_dcf(dev, IDX, IDX, begin, end));

  obj2020->RemoveAndDestroyLastSub();
  obj2020->InsertAndSetSub(SUBIDX, SUB_TYPE, sub_type(0));

  const auto ret =
      co_dev_dn_dcf_req(dev, begin, end - 1u, CoCsdoDnCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::num_called);
  CoCsdoDnCon::Check(nullptr, IDX, SUBIDX, CO_SDO_AC_TYPE_LEN_LO, nullptr);
  CHECK_EQUAL(0, co_dev_get_val_u16(dev, IDX, SUBIDX));
}

/// \Given a pointer to the device (co_dev_t), a concise DCF buffer with
///        an index of an object which is not present in a device
///
/// \When co_dev_dn_dcf_req() is called with pointers to the beginning and the
///       end of the buffer and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called once with a null
///       pointer, index of a non-existing object, a sub-index, CO_SDO_AC_NO_OBJ
///       and a null pointer
TEST(CO_Csdo, CoDevDnDcfReq_NoObj) {
  const size_t CONCISE_DCF_SUB_TYPE_SIZE =
      ConciseDcfSize(ConciseDcfEntrySize(sizeof(sub_type)));
  uint_least8_t concise_dcf[CONCISE_DCF_SUB_TYPE_SIZE] = {0};
  uint_least8_t* const begin = concise_dcf;
  uint_least8_t* const end = concise_dcf + CONCISE_DCF_SUB_TYPE_SIZE;
  CHECK_EQUAL(CONCISE_DCF_SUB_TYPE_SIZE,
              co_dev_write_dcf(dev, IDX, IDX, begin, end));
  CHECK_EQUAL(0, co_dev_remove_obj(dev, obj2020->Get()));
  POINTERS_EQUAL(obj2020->Get(), obj2020->Reclaim());

  const auto ret =
      co_dev_dn_dcf_req(dev, begin, end, CoCsdoDnCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::num_called);
  CoCsdoDnCon::Check(nullptr, IDX, SUBIDX, CO_SDO_AC_NO_OBJ, nullptr);
}

/// \Given a pointer to the device (co_dev_t), a concise DCF buffer with
///        an existing object index but non-existing sub-index
///
/// \When co_dev_dn_dcf_req() is called with pointers to the beginning and the
///       end of the buffer with concise DCF and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called once with a null
///       pointer, the index, the sub-index, CO_SDO_AC_NO_SUB and
///       a null pointer
TEST(CO_Csdo, CoDevDnDcfReq_NoSub) {
  const size_t CONCISE_DCF_SUB_TYPE_SIZE =
      ConciseDcfSize(ConciseDcfEntrySize(sizeof(sub_type)));
  uint_least8_t concise_dcf[CONCISE_DCF_SUB_TYPE_SIZE] = {0};
  uint_least8_t* const begin = concise_dcf;
  uint_least8_t* const end = concise_dcf + CONCISE_DCF_SUB_TYPE_SIZE;
  CHECK_EQUAL(CONCISE_DCF_SUB_TYPE_SIZE,
              co_dev_write_dcf(dev, IDX, IDX, begin, end));
  obj2020->RemoveAndDestroyLastSub();

  const auto ret =
      co_dev_dn_dcf_req(dev, begin, end, CoCsdoDnCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::num_called);
  CoCsdoDnCon::Check(nullptr, IDX, SUBIDX, CO_SDO_AC_NO_SUB, nullptr);
}

/// \Given a pointer to the device (co_dev_t), a concise DCF with many entries
///
/// \When co_dev_dn_dcf_req() is called with pointers to the beginning and the
///       end of the buffer and a confirmation function but download indication
///       function returns an abort code
///
/// \Then 0 is returned, the confirmation function is called once with a null
///       pointer, an index, a sub-index, CO_SDO_AC_ERROR and
///       a null pointer, the requested value is not set
TEST(CO_Csdo, CoDevDnDcfReq_ManyEntriesButDnIndFail) {
  const size_t CONCISE_DCF_COMBINED_SIZE =
      ConciseDcfSize(ConciseDcfEntrySize(sizeof(sub_type)) +
                     ConciseDcfEntrySize(sizeof(sub_type)));
  uint_least8_t concise_dcf[CONCISE_DCF_COMBINED_SIZE] = {0};
  uint_least8_t* const begin = concise_dcf;
  uint_least8_t* const end = concise_dcf + CONCISE_DCF_COMBINED_SIZE;
  const co_unsigned16_t OTHER_IDX = 0x2021u;
  std::unique_ptr<CoObjTHolder> obj2021;
  CreateObjInDev(obj2021, OTHER_IDX);
  obj2021->InsertAndSetSub(0x00u, SUB_TYPE, sub_type(0));
  CHECK_EQUAL(CONCISE_DCF_COMBINED_SIZE,
              co_dev_write_dcf(dev, IDX, OTHER_IDX, begin, end));

  co_sub_set_dn_ind(obj2020->GetLastSub(), co_sub_failing_dn_ind, nullptr);
  const auto ret =
      co_dev_dn_dcf_req(dev, begin, end, CoCsdoDnCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::num_called);
  CoCsdoDnCon::Check(nullptr, IDX, SUBIDX, CO_SDO_AC_ERROR, nullptr);
  CHECK_EQUAL(0, co_dev_get_val_u16(dev, IDX, SUBIDX));
}

/// \Given a pointer to the device (co_dev_t), a concise DCF buffer
///
/// \When co_dev_dn_dcf_req() is called with pointers to the beginning and the
///       end of the buffer and no confirmation function
///
/// \Then 0 is returned and the requested value is changed
TEST(CO_Csdo, CoDevDnDcfReq_NoCoCsdoDnCon) {
  const size_t CONCISE_DCF_SUB_TYPE_SIZE =
      ConciseDcfSize(ConciseDcfEntrySize(sizeof(sub_type)));
  uint_least8_t concise_dcf[CONCISE_DCF_SUB_TYPE_SIZE] = {0};
  uint_least8_t* const begin = concise_dcf;
  uint_least8_t* const end = concise_dcf + CONCISE_DCF_SUB_TYPE_SIZE;
  co_sub_set_val_u16(obj2020->GetLastSub(), VAL);
  CHECK_EQUAL(CONCISE_DCF_SUB_TYPE_SIZE,
              co_dev_write_dcf(dev, IDX, IDX, begin, end));
  co_sub_set_val_u16(obj2020->GetLastSub(), 0);

  const auto ret = co_dev_dn_dcf_req(dev, begin, end, nullptr, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(VAL, co_dev_get_val_u16(dev, IDX, SUBIDX));
}

/// \Given a pointer to the device (co_dev_t), a concise DCF buffer
///
/// \When co_dev_dn_dcf_req() is called with pointers to the beginning and
///       the end of the buffer and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called once with a null
///       pointer, an index, a sub-index, 0 as the abort code and a null pointer,
///       the requested value is set
TEST(CO_Csdo, CoDevDnDcfReq_Nominal) {
  const size_t CONCISE_DCF_SUB_TYPE_SIZE =
      ConciseDcfSize(ConciseDcfEntrySize(sizeof(sub_type)));
  uint_least8_t concise_dcf[CONCISE_DCF_SUB_TYPE_SIZE] = {0};
  uint_least8_t* const begin = concise_dcf;
  uint_least8_t* const end = concise_dcf + CONCISE_DCF_SUB_TYPE_SIZE;
  co_sub_set_val_u16(obj2020->GetLastSub(), VAL);
  CHECK_EQUAL(CONCISE_DCF_SUB_TYPE_SIZE,
              co_dev_write_dcf(dev, IDX, IDX, begin, end));
  co_sub_set_val_u16(obj2020->GetLastSub(), 0);

  const auto ret =
      co_dev_dn_dcf_req(dev, begin, end, CoCsdoDnCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CHECK_EQUAL(1u, CoCsdoDnCon::num_called);
  CoCsdoDnCon::Check(nullptr, IDX, SUBIDX, 0, nullptr);
  CHECK_EQUAL(VAL, co_dev_get_val_u16(dev, IDX, SUBIDX));
}

///@}

/// @name co_dev_up_req()
///@{

/// \Given a pointer to the device (co_dev_t) containing an object with a sub-object inserted, the sub-object has no read access
///
/// \When co_dev_up_req() is called with an index and a sub-index of an existing entry, a memory buffer to store the requested value and a confirmation function
///
/// \Then 0 is returned; confirmation function is called with a null pointer, the index, the sub-index, CO_SDO_AC_NO_READ, no memory buffer and a null user-specified data pointer, the supplied memory buffer is empty and does not contain the requested value
///       \Calls get_errc()
///       \IfCalls{LELY_NO_MALLOC, membuf_init()}
///       \Calls co_sdo_req_init()
///       \Calls co_dev_find_obj()
///       \Calls co_obj_find_sub()
///       \Calls co_obj_get_code()
///       \Calls co_sub_up_ind()
///       \Calls co_sdo_req_fini()
///       \Calls membuf_fini()
///       \Calls set_errc()
TEST(CO_Csdo, CoDevUpReq_NoReadAccess) {
  co_dev_set_val_u16(dev, IDX, SUBIDX, 0x1234u);
  co_sub_set_access(obj2020->GetLastSub(), CO_ACCESS_WO);

  char buffer[sizeof(sub_type)] = {0};
  membuf mbuf = MEMBUF_INIT;
  membuf_init(&mbuf, buffer, sizeof(sub_type));

  const auto ret =
      co_dev_up_req(dev, IDX, SUBIDX, &mbuf, CoCsdoUpCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CoCsdoUpCon::Check(nullptr, IDX, SUBIDX, CO_SDO_AC_NO_READ, nullptr, 0,
                     nullptr);
  MembufCheck(&mbuf, buffer, 0);
}

/// \Given a pointer to the device (co_dev_t) containing an object with a sub-object inserted, the sub-object has no read access
///
/// \When co_dev_up_req() is called with an index and a sub-index of an existing entry, a memory buffer to store the requested value and no confirmation function
///
/// \Then 0 is returned; the memory buffer is not empty and contains the requested value
///       \Calls get_errc()
///       \IfCalls{LELY_NO_MALLOC, membuf_init()}
///       \Calls co_sdo_req_init()
///       \Calls co_dev_find_obj()
///       \Calls co_obj_find_sub()
///       \Calls co_obj_get_code()
///       \Calls co_sub_up_ind()
///       \Calls membuf_reserve()
///       \Calls membuf_size()
///       \Calls co_sdo_req_fini()
///       \Calls membuf_fini()
///       \Calls set_errc()
TEST(CO_Csdo, CoDevUpReq_NoConfirmationFunction) {
  co_dev_set_val_u16(dev, IDX, SUBIDX, 0x1234u);

  char buffer[sizeof(sub_type)] = {0};
  membuf mbuf = MEMBUF_INIT;
  membuf_init(&mbuf, buffer, sizeof(sub_type));

  const auto ret = co_dev_up_req(dev, IDX, SUBIDX, &mbuf, nullptr, nullptr);

  CHECK_EQUAL(0, ret);
  MembufCheck(&mbuf, buffer, sizeof(sub_type));
  CHECK_EQUAL(0x1234u,
              ldle_u16(static_cast<uint_least8_t*>(membuf_begin(&mbuf))));
}

/// \Given a pointer to the device (co_dev_t) containing an object with a sub-object inserted
///
/// \When co_dev_up_req() is called with an index and a sub-index of an existing entry, no memory buffer to store the requested value and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called with a null pointer, the index, the sub-index, 0 as the abort code, a non-null uploaded bytes pointer, pointing to the requested value, a size of the value and a null user-specified data pointer
///       \Calls get_errc()
///       \IfCalls{LELY_NO_MALLOC, membuf_init()}
///       \Calls co_sdo_req_init()
///       \Calls co_dev_find_obj()
///       \Calls co_obj_find_sub()
///       \Calls co_obj_get_code()
///       \Calls co_sub_up_ind()
///       \Calls membuf_reserve()
///       \Calls membuf_size()
///       \Calls co_sdo_req_fini()
///       \Calls membuf_fini()
///       \Calls set_errc()
TEST(CO_Csdo, CoDevUpReq_NoBufPtr) {
  co_dev_set_val_u16(dev, IDX, SUBIDX, 0x1234u);
  const auto ret =
      co_dev_up_req(dev, IDX, SUBIDX, nullptr, CoCsdoUpCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  POINTERS_EQUAL(nullptr, CoCsdoUpCon::sdo);
  CHECK_EQUAL(IDX, CoCsdoUpCon::idx);
  CHECK_EQUAL(SUBIDX, CoCsdoUpCon::subidx);
  CHECK_EQUAL(0, CoCsdoUpCon::ac);
  CHECK(CoCsdoUpCon::ptr != nullptr);
  CHECK_EQUAL(sizeof(sub_type), CoCsdoUpCon::n);
  POINTERS_EQUAL(nullptr, CoCsdoUpCon::data);
  CHECK_EQUAL(0x1234u, ldle_u16(CoCsdoUpCon::buf));
}

/// \Given a pointer to the device (co_dev_t)
///
/// \When co_dev_up_req() is called with an index and a sub-index of an existing entry which is not present in the device, a memory buffer to store the requested value and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called with a null pointer, the index, the sub-index, CO_SDO_AC_NO_OBJ, a null uploaded bytes pointer, 0 as a size of the value and a null user-specified data pointer; the memory buffer remains empty
///       \Calls get_errc()
///       \IfCalls{LELY_NO_MALLOC, membuf_init()}
///       \Calls co_sdo_req_init()
///       \Calls co_dev_find_obj()
///       \Calls co_sdo_req_fini()
///       \Calls membuf_fini()
///       \Calls set_errc()
TEST(CO_Csdo, CoDevUpReq_NoObj) {
  char buffer[sizeof(sub_type)] = {0};
  membuf mbuf = MEMBUF_INIT;
  membuf_init(&mbuf, buffer, sizeof(sub_type));

  const auto ret = co_dev_up_req(dev, INVALID_IDX, INVALID_SUBIDX, &mbuf,
                                 CoCsdoUpCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CoCsdoUpCon::Check(nullptr, INVALID_IDX, INVALID_SUBIDX, CO_SDO_AC_NO_OBJ,
                     nullptr, 0, nullptr);
  MembufCheck(&mbuf, buffer, 0);
}

/// \Given a pointer to the device (co_dev_t) containing an object
///
/// \When co_dev_up_req() is called with an index of the existing object and a sub-index of a non-existing sub-object, a memory buffer to store the requested value and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called with a null pointer, the index, the sub-index, CO_SDO_AC_NO_SUB, a null uploaded bytes pointer, 0 as a size of the value and a null user-specified data pointer; the memory buffer remains empty
///       \Calls get_errc()
///       \IfCalls{LELY_NO_MALLOC, membuf_init()}
///       \Calls co_sdo_req_init()
///       \Calls co_dev_find_obj()
///       \Calls co_obj_find_sub()
///       \Calls co_sdo_req_fini()
///       \Calls membuf_fini()
///       \Calls set_errc()
TEST(CO_Csdo, CoDevUpReq_NoSub) {
  char buffer[sizeof(sub_type)] = {0};
  membuf mbuf = MEMBUF_INIT;
  membuf_init(&mbuf, buffer, sizeof(sub_type));

  const auto ret = co_dev_up_req(dev, IDX, INVALID_SUBIDX, &mbuf,
                                 CoCsdoUpCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CoCsdoUpCon::Check(nullptr, IDX, INVALID_SUBIDX, CO_SDO_AC_NO_SUB, nullptr, 0,
                     nullptr);
  MembufCheck(&mbuf, buffer, 0);
}

/// \Given a pointer to the device (co_dev_t) containing an array object, a size of the array is declared as too small
///
/// \When co_dev_up_req() is called with an index and a sub-index of the first element of the array, a memory buffer to store the requested value and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called with a null pointer, the index, the sub-index, CO_SDO_AC_NO_DATA, a null uploaded bytes pointer, 0 as a size of the value and a null user-specified data pointer; the memory buffer remains empty
///       \Calls get_errc()
///       \IfCalls{LELY_NO_MALLOC, membuf_init()}
///       \Calls co_sdo_req_init()
///       \Calls co_dev_find_obj()
///       \Calls co_obj_find_sub()
///       \Calls co_obj_get_code()
///       \Calls co_obj_get_val_u8()
///       \Calls co_sdo_req_fini()
///       \Calls membuf_fini()
///       \Calls set_errc()
TEST(CO_Csdo, CoDevUpReq_ArrayObject_NoData) {
  const co_unsigned8_t ELEMENT_SUBIDX = 0x01u;

  char buffer[sizeof(sub_type)] = {0};
  membuf mbuf = MEMBUF_INIT;
  membuf_init(&mbuf, buffer, sizeof(sub_type));

  std::unique_ptr<CoObjTHolder> obj2021;  // TODO: won't it be destroyed?
  CreateObjInDev(obj2021, ARR_IDX);
  co_obj_set_code(obj2021->Get(), CO_OBJECT_ARRAY);
  obj2021->InsertAndSetSub(SUBIDX, CO_DEFTYPE_UNSIGNED8, co_unsigned8_t(0x00u));
  obj2021->InsertAndSetSub(ELEMENT_SUBIDX, CO_DEFTYPE_UNSIGNED8,
                           co_unsigned8_t(0));

  const auto ret = co_dev_up_req(dev, ARR_IDX, ELEMENT_SUBIDX, &mbuf,
                                 CoCsdoUpCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CoCsdoUpCon::Check(nullptr, ARR_IDX, ELEMENT_SUBIDX, CO_SDO_AC_NO_DATA,
                     nullptr, 0, nullptr);
  MembufCheck(&mbuf, buffer, 0);
}

/// \Given a pointer to the device (co_dev_t) containing an array object, a size of the array is declared correctly
///
/// \When co_dev_up_req() is called with an index and a sub-index of the first element of the array, a memory buffer to store the requested value and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called with a null pointer, the index, the sub-index, CO_SDO_AC_NO_DATA, a null uploaded bytes pointer, 0 as a size of the value and a null user-specified data pointer; the memory buffer remains empty
///       \Calls get_errc()
///       \IfCalls{LELY_NO_MALLOC, membuf_init()}
///       \Calls co_sdo_req_init()
///       \Calls co_dev_find_obj()
///       \Calls co_obj_find_sub()
///       \Calls co_obj_get_code()
///       \Calls co_obj_get_val_u8()
///       \Calls co_sub_up_ind()
///       \Calls membuf_reserve()
///       \Calls membuf_size()
///       \Calls co_sdo_req_fini()
///       \Calls membuf_fini()
///       \Calls set_errc()
TEST(CO_Csdo, CoDevUpReq_ArrayObject_DataPresent) {
  const co_unsigned8_t ELEMENT_SUBIDX = 0x01u;

  char buffer[sizeof(sub_type)] = {0};
  membuf mbuf = MEMBUF_INIT;
  membuf_init(&mbuf, buffer, sizeof(sub_type));

  std::unique_ptr<CoObjTHolder> obj2021;
  CreateObjInDev(obj2021, ARR_IDX);
  co_obj_set_code(obj2021->Get(), CO_OBJECT_ARRAY);
  obj2021->InsertAndSetSub(0x00, CO_DEFTYPE_UNSIGNED8, ELEMENT_SUBIDX);
  obj2021->InsertAndSetSub(ELEMENT_SUBIDX, SUB_TYPE, sub_type(0x1234u));

  const auto ret = co_dev_up_req(dev, ARR_IDX, ELEMENT_SUBIDX, &mbuf,
                                 CoCsdoUpCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CoCsdoUpCon::Check(nullptr, ARR_IDX, ELEMENT_SUBIDX, 0, membuf_begin(&mbuf),
                     sizeof(sub_type), nullptr);
  MembufCheck(&mbuf, buffer, sizeof(sub_type));
  CHECK_EQUAL(0x1234u, ldle_u16(CoCsdoUpCon::buf));
}

namespace CoDevUpReq_ReqZero {
co_unsigned32_t
req_up_ind(const co_sub_t* sub, co_sdo_req* req, co_unsigned32_t ac, void*) {
  co_sub_on_up(sub, req, &ac);
  req->buf = nullptr;
  req->size = 0;

  return 0;
}
}  // namespace CoDevUpReq_ReqZero

/// \Given a pointer to the device (co_dev_t) containing an object with an upload indication function which sets 0 as the requested size
///
/// \When co_dev_up_req() is called with an index and a sub-index of the object, no memory buffer to store the requested value and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called with a null pointer, the index, the sub-index, CO_SDO_AC_NO_DATA, a null uploaded bytes pointer, 0 as a size of the value and a null user-specified data pointer; two 0x00 bytes are copied to the memory buffer
///       \Calls get_errc()
///       \IfCalls{LELY_NO_MALLOC, membuf_init()}
///       \Calls co_sdo_req_init()
///       \Calls co_dev_find_obj()
///       \Calls co_obj_find_sub()
///       \Calls co_obj_get_code()
///       \Calls co_sub_up_ind()
///       \Calls membuf_reserve()
///       \Calls membuf_size()
///       \Calls co_sdo_req_fini()
///       \Calls membuf_fini()
///       \Calls set_errc()
TEST(CO_Csdo, CoDevUpReq_ReqZero) {
  using namespace CoDevUpReq_ReqZero;

  co_obj_set_up_ind(obj2020->Get(), req_up_ind, nullptr);
  char buffer[sizeof(sub_type)] = {0};
  membuf mbuf = MEMBUF_INIT;
  membuf_init(&mbuf, buffer, sizeof(sub_type));

  const auto ret =
      co_dev_up_req(dev, IDX, SUBIDX, &mbuf, CoCsdoUpCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CoCsdoUpCon::Check(nullptr, IDX, SUBIDX, 0, membuf_begin(&mbuf),
                     sizeof(sub_type), nullptr);
  MembufCheck(&mbuf, buffer, sizeof(sub_type));
  CHECK_EQUAL(0x0000u, ldle_u16(CoCsdoUpCon::buf));
}

namespace CoDevUpReq_IndBufIsReqBuf {
static char ind_buffer[sizeof(sub_type)] = {0};
static membuf ind_mbuf = {ind_buffer, ind_buffer,
                          ind_buffer + sizeof(sub_type)};

co_unsigned32_t
req_up_ind(const co_sub_t* sub, co_sdo_req* req, co_unsigned32_t ac, void*) {
  req->membuf = &ind_mbuf;

  co_sub_on_up(sub, req, &ac);

  return 0;
}
}  // namespace CoDevUpReq_IndBufIsReqBuf

/// \Given a pointer to the device (co_dev_t) containing an object with an upload indication function which sets the buffer with bytes to upload and a memory buffer
///
/// \When co_dev_up_req() is called with an index and a sub-index of the object, the same memory buffer as set by the indication function, to store the requested value and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called with a null pointer, the index, the sub-index, 0 as the abort code, a pointer to the beginning of the memory buffer, a size of the value and a null user-specified data pointer; the memory buffer contains the requested value
///       \Calls get_errc()
///       \IfCalls{LELY_NO_MALLOC, membuf_init()}
///       \Calls co_sdo_req_init()
///       \Calls co_dev_find_obj()
///       \Calls co_obj_find_sub()
///       \Calls co_obj_get_code()
///       \Calls co_sub_up_ind()
///       \Calls co_sdo_req_last()
///       \Calls co_sdo_req_fini()
///       \Calls membuf_fini()
///       \Calls set_errc()
TEST(CO_Csdo, CoDevUpReq_IndBufIsReqBuf) {
  using namespace CoDevUpReq_IndBufIsReqBuf;

  co_obj_set_up_ind(obj2020->Get(), req_up_ind, nullptr);
  co_dev_set_val_u16(dev, IDX, SUBIDX, 0x1234u);

  const auto ret =
      co_dev_up_req(dev, IDX, SUBIDX, &ind_mbuf, CoCsdoUpCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CoCsdoUpCon::Check(nullptr, IDX, SUBIDX, 0, membuf_begin(&ind_mbuf),
                     sizeof(sub_type), nullptr);
  MembufCheck(&ind_mbuf, ind_buffer, sizeof(sub_type));
  CHECK_EQUAL(0x1234u, ldle_u16(CoCsdoUpCon::buf));
}

namespace CoDevUpReq_NotAbleToComplete {
co_unsigned32_t
req_up_ind(const co_sub_t* sub, co_sdo_req* req, co_unsigned32_t ac, void*) {
  co_sub_on_up(sub, req, &ac);
  req->nbyte = 0;  // function was not able to complete the download

  return 0;
}
}  // namespace CoDevUpReq_NotAbleToComplete

/// \Given a pointer to the device (co_dev_t) containing an object with an upload indication function which is not able to complete the download
///
/// \When co_dev_up_req() is called with an index and a sub-index of the sub-object, a memory buffer to store the requested value and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called with a null pointer, the index, the sub-index, CO_SDO_AC_NO_MEM, a null memory buffer pointer, 0 as a size of the value and a null user-specified data pointer; the memory buffer contains the requested value
TEST(CO_Csdo, CoDevUpReq_NotAbleToComplete) {
  using namespace CoDevUpReq_NotAbleToComplete;

  membuf mbuf = MEMBUF_INIT;
  char buffer[sizeof(sub_type)] = {0};
  membuf_init(&mbuf, buffer, sizeof(sub_type));

  co_dev_set_val_u16(dev, IDX, SUBIDX, 0x1234u);
  co_obj_set_up_ind(obj2020->Get(), req_up_ind, nullptr);

  const auto ret =
      co_dev_up_req(dev, IDX, SUBIDX, &mbuf, CoCsdoUpCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CoCsdoUpCon::Check(nullptr, IDX, SUBIDX, CO_SDO_AC_NO_MEM, nullptr, 0,
                     nullptr);
  MembufCheck(&mbuf, buffer, sizeof(sub_type));
  CHECK_EQUAL(0x1234u,
              ldle_u16(static_cast<uint_least8_t*>(membuf_begin(&mbuf))));
}

namespace CoDevUpReq_ExternalBufferTooSmall {
static char ind_buffer[sizeof(sub_type)] = {0};
static membuf ind_mbuf = {ind_buffer, ind_buffer,
                          ind_buffer + sizeof(sub_type)};

co_unsigned32_t
req_up_ind(const co_sub_t* sub, co_sdo_req* req, co_unsigned32_t ac, void*) {
  req->membuf = &ind_mbuf;

  co_sub_on_up(sub, req, &ac);

  return 0;
}
}  // namespace CoDevUpReq_ExternalBufferTooSmall

/// \Given a pointer to the device (co_dev_t) containing an object with an upload indication function which sets the requested size as 2, offset as 0 and nbyte as 0
///
/// \When co_dev_up_req() is called with an index and a sub-index of the object, a memory buffer to store the requested value and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called with a null pointer, the index, the sub-index, 0 as the abort code, a pointer to the memory buffer, requested size of the value and a null user-specified data pointer; if LELY_NO_MALLOC: the supplied buffer remains empty; else the memory buffer is not empty and contains the requested value
///       \Calls get_errc()
///       \IfCalls{LELY_NO_MALLOC, membuf_init()}
///       \Calls co_sdo_req_init()
///       \Calls co_dev_find_obj()
///       \Calls co_obj_find_sub()
///       \Calls co_obj_get_code()
///       \Calls co_sub_up_ind()
///       \Calls co_sdo_req_last()
///       \Calls co_sdo_req_fini()
///       \Calls membuf_fini()
///       \Calls set_errc()
TEST(CO_Csdo, CoDevUpReq_ExternalBufferTooSmall) {
  using namespace CoDevUpReq_ExternalBufferTooSmall;

  co_dev_set_val_u16(dev, IDX, SUBIDX, 0x1234u);
  co_obj_set_up_ind(obj2020->Get(), req_up_ind, nullptr);

  const size_t EXT_BUFSIZE = sizeof(sub_type) - 1u;
  membuf ext_mbuf = MEMBUF_INIT;
#if LELY_NO_MALLOC
  char ext_buffer[EXT_BUFSIZE] = {0};
  membuf_init(&ext_mbuf, ext_buffer, EXT_BUFSIZE);
#else
  const auto mret = membuf_reserve(&ext_mbuf, EXT_BUFSIZE);
  CHECK_COMPARE(mret, >=, EXT_BUFSIZE);
#endif

  const auto ret =
      co_dev_up_req(dev, IDX, SUBIDX, &ext_mbuf, CoCsdoUpCon::func, nullptr);

  CHECK_EQUAL(0, ret);
#if LELY_NO_MALLOC
  CoCsdoUpCon::Check(nullptr, IDX, SUBIDX, CO_SDO_AC_NO_MEM, nullptr, 0,
                     nullptr);
  MembufCheck(&ext_mbuf, ext_buffer, 0);
#else
  CoCsdoUpCon::CheckNonempty(nullptr, IDX, SUBIDX, 0, sizeof(sub_type),
                             nullptr);
#endif
}

namespace CoDevUpReq_ExternalBufferNotLast {
static size_t num_called = 0;
static char ind_buffer[sizeof(sub_type)] = {0};
static membuf ind_mbuf = {ind_buffer, ind_buffer,
                          ind_buffer + sizeof(sub_type)};

co_unsigned32_t
req_up_ind(const co_sub_t* sub, co_sdo_req* req, co_unsigned32_t ac, void*) {
  req->membuf = &ind_mbuf;

  co_sub_on_up(sub, req, &ac);
  if (num_called < 1) req->nbyte = 0;
  num_called++;

  return 0;
}
}  // namespace CoDevUpReq_ExternalBufferNotLast

/// \Given a pointer to the device (co_dev_t) containing an object with an upload indication function which sets the requested size as greater than the size of the requested entry
///
/// \When co_dev_up_req() is called with an index and a sub-index of the sub-object, a memory buffer to store the requested value and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called with a null pointer, the index, the sub-index, 0 as the abort code, a pointer to the memory buffer, requested size of the value and a null user-specified data pointer; the memory buffer contains the requested value
///       \Calls get_errc()
///       \IfCalls{LELY_NO_MALLOC, membuf_init()}
///       \Calls co_sdo_req_init()
///       \Calls co_dev_find_obj()
///       \Calls co_obj_find_sub()
///       \Calls co_obj_get_code()
///       \Calls co_sub_up_ind()
///       \Calls co_sdo_req_last()
///       \Calls co_sdo_req_fini()
///       \Calls membuf_fini()
///       \Calls set_errc()
TEST(CO_Csdo, CoDevUpReq_ExternalBufferNotLast) {
  using namespace CoDevUpReq_ExternalBufferNotLast;

  co_dev_set_val_u16(dev, IDX, SUBIDX, 0x1234u);
  co_obj_set_up_ind(obj2020->Get(), req_up_ind, nullptr);

  const size_t EXT_BUFSIZE = sizeof(sub_type) + 1u;
  membuf ext_mbuf = MEMBUF_INIT;
#if LELY_NO_MALLOC
  char ext_buffer[EXT_BUFSIZE] = {0};
  membuf_init(&ext_mbuf, ext_buffer, EXT_BUFSIZE);
  CHECK_EQUAL(EXT_BUFSIZE, membuf_reserve(&ext_mbuf, EXT_BUFSIZE));
#else
  CHECK_EQUAL(LELY_MEMBUF_SIZE, membuf_reserve(&ext_mbuf, EXT_BUFSIZE));
#endif

  const auto ret =
      co_dev_up_req(dev, IDX, SUBIDX, &ext_mbuf, CoCsdoUpCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CoCsdoUpCon::Check(nullptr, IDX, SUBIDX, 0, membuf_begin(&ext_mbuf),
                     sizeof(sub_type), nullptr);
  CHECK_EQUAL(0x1234u, ldle_u16(CoCsdoUpCon::buf));
}

namespace CoDevUpReq_ExternalBuffer {
static char ind_buffer[sizeof(sub_type)] = {0};
static membuf ind_mbuf = {ind_buffer, ind_buffer,
                          ind_buffer + sizeof(sub_type)};

co_unsigned32_t
req_up_ind(const co_sub_t* sub, co_sdo_req* req, co_unsigned32_t ac, void*) {
  req->membuf = &ind_mbuf;

  co_sub_on_up(sub, req, &ac);

  return 0;
}
}  // namespace CoDevUpReq_ExternalBuffer

/// \Given a pointer to the device (co_dev_t) containing an object with an upload indication function which sets the requested size as 2, offset as 0 and nbyte as 0
///
/// \When co_dev_up_req() is called with an index and a sub-index of the object, a memory buffer to store the requested value and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called with a null pointer, the index, the sub-index, 0 as the abort code, a pointer to the memory buffer, requested size of the value and a null user-specified data pointer; the memory buffer contains the requested value
///       \Calls get_errc()
///       \IfCalls{LELY_NO_MALLOC, membuf_init()}
///       \Calls co_sdo_req_init()
///       \Calls co_dev_find_obj()
///       \Calls co_obj_find_sub()
///       \Calls co_obj_get_code()
///       \Calls co_sub_up_ind()
///       \Calls co_sdo_req_last()
///       \Calls co_sdo_req_fini()
///       \Calls membuf_fini()
///       \Calls set_errc()
TEST(CO_Csdo, CoDevUpReq_ExternalBuffer) {
  using namespace CoDevUpReq_ExternalBuffer;

  co_dev_set_val_u16(dev, IDX, SUBIDX, 0x1234u);
  co_obj_set_up_ind(obj2020->Get(), req_up_ind, nullptr);

  membuf ext_mbuf = MEMBUF_INIT;
#if LELY_NO_MALLOC
  char ext_buffer[sizeof(sub_type)] = {0};
  membuf_init(&ext_mbuf, ext_buffer, sizeof(sub_type));
  CHECK_EQUAL(sizeof(sub_type), membuf_reserve(&ext_mbuf, sizeof(sub_type)));
#else
  CHECK_EQUAL(LELY_MEMBUF_SIZE, membuf_reserve(&ext_mbuf, sizeof(sub_type)));
#endif

  const auto ret =
      co_dev_up_req(dev, IDX, SUBIDX, &ext_mbuf, CoCsdoUpCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CoCsdoUpCon::Check(nullptr, IDX, SUBIDX, 0, membuf_begin(&ext_mbuf),
                     sizeof(sub_type), nullptr);
  CHECK_EQUAL(0x1234u, ldle_u16(CoCsdoUpCon::buf));
}

/// \Given a pointer to the device (co_dev_t) containing an object with a default set as an upload indication function
///
/// \When co_dev_up_req() is called with an index and a sub-index of the object, a memory buffer to store the requested value and a confirmation function
///
/// \Then 0 is returned, the confirmation function is called with a null pointer, the index, the sub-index, 0 as the abort code, a pointer to the memory buffer, requested size of the value and a null user-specified data pointer, the memory buffer contains the requested value
///       \Calls get_errc()
///       \IfCalls{LELY_NO_MALLOC, membuf_init()}
///       \Calls co_sdo_req_init()
///       \Calls co_dev_find_obj()
///       \Calls co_obj_find_sub()
///       \Calls co_obj_get_code()
///       \Calls co_sub_up_ind()
///       \Calls membuf_reserve()
///       \Calls membuf_size()
///       \Calls co_sdo_req_fini()
///       \Calls membuf_fini()
///       \Calls set_errc()
TEST(CO_Csdo, CoDevUpReq_Nominal) {
  membuf mbuf = MEMBUF_INIT;
  char buffer[sizeof(sub_type)] = {0};
  membuf_init(&mbuf, buffer, sizeof(sub_type));

  co_dev_set_val_u16(dev, IDX, SUBIDX, 0x1234u);

  const auto ret =
      co_dev_up_req(dev, IDX, SUBIDX, &mbuf, CoCsdoUpCon::func, nullptr);

  CHECK_EQUAL(0, ret);
  CoCsdoUpCon::Check(nullptr, IDX, SUBIDX, 0, membuf_begin(&mbuf),
                     sizeof(sub_type), nullptr);
  MembufCheck(&mbuf, buffer, sizeof(sub_type));
  CHECK_EQUAL(0x1234u, ldle_u16(CoCsdoUpCon::buf));
}

///@}
