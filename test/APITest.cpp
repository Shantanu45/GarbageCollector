#include "api.h"
#include "gtest/gtest.h"

namespace
{
	TEST(API, TESTFN)
	{
		int i = GetInt();
		EXPECT_EQ(i, 5);
	}
}