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
#include <cutter.h>

#include "DBClientUser.h"
#include "Helpers.h"
#include "Hatohol.h"

namespace testDBClientUser {

static UserInfo testUserInfo[] = {
{
	0,                 // id
	"cheesecake",      // name
	"CDEF~!@#$%^&*()", // password
}, {
	0,                 // id
	"pineapple",       // name
	"Po+-\\|}{\":?><", // password
}, {
	0,                 // id
	"m1ffy@v@",        // name
	"",                // password
}
};
static size_t NumTestUserInfo = sizeof(testUserInfo) / sizeof(UserInfo);

static const char *TEST_DB_NAME = "test_db_user";
static const char *TEST_DB_USER = "hatohol_test_user";
static const char *TEST_DB_PASSWORD = ""; // empty: No password is used

static void loadTestDBUser(void)
{
	DBClientUser dbUser;
	for (size_t i = 0; i < NumTestUserInfo; i++)
		dbUser.addUserInfo(testUserInfo[i]);
}

static void setupTestDBUsers(bool dbRecreate = true)
{
	DBClient::setDefaultDBParams(DB_DOMAIN_ID_USERS, TEST_DB_NAME,
	                             TEST_DB_USER, TEST_DB_PASSWORD);
	makeTestMySQLDBIfNeeded(TEST_DB_NAME, dbRecreate);
}

void cut_setup(void)
{
	hatoholInit();
	setupTestDBUsers();
}

void cut_teardown(void)
{
}

// ---------------------------------------------------------------------------
// Test cases
// ---------------------------------------------------------------------------
void test_dbDomainId(void)
{
	DBClientUser dbUser;
	cppcut_assert_equal(DB_DOMAIN_ID_USERS,
	                    dbUser.getDBAgent()->getDBDomainId());
}

void test_createDB(void)
{
	// create an instance
	// Tables in the DB will be automatically created.
	DBClientUser dbUser;

	// check the version
	string statement = "select * from _dbclient_version";
	string expect =
	  StringUtils::sprintf(
	    "%d|%d\n", DB_DOMAIN_ID_USERS,
	               DBClientUser::USER_DB_VERSION);
	assertDBContent(dbUser.getDBAgent(), statement, expect);
}

void test_addUser(void)
{
	loadTestDBUser();
	DBClientUser dbUser;

	// check the version
	string statement = "select * from ";
	statement += DBClientUser::TABLE_NAME_USERS;
	statement += " ORDER BY id ASC";
	string expect;
	for (size_t i = 0; i < NumTestUserInfo; i++) {
		const UserInfo &userInfo = testUserInfo[i];
		expect += StringUtils::sprintf("%zd|%s|%s\n",
		  i+1, userInfo.name.c_str(),
		  Utils::sha256(userInfo.password).c_str());
	}
	assertDBContent(dbUser.getDBAgent(), statement, expect);
}

void test_getUserId(void)
{
	loadTestDBUser();
	const int targetIdx = 1;
	DBClientUser dbUser;
	int userId = dbUser.getUserId(testUserInfo[targetIdx].name,
	                              testUserInfo[targetIdx].password);
	cppcut_assert_equal(targetIdx+1, userId);
}

void test_getUserIdWrongUserPassword(void)
{
	loadTestDBUser();
	const int targetIdx = 1;
	DBClientUser dbUser;
	int userId = dbUser.getUserId(testUserInfo[targetIdx-1].name,
	                              testUserInfo[targetIdx].password);
	cppcut_assert_equal(INVALID_USER_ID, userId);
}

} // namespace testDBClientUser
