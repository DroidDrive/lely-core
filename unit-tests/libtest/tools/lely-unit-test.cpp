/**@file
 * This file is part of the CANopen Library Unit Test Suite.
 *
 * @copyright 2020 N7 Space Sp. z o.o.
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

#include "lely-unit-test.hpp"

co_csdo_t* CoCsdoDnCon::sdo = nullptr;
co_unsigned16_t CoCsdoDnCon::idx = 0;
co_unsigned8_t CoCsdoDnCon::subidx = 0;
co_unsigned32_t CoCsdoDnCon::ac = 0;
void* CoCsdoDnCon::data = nullptr;
unsigned int CoCsdoDnCon::num_called = 0;

int CanSend::ret = 0;
void* CanSend::data = nullptr;
unsigned int CanSend::num_called = 0;
can_msg CanSend::msg = CAN_MSG_INIT;
can_msg* CanSend::msg_buf = &CanSend::msg;
size_t CanSend::buf_size = 1u;
