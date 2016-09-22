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
#pragma once

#include "scraps/config.h"

#include "scraps/math/Matrix.h"
#include "scraps/math/Vector.h"

namespace scraps {

struct ColorGamut {
    math::Vector<double, 2> r, g, b, w;

    constexpr math::Matrix<double, 3, 3> rgbToXYZ() const {
        math::Matrix<double, 3, 3> chromatacity{
                      r.x,           g.x,           b.x,
                      r.y,           g.y,           b.y,
            1 - r.x - r.y, 1 - g.x - g.y, 1 - b.x - b.y,
        };
        math::Vector<double, 3> white{w.x, w.y, (1 - w.x - w.y)};
        auto luminance = chromatacity.inverse() * (white / w.y);
        return chromatacity * math::Matrix<double, 3, 3>::Scaling(luminance);
    }

    constexpr bool operator==(const ColorGamut& other) const {
        return r == other.r && g == other.g && b == other.b && w == other.w;
    }

    constexpr bool operator!=(const ColorGamut& other) const { return !(*this == other); }
};

namespace ColorGamuts {
    static constexpr math::Vector<double, 2> kWhiteD65{0.3127, 0.3290};
    static constexpr math::Vector<double, 2> kWhiteC{0.310, 0.316};

    static constexpr ColorGamut kSRGB{
        {0.6400, 0.3300},
        {0.3000, 0.6000},
        {0.1500, 0.0600},
        kWhiteD65
    };
    static constexpr auto kBT709 = kSRGB;

    static constexpr ColorGamut kBT470M{
        { 0.67,  0.33},
        { 0.21,  0.71},
        { 0.14,  0.08},
        kWhiteC
    };

    static constexpr ColorGamut kBT470BG{
        {0.64,  0.33},
        {0.29,  0.60},
        {0.15,  0.06},
        kWhiteD65
    };
    static constexpr auto kBT601_625 = kBT470BG;

    static constexpr ColorGamut kBT601_525{
        {0.630, 0.340},
        {0.310, 0.595},
        {0.155, 0.070},
        kWhiteD65
    };
    static constexpr auto kSMPTE240M = kBT601_525;

    static constexpr ColorGamut kGenericFilm{
        {0.681, 0.319},
        {0.243, 0.692},
        {0.145, 0.049},
        kWhiteC
    };

    static constexpr ColorGamut kBT2020{
        {0.708, 0.292},
        {0.170, 0.797},
        {0.131, 0.046},
        kWhiteD65
    };

    static constexpr ColorGamut kSMPTE431_2{
        {0.680, 0.320},
        {0.265, 0.690},
        {0.150, 0.060},
        {0.314, 0.351},
    };

    static constexpr ColorGamut kSMPTE432_1{
        {0.680,  0.320},
        {0.265,  0.690},
        {0.150,  0.060},
        kWhiteD65
    };
}

} // namespace scraps
