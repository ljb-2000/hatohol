/*
 * Copyright (C) 2014 Project Hatohol
 *
 * This file is part of Hatohol.
 *
 * Hatohol is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * Hatohol is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Hatohol. If not, see <http://www.gnu.org/licenses/>.
 */

#include <cppcutter.h>
#include <SimpleSemaphore.h>
#include "Helpers.h"
#include "HatoholArmPluginInterface.h"
#include "HatoholArmPluginInterfaceTest.h"

using namespace std;
using namespace mlpl;
using namespace qpid::messaging;

namespace testHatoholArmPluginInterface {

static const size_t TIMEOUT = 5000;

void _assertHapiItemDataHeader(
  const HapiItemDataHeader *header,
  const ItemDataType &type, const ItemId &itemId, const bool &isNull = false)
{
	cppcut_assert_equal(isNull, (bool)EndianConverter::LtoN(header->flags));
	cppcut_assert_equal(type,
	                    (ItemDataType)EndianConverter::LtoN(header->type));
	cppcut_assert_equal(itemId,
	                    (ItemId)EndianConverter::LtoN(header->itemId));
}
#define assertHapiItemDataHeader(H,T,I,...) \
  cut_trace(_assertHapiItemDataHeader(H,T,I,##__VA_ARGS__))

template<typename ValueType, typename BodyType>
void _assertHapiItemDataBody(const void *body, const ValueType &expect)
{
	const BodyType *valuePtr = static_cast<const BodyType *>(body);
	cppcut_assert_equal(expect,
	                    (ValueType)EndianConverter::LtoN(*valuePtr));
}
#define assertHapiItemDataBody(VT,BT,BP,E) \
  cut_trace((_assertHapiItemDataBody<VT,BT>)(BP,E))

// ---------------------------------------------------------------------------
// Test cases
// ---------------------------------------------------------------------------
void test_constructor(void)
{
	HatoholArmPluginInterface hapi;
}

void test_onConnected(void)
{
	HatoholArmPluginInterfaceTest hapi;
	hapi.assertStartAndWaitConnected();
}

void test_sendAndonReceived(void)
{
	const string testMessage = "FOO";

	// start HAPI pair
	HatoholArmPluginInterfaceTest hapiSv;
	hapiSv.assertStartAndWaitConnected();

	HatoholArmPluginInterfaceTest hapiCl(hapiSv);
	hapiCl.assertStartAndWaitConnected();

	// wait for the completion of the initiation
	hapiSv.assertWaitInitiated();

	// send the message and receive it
	hapiSv.setMessageIntercept();
	hapiCl.send(testMessage);
	cppcut_assert_equal(SimpleSemaphore::STAT_OK,
	                    hapiSv.getRcvSem().timedWait(TIMEOUT));
	cppcut_assert_equal(testMessage, hapiSv.getMessage());
}

void test_registCommandHandler(void)
{
	struct Hapi : public HatoholArmPluginInterfaceTest {
		const HapiCommandCode testCmdCode;
		HapiCommandCode       gotCmdCode;
		SimpleSemaphore       handledSem;

		Hapi(void)
		: testCmdCode((HapiCommandCode)5),
		  gotCmdCode((HapiCommandCode)0),
		  handledSem(0)
		{
			registerCommandHandler(
			  testCmdCode, (CommandHandler)&Hapi::handler);
		}

		void handler(const HapiCommandHeader *header)
		{
			gotCmdCode = (HapiCommandCode)header->code;
			handledSem.post();
		}
	} hapiSv;
	hapiSv.assertStartAndWaitConnected();

	HatoholArmPluginInterfaceTest hapiCl(hapiSv);
	hapiCl.assertStartAndWaitConnected();

	// wait for the completion of the initiation
	hapiSv.assertWaitInitiated();

	// send command code and wait for the callback.
	SmartBuffer cmdBuf;
	hapiCl.callSetupCommandHeader<void>(cmdBuf, hapiSv.testCmdCode);
	hapiCl.send(cmdBuf);

	cppcut_assert_equal(SimpleSemaphore::STAT_OK,
	                    hapiSv.handledSem.timedWait(TIMEOUT));
	cppcut_assert_equal(hapiSv.testCmdCode, hapiSv.gotCmdCode);
}

void test_onGotResponse(void)
{
	struct Hapi : public HatoholArmPluginInterfaceTest {
		HapiResponseCode      gotResCode;
		SimpleSemaphore       gotResSem;

		Hapi(void)
		: gotResCode(NUM_HAPI_CMD_RES),
		  gotResSem(0)
		{
		}
		
		void onGotResponse(const HapiResponseHeader *header,
		                   SmartBuffer &resBuf) override
		{
			gotResCode =
			  static_cast<HapiResponseCode>(header->code);
			gotResSem.post();
		}
	} hapiSv;
	hapiSv.assertStartAndWaitConnected();

	HatoholArmPluginInterfaceTest hapiCl(hapiSv);
	hapiCl.assertStartAndWaitConnected();

	// wait for the completion of the initiation
	hapiSv.assertWaitInitiated();

	// send a command code and wait for the callback.
	SmartBuffer resBuf;
	hapiCl.callSetupResponseBuffer<void>(resBuf);
	HapiResponseHeader *header = resBuf.getPointer<HapiResponseHeader>(0);
	header->sequenceId = 0;
	hapiCl.send(resBuf);

	cppcut_assert_equal(SimpleSemaphore::STAT_OK,
	                    hapiSv.gotResSem.timedWait(TIMEOUT));
	cppcut_assert_equal(HAPI_RES_OK, hapiSv.gotResCode);
}

void test_getString(void)
{
	SmartBuffer buf(10);
	char *head = buf.getPointer<char>(1);
	const size_t offset = 3;
	head[offset+0] = 'A';
	head[offset+1] = 'B';
	head[offset+2] = '\0';
	cut_assert_equal_string(
	  "AB", HatoholArmPluginInterface::getString(buf, head, offset, 2));
}

void test_getStringWithTooLongParam(void)
{
	SmartBuffer buf(3);
	char *head = buf.getPointer<char>(0);
	const size_t offset = 0;
	head[offset+0] = 'A';
	head[offset+1] = 'B';
	head[offset+2] = '\0';
	cut_assert_equal_string(
	  NULL,
	  HatoholArmPluginInterface::getString(buf, head, offset, 3));
}

void test_getStringWithoutNullTerm(void)
{
	SmartBuffer buf(3);
	char *head = buf.getPointer<char>(0);
	const size_t offset = 0;
	head[offset+0] = 'A';
	head[offset+1] = 'B';
	head[offset+2] = 'C';
	cut_assert_equal_string(
	  NULL,
	  HatoholArmPluginInterface::getString(buf, head, offset, 2));
}

void test_putString(void)
{
	char _buf[50];
	char *refAddr = &_buf[15];
	char *buf = &_buf[20];
	string str = "Cat";
	uint16_t offset;
	uint16_t length;
	char *next = HatoholArmPluginInterface::putString(buf, refAddr, str,
	                                                  &offset, &length);
	cppcut_assert_equal((uint16_t)5, offset);
	cppcut_assert_equal((uint16_t)3, length);
	cppcut_assert_equal(&_buf[24], next);
	cut_assert_equal_string("Cat", buf);
}

void test_getIncrementedSequenceId(void)
{
	HatoholArmPluginInterfaceTest plugin;
	cppcut_assert_equal(1u, plugin.callGetIncrementedSequenceId());
	cppcut_assert_equal(2u, plugin.callGetIncrementedSequenceId());
	cppcut_assert_equal(3u, plugin.callGetIncrementedSequenceId());
}

void test_getIncrementedSequenceIdAroundMax(void)
{
	HatoholArmPluginInterfaceTest plugin;
	plugin.callSetSequenceId(HatoholArmPluginInterface::SEQ_ID_MAX-1);
	cppcut_assert_equal(
	  HatoholArmPluginInterface::SEQ_ID_MAX,
	  plugin.callGetIncrementedSequenceId());
	cppcut_assert_equal(0u, plugin.callGetIncrementedSequenceId());
	cppcut_assert_equal(1u, plugin.callGetIncrementedSequenceId());
}

void test_appendItemBool(void)
{
	SmartBuffer sbuf;
	const ItemId itemId0 = 5;
	const ItemId itemId1 = 309;
	ItemDataPtr itemFalse(new ItemBool(itemId0, false), false);
	ItemDataPtr itemTrue(new ItemBool(itemId1, true), false);

	const size_t expectBodySize = 1;
	const size_t expectSize = sizeof(HapiItemDataHeader) + expectBodySize;

	// append first one
	HatoholArmPluginInterface::appendItemData(sbuf, itemFalse);
	cppcut_assert_equal(expectSize, sbuf.index());
	const HapiItemDataHeader *header =
	  sbuf.getPointer<HapiItemDataHeader>(0);
	assertHapiItemDataHeader(header, ITEM_TYPE_BOOL, itemId0);
	assertHapiItemDataBody(bool, uint8_t, header + 1, false);

	// append 2nd one
	HatoholArmPluginInterface::appendItemData(sbuf, itemTrue);
	cppcut_assert_equal(2*expectSize, sbuf.index());
	header = sbuf.getPointer<HapiItemDataHeader>(expectSize);
	assertHapiItemDataHeader(header, ITEM_TYPE_BOOL, itemId1);
	assertHapiItemDataBody(bool, uint8_t, header + 1, true);
}

} // namespace testHatoholArmPluginInterface