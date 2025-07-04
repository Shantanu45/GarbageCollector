#include "MemoryManager/ObjectHeader.h"
#include "gtest/gtest.h"

namespace {

    TEST(Header, MarkSweep) {
        ObjectHeader header = {
            .size = 0xA,
            .mark = 1,
        };

        EXPECT_EQ(header.size, 0xA);
        EXPECT_EQ(header.mark, 1);

        EXPECT_EQ(header.toInt(), 0x010A0000);
        EXPECT_EQ((uint32_t)header, header.toInt());

        header.mark = 0;
        EXPECT_EQ(header.toInt(), 0x000A0000);

        header.size = 0xFF;
        EXPECT_EQ(header.toInt(), 0x00FF0000);
    }

    TEST(Header, ReferenceCounting) {
        ObjectHeader header = {
            .size = 0xA,
            .rc = 10,
        };

        EXPECT_EQ(header.size, 0xA);
        EXPECT_EQ(header.rc, 10);

        EXPECT_EQ(header.toInt(), 0x0A0A0000);
        EXPECT_EQ((uint32_t)header, header.toInt());


        header.rc++;
        EXPECT_EQ(header.toInt(), 0x0B0A0000);

        header.size = 0xFF;
        EXPECT_EQ(header.toInt(), 0x0BFF0000);
    }

}  // namespace