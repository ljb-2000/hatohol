/*
 * Copyright (C) 2013 Project Hatohol
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
#include "AtomicValue.h"
using namespace mlpl;

namespace testAtomicValue {

// ---------------------------------------------------------------------------
// Test cases
// ---------------------------------------------------------------------------
void test_Define(void)
{
	AtomicValue<int> val;
}

void test_DefineWithInit(void)
{
	AtomicValue<int> val(5);
}

void test_get(void)
{
	int initValue = -3;
	AtomicValue<int> val(initValue);
	cppcut_assert_equal(initValue, val.get());
}

void test_setAndGet(void)
{
	int initValue = -3;
	AtomicValue<int> val;
	val.set(initValue);
	cppcut_assert_equal(initValue, val.get());
}

void test_add(void)
{
	const int initValue = 5;
	const int addedValue = 3;
	AtomicValue<int> val(initValue);
	cppcut_assert_equal(initValue + addedValue, val.add(addedValue));
}

} // namespace testAtomicValue
