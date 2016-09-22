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
#include "scraps/ColorGamut.h"

#include "scraps/math/Vector.h"

namespace scraps {

struct TransferFunction {
    double(*toVoltage)(double);
    double(*toLinear)(double);

    constexpr bool operator==(const TransferFunction& other) const {
        return toVoltage == other.toVoltage && toLinear == other.toLinear;
    }

    constexpr bool operator!=(const TransferFunction& other) const { return !(*this == other); }
};

namespace TransferFunctions {
    template <int GammaNumerator, int GammaDenominator>
    double GammaToVoltage(double l) { return pow(l, 1.0 / (static_cast<double>(GammaNumerator) / GammaDenominator)); }

    template <int GammaNumerator, int GammaDenominator>
    double GammaToLinear(double v) { return pow(v, static_cast<double>(GammaNumerator) / GammaDenominator); }

    template <int GammaNumerator, int GammaDenominator>
    constexpr TransferFunction kGamma{GammaToVoltage<GammaNumerator, GammaDenominator>, GammaToLinear<GammaNumerator, GammaDenominator>};

    constexpr auto kBT709Alpha = 1.099296826809422;
    constexpr auto kBT709Beta  = 0.018053968510807;

    inline double BT709ToVoltage(double l) {
        return l < kBT709Beta ? l * 4.5 : (kBT709Alpha * pow(l, 0.45) - (kBT709Alpha - 1.0));
    }

    inline double BT709ToLinear(double v) {
        return v < (kBT709Beta * 4.5) ? v / 4.5 : pow((v + kBT709Alpha - 1.0) / kBT709Alpha, 1.0 / 0.45);
    }

    constexpr TransferFunction kBT709{BT709ToVoltage, BT709ToLinear};
    constexpr auto kBT601 = kBT709;
    constexpr auto kBT2020 = kBT709;

    constexpr auto kSRGBAlpha = 1.055;
    constexpr auto kSRGBBeta = 0.0031308;

    inline double SRGBToVoltage(double l) {
        return l < kSRGBBeta ? l * 12.92 : (kSRGBAlpha * pow(l, 1.0 / 2.4) - (kSRGBAlpha - 1.0));
    }

    inline double SRGBToLinear(double v) {
        return v < (kSRGBBeta * 12.92) ? v / 12.92 : pow((v + kSRGBAlpha - 1.0) / kSRGBAlpha, 2.4);
    }

    constexpr TransferFunction kSRGB{SRGBToVoltage, SRGBToLinear};
}

struct ColorProfile {
    constexpr ColorProfile(ColorGamut colorGamut, TransferFunction transferFunction)
        : colorGamut(std::move(colorGamut)), transferFunction(transferFunction) {}

    constexpr bool operator==(const ColorProfile& other) const {
        return colorGamut == other.colorGamut && transferFunction == other.transferFunction;
    }

    constexpr bool operator!=(const ColorProfile& other) const { return !(*this == other); }

    ColorGamut colorGamut;
    TransferFunction transferFunction;
};

namespace ColorProfiles {
    static constexpr ColorProfile kSRGB{ColorGamuts::kSRGB, TransferFunctions::kSRGB};
    static constexpr ColorProfile kBT709{ColorGamuts::kBT709, TransferFunctions::kBT709};
    static constexpr ColorProfile kBT2020{ColorGamuts::kBT2020, TransferFunctions::kBT2020};
}

inline math::Vector<double, 3> TransformColor(math::Vector<double, 3> rgb, ColorProfile from, ColorProfile to) {
    auto linear = math::Vector<double, 3>{from.transferFunction.toLinear(rgb.r), from.transferFunction.toLinear(rgb.g), from.transferFunction.toLinear(rgb.b)};
    auto corrected = to.colorGamut.rgbToXYZ().inverse() * from.colorGamut.rgbToXYZ() * linear;
    auto voltage = math::Vector<double, 3>{to.transferFunction.toVoltage(corrected.r), to.transferFunction.toVoltage(corrected.g), to.transferFunction.toVoltage(corrected.b)};
    return voltage;
}

} // namespace scraps
