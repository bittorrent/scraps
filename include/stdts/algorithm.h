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

#include <algorithm>

#if __cpp_lib_experimental_sample

#include <experimental/algorithm>

namespace stdts {
    using sample = std::experimental::sample;
} // namespace stdts

#else

namespace stdts {
namespace detail {

/**
* Reservoir sampling Algorithm R
* https://en.wikipedia.org/wiki/Reservoir_sampling#Algorithm_R
*/
template <typename PopulationIterator, typename SampleIterator, typename Distance,
          typename UniformRandomNumberGenerator>
SampleIterator sample(
    PopulationIterator first, PopulationIterator last,
    SampleIterator out,
    Distance n,
    UniformRandomNumberGenerator&& g,
    std::input_iterator_tag
) {
    auto k = Distance{0};

    for (; first != last && k < n; ++first, ++k) {
        out[k] = *first;
    }

    for (; first != last; ++first, ++k) {
        auto i = std::uniform_int_distribution<Distance>(0, k)(g);

        if (i < n) {
          out[i] = *first;
        }
    }

    return out + std::min(n, k);
}

/**
* Selection sampling Algorithm S
* http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2014/n3842.pdf
*/
template <typename PopulationIterator, typename SampleIterator, typename Distance,
          typename UniformRandomNumberGenerator>
SampleIterator sample(
    PopulationIterator first, PopulationIterator last,
    SampleIterator out,
    Distance n,
    UniformRandomNumberGenerator&& g,
    std::forward_iterator_tag
) {
    auto unsampledSize = static_cast<Distance>(std::distance(first, last));

    for (n = std::min(n, unsampledSize); n != 0; ++first) {
        auto i = std::uniform_int_distribution<Distance>(0, --unsampledSize)(g);

        if (i < n) {
            *out = *first;
            ++out;
            --n;
        }
    }

    return out;
}

} // namespace detail

template <typename PopulationIterator, typename SampleIterator, typename Distance,
          typename UniformRandomNumberGenerator>
SampleIterator sample(
    PopulationIterator first, PopulationIterator last,
    SampleIterator out,
    Distance n,
    UniformRandomNumberGenerator&& g
) {
    return detail::sample(
        first, last,
        out,
        static_cast<std::common_type_t<Distance, typename std::iterator_traits<PopulationIterator>::difference_type>>(n),
        std::forward<UniformRandomNumberGenerator>(g),
        typename std::iterator_traits<PopulationIterator>::iterator_category{}
    );
}

} // namespace stdts

#endif
