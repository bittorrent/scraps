/**
* Copyright 2016 BitTorrent Inc.
*
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
*
*    http://www.apache.org/licenses/LICENSE-2.0
*
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "scraps/ColorProfile.h"

#include <gtest/gtest.h>

using namespace scraps;

TEST(ColorProfile, comparison) {
    EXPECT_EQ(ColorProfiles::kSRGB, ColorProfiles::kSRGB);
    EXPECT_NE(ColorProfiles::kSRGB, ColorProfiles::kBT709);
}

TEST(ColorProfile, transferFunctionInversion) {
    for (auto& function : {
        TransferFunctions::kSRGB,
        TransferFunctions::kBT709,
        TransferFunctions::kGamma<26, 10>,
    }) {
        for (auto i = 0; i <= 20; ++i) {
            EXPECT_FLOAT_EQ(function.toVoltage(function.toLinear(i / 20.0)), i / 20.0);
        }
    }
}

TEST(ColorProfile, transformations) {
    math::Vector<double, 3> in{0.1, 0.7, 0.95};

    for (auto& profile : {
        ColorProfiles::kSRGB,
        ColorProfiles::kBT709,
    }) {
        auto out = TransformColor(in, profile, profile);
        EXPECT_FLOAT_EQ(out.r, in.r);
        EXPECT_FLOAT_EQ(out.g, in.g);
        EXPECT_FLOAT_EQ(out.b, in.b);
    }

    for (auto& test : {
        std::make_tuple(ColorProfiles::kSRGB, ColorProfiles::kBT709, math::Vector<double, 3>{0.0396, 0.6667, 0.9440}),
        std::make_tuple(ColorProfiles::kSRGB, ColorProfiles::kBT2020, math::Vector<double, 3>{0.4243, 0.6469, 0.9154}),
    }) {
        auto out = TransformColor(in, std::get<0>(test), std::get<1>(test));
        EXPECT_NEAR(out.r, std::get<2>(test).r, 0.01);
        EXPECT_NEAR(out.g, std::get<2>(test).g, 0.01);
        EXPECT_NEAR(out.b, std::get<2>(test).b, 0.01);
    }
}
