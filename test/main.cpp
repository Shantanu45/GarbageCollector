#include <iostream>
#include "gtest/gtest.h"

#pragma region SIMPLE TEST

TEST(SimpleTest, TestValues)
{
	int x = 1;

	std::string str = "Shan";

	EXPECT_EQ(x, 1);		// Equal
	EXPECT_GT(x, 0);		// Greater than

	ASSERT_EQ(x, 1);

	EXPECT_FALSE(false);
	EXPECT_TRUE(true);
	EXPECT_STREQ(str.c_str(), "Shan");		// String equal

}

#pragma endregion

// ============================				MAIN			============================ //

int main(int argc, char** argv) {
	::testing::InitGoogleTest(&argc, argv);
	return RUN_ALL_TESTS();
}