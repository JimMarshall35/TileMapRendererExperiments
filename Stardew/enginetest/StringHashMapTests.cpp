#include <gtest/gtest.h>
#include "StringKeyHashMap.h"

TEST(HashMap, InsertAndSearch)
{
	struct HashMap hashMap;
	HashmapInit(&hashMap, 10, sizeof(int));

	int i = 42;
	HashmapInsert(&hashMap, "FortyTwo", &i);
	i = 64;
	HashmapInsert(&hashMap, "SixtyFour", &i);
	i = 180;
	HashmapInsert(&hashMap, "ONE HUNDRED AND EIGHTY!!", &i);
	
	int* pInt = (int*)HashmapSearch(&hashMap, "FortyTwo");
	ASSERT_NE(pInt, nullptr);
	ASSERT_EQ(*pInt, 42);

	pInt = (int*)HashmapSearch(&hashMap, "SixtyFour");
	ASSERT_NE(pInt, nullptr);
	ASSERT_EQ(*pInt, 64);
	
	pInt = (int*)HashmapSearch(&hashMap, "ONE HUNDRED AND EIGHTY!!");
	ASSERT_NE(pInt, nullptr);
	ASSERT_EQ(*pInt, 180);

	pInt = (int*)HashmapSearch(&hashMap, "three");
	ASSERT_EQ(pInt, nullptr);

	ASSERT_EQ(hashMap.size, 3);

	HashMapDeInit(&hashMap);
}

TEST(HashMap, InsertAndSearchStruct)
{
	struct TestStruct
	{
		int a;
		char b;
	};
	struct HashMap hashMap;
	HashmapInit(&hashMap, 10, sizeof(TestStruct));

	TestStruct i = {42, 'a'};
	HashmapInsert(&hashMap, "FortyTwo", &i);
	i = { 29, 'b' };
	HashmapInsert(&hashMap, "TwentyNine", &i); /* this tests an index collision as well, currently */
	i = { 8, 'c'};
	HashmapInsert(&hashMap, "Eight", &i);

	TestStruct* pInt = (TestStruct*)HashmapSearch(&hashMap, "FortyTwo");
	ASSERT_NE(pInt, nullptr);
	ASSERT_EQ(pInt->a, 42);
	ASSERT_EQ(pInt->b, 'a');

	pInt = (TestStruct*)HashmapSearch(&hashMap, "TwentyNine");
	ASSERT_NE(pInt, nullptr);
	ASSERT_EQ(pInt->a, 29);
	ASSERT_EQ(pInt->b, 'b');

	pInt = (TestStruct*)HashmapSearch(&hashMap, "Eight");
	ASSERT_NE(pInt, nullptr);
	ASSERT_EQ(pInt->a, 8);
	ASSERT_EQ(pInt->b, 'c');

	pInt = (TestStruct*)HashmapSearch(&hashMap, "fkoddfs");
	ASSERT_EQ(pInt, nullptr);

	HashMapDeInit(&hashMap);
}
