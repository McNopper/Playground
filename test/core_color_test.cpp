#include <gtest/gtest.h>

#include "core/core.h"

TEST(TestColor, BT709)
{
    auto m = toXYZ(COLOR_PRIMARY_BT709);

    EXPECT_NEAR(m[0].x, 0.412391f, 0.000001f);
    EXPECT_NEAR(m[0].y, 0.357584f, 0.000001f);
    EXPECT_NEAR(m[0].z, 0.180481f, 0.000001f);

    EXPECT_NEAR(m[1].x, 0.212639f, 0.000001f);
    EXPECT_NEAR(m[1].y, 0.715169f, 0.000001f);
    EXPECT_NEAR(m[1].z, 0.072192f, 0.000001f);

    EXPECT_NEAR(m[2].x, 0.019331f, 0.000001f);
    EXPECT_NEAR(m[2].y, 0.119195f, 0.000001f);
    EXPECT_NEAR(m[2].z, 0.950532f, 0.000001f);

    //

    auto mi = inverse(m);

    EXPECT_NEAR(mi[0].x, 3.240970f, 0.000001f);
    EXPECT_NEAR(mi[0].y, -1.537383f, 0.000001f);
    EXPECT_NEAR(mi[0].z, -0.498611f, 0.000001f);

    EXPECT_NEAR(mi[1].x, -0.969244f, 0.000001f);
    EXPECT_NEAR(mi[1].y, 1.875968f, 0.000001f);
    EXPECT_NEAR(mi[1].z, 0.041555f, 0.000001f);

    EXPECT_NEAR(mi[2].x, 0.055630f, 0.000001f);
    EXPECT_NEAR(mi[2].y, -0.203977f, 0.000001f);
    EXPECT_NEAR(mi[2].z, 1.056972f, 0.000001f);
}

TEST(TestColor, BT2020)
{
    auto m = toXYZ(COLOR_PRIMARY_BT2020);

    EXPECT_NEAR(m[0].x, 0.636958f, 0.000001f);
    EXPECT_NEAR(m[0].y, 0.144617f, 0.000001f);
    EXPECT_NEAR(m[0].z, 0.168881f, 0.000001f);

    EXPECT_NEAR(m[1].x, 0.262700f, 0.000001f);
    EXPECT_NEAR(m[1].y, 0.677998f, 0.000001f);
    EXPECT_NEAR(m[1].z, 0.059302f, 0.000001f);

    EXPECT_NEAR(m[2].x, 0.000000f, 0.000001f);
    EXPECT_NEAR(m[2].y, 0.028073f, 0.000001f);
    EXPECT_NEAR(m[2].z, 1.060985f, 0.000001f);

    //

    auto mi = inverse(m);

    EXPECT_NEAR(mi[0].x, 1.716651f, 0.000001f);
    EXPECT_NEAR(mi[0].y, -0.355671f, 0.000001f);
    EXPECT_NEAR(mi[0].z, -0.253366f, 0.000001f);

    EXPECT_NEAR(mi[1].x, -0.666684f, 0.000001f);
    EXPECT_NEAR(mi[1].y, 1.616481f, 0.000001f);
    EXPECT_NEAR(mi[1].z, 0.015769f, 0.000001f);

    EXPECT_NEAR(mi[2].x, 0.017640f, 0.000001f);
    EXPECT_NEAR(mi[2].y, -0.042771f, 0.000001f);
    EXPECT_NEAR(mi[2].z, 0.942103f, 0.000001f);
}
