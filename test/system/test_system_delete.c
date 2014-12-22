/*
* kinetic-c
* Copyright (C) 2014 Seagate Technology.
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
*
*/
#include "system_test_fixture.h"
#include "kinetic_client.h"

static SystemTestFixture Fixture;
static uint8_t KeyData[1024];
static ByteBuffer KeyBuffer;
static uint8_t TagData[1024];
static ByteBuffer TagBuffer;
static ByteArray TestValue;
static uint8_t ValueData[KINETIC_OBJ_SIZE];
static ByteBuffer ValueBuffer;

void setUp(void)
{
    SystemTestSetup(&Fixture, 1);

    KeyBuffer = ByteBuffer_CreateAndAppendCString(KeyData, sizeof(KeyData), "DELETE test key");
    TagBuffer = ByteBuffer_CreateAndAppendCString(TagData, sizeof(TagData), "SomeTagValue");
    TestValue = ByteArray_CreateWithCString("lorem ipsum... blah blah blah... etc.");
    ValueBuffer = ByteBuffer_CreateAndAppendCString(ValueData, sizeof(ValueData), "lorem ipsum... blah blah blah... etc.");
}

void tearDown(void)
{
    SystemTestTearDown(&Fixture);
}

void test_Delete_should_delete_an_object_from_device(void)
{
    LOG_LOCATION;
    KineticStatus status;

    // Create an object so that we have something to delete
    KineticEntry putEntry = {
        .key = KeyBuffer,
        .tag = TagBuffer,
        .algorithm = KINETIC_ALGORITHM_SHA1,
        .value = ValueBuffer,
        .force = true,
        .synchronization = KINETIC_SYNCHRONIZATION_WRITETHROUGH,
    };
    status = KineticClient_Put(&Fixture.session, &putEntry, NULL);
    TEST_ASSERT_EQUAL_KineticStatus(KINETIC_STATUS_SUCCESS, status);
    // TEST_ASSERT_EQUAL_ByteArray(Key, putEntry.key.array);
    // TEST_ASSERT_EQUAL_ByteArray(Tag, putEntry.tag.array);
    TEST_ASSERT_EQUAL(KINETIC_ALGORITHM_SHA1, putEntry.algorithm);

    // Validate the object exists initially
    KineticEntry getEntry = {
        .key = KeyBuffer,
        .tag = TagBuffer,
        .algorithm = KINETIC_ALGORITHM_SHA1,
        .value = ValueBuffer,
        .force = true,
        .synchronization = KINETIC_SYNCHRONIZATION_WRITETHROUGH,
    };
    status = KineticClient_Get(&Fixture.session, &getEntry, NULL);
    TEST_ASSERT_EQUAL_KineticStatus(KINETIC_STATUS_SUCCESS, status);
    TEST_ASSERT_EQUAL_ByteArray(putEntry.key.array, getEntry.key.array);
    TEST_ASSERT_EQUAL_ByteArray(putEntry.tag.array, getEntry.tag.array);
    TEST_ASSERT_EQUAL(putEntry.algorithm, getEntry.algorithm);
    TEST_ASSERT_EQUAL_ByteBuffer(putEntry.value, getEntry.value);

    // Delete the object
    KineticEntry deleteEntry = {
        .key = KeyBuffer,
    };
    status = KineticClient_Delete(&Fixture.session, &deleteEntry, NULL);
    TEST_ASSERT_EQUAL_KineticStatus(KINETIC_STATUS_SUCCESS, status);
    TEST_ASSERT_EQUAL(0, deleteEntry.value.bytesUsed);

    // Validate the object no longer exists
    KineticEntry regetEntryMetadata = {
        .key = KeyBuffer,
        .metadataOnly = true,
    };
    status = KineticClient_Get(&Fixture.session, &regetEntryMetadata, NULL);
    TEST_ASSERT_EQUAL_KineticStatus(KINETIC_STATUS_NOT_FOUND, status);
    TEST_ASSERT_ByteArray_EMPTY(regetEntryMetadata.value.array);
}

/*******************************************************************************
* ENSURE THIS IS AFTER ALL TESTS IN THE TEST SUITE
*******************************************************************************/
SYSTEM_TEST_SUITE_TEARDOWN(&Fixture)
